import tkinter as tk
from tkinter import messagebox
import subprocess
import sys
import os

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
        
        # Canvas
        self.canvas = tk.Canvas(root, width=CANVAS_SIZE, height=CANVAS_SIZE, bg="white")
        self.canvas.pack(padx=10, pady=10)
        
        self.segments = [] # list of ((x1,y1), (x2,y2))
        self.start_point = None
        self.temp_line = None
        
        self.draw_grid()
        
        # Bindings
        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<B1-Motion>", self.on_drag)
        self.canvas.bind("<ButtonRelease-1>", self.on_release)
        
    def draw_grid(self):
        for i in range(GRID_SIZE + 1):
            coord = i * CELL_SIZE
            self.canvas.create_line(coord, 0, coord, CANVAS_SIZE, fill="#e0e0e0")
            self.canvas.create_line(0, coord, CANVAS_SIZE, coord, fill="#e0e0e0")

    def snap_to_grid(self, x, y):
        gx = round(x / CELL_SIZE)
        gy = round(y / CELL_SIZE)
        # Limit to grid
        gx = max(0, min(gx, GRID_SIZE))
        gy = max(0, min(gy, GRID_SIZE))
        return gx, gy
        
    def on_click(self, event):
        gx, gy = self.snap_to_grid(event.x, event.y)
        self.start_point = (gx, gy)
        
    def on_drag(self, event):
        if not self.start_point: return
        gx, gy = self.snap_to_grid(event.x, event.y)
        
        # Force orthogonal drawing (either horizontal or vertical)
        if abs(gx - self.start_point[0]) > abs(gy - self.start_point[1]):
            gy = self.start_point[1]
        else:
            gx = self.start_point[0]
            
        if self.temp_line:
            self.canvas.delete(self.temp_line)
            
        x1, y1 = self.start_point[0] * CELL_SIZE, self.start_point[1] * CELL_SIZE
        x2, y2 = gx * CELL_SIZE, gy * CELL_SIZE
        self.temp_line = self.canvas.create_line(x1, y1, x2, y2, fill="red", width=3)
        
    def on_release(self, event):
        if not self.start_point: return
        gx, gy = self.snap_to_grid(event.x, event.y)
        
        if abs(gx - self.start_point[0]) > abs(gy - self.start_point[1]):
            gy = self.start_point[1]
        else:
            gx = self.start_point[0]
            
        if (gx, gy) != self.start_point:
            self.segments.append((self.start_point, (gx, gy)))
            x1, y1 = self.start_point[0] * CELL_SIZE, self.start_point[1] * CELL_SIZE
            x2, y2 = gx * CELL_SIZE, gy * CELL_SIZE
            self.canvas.create_line(x1, y1, x2, y2, fill="blue", width=3)
            
        if self.temp_line:
            self.canvas.delete(self.temp_line)
            self.temp_line = None
            
        self.start_point = None

    def clear_canvas(self):
        self.segments.clear()
        self.canvas.delete("all")
        self.draw_grid()
        
    def generate_map(self):
        if not self.segments:
            messagebox.showwarning("Warning", "Draw something first!")
            return
            
        # 准备数据给 C++
        input_data = ""
        for (x1, y1), (x2, y2) in self.segments:
            input_data += f"{x1} {y1} {x2} {y2}\n"
            
        # 查找可执行文件
        exe_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "bin", "auto_mapper.exe")
        output_map = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "ui_output.map")
        
        if not os.path.exists(exe_path):
            messagebox.showerror("Error", f"Executable not found: {exe_path}\nPlease build C++ project first.")
            return
            
        try:
            # 调用 C++，通过 stdin 灌入数据
            process = subprocess.Popen(
                [exe_path, output_map],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            stdout, stderr = process.communicate(input=input_data)
            
            if process.returncode == 0:
                messagebox.showinfo("Success", f"Map generated at:\n{output_map}\n\nOutput:\n{stdout}")
            else:
                messagebox.showerror("Error", f"C++ Engine Failed!\n\nStderr:\n{stderr}\nStdout:\n{stdout}")
        except Exception as e:
            messagebox.showerror("Exception", str(e))

if __name__ == "__main__":
    root = tk.Tk()
    app = AutoMapperUI(root)
    root.mainloop()
