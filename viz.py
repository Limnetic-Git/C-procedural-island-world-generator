import raylib
import subprocess
import time

def hex_to_rgba(hex_color):
    hex_color = hex_color.lstrip('#')

    if len(hex_color) == 6:
        hex_color += 'FF'
    return (
        int(hex_color[0:2], 16),
        int(hex_color[2:4], 16),
        int(hex_color[4:6], 16),
        int(hex_color[6:8], 16)
    )

generation_process = subprocess.Popen("./test")
generation_process.wait()


WIN_WIDTH, WIN_HEIGHT = 1024, 1024
raylib.InitWindow(WIN_WIDTH, WIN_HEIGHT, b"test")
raylib.SetTargetFPS(60)


world_size = 256
tile_size = WIN_HEIGHT // world_size


with open("world.txt", "r") as file:
    lines = file.readlines()
    world = eval(lines[0])

biom_colors = ["#00a3e8", "#f52432", "#efe2ad", "#24b04d", "#fff004", "#ffffff"]
for i, color in enumerate(biom_colors):
    biom_colors[i] = hex_to_rgba(color)

while raylib.WindowShouldClose:
    raylib.BeginDrawing()
    raylib.ClearBackground(raylib.BLUE)
    for x in range(world_size):
        for y in range(world_size):
            try:
                raylib.DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, biom_colors[world[x][y]])
            except IndexError:
                pass
    if raylib.IsKeyPressed(raylib.KEY_SPACE):
        generation_process = subprocess.Popen("./test")
        generation_process.wait()
        with open("world.txt", "r") as file:
            lines = file.readlines()
            world = eval(lines[0])


    raylib.EndDrawing()

raylib.CloseWindow()
