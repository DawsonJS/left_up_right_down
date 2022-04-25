#ifndef ROOMS_H
#define ROOMS_H

#define SCALAR 2
#define TILE_SIZE 32
#define ROOM_SIZE 10
#define NUM_ROOMS 2

typedef enum _TileType {
    AIR = 0,
    GROUND = 1,
    EXIT = 2,
    START = 3,
    STALAGMITE = 4,
    STALACTITE = 5,
    RAIL = 6,
} TileType;

typedef struct _Tile {
    Texture2D texture;
    TileType type;
} Tile;

TileType room_tile[NUM_ROOMS][ROOM_SIZE][ROOM_SIZE] = {{1, 2, 2, 1, 1, 1, 1, 1, 1, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        3, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        3, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                                                       {1, 1, 3, 3, 1, 1, 1, 1, 1, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 2,
                                                        1, 0, 1, 1, 1, 1, 1, 0, 0, 2,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

#endif

