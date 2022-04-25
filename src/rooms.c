#include "rooms.h"

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
                                                       {1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 2,
                                                        1, 0, 1, 1, 1, 1, 1, 0, 0, 2,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                        1, 3, 3, 1, 1, 1, 1, 1, 1, 1}};

bool IsSolid(TileType tile) {
    bool result = false;
    switch (tile) {
        case GROUND:
            result = true;
            break;
        default:
            break;
    }
    return result;
}

bool IsDeath(TileType tile) {
    bool result = false;
    switch (tile) {
        case STALAGMITE:
            result = true;
            break;
        case STALACTITE:
            result = true;
            break;
        default:
            break;
    }
    return result;
}

void RotateRoom(Room* room) {
    for (int i = 0; i < (ROOM_SIZE + 1) / 2; i ++) {
        for (int j = 0; j < ROOM_SIZE / 2; j++) {
            TileType temp = room->tiles[ROOM_SIZE - 1 - j][i];
            room->tiles[ROOM_SIZE - 1 - j][i] = room->tiles[ROOM_SIZE - 1 - i][ROOM_SIZE - j - 1];
            room->tiles[ROOM_SIZE - 1 - i][ROOM_SIZE - j - 1] = room->tiles[j][ROOM_SIZE - 1 -i];
            room->tiles[j][ROOM_SIZE - 1 - i] = room->tiles[i][j];
            room->tiles[i][j] = temp;
        }
    }

    // Update Start Position
    for (int i = 0; i < ROOM_SIZE; i++) {
        for (int j = 0; j < ROOM_SIZE; j++) {
            if (room->tiles[i][j] == START) {
                room->start.x = j;
                room->start.y = i;
            }
        }
    }

    room->rotations = ((room->rotations + 1) > 3) ? 0 : room->rotations + 1;
}

int LoadRoom(Room* dyn_room, int room_num) {
    // Bounds check for desired room_num
    if (room_num < 0) room_num = 0;
    if (room_num >= NUM_ROOMS) room_num = (NUM_ROOMS - 1);

    // Copy tile data from static rooms
    for (int i = 0; i < ROOM_SIZE; i++) {
            for (int j = 0; j < ROOM_SIZE; j++) {
                dyn_room->tiles[i][j] = room_tile[room_num][i][j];
                if (dyn_room->tiles[i][j] == START) {
                    dyn_room->start.x = j;
                    dyn_room->start.y = i;
                }
            }
    }
    // Rotate room to match previous
    for (int r = 0; r < dyn_room->rotations; r++) {
        RotateRoom(dyn_room);
    }
    // Generate backgrounds
    for (int i = 0; i < (ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE); i++) {
        for (int j = 0; j < (ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE); j++) {
            dyn_room->background[i][j] = GetRandomValue(1, 10);
        }
    }

    int next_room = 0;
    if ((room_num + 1) < NUM_ROOMS) {
        next_room = room_num + 1;
    }
    return next_room;
}

