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
#include "rooms.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>


//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static double groundedTime = 0.0f;
static int rotations = 0;
static int next_room = 0;

void LoadRoom(int room_num);

#define MAX_OXYGEN 100
#define BACKGROUND_SIZE 64


typedef struct _Room {
    TileType tiles[ROOM_SIZE][ROOM_SIZE];
    Vector2 start;
    int background[ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE][ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE];
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
    float oxygen;
} Player;

PlayerSheets playerSprite;
Player player;
Room room;
Tile ground;
Tile stalagmite;
Tile stalactite;
Tile rail;

Texture2D oxygen_bar;
Texture2D background;

Music GameMusic;

Sound groundedSound;
Sound rotatingSound;
Sound fallingSound;
Sound deathSound;

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
                                    LoadImage("resources/art/Stalagmite_Rotate-Sheet.png"),
                                    LoadImage("resources/art/Stalactite_Rotate-Sheet.png")};

    Image oxygen_bar_image = LoadImage("resources/art/Oxygen_Bar-Sheet.png");
    Image background_image = LoadImage("resources/art/Backgrounds-Sheet.png");
    for (int i = 0; i < 4; i++) {
        ImageResize(&(sprite_sheet_images[i]), sprite_sheet_images[i].width * SCALAR, sprite_sheet_images[i].height * SCALAR);
    }
    for (int i = 0; i < 3; i++) {
        ImageResize(&(tile_texture_images[i]), tile_texture_images[i].width * SCALAR, tile_texture_images[i].height * SCALAR);
    }

    playerSprite = (PlayerSheets){.idle = LoadTextureFromImage(sprite_sheet_images[0]),
                                  .horizontal = LoadTextureFromImage(sprite_sheet_images[1]),
                                  .fall = LoadTextureFromImage(sprite_sheet_images[2]),
                                  .grounded = LoadTextureFromImage(sprite_sheet_images[3])};
    ground = (Tile){.texture = LoadTextureFromImage(tile_texture_images[0]), .type = GROUND};
    stalagmite = (Tile){.texture = LoadTextureFromImage(tile_texture_images[1]), .type = STALAGMITE};
    stalactite = (Tile){.texture = LoadTextureFromImage(tile_texture_images[2]), .type = STALACTITE};
    player = (Player) {(Vector2){.x = 0, .y = 0}, (Vector2){0.0f, 0.0f}, IDLE, RIGHT, 16 * SCALAR, 16 * SCALAR, MAX_OXYGEN};
    oxygen_bar = LoadTextureFromImage(oxygen_bar_image);
    background = LoadTextureFromImage(background_image);
    LoadRoom(next_room);
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
    GameMusic = LoadMusicStream("resources/music/Gameplay-Music.wav");
    PlayMusicStream(GameMusic);

    groundedSound = LoadSound("resources/music/GroundedSound.wav");
    rotatingSound = LoadSound("resources/music/RotatingSound.wav");
    deathSound = LoadSound("resources/music/DeathSound.wav");
    fallingSound = LoadSound("resources/music/fallingSound.wav");

    SetSoundVolume(groundedSound, 0.4);
    SetSoundVolume(rotatingSound, 0.4);
    SetSoundVolume(deathSound, 0.4);
    SetSoundVolume(fallingSound, 0.4);

    SetMusicVolume(GameMusic, 0.30);
}

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

void PlayerDeath() {
    LoadRoom(next_room - 1);
}
bool CheckCollisionY(void) {
    int left_tile = player.position.x / TILE_SIZE;
    int right_tile = (player.position.x / TILE_SIZE) + 1;
    int bottom_tile = (player.position.y / TILE_SIZE)+ 1;

    if(left_tile < 0) left_tile = 0;
    if(right_tile > ROOM_SIZE) right_tile = ROOM_SIZE;
    if(bottom_tile > ROOM_SIZE) bottom_tile = ROOM_SIZE;

    bool any_collision = false;
    if (room.tiles[bottom_tile][left_tile] == EXIT || room.tiles[bottom_tile][right_tile] == EXIT) {
        LoadRoom(next_room);
        return false;
    }
    if (IsDeath(room.tiles[bottom_tile][left_tile]) || IsDeath(room.tiles[bottom_tile][right_tile])) {
        if (!IsSoundPlaying(deathSound)) {
            PlaySound(deathSound);
        }
        PlayerDeath();
        return false;
    }
    for(int j = left_tile; j <= right_tile; j++)
    {
        TileType t = room.tiles[bottom_tile][j];
        if (IsSolid(t) && ((player.position.x < j * TILE_SIZE) || ((player.position.x + player.width) > j * TILE_SIZE))) {
            any_collision = true;
            if ((player.position.y + player.height) > (bottom_tile * TILE_SIZE)) {
                if (!IsSoundPlaying(groundedSound)) {
                    PlaySound(groundedSound);
                }
                player.state = GROUNDED;
                groundedTime = GetTime();
                player.velocity.y = 0.0f;
                player.position.y = (bottom_tile - 1) * TILE_SIZE;
            }
        }
    }
    if (!any_collision) {
        if (!IsSoundPlaying(fallingSound)) {
            PlaySound(fallingSound);
        }
        player.state = FALL;
    }
    return any_collision;
}

bool CheckCollisionX(void) {
    int left_tile = player.position.x / TILE_SIZE;
    int right_tile = (player.position.x / TILE_SIZE) + 1;
    int top_tile = player.position.y / TILE_SIZE;

    if(left_tile < 0) left_tile = 0;
    if(right_tile > ROOM_SIZE) right_tile = ROOM_SIZE;
    if(top_tile < 0) top_tile = 0;

    bool any_collision = false;
    if (room.tiles[top_tile][right_tile] == EXIT) {
        LoadRoom(next_room);
        return false;
    }
    if (IsDeath(room.tiles[top_tile][right_tile])) {
        PlayerDeath();
        return false;
    }
    for(int j = left_tile; j <= right_tile; j++)
    {
        TileType t = room.tiles[top_tile][j];
        if (IsSolid(t)) {
            if (player.state == ROTATING) {
                player.velocity.x = (j == left_tile) ? 0.1f : -0.1f;
            }
            any_collision = true;
        }
    }
    return any_collision;
}

void RotateRoom(void) {
    if (!IsSoundPlaying(rotatingSound)) {
        PlaySound(rotatingSound);
    }
    Vector2 oldposition = player.position;
    float cosine = cos(90.0 * PI / 180.0);
    float sine = sin(90.0 * PI / 180.0);

    for (int i = 0; i < (ROOM_SIZE + 1) / 2; i ++) {
        for (int j = 0; j < ROOM_SIZE / 2; j++) {
            TileType temp = room.tiles[ROOM_SIZE - 1 - j][i];
            room.tiles[ROOM_SIZE - 1 - j][i] = room.tiles[ROOM_SIZE - 1 - i][ROOM_SIZE - j - 1];
            room.tiles[ROOM_SIZE - 1 - i][ROOM_SIZE - j - 1] = room.tiles[j][ROOM_SIZE - 1 -i];
            room.tiles[j][ROOM_SIZE - 1 - i] = room.tiles[i][j];
            room.tiles[i][j] = temp;
        }
    }

    Vector2 center = (Vector2){.x = (float)(ROOM_SIZE / 2) * TILE_SIZE, .y = (float)(ROOM_SIZE / 2) * TILE_SIZE};

    player.position.x = (oldposition.x - center.x) * cosine - (oldposition.y - center.y) * sine + (center.x);
    player.position.y = (oldposition.x - center.x) * sine + (oldposition.y - center.y) * cosine + (center.y);
    
    rotations++;
    if (rotations > 3) {
        rotations = 0;
    }


}

void LoadRoom(int room_num) {
    if (room_num < 0) room_num = 0;
    if (room_num >= NUM_ROOMS) room_num = (NUM_ROOMS - 1);
    for (int i = 0; i < ROOM_SIZE; i++) {
            for (int j = 0; j < ROOM_SIZE; j++) {
                room.tiles[i][j] = room_tile[room_num][i][j];
                if (room.tiles[i][j] == START) {
                    room.start.x = j;
                    room.start.y = i;
                }
            }
    }
    for (int r = 0; r < rotations; r++) {
        for (int i = 0; i < (ROOM_SIZE + 1) / 2; i ++) {
                for (int j = 0; j < ROOM_SIZE / 2; j++) {
                    TileType temp = room.tiles[ROOM_SIZE - 1 - j][i];
                    room.tiles[ROOM_SIZE - 1 - j][i] = room.tiles[ROOM_SIZE - 1 - i][ROOM_SIZE - j - 1];
                    room.tiles[ROOM_SIZE - 1 - i][ROOM_SIZE - j - 1] = room.tiles[j][ROOM_SIZE - 1 -i];
                    room.tiles[j][ROOM_SIZE - 1 - i] = room.tiles[i][j];
                    room.tiles[i][j] = temp;
                    if (room.tiles[i][j] == START) {
                        room.start.x = j;
                        room.start.y = i;
                    }
                }
        }
    }

    for (int i = 0; i < (ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE); i++) {
        for (int j = 0; j < (ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE); j++) {
            room.background[i][j] = GetRandomValue(1, 10);
        }
    }

    player.position.x = room.start.x * TILE_SIZE;
    player.position.y = room.start.y * TILE_SIZE;
    player.oxygen = MAX_OXYGEN;
    next_room++;
    if (next_room >= NUM_ROOMS) next_room = 0;
}


// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    if (!IsMusicStreamPlaying(GameMusic)) {
        PlayMusicStream(GameMusic);
    }
    UpdateMusicStream(GameMusic);
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
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        if (player.state == IDLE) player.state = WALKING;
        player.direction = LEFT;
        player.velocity.x = -2.0f;
    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
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

    player.oxygen = player.oxygen - GetFrameTime() * 10;
    if (player.oxygen <= 0) {
        PlayerDeath();
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    ClearBackground(BLACK);
    for (int i = 0; i < (ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE); i++) {
        for (int j = 0; j < (ROOM_SIZE * TILE_SIZE / BACKGROUND_SIZE); j++) {
            DrawTextureRec(background, 
                            (Rectangle){(float)(BACKGROUND_SIZE) * (room.background[i][j]),
                            0.0f, (float)(BACKGROUND_SIZE), 
                            (float)(BACKGROUND_SIZE)}, (Vector2){.x = j * BACKGROUND_SIZE, .y = i * BACKGROUND_SIZE}, WHITE);
        }
    }
                            
             
    static int frame = 0;
    if (frame >= 40 && player.state != GROUNDED) frame = 0;

    // DrawFPS(GetScreenWidth() - 90, GetScreenHeight() - 30);
    // TODO: Draw GAMEPLAY screen here!
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
    for (int i = 0; i < ROOM_SIZE; i++) {
        for (int j = 0; j < ROOM_SIZE; j++) {
            switch (room.tiles[i][j]) {
                case GROUND:
                    DrawTextureRec(ground.texture, 
                        (Rectangle){(float)(ground.texture.width / 15) * (i + j),
                        0.0f, (float)(ground.texture.width / 15), (float)(ground.texture.height)},
                        (Vector2){.x = j * TILE_SIZE, .y = i * TILE_SIZE}, WHITE);
                     break;
                case STALAGMITE:
                    DrawTextureRec(stalagmite.texture,
                            (Rectangle){(float)(stalagmite.texture.width / 16) * ((i + j) % 4) + rotations * (stalagmite.texture.width / 4),
                            0.0f, (float)(stalagmite.texture.width / 16), (float)(stalagmite.texture.height)},
                            (Vector2){.x = j * TILE_SIZE, .y = i * TILE_SIZE}, WHITE);
                    /* DrawTexturePro(stalagmite.texture, (Rectangle){(float)(stalagmite.texture.width / 4) * ((i + j) % 4), 0.0f,
                            (float)(stalagmite.texture.width / 4), (float)(stalagmite.texture.height)}, (Rectangle){j * TILE_SIZE,
                            i * TILE_SIZE, stalagmite.texture.width, stalagmite.texture.height},
                            (Vector2){.x = (j * TILE_SIZE + (j * TILE_SIZE + TILE_SIZE)) / 2,
                            .y = (i * TILE_SIZE + (i * TILE_SIZE + TILE_SIZE)) / 2}, 90.0 * (rotations), WHITE); */
                    break;
                case STALACTITE:
                    DrawTextureRec(stalactite.texture,
                            (Rectangle){(float)(stalactite.texture.width / 4) * rotations,
                            0.0f, (float)(stalactite.texture.width / 4), (float)(stalactite.texture.height)},
                            (Vector2){.x = j * TILE_SIZE, .y = i * TILE_SIZE}, WHITE);
                    break;
            }
        }
    }
    DrawTextureRec(oxygen_bar, (Rectangle){0.0f, 0.0f, (float)(oxygen_bar.width), (float)(oxygen_bar.height)}, (Vector2){.x = 0.0f, .y = 0.0f}, WHITE);
    DrawRectangleRec((Rectangle){player.oxygen / MAX_OXYGEN * oxygen_bar.width, TILE_SIZE / 4 + 2, oxygen_bar.width - (player.oxygen / MAX_OXYGEN * oxygen_bar.width), TILE_SIZE / 2}, RED);
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
