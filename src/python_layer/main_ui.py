import tkinter as tk
from tkinter import messagebox
import subprocess
import sys
import os
import ctypes

class CSegment(ctypes.Structure):
    _fields_ = [
        ("x1", ctypes.c_int),
        ("y1", ctypes.c_int),
        ("x2", ctypes.c_int),
        ("y2", ctypes.c_int),
    ]

GRID_SIZE = 20
CELL_SIZE = 30
CANVAS_SIZE = GRID_SIZE * CELL_SIZE

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
        
        tk.Label(toolbar, text="Zoom: MouseWheel or Z/X. C to reset.", fg="gray").pack(side=tk.LEFT, padx=10)

        # Canvas Frame
        self.canvas = tk.Canvas(root, bg="white")
        self.canvas.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        self.canvas.bind("<Configure>", lambda e: self.auto_zoom_if_needed())
        
        self.zoom_factor = 1.0
        self.pan_x = 0.0
        self.pan_y = 0.0
        self._initial_zoom_done = False
        
        self.segments = [] # list of ((x1,y1), (x2,y2))
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

    def to_physical(self, gx, gy):
        raw_shift_x = self.map_x / 2.0
        grid_shift_x = round((raw_shift_x - 20.0) / 40.0) * 40.0 + 20.0
        grid_shift_y = 42.0
        px = (gx - gy) * 40.0 + grid_shift_x
        py = (gx + gy) * 28.0 + grid_shift_y
        return px, py

    def physical_to_logical(self, px, py):
        raw_shift_x = self.map_x / 2.0
        grid_shift_x = round((raw_shift_x - 20.0) / 40.0) * 40.0 + 20.0
        grid_shift_y = 42.0
        A = (px - grid_shift_x) / 40.0
        B = (py - grid_shift_y) / 28.0
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
        min_px = 20.0
        k = int((self.map_x - 20) / 40)
        max_px = k * 40.0 + 20.0
        
        min_py = 14.0
        j = int((self.map_y - 14) / 28)
        max_py = j * 28.0 + 14.0
        
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

        # draw segments
        for (gx1, gy1), (gx2, gy2) in self.segments:
            px1, py1 = self.to_physical(gx1, gy1)
            px2, py2 = self.to_physical(gx2, gy2)
            s_x1, s_y1 = self.to_screen(px1, py1)
            s_x2, s_y2 = self.to_screen(px2, py2)
            self.canvas.create_line(s_x1, s_y1, s_x2, s_y2, fill="blue", width=3)
            
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
            self.segments.append((self.start_point, (gx, gy)))
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
        self.draw_all()
        
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
        project_root = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
        dll_path = os.path.join(project_root, "build", "mingw-release", "libauto_mapper.dll")
        output_map = os.path.join(project_root, "ui_output.map")
        
        if not os.path.exists(dll_path):
            messagebox.showerror("Error", f"DLL not found: {dll_path}\nPlease build C++ project first.")
            return
            
        try:
            # 加载 DLL
            lib = ctypes.CDLL(dll_path)
            
            # 配置 C 函数签名
            lib.generate_map_from_segments.argtypes = [
                ctypes.c_char_p,                 # output_path
                ctypes.POINTER(CSegment),        # segments
                ctypes.c_int,                    # num_segments
                ctypes.c_int,                    # grid_size
                ctypes.c_float,                  # map_size_x
                ctypes.c_float                   # map_size_y
            ]
            lib.generate_map_from_segments.restype = ctypes.c_bool
            
            # 准备数据
            output_path_bytes = output_map.encode('utf-8')
            num_segments = len(self.segments)
            SegmentArray = CSegment * num_segments
            segments_arr = SegmentArray()
            
            for i, ((x1, y1), (x2, y2)) in enumerate(self.segments):
                segments_arr[i].x1 = int(x1)
                segments_arr[i].y1 = int(y1)
                segments_arr[i].x2 = int(x2)
                segments_arr[i].y2 = int(y2)
                
            # 执行 C++ 引擎
            success = lib.generate_map_from_segments(
                output_path_bytes,
                segments_arr,
                num_segments,
                GRID_SIZE,
                float(map_x),
                float(map_y)
            )
            
            if success:
                messagebox.showinfo("Success", f"Map generated at:\n{output_map}")
            else:
                messagebox.showerror("Error", "C++ Engine Failed! Please check console logs.")
                
        except Exception as e:
            messagebox.showerror("Exception", f"Failed to interact with DLL: {str(e)}")

if __name__ == "__main__":
    root = tk.Tk()
    app = AutoMapperUI(root)
    root.mainloop()
