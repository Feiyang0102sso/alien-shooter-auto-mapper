import tkinter as tk
from tkinter import messagebox, ttk, filedialog
import ctypes
import json
from pathlib import Path

class CSegment(ctypes.Structure):
    _fields_ = [
        ("x1", ctypes.c_int),
        ("y1", ctypes.c_int),
        ("x2", ctypes.c_int),
        ("y2", ctypes.c_int),
        ("wall_type", ctypes.c_int),
    ]

class CDoor(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_int),
        ("y", ctypes.c_int),
        ("wall_type", ctypes.c_int),
        ("direction_type", ctypes.c_int),
        ("size", ctypes.c_int),
        ("door_state", ctypes.c_int),
        ("light_state", ctypes.c_int),
        ("z_offset", ctypes.c_float),
    ]

class CStandardDoorZConfig(ctypes.Structure):
    _fields_ = [
        ("jam_min_z", ctypes.c_float),
        ("jam_max_z", ctypes.c_float),
        ("dead_open_min_z", ctypes.c_float),
        ("dead_open_max_z", ctypes.c_float),
    ]

GRID_SIZE = 20
CELL_SIZE = 30
CANVAS_SIZE = GRID_SIZE * CELL_SIZE
PROJECT_ROOT = Path(__file__).resolve().parents[2]
DLL_PATH = PROJECT_ROOT / "build" / "mingw-release" / "libauto_mapper.dll"
OUTPUT_MAP_PATH = PROJECT_ROOT / "ui_output.map"

WALL_TYPE_STANDARD = 0
WALL_TYPE_LAB = 1

DOOR_STATE_CLOSED = 0
DOOR_STATE_OPEN = 1

LIGHT_STATE_GREEN = 0
LIGHT_STATE_RED = 1
LIGHT_STATE_BROKEN = 2

ACTIVE_SIGNAL_OPEN = 0
ACTIVE_SIGNAL_CLOSED = 1


# ── Tool Brush Constants ──
TOOL_WALL = 0
TOOL_ACTIVE_DOOR = 1
TOOL_DEAD_DOOR_CLOSED = 2
TOOL_DEAD_DOOR_JAMMED = 3
TOOL_DEAD_DOOR_OPEN = 4

# ── Wall profiles: (step_x, step_y, grid_divisor, display_label, line_color) ──
WALL_PROFILES = {
    WALL_TYPE_STANDARD: (40.0, 28.0, 1, "Standard (40×28)", "blue"),
    WALL_TYPE_LAB:      (90.0, 64.0, 2, "Lab (90×64)",      "#00AA00"),
}

class AutoMapperUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Auto Mapper - Logical Grid Canvas")
        
        # Toolbar
        toolbar = tk.Frame(root)
        toolbar.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
        
        btn_generate = tk.Button(toolbar, text="Generate Map", command=self.generate_map, bg="lightblue")
        btn_generate.pack(side=tk.LEFT)
        
        btn_clear = tk.Button(toolbar, text="Clear", command=self.clear_canvas)
        btn_clear.pack(side=tk.LEFT, padx=5)

        btn_export = tk.Button(toolbar, text="Export JSON", command=self.export_segments, bg="#EAEAEA")
        btn_export.pack(side=tk.LEFT, padx=5)

        btn_import = tk.Button(toolbar, text="Import JSON", command=self.import_segments, bg="#EAEAEA")
        btn_import.pack(side=tk.LEFT, padx=5)
        
        # Wall type selector
        tk.Label(toolbar, text="Wall:").pack(side=tk.LEFT, padx=(10, 2))
        self.wall_type_var = tk.IntVar(value=WALL_TYPE_STANDARD)
        wall_labels = [WALL_PROFILES[wt][3] for wt in sorted(WALL_PROFILES.keys())]
        self.wall_combo = ttk.Combobox(toolbar, values=wall_labels, width=16, state="readonly")
        self.wall_combo.current(0)
        self.wall_combo.pack(side=tk.LEFT)
        self.wall_combo.bind("<<ComboboxSelected>>", lambda e: self._on_wall_type_changed())
        
        # Map Size inputs
        tk.Label(toolbar, text="Map X:").pack(side=tk.LEFT, padx=(10, 2))
        self.entry_map_x = tk.Entry(toolbar, width=5)
        self.entry_map_x.insert(0, "600")
        self.entry_map_x.pack(side=tk.LEFT)
        self.entry_map_x.bind("<KeyRelease>", lambda e: self.auto_zoom())
        
        tk.Label(toolbar, text="Map Y:").pack(side=tk.LEFT, padx=(10, 2))
        self.entry_map_y = tk.Entry(toolbar, width=5)
        self.entry_map_y.insert(0, "600")
        self.entry_map_y.pack(side=tk.LEFT)
        self.entry_map_y.bind("<KeyRelease>", lambda e: self.auto_zoom())
        
        self.gen_floor_var = tk.BooleanVar(value=True)
        tk.Checkbutton(toolbar, text="Floor", variable=self.gen_floor_var).pack(side=tk.LEFT, padx=(10, 2))
        
        self.gen_ceiling_var = tk.BooleanVar(value=True)
        tk.Checkbutton(toolbar, text="Ceiling", variable=self.gen_ceiling_var).pack(side=tk.LEFT, padx=(2, 2))

        tk.Label(toolbar, text="Zoom: MouseWheel or Z/X. C to reset.", fg="gray").pack(side=tk.LEFT, padx=10)

        # Brush Toolbar (Drawing Tool Selection)
        brush_bar = tk.LabelFrame(root, text="Drawing Tools / Brushes", padx=5, pady=5)
        brush_bar.pack(side=tk.TOP, fill=tk.X, padx=10, pady=(0, 5))
        
        self.tool_var = tk.IntVar(value=TOOL_WALL)
        
        self.rb_wall = tk.Radiobutton(brush_bar, text="🧱 Wall", variable=self.tool_var, value=TOOL_WALL)
        self.rb_wall.pack(side=tk.LEFT, padx=10)
        
        self.rb_act_door = tk.Radiobutton(brush_bar, text="🟢 Active Door", variable=self.tool_var, value=TOOL_ACTIVE_DOOR)
        self.rb_act_door.pack(side=tk.LEFT, padx=10)
        
        self.rb_dead_close = tk.Radiobutton(brush_bar, text="🔴 Dead Door (Closed)", variable=self.tool_var, value=TOOL_DEAD_DOOR_CLOSED)
        self.rb_dead_close.pack(side=tk.LEFT, padx=10)
        
        self.rb_dead_jam = tk.Radiobutton(brush_bar, text="🟡 Dead Door (Jammed)", variable=self.tool_var, value=TOOL_DEAD_DOOR_JAMMED)
        self.rb_dead_jam.pack(side=tk.LEFT, padx=10)
        
        self.rb_dead_open = tk.Radiobutton(brush_bar, text="🔵 Dead Door (Open)", variable=self.tool_var, value=TOOL_DEAD_DOOR_OPEN)
        self.rb_dead_open.pack(side=tk.LEFT, padx=10)

        tk.Label(brush_bar, text="Active signal:").pack(side=tk.LEFT, padx=(15, 2))
        self.active_signal_var = tk.IntVar(value=ACTIVE_SIGNAL_CLOSED)
        self.rb_active_signal_open = tk.Radiobutton(brush_bar, text="Open", variable=self.active_signal_var, value=ACTIVE_SIGNAL_OPEN)
        self.rb_active_signal_open.pack(side=tk.LEFT, padx=2)
        self.rb_active_signal_closed = tk.Radiobutton(brush_bar, text="Closed", variable=self.active_signal_var, value=ACTIVE_SIGNAL_CLOSED)
        self.rb_active_signal_closed.pack(side=tk.LEFT, padx=2)
        self._update_brush_ui()

        # Canvas Frame
        self.canvas = tk.Canvas(root, bg="white")
        self.canvas.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        self.canvas.bind("<Configure>", lambda e: self.auto_zoom_if_needed())
        
        self.zoom_factor = 1.0
        self.pan_x = 0.0
        self.pan_y = 0.0
        self._initial_zoom_done = False
        
        self.segments = []  # list of ((x1,y1), (x2,y2), wall_type)
        self.doors = []     # list of (x, y, wall_type, direction_type, size, door_state, light_state, z_offset)
        self.auto_mapper_lib = self._load_auto_mapper_lib()
        self.standard_door_z_config = self._load_standard_door_z_config()
        self.start_point = None
        self.temp_line = None
        self._pan_start_x = None
        self._pan_start_y = None
        
        # Bindings
        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<B1-Motion>", self.on_drag)
        self.canvas.bind("<ButtonRelease-1>", self.on_release)
        self.canvas.bind("<Button-2>", self.on_pan_start)
        self.canvas.bind("<B2-Motion>", self.on_pan_drag)
        self.canvas.bind("<ButtonRelease-2>", self.on_pan_release)
        self.canvas.bind("<MouseWheel>", self.on_mouse_wheel)
        self.root.bind("<z>", lambda e: self.zoom(1.2))
        self.root.bind("<x>", lambda e: self.zoom(1/1.2))
        self.root.bind("<c>", lambda e: self.reset_zoom())
        
    def auto_zoom_if_needed(self):
        if not self._initial_zoom_done and self.canvas.winfo_width() > 10:
            self.auto_zoom()
            self._initial_zoom_done = True
            
    @property
    def map_x(self):
        try: return float(self.entry_map_x.get())
        except: return 600.0

    @property
    def map_y(self):
        try: return float(self.entry_map_y.get())
        except: return 600.0

    @property
    def wall_type(self):
        return self.wall_combo.current()

    @property
    def step_x(self):
        return WALL_PROFILES[self.wall_type][0]

    @property
    def step_y(self):
        return WALL_PROFILES[self.wall_type][1]

    def _on_wall_type_changed(self):
        """切换墙壁类型时重绘画布（网格密度会变化）"""
        self._update_brush_ui()
        self.draw_all()

    def _update_brush_ui(self):
        if self.wall_type == WALL_TYPE_LAB:
            self.rb_act_door.config(text="🔒 Laser Door (Closed)")
            self.rb_dead_close.config(text="🟢 Laser Door (Open)")
            self.rb_dead_jam.config(text="🚪 Decoration Door")
            self.rb_dead_open.pack_forget()

            if self.tool_var.get() == TOOL_DEAD_DOOR_OPEN:
                self.tool_var.set(TOOL_ACTIVE_DOOR)
            return

        self.rb_act_door.config(text="🟢 Active Door")
        self.rb_dead_close.config(text="🔴 Dead Door (Closed)")
        self.rb_dead_jam.config(text="🟡 Dead Door (Jammed)")
        self.rb_dead_open.config(text="🔵 Dead Door (Open)")

        if not self.rb_dead_open.winfo_ismapped():
            self.rb_dead_open.pack(side=tk.LEFT, padx=10)

    def _get_active_light_state(self):
        if self.active_signal_var.get() == ACTIVE_SIGNAL_OPEN:
            return LIGHT_STATE_GREEN

        return LIGHT_STATE_RED

    def _load_auto_mapper_lib(self):
        if not DLL_PATH.exists():
            print(f"DLL not found: {DLL_PATH}")
            return None

        return ctypes.CDLL(str(DLL_PATH))

    def _load_standard_door_z_config(self):
        lib = self.auto_mapper_lib
        if lib is None:
            return {}

        lib.get_standard_door_z_config.argtypes = [
            ctypes.c_int,
            ctypes.POINTER(CStandardDoorZConfig)
        ]
        lib.get_standard_door_z_config.restype = ctypes.c_bool

        configs = {}
        for size in (1, 2):
            config = CStandardDoorZConfig()
            success = lib.get_standard_door_z_config(size, ctypes.byref(config))
            if success:
                configs[size] = config

        print(f"Loaded standard door z config from C++: {sorted(configs.keys())}")
        return configs

    def _get_standard_door_z_config(self, size):
        config = self.standard_door_z_config.get(size)
        if config is not None:
            return config

        config = self.standard_door_z_config.get(1)
        if config is not None:
            return config

        messagebox.showerror("Error", f"Door z config not loaded from C++ DLL:\n{DLL_PATH}\nPlease build the C++ project first.")
        return None

    def _get_jam_z_offset(self, size):
        lib = self.auto_mapper_lib
        if lib is None:
            messagebox.showerror("Error", f"Door z config not loaded from C++ DLL:\n{DLL_PATH}\nPlease build the C++ project first.")
            return None

        lib.get_standard_door_jam_z_offset.argtypes = [
            ctypes.c_int,
            ctypes.POINTER(ctypes.c_float)
        ]
        lib.get_standard_door_jam_z_offset.restype = ctypes.c_bool

        z_offset = ctypes.c_float()
        success = lib.get_standard_door_jam_z_offset(size, ctypes.byref(z_offset))
        if not success:
            messagebox.showerror("Error", f"Failed to load jammed door z offset from C++ DLL:\n{DLL_PATH}")
            return None

        return z_offset.value

    def _get_dead_open_z_offset(self, size):
        config = self._get_standard_door_z_config(size)
        if config is None:
            return None

        return (config.dead_open_min_z + config.dead_open_max_z) / 2.0

    def _get_grid_shift(self, sx, sy):
        # Determine step and remainder dynamically using grid_divisor from WALL_PROFILES
        divisor = 1
        for wt, prof in WALL_PROFILES.items():
            if abs(prof[0] - sx) < 0.01:
                divisor = prof[2]
                break

        grid_step_x = sx / divisor
        grid_step_y = sy / divisor
        remainder_x = grid_step_x / 2.0
        remainder_y = grid_step_y / 2.0

        raw_shift_x = self.map_x / 2.0
        grid_shift_x = round((raw_shift_x - remainder_x) / grid_step_x) * grid_step_x + remainder_x

        raw_shift_y = remainder_y
        grid_shift_y = round((raw_shift_y - remainder_y) / grid_step_y) * grid_step_y + remainder_y + sy

        return grid_shift_x, grid_shift_y

    def to_physical(self, gx, gy, sx=None, sy=None):
        """Grid -> Physical coords. Uses current brush steps unless overridden."""
        if sx is None:
            sx = self.step_x
        if sy is None:
            sy = self.step_y
        
        grid_shift_x, grid_shift_y = self._get_grid_shift(sx, sy)

        px = (gx - gy) * sx + grid_shift_x
        py = (gx + gy) * sy + grid_shift_y
        return px, py

    def physical_to_logical(self, px, py, sx=None, sy=None):
        """Physical -> Grid coords. Uses current brush steps unless overridden."""
        if sx is None:
            sx = self.step_x
        if sy is None:
            sy = self.step_y

        grid_shift_x, grid_shift_y = self._get_grid_shift(sx, sy)

        A = (px - grid_shift_x) / sx
        B = (py - grid_shift_y) / sy
        gx = (A + B) / 2.0
        gy = (B - A) / 2.0
        return gx, gy

    def to_screen(self, px, py):
        canvas_w = self.canvas.winfo_width()
        canvas_h = self.canvas.winfo_height()
        screen_w = self.map_x * self.zoom_factor
        screen_h = self.map_y * self.zoom_factor
        offset_x = (canvas_w - screen_w) / 2 + self.pan_x
        offset_y = (canvas_h - screen_h) / 2 + self.pan_y
        sx = px * self.zoom_factor + offset_x
        sy = py * self.zoom_factor + offset_y
        return sx, sy

    def screen_to_physical(self, sx, sy):
        canvas_w = self.canvas.winfo_width()
        canvas_h = self.canvas.winfo_height()
        screen_w = self.map_x * self.zoom_factor
        screen_h = self.map_y * self.zoom_factor
        offset_x = (canvas_w - screen_w) / 2 + self.pan_x
        offset_y = (canvas_h - screen_h) / 2 + self.pan_y
        px = (sx - offset_x) / self.zoom_factor
        py = (sy - offset_y) / self.zoom_factor
        return px, py

    @property
    def grid_bounds(self):
        sx = self.step_x
        sy = self.step_y
        half_sx = sx / 2.0
        half_sy = sy / 2.0

        min_px = half_sx
        k = int((self.map_x - half_sx) / sx)
        max_px = k * sx + half_sx

        min_py = half_sy
        j = int((self.map_y - half_sy) / sy)
        max_py = j * sy + half_sy

        if max_px < min_px: max_px = min_px
        if max_py < min_py: max_py = min_py

        return min_px, min_py, max_px, max_py

    def snap_to_grid(self, sx, sy):
        px, py = self.screen_to_physical(sx, sy)
        min_px, min_py, max_px, max_py = self.grid_bounds
        
        # Clamp loosely first
        clamped_px = max(min_px, min(px, max_px))
        clamped_py = max(min_py, min(py, max_py))
        
        base_gx, base_gy = self.physical_to_logical(clamped_px, clamped_py)
        base_gx = int(round(base_gx))
        base_gy = int(round(base_gy))
        
        # Search neighborhood for the closest valid point that is STRICTLY inside the grid bounds
        best_point = (base_gx, base_gy)
        min_dist = float('inf')
        
        for i in range(-4, 5):
            for j in range(-4, 5):
                test_gx = base_gx + i
                test_gy = base_gy + j
                test_px, test_py = self.to_physical(test_gx, test_gy)
                
                # Check if physical coordinates are within the exact grid boundaries
                if min_px <= test_px <= max_px and min_py <= test_py <= max_py:
                    dist = (test_px - px)**2 + (test_py - py)**2
                    if dist < min_dist:
                        min_dist = dist
                        best_point = (test_gx, test_gy)
                        
        # If for some reason we found no valid points (extremely small map?), return base
        if min_dist == float('inf'):
            return base_gx, base_gy
            
        return best_point

    def on_mouse_wheel(self, event):
        if event.delta > 0:
            self.zoom(1.1)
        else:
            self.zoom(1/1.1)

    def zoom(self, factor):
        self.zoom_factor *= factor
        self.zoom_factor = max(0.01, min(self.zoom_factor, 10.0))
        self.draw_all()

    def reset_zoom(self):
        self.auto_zoom()

    def auto_zoom(self):
        self.root.update_idletasks()
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()
        if width <= 1: width = 800
        if height <= 1: height = 600
        
        self.pan_x = 0.0
        self.pan_y = 0.0
        
        if self.map_x > 0 and self.map_y > 0:
            zx = (width * 0.95) / self.map_x
            zy = (height * 0.95) / self.map_y
            self.zoom_factor = min(zx, zy)
            self.zoom_factor = max(0.01, min(self.zoom_factor, 10.0))
        self.draw_all()

    def draw_all(self):
        self.canvas.delete("all")
        if self.map_x <= 0 or self.map_y <= 0: return
        
        # Determine logical limits to draw grid
        corners = [(0,0), (self.map_x, 0), (0, self.map_y), (self.map_x, self.map_y)]
        log_corners = [self.physical_to_logical(px, py) for px, py in corners]
        min_g = min(min(g[0] for g in log_corners), min(g[1] for g in log_corners)) - 2
        max_g = max(max(g[0] for g in log_corners), max(g[1] for g in log_corners)) + 2

        # Draw isometric grid
        for gx in range(int(min_g), int(max_g) + 1):
            px1, py1 = self.to_physical(gx, min_g)
            px2, py2 = self.to_physical(gx, max_g)
            sx1, sy1 = self.to_screen(px1, py1)
            sx2, sy2 = self.to_screen(px2, py2)
            self.canvas.create_line(sx1, sy1, sx2, sy2, fill="#e0e0e0")

        for gy in range(int(min_g), int(max_g) + 1):
            px1, py1 = self.to_physical(min_g, gy)
            px2, py2 = self.to_physical(max_g, gy)
            sx1, sy1 = self.to_screen(px1, py1)
            sx2, sy2 = self.to_screen(px2, py2)
            self.canvas.create_line(sx1, sy1, sx2, sy2, fill="#e0e0e0")

        # Draw segments (each uses its own wall_type's step sizes and color)
        for (gx1, gy1), (gx2, gy2), seg_wt in self.segments:
            seg_sx, seg_sy, _, _, seg_color = WALL_PROFILES[seg_wt]
            px1, py1 = self.to_physical(gx1, gy1, seg_sx, seg_sy)
            px2, py2 = self.to_physical(gx2, gy2, seg_sx, seg_sy)
            s_x1, s_y1 = self.to_screen(px1, py1)
            s_x2, s_y2 = self.to_screen(px2, py2)
            self.canvas.create_line(s_x1, s_y1, s_x2, s_y2, fill=seg_color, width=3)
            
        # Draw doors
        for x, y, wt, dt, sz, ds, ls, zo in self.doors:
            seg_sx, seg_sy, _, _, _ = WALL_PROFILES[wt]
            draw_size = sz
            if wt == WALL_TYPE_LAB:
                draw_size = 1

            px1, py1 = self.to_physical(x, y, seg_sx, seg_sy)
            if dt == 0:  # A direction (vertical)
                px2, py2 = self.to_physical(x, y + draw_size, seg_sx, seg_sy)
            else:  # B direction (horizontal)
                px2, py2 = self.to_physical(x + draw_size, y, seg_sx, seg_sy)
                
            s_x1, s_y1 = self.to_screen(px1, py1)
            s_x2, s_y2 = self.to_screen(px2, py2)
            
            if wt == WALL_TYPE_LAB and ls == 2:
                color = "#8B0000"
                dot_color = "#FF0000"
                hollow = False
            elif wt == WALL_TYPE_LAB and ds == DOOR_STATE_OPEN:
                color = "#32CD32"
                dot_color = "#D3D3D3"
                hollow = True
            elif wt == WALL_TYPE_LAB:
                color = "#00A6D6"
                dot_color = "#7DF9FF"
                hollow = False
            elif ls == LIGHT_STATE_RED:
                color = "#2E8B57"      # SeaGreen
                dot_color = "#32CD32"  # LimeGreen
                hollow = False
            elif ls == LIGHT_STATE_GREEN:
                color = "#32CD32"
                dot_color = "#00FF00"
                hollow = False
            else:
                if zo == 0.0:  # Closed - dark red
                    color = "#8B0000"
                    dot_color = "#FF0000"
                    hollow = False
                elif ds == DOOR_STATE_CLOSED:
                    color = "#D2691E"
                    dot_color = "#FFD700"  # Gold
                    hollow = False
                else:
                    color = "#808080"
                    dot_color = "#D3D3D3"
                    hollow = True
            
            # Draw door line
            self.canvas.create_line(s_x1, s_y1, s_x2, s_y2, fill=color, width=5)
            
            # Draw status indicator dot in the middle
            mid_x = (s_x1 + s_x2) / 2.0
            mid_y = (s_y1 + s_y2) / 2.0
            r = 4
            if hollow:
                self.canvas.create_oval(mid_x - r, mid_y - r, mid_x + r, mid_y + r, fill="white", outline=color, width=2)
            else:
                self.canvas.create_oval(mid_x - r, mid_y - r, mid_x + r, mid_y + r, fill=dot_color, outline="")
            
        # 标注原点 (gx=0, gy=0)
        opx, opy = self.to_physical(0, 0)
        osx, osy = self.to_screen(opx, opy)
        r = 5  # 原点红点半径
        self.canvas.create_oval(osx - r, osy - r, osx + r, osy + r, fill="red", outline="darkred", width=1)

        # Mask out everything outside the map
        canvas_w = self.canvas.winfo_width()
        canvas_h = self.canvas.winfo_height()
        min_px, min_py, max_px, max_py = self.grid_bounds
        sx1, sy1 = self.to_screen(min_px, min_py)
        sx2, sy2 = self.to_screen(max_px, max_py)
        
        self.canvas.create_rectangle(0, 0, canvas_w, sy1, fill="white", outline="")
        self.canvas.create_rectangle(0, sy2, canvas_w, canvas_h, fill="white", outline="")
        self.canvas.create_rectangle(0, sy1, sx1, sy2, fill="white", outline="")
        self.canvas.create_rectangle(sx2, sy1, canvas_w, sy2, fill="white", outline="")
        
        self.canvas.create_rectangle(sx1, sy1, sx2, sy2, outline="black", width=2)

    def get_clamped_orthogonal(self, gx0, gy0, gx, gy):
        if abs(gx - gx0) > abs(gy - gy0):
            gy = gy0
        else:
            gx = gx0
            
        min_px, min_py, max_px, max_py = self.grid_bounds
        px, py = self.to_physical(gx, gy)
        
        # Step back towards start_point until inside bounds
        while (gx != gx0 or gy != gy0) and not (min_px <= px <= max_px and min_py <= py <= max_py):
            if gx > gx0: gx -= 1
            elif gx < gx0: gx += 1
            elif gy > gy0: gy -= 1
            elif gy < gy0: gy += 1
            px, py = self.to_physical(gx, gy)
            
        return gx, gy

    def on_click(self, event):
        gx, gy = self.snap_to_grid(event.x, event.y)
        self.start_point = (gx, gy)
        
    def on_drag(self, event):
        if not self.start_point: return
        gx, gy = self.snap_to_grid(event.x, event.y)
        
        gx, gy = self.get_clamped_orthogonal(self.start_point[0], self.start_point[1], gx, gy)
            
        if self.temp_line:
            self.canvas.delete(self.temp_line)
            
        px1, py1 = self.to_physical(self.start_point[0], self.start_point[1])
        px2, py2 = self.to_physical(gx, gy)
        s_x1, s_y1 = self.to_screen(px1, py1)
        s_x2, s_y2 = self.to_screen(px2, py2)
        self.temp_line = self.canvas.create_line(s_x1, s_y1, s_x2, s_y2, fill="red", width=3)
        
    def on_release(self, event):
        if not self.start_point: return
        gx, gy = self.snap_to_grid(event.x, event.y)
        
        if (gx, gy) != self.start_point:
            gx, gy = self.get_clamped_orthogonal(self.start_point[0], self.start_point[1], gx, gy)
            
        if (gx, gy) != self.start_point:
            tool = self.tool_var.get()
            if tool == TOOL_WALL:
                self.segments.append((self.start_point, (gx, gy), self.wall_type))
            else:
                x1, y1 = self.start_point
                x2, y2 = gx, gy
                
                dx = abs(x2 - x1)
                dy = abs(y2 - y1)
                
                if x1 == x2:  # A direction (vertical)
                    direction_type = 0
                    size = min(2, max(1, dy))
                    pos_x = x1
                    pos_y = min(y1, y2)
                else:  # B direction (horizontal)
                    direction_type = 1
                    size = min(2, max(1, dx))
                    pos_x = min(x1, x2)
                    pos_y = y1
                
                wt = self.wall_type
                if wt == WALL_TYPE_LAB:
                    size = 1

                if wt == WALL_TYPE_LAB:
                    if tool == TOOL_ACTIVE_DOOR:
                        # Laser closed uses frame and pillar.
                        self.doors.append((pos_x, pos_y, wt, direction_type, size, DOOR_STATE_CLOSED, LIGHT_STATE_RED, 0.0))
                    elif tool == TOOL_DEAD_DOOR_CLOSED:
                        # Laser open uses frame only.
                        self.doors.append((pos_x, pos_y, wt, direction_type, size, DOOR_STATE_OPEN, LIGHT_STATE_RED, 0.0))
                    elif tool == TOOL_DEAD_DOOR_JAMMED:
                        # Decoration door uses a single frame.
                        self.doors.append((pos_x, pos_y, wt, direction_type, size, DOOR_STATE_CLOSED, LIGHT_STATE_BROKEN, 0.0))
                elif tool == TOOL_ACTIVE_DOOR:
                    # Active panel uses fixed open assets.
                    self.doors.append((pos_x, pos_y, wt, direction_type, size, DOOR_STATE_OPEN, self._get_active_light_state(), 0.0))
                elif tool == TOOL_DEAD_DOOR_CLOSED:
                    # Dead closed uses dead panel assets.
                    self.doors.append((pos_x, pos_y, wt, direction_type, size, DOOR_STATE_CLOSED, LIGHT_STATE_BROKEN, 0.0))
                elif tool == TOOL_DEAD_DOOR_JAMMED:
                    # Dead jammed uses the configured z range.
                    z_offset = self._get_jam_z_offset(size)
                    if z_offset is not None:
                        self.doors.append((pos_x, pos_y, wt, direction_type, size, DOOR_STATE_CLOSED, LIGHT_STATE_BROKEN, z_offset))
                elif tool == TOOL_DEAD_DOOR_OPEN:
                    # Dead open uses a fixed z offset.
                    z_offset = self._get_dead_open_z_offset(size)
                    if z_offset is not None:
                        self.doors.append((pos_x, pos_y, wt, direction_type, size, DOOR_STATE_OPEN, LIGHT_STATE_BROKEN, z_offset))
                    
            self.draw_all()
            
        if self.temp_line:
            self.canvas.delete(self.temp_line)
            self.temp_line = None
        self.start_point = None

    def on_pan_start(self, event):
        self._pan_start_x = event.x
        self._pan_start_y = event.y

    def on_pan_drag(self, event):
        if self._pan_start_x is not None and self._pan_start_y is not None:
            dx = event.x - self._pan_start_x
            dy = event.y - self._pan_start_y
            self.pan_x += dx
            self.pan_y += dy
            self._pan_start_x = event.x
            self._pan_start_y = event.y
            self.draw_all()

    def on_pan_release(self, event):
        self._pan_start_x = None
        self._pan_start_y = None

    def clear_canvas(self):
        self.segments.clear()
        self.doors.clear()
        self.draw_all()

    def export_segments(self):
        if not self.segments and not self.doors:
            messagebox.showwarning("Warning", "Draw segments or doors to export first!")
            return
            
        file_path = filedialog.asksaveasfilename(
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")],
            title="Export Drawn Segments and Doors"
        )
        if not file_path:
            return
            
        try:
            data = {
                "map_size_x": self.map_x,
                "map_size_y": self.map_y,
                "segments": [
                    {
                        "start": {"x": x1, "y": y1},
                        "end": {"x": x2, "y": y2},
                        "wall_type": wt,
                        "floor_type": 0
                    }
                    for (x1, y1), (x2, y2), wt in self.segments
                ],
                "doors": [
                    {
                        "pos": {"x": x, "y": y},
                        "wall_type": wt,
                        "direction_type": dt,
                        "size": sz,
                        "door_state": ds,
                        "light_state": ls,
                        "z_offset": zo
                    }
                    for x, y, wt, dt, sz, ds, ls, zo in self.doors
                ]
            }
            
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=2, ensure_ascii=False)
                
            messagebox.showinfo("Success", f"Data exported successfully to:\n{file_path}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to export data: {str(e)}")

    def import_segments(self):
        file_path = filedialog.askopenfilename(
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")],
            title="Import JSON"
        )
        if not file_path:
            return
            
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
                
            map_size_x = data.get("map_size_x", 600.0)
            map_size_y = data.get("map_size_y", 600.0)
            
            self.entry_map_x.delete(0, tk.END)
            try:
                val_x = int(map_size_x) if float(map_size_x).is_integer() else float(map_size_x)
            except:
                val_x = map_size_x
            self.entry_map_x.insert(0, str(val_x))
            
            self.entry_map_y.delete(0, tk.END)
            try:
                val_y = int(map_size_y) if float(map_size_y).is_integer() else float(map_size_y)
            except:
                val_y = map_size_y
            self.entry_map_y.insert(0, str(val_y))
            
            segments = []
            json_segs = data.get("segments", [])
            for seg in json_segs:
                start = seg.get("start", {})
                end = seg.get("end", {})
                wt = seg.get("wall_type", WALL_TYPE_STANDARD)
                
                x1 = start.get("x", 0)
                y1 = start.get("y", 0)
                x2 = end.get("x", 0)
                y2 = end.get("y", 0)
                
                segments.append(((x1, y1), (x2, y2), wt))
                
            doors = []
            json_doors = data.get("doors", [])
            for d in json_doors:
                pos = d.get("pos", {})
                x = pos.get("x", 0)
                y = pos.get("y", 0)
                wt = d.get("wall_type", 0)
                dt = d.get("direction_type", 0)
                sz = d.get("size", 1)
                ds = d.get("door_state", 0)
                ls = d.get("light_state", 0)
                zo = d.get("z_offset", 0.0)
                doors.append((x, y, wt, dt, sz, ds, ls, zo))
                
            self.segments = segments
            self.doors = doors
            self.auto_zoom()
            self.draw_all()
            
            messagebox.showinfo("Success", f"Imported {len(segments)} segments successfully.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to import segments: {str(e)}")

    def generate_map(self):
        if not self.segments:
            messagebox.showwarning("Warning", "Draw something first!")
            return
            
        map_x = self.entry_map_x.get()
        map_y = self.entry_map_y.get()
        if not map_x.isdigit() or not map_y.isdigit():
            messagebox.showerror("Error", "Map size must be numbers")
            return
            
        # 查找 DLL
        if not DLL_PATH.exists():
            messagebox.showerror("Error", f"DLL not found: {DLL_PATH}\nPlease build C++ project first.")
            return
            
        try:
            # 加载 DLL
            lib = ctypes.CDLL(str(DLL_PATH))
            
            # 配置 C 函数签名 (including doors pointer and count)
            lib.generate_map_from_segments.argtypes = [
                ctypes.c_char_p,                 # output_path
                ctypes.POINTER(CSegment),        # segments
                ctypes.c_int,                    # num_segments
                ctypes.POINTER(CDoor),           # doors
                ctypes.c_int,                    # num_doors
                ctypes.c_float,                  # map_size_x
                ctypes.c_float,                  # map_size_y
                ctypes.c_bool,                   # gen_floor
                ctypes.c_bool                    # gen_ceiling
            ]
            lib.generate_map_from_segments.restype = ctypes.c_bool
            
            # 准备数据
            output_path_bytes = str(OUTPUT_MAP_PATH).encode('utf-8')
            num_segments = len(self.segments)
            SegmentArray = CSegment * num_segments
            segments_arr = SegmentArray()
            
            for i, ((x1, y1), (x2, y2), wt) in enumerate(self.segments):
                segments_arr[i].x1 = int(x1)
                segments_arr[i].y1 = int(y1)
                segments_arr[i].x2 = int(x2)
                segments_arr[i].y2 = int(y2)
                segments_arr[i].wall_type = wt
                
            num_doors = len(self.doors)
            DoorArray = CDoor * num_doors
            doors_arr = DoorArray()
            
            for i, (x, y, wt, dt, sz, ds, ls, zo) in enumerate(self.doors):
                doors_arr[i].x = int(x)
                doors_arr[i].y = int(y)
                doors_arr[i].wall_type = int(wt)
                doors_arr[i].direction_type = int(dt)
                doors_arr[i].size = int(sz)
                doors_arr[i].door_state = int(ds)
                doors_arr[i].light_state = int(ls)
                doors_arr[i].z_offset = float(zo)
                
            # 执行 C++ 引擎
            success = lib.generate_map_from_segments(
                output_path_bytes,
                segments_arr,
                num_segments,
                doors_arr,
                num_doors,
                float(map_x),
                float(map_y),
                self.gen_floor_var.get(),
                self.gen_ceiling_var.get()
            )
            
            if success:
                messagebox.showinfo("Success", f"Map generated at:\n{OUTPUT_MAP_PATH}")
            else:
                messagebox.showerror("Error", "C++ Engine Failed! Please check console logs.")
                
        except Exception as e:
            messagebox.showerror("Exception", f"Failed to interact with DLL: {str(e)}")

if __name__ == "__main__":
    root = tk.Tk()
    app = AutoMapperUI(root)
    root.mainloop()
