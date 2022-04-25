#ifndef ROOMS_H
#define ROOMS_H

#include "raylib.h"
#include "screens.h"

#define TILE_SIZE (SCALAR * 16)
#define ROOM_SIZE 10
#define NUM_ROOMS 4
#define BACKGROUND_SIZE 64

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

typedef struct _Room {
    TileType tiles[ROOM_SIZE][ROOM_SIZE];
    Vector2 start;
    int background[ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE][ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE];
    int rotations;
} Room;


bool IsSolid(TileType tile);
bool IsDeath(TileType tile);

int LoadRoom(Room* room, int room_num);
void RotateRoom(Room* room);

#endif

