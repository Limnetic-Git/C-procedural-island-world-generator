import raylib
import subprocess
import time

start = time.time()
subprocess.Popen("./test")
print(time.time() - start)

WIN_WIDTH, WIN_HEIGHT = 1024, 1024
raylib.InitWindow(WIN_WIDTH, WIN_HEIGHT, b"test")
raylib.SetTargetFPS(60)


world_size = 256
tile_size = WIN_HEIGHT // world_size


with open("world.txt", "r") as file:
    lines = file.readlines()
    world = eval(lines[0])


biom_colors = [raylib.BLUE, raylib.BLACK, raylib.GREEN, raylib.YELLOW, raylib.WHITE]

while raylib.WindowShouldClose:
    raylib.BeginDrawing()
    raylib.ClearBackground(raylib.BLUE)
    for x in range(world_size):
        for y in range(world_size):
            raylib.DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, biom_colors[world[x][y]])

    raylib.EndDrawing()

raylib.CloseWindow()
