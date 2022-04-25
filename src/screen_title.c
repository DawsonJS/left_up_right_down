/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
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

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

Texture2D logo;
Music titleMusic;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    // TODO: Initialize TITLE screen variables here!
    Image logo_image = LoadImage("resources/art/TitleCard.png");
    ImageResizeNN(&logo_image, logo_image.width * (SCALAR / 2), logo_image.height * (SCALAR / 2));
    titleMusic = LoadMusicStream("resources/music/TitleSong.wav");
    logo = LoadTextureFromImage(logo_image);
    PlayMusicStream(titleMusic);
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    // TODO: Update TITLE screen variables here!
    if (!IsMusicStreamPlaying(titleMusic)) {
        PlayMusicStream(titleMusic);
    }
    UpdateMusicStream(titleMusic);

    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_SPACE) || IsGestureDetected(GESTURE_TAP))
    {
        //finishScreen = 1;   // OPTIONS
        finishScreen = 2;   // GAMEPLAY
        PlaySound(fxCoin);
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    DrawTextureRec(logo, (Rectangle){0.0f, 0.0f, logo.width, logo.height}, (Vector2){0.0f, 0.0f}, WHITE);
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // TODO: Unload TITLE screen variables here!
    StopMusicStream(titleMusic);
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    // UnloadMusicStream(titleMusic);
    // UnloadTexture(logo);
    return finishScreen;
}
