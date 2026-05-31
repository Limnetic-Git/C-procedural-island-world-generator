#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define world_size 256
#define bioms_count 3

int world[world_size][world_size] = { 0 };
int bioms_blocks[world_size * world_size][3] = { 0 };
int rivers_only[world_size][world_size] = { 0 };

void saveWorld()
{
    FILE* f = fopen("world.txt", "w");
    fprintf(f, "[");
    for (int x = 0; x < world_size; x++) {
        fprintf(f, "[");
        for (int y = 0; y < world_size; y++) {
            fprintf(f, "%d,", world[x][y]);
        }
        fprintf(f, "],");
    }
    fprintf(f, "]");
}

int random_int(int a, int b)
{
    return a + rand() % (b - a + 1);
}

int countNeighbours(int x, int y)
{
    int waterCount = 0;

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0)
                continue;
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || nx >= world_size || ny < 0 || ny >= world_size) {
                waterCount++;
            } else if (world[nx][ny] == 0) {
                waterCount++;
            }
        }
    }
    return 8 - waterCount;
}

int caluculateNextForm(int x, int y)
{
    int n = countNeighbours(x, y);
    if (n >= 5)
        return 1;
    if (n <= 3)
        return 0;
    return world[x][y];
}

void printWorld()
{
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            printf("%i ", world[x][y]);
        }
        printf("\n");
    }
}

void initNoise()
{
    int center = world_size / 2;
    int max_chance = hypot(center - 0, center - 0);
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            int dist = hypot(center - x, center - y);
            int land_chance = max_chance - dist;
            int r = random_int(0, max_chance);
            if (r <= land_chance) {
                world[x][y] = 1;
            }
        }
    }
}

void smoofIteration()
{
    int step_world[world_size][world_size];

    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            step_world[x][y] = caluculateNextForm(x, y);
        }
    }
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            world[x][y] = step_world[x][y];
        }
    }
}

void addBiomClusterBlock(int rx, int ry, int biom)
{
    int i = 0;
    while (true) {
        if (bioms_blocks[i][0] == 0) {
            bioms_blocks[i][0] = rx;
            bioms_blocks[i][1] = ry;
            bioms_blocks[i][2] = biom;
            break;
        }
        i++;
    }
}

void generateBiomClusters()
{
    for (int biom = 2; biom < bioms_count + 2; biom++) {
        int rx = random_int(0, world_size - 1);
        int ry = random_int(0, world_size - 1);
        while (world[rx][ry] != 1) {
            rx = random_int(0, world_size - 1);
            ry = random_int(0, world_size - 1);
        }
        world[rx][ry] = biom;
        addBiomClusterBlock(rx, ry, biom);
    }
}

void growBioms()
{
    int step_world[world_size][world_size];
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            step_world[x][y] = world[x][y];
        }
    }
    for (int i = 0; i < world_size * world_size; i++) {
        if (bioms_blocks[i][0] != 0) {
            int x = bioms_blocks[i][0];
            int y = bioms_blocks[i][1];
            int biom = bioms_blocks[i][2];

            bool dirs[4] = { false };
            bool haveGroundAround = false;
            if (step_world[x][y - 1] == 1)
                dirs[0] = true;
            if (step_world[x - 1][y] == 1)
                dirs[1] = true;
            if (step_world[x][y + 1] == 1)
                dirs[2] = true;
            if (step_world[x + 1][y] == 1)
                dirs[3] = true;
            for (int j = 0; j < 4; j++) {
                if (dirs[j]) {
                    haveGroundAround = true;
                    break;
                }
            }
            if (haveGroundAround) {
                int r = random_int(0, 3);
                while (!dirs[r])
                    r = random_int(0, 3);
                if (r == 0) {
                    step_world[x][y - 1] = biom;
                    addBiomClusterBlock(x, y - 1, biom);
                } else if (r == 1) {
                    step_world[x - 1][y] = biom;
                    addBiomClusterBlock(x - 1, y, biom);
                } else if (r == 2) {
                    step_world[x][y + 1] = biom;
                    addBiomClusterBlock(x, y + 1, biom);
                } else if (r == 3) {
                    step_world[x + 1][y] = biom;
                    addBiomClusterBlock(x + 1, y, biom);
                }
            } else {
                bioms_blocks[i][0] = 0;
            }
        }
    }
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            world[x][y] = step_world[x][y];
        }
    }
}

void generateRivers()
{
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            if (world[x][y] >= 2) {
                int biom = world[x][y];
                bool willBeRiver = false;
                if (world[x - 1][y] != 0 && world[x - 1][y] != biom)
                    willBeRiver = true;
                else if (world[x + 1][y] != 0 && world[x + 1][y] != biom)
                    willBeRiver = true;
                else if (world[x][y + 1] != 0 && world[x][y + 1] != biom)
                    willBeRiver = true;
                else if (world[x][y - 1] != 0 && world[x][y - 1] != biom)
                    willBeRiver = true;
                if (willBeRiver)
                    rivers_only[x][y] = 1;
            }
        }
    }
}

void printRivers()
{
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            if (rivers_only[x][y] == 1)
                world[x][y] = 0;
        }
    }
}

void extendRivers()
{
    int step_rivers_only[world_size][world_size] = { 0 };
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            if (rivers_only[x][y] == 1) {
                if (x != 0 && x != world_size - 1 && y != 0 && y != world_size - 1) {
                    step_rivers_only[x - 1][y] = 1;
                    step_rivers_only[x + 1][y] = 1;
                    step_rivers_only[x][y + 1] = 1;
                    step_rivers_only[x][y - 1] = 1;
                }
            }
        }
    }
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            if (step_rivers_only[x][y] == 1) {
                rivers_only[x][y] = 1;
            }
        }
    }
}

void deleteIslands()
{
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            if (world[x][y] == 1)
                world[x][y] = 0;
        }
    }
}

void deleteBioms()
{
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            if (world[x][y] > 1)
                world[x][y] = 1;
        }
    }
}

void smoofRivers()
{
    int world_copy[world_size][world_size] = { 0 };
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            world_copy[x][y] = world[x][y];
        }
    }
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            world[x][y] = rivers_only[x][y];
        }
    }
    for (int i = 0; i < 10; i++)
        smoofIteration();
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            rivers_only[x][y] = world[x][y];
        }
    }
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            world[x][y] = world_copy[x][y];
        }
    }
}

int main()
{
    srand(time(NULL));
    initNoise();
    for (int i = 0; i < 20; i++)
        smoofIteration();
    generateBiomClusters();
    for (int i = 0; i < world_size; i++) {
        growBioms();
    }
    generateRivers();
    extendRivers();
    deleteBioms();

    generateBiomClusters();
    for (int i = 0; i < world_size; i++) {
        growBioms();
    }
    smoofRivers();

    printRivers();
    deleteIslands();

    saveWorld();
    return 0;
}
