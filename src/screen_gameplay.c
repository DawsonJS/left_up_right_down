/********************************************************************************************** *
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/
#include "raylib.h"
#include "screens.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define SCALAR 2

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static double groundedTime = 0.0f;

typedef enum _TileType {
    AIR = 0,
    GROUND = 1,
    STALAGMITE = 2,
    STALACTITE = 3
} TileType;

typedef struct _Tile {
    Texture2D texture;
    TileType type;
} Tile;

typedef struct _Room {
    TileType tiles[8][8];
} Room;

typedef enum _PlayerState {
    IDLE,
    FALL,
    WALKING,
    GROUNDED,
    ROTATING
} PlayerState;

typedef enum _PlayerDirection {
    RIGHT = 1,
    LEFT = -1
} PlayerDirection;

typedef struct _PlayerSheets {
    Texture2D idle;
    Texture2D horizontal;
    Texture2D fall;
    Texture2D grounded;
} PlayerSheets;


typedef struct _Player {
    Vector2 position;
    Vector2 velocity;
    PlayerState state;
    PlayerDirection direction;
    int width;
    int height;
} Player;

PlayerSheets playerSprite;
Player player;
Room room;
Tile ground;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    Image sprite_sheet_images[4] = {LoadImage("resources/art/Miner_Idle-Sheet.png"),
                                    LoadImage("resources/art/Miner_Walk-Sheet.png"),
                                    LoadImage("resources/art/Miner_Fall-Sheet.png"),
                                    LoadImage("resources/art/Miner_Getup-Sheet.png")};
    Image tile_texture_images[3] = {LoadImage("resources/art/Ground_Tiles-Sheet.png"),
                                    LoadImage("resources/art/Stalagmite.png"),
                                    LoadImage("resources/art/Stalactite.png")};
    for (int i = 0; i < 4; i++) {
        ImageResize(&(sprite_sheet_images[i]), sprite_sheet_images[i].width * SCALAR, sprite_sheet_images[i].height * SCALAR);
    }
    for (int i = 0; i < 3; i++) {
        ImageResize(&(tile_texture_images[i]), tile_texture_images[i].width * SCALAR, tile_texture_images[i].height * SCALAR);
    }
    player = (Player) {(Vector2){.x = GetScreenWidth() - 2 * (16 * SCALAR), .y = GetScreenHeight() / 2}, (Vector2){0.0f, 0.0f}, IDLE, RIGHT, 16 * SCALAR, 16 * SCALAR};
    playerSprite = (PlayerSheets){.idle = LoadTextureFromImage(sprite_sheet_images[0]),
                                  .horizontal = LoadTextureFromImage(sprite_sheet_images[1]),
                                  .fall = LoadTextureFromImage(sprite_sheet_images[2]),
                                  .grounded = LoadTextureFromImage(sprite_sheet_images[3])};
    ground = (Tile){.texture = LoadTextureFromImage(tile_texture_images[0]), .type = GROUND};
    room = (Room){.tiles = {  1, 1, 1, 1, 1, 1, 1, 1,
                        1, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 0, 0, 0, 1, 1, 1,
                        1, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 0, 0, 0, 0, 0, 1,
                        1, 0, 0, 0, 0, 0, 0, 1,
                        1, 1, 1, 1, 1, 1, 1, 1}};
    framesCounter = 0;
    finishScreen = 0;
    SetTargetFPS(60);
    for (int i = 0; i < 4; i++) {
        UnloadImage(sprite_sheet_images[i]);
    }
    for (int i = 0; i < 3; i++) {
        UnloadImage(tile_texture_images[i]);
    }
    srand(time(NULL));
}

bool CheckCollisionY(void) {
    int left_tile = player.position.x / 32;
    int right_tile = (player.position.x / 32) + 1;
    int bottom_tile = (player.position.y / 32)+ 1;

    if(left_tile < 0) left_tile = 0;
    if(right_tile > 7) right_tile = 7;
    if(bottom_tile > 7) bottom_tile = 7;

    bool any_collision = false;
    for(int j = left_tile; j <= right_tile; j++)
    {
        TileType t = room.tiles[bottom_tile][j];
        if (t == GROUND && ((player.position.x < j * 32) || ((player.position.x + player.width) > j * 32))) {
            any_collision = true;
            if ((player.position.y + player.height) > (bottom_tile * 32)) {
                player.state = GROUNDED;
                groundedTime = GetTime();
                player.velocity.y = 0.0f;
                player.position.y = (bottom_tile - 1) * 32;
            }
        } else {
            player.state = FALL;
        }
    }
    return any_collision;
}

bool CheckCollisionX(void) {
    int left_tile = player.position.x / 32;
    int right_tile = (player.position.x / 32) + 1;
    int top_tile = player.position.y / 32;

    if(left_tile < 0) left_tile = 0;
    if(right_tile > 7) right_tile = 7;
    if(top_tile < 0) top_tile = 0;

    bool any_collision = false;
    for(int j = left_tile; j <= right_tile; j++)
    {
        TileType t = room.tiles[top_tile][j];
        if (t == GROUND) {
            any_collision = true;
        }
    }
    return any_collision;
}

void RotateRoom(void) {
    Vector2 oldposition = player.position;
    float cosine = cos(90.0 * PI / 180.0);
    float sine = sin(90.0 * PI / 180.0);

    TileType old_tiles[8][8];

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            old_tiles[i][j] = room.tiles[i][j];
            room.tiles[i][j] = AIR;
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int new_row = floor(((float)j - 3.5) * sine + ((float)i - 3.5) * cosine + 3.5);
            int new_col = floor(((float)j - 3.5) * cosine - ((float)i - 3.5) * sine + 3.5);
            room.tiles[new_row][new_col] = old_tiles[i][j];
        }
    }

    player.position.x = (oldposition.x - (GetScreenWidth() / 2)) * cos((float)(90 * PI / 180)) - (oldposition.y - (GetScreenHeight() / 2)) * sin((float)(90 * PI / 180)) + (GetScreenWidth() / 2);
    player.position.y = (oldposition.x - (GetScreenWidth() / 2)) * sin((float)(90 * PI / 180)) + (oldposition.y - (GetScreenHeight() / 2)) * cos((float)(90 * PI / 180)) + (GetScreenHeight() / 2);
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    player.position.x += player.velocity.x;
    if (CheckCollisionX()) {
        player.position.x -= player.velocity.x;
    }
    player.position.y += player.velocity.y;
    if (CheckCollisionY()) {
        player.position.y -= player.velocity.y;
    }
    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
    if (IsKeyDown(KEY_LEFT)) {
        if (player.state == IDLE) player.state = WALKING;
        player.direction = LEFT;
        player.velocity.x = -2.0f;
    } else if (IsKeyDown(KEY_RIGHT)) {
        if (player.state == IDLE) player.state = WALKING;
        player.direction = RIGHT;
        player.velocity.x = 2.0f;
    } else {
        if (player.state == WALKING) player.state = IDLE;
        player.velocity.x = 0.0f;
    }

    if (player.state == FALL) player.velocity.y += 9.8f * GetFrameTime();
    
    if (IsKeyPressed(KEY_R)) {
        RotateRoom();
    }

    if (IsKeyPressed(KEY_B)) // Reset physics input
    {
        player = (Player) {(Vector2){.x = GetScreenWidth() - 2 * (16 * SCALAR), .y = GetScreenHeight() / 2}, (Vector2){-0.1f, 0.0f}, IDLE, RIGHT, 16 * SCALAR, 16 * SCALAR};
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    ClearBackground(BLACK);
    static int frame = 0;
    if (frame >= 40 && player.state != GROUNDED) frame = 0;

    DrawFPS(GetScreenWidth() - 90, GetScreenHeight() - 30);
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(player.position.x, player.position.y, player.width, player.height, RED);
    switch (player.state) {
        case IDLE:
            DrawTextureRec(playerSprite.idle, 
                (Rectangle){(float)(playerSprite.idle.width / 5) * (frame / 8),
                0.0f, player.direction * (float)(playerSprite.idle.width / 5), 
                (float)(playerSprite.idle.height)}, player.position, WHITE);
            break;
        case WALKING:
            DrawTextureRec(playerSprite.horizontal, 
                (Rectangle){(float)(playerSprite.horizontal.width / 8) * (frame / 5),
                0.0f, player.direction * (float)(playerSprite.horizontal.width / 8), 
                (float)(playerSprite.horizontal.height)}, player.position, WHITE);
            break;
        case FALL:
            DrawTextureRec(playerSprite.fall, 
                (Rectangle){(float)(playerSprite.fall.width / 4) * (frame / 10),
                0.0f, player.direction * (float)(playerSprite.fall.width / 4), 
                (float)(playerSprite.fall.height)}, player.position, WHITE);
            break;
        case GROUNDED:
            if (frame >= 4) frame = 0;
            DrawTextureRec(playerSprite.grounded, 
                (Rectangle){(float)(playerSprite.grounded.width / 4) * frame,
                0.0f, player.direction * (float)(playerSprite.grounded.width / 4), 
                (float)(playerSprite.grounded.height)}, player.position, WHITE);
            if (GetTime() - groundedTime >= 0.25 || frame == 0) {
                groundedTime = GetTime();
                frame++;
            }
            if (frame > 3) {
                frame = 0;
                player.state = IDLE;
            }
            break;
        case ROTATING:
            DrawTextureRec(playerSprite.idle, 
                (Rectangle){0.0f,
                0.0f, player.direction * (float)(playerSprite.idle.width / 5), 
                (float)(playerSprite.idle.height)}, player.position, WHITE);
            break;
    }
    if (player.state != GROUNDED) frame++;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            switch (room.tiles[i][j]) {
                case GROUND:
                    DrawTextureRec(ground.texture, 
                        (Rectangle){(float)(ground.texture.width / 15) * (rand() % 15),
                        0.0f, (float)(ground.texture.width / 15), (float)(ground.texture.height)}, (Vector2){.x = j * 32, .y = i * 32}, WHITE);
                     break;
                case STALAGMITE:
                    break;
                case STALACTITE:
                    break;
             }
            if (j == (int)(player.position.x / 32) && i == (int)(player.position.y / 32)) {
                DrawRectangleLines(j * 32, i * 32, 32, 32, RED);
            } else if (j == (int)((player.position.x / 32) + 1) && i == (int)((player.position.y / 32) + 1)) {
                DrawRectangleLines(j * 32, i * 32, 32, 32, RED);
            } else if (j == (int)(player.position.x / 32) && i == (int)((player.position.y / 32) + 1)) {
                DrawRectangleLines(j * 32, i * 32, 32, 32, RED);
            } else if (j == (int)((player.position.x / 32) + 1) && i == (int)(player.position.y / 32)) {
                DrawRectangleLines(j * 32, i * 32, 32, 32, RED);
            } else {
                DrawRectangleLines(j * 32, i * 32, 32, 32, WHITE);
            }
        }
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}
