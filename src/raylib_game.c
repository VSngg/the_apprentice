/*******************************************************************************************
*
*   raylib gamejam template
*
*   Template originally created with raylib 4.5-dev, last time updated with raylib 5.0
*
*   Template licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#if defined(PLATFORM_WEB)
    #define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
    #include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif

#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for: 

#include "core.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    SCREEN_LOGO = 0, 
    SCREEN_TITLE, 
    SCREEN_GAMEPLAY, 
    SCREEN_ENDING
} GameScreen;

// TODO: Define your custom data types here

typedef Vector2 Vec2;
typedef Rectangle Rect;

typedef struct Player {
    Vec2 pos;
    F32  speed;
} Player;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const I32 screenWidth = 1200;
static const I32 screenHeight = 600;

static RenderTexture2D target = { 0 };  // Render texture to render our game

// TODO: Define global variables here, recommended to make them static

static Color Color_Palette[8] = {
    {  73,  84,  53, 255 },
    { 138, 142,  72, 255 },
    { 222, 191, 137, 255 },
    { 164, 101,  62, 255 },
    { 144,  46,  41, 255 },
    {  36,  23,  27, 255 },
    {  93,  69,  62, 255 },
    { 144, 124, 104, 255 },
};

#define PAL0 Color_Palette[0]
#define PAL1 Color_Palette[1]
#define PAL2 Color_Palette[2]
#define PAL3 Color_Palette[3]
#define PAL4 Color_Palette[4]
#define PAL5 Color_Palette[5]
#define PAL6 Color_Palette[6]
#define PAL7 Color_Palette[7]

#define TILE_SIZE 64

static Texture atlas;
bool show_atlas = true;
Player player = {0};

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame
Rect get_atlas(int row, int col);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);         // Disable raylib trace log messages
#endif

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "The Apprentice");
    
    // TODO: Load resources / Initialize variables at this point
    Image atlas_image = LoadImage("src/resources/atlas.png");
    atlas = LoadTextureFromImage(atlas_image); 
    UnloadImage(atlas_image);

    player.pos = (Vec2){screenWidth/2.0f - TILE_SIZE/2, screenHeight/2.0f - TILE_SIZE/2};
    player.speed = 200.0f;

    // Render texture to draw full screen, enables screen scaling
    // NOTE: If screen is scaled, mouse input should be scaled proportionally
    target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);     // Set our game frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);
    UnloadTexture(atlas);
    
    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------------
// Module functions definition
//--------------------------------------------------------------------------------------------
// Update and draw frame
void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update variables / Implement example logic at this point
    //----------------------------------------------------------------------------------

    F32 dt = GetFrameTime();

    if (IsKeyPressed(KEY_TAB)) {
        const char* text = show_atlas ? "Hiding atlas" : "Showing atlas";
        TraceLog(LOG_INFO, text);
        show_atlas = !show_atlas;
    }

    Vec2 input = {0};

    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        input.y += 1;
    }
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        input.y -= 1;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        input.x -= 1;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        input.x += 1;
    }

    input = Vector2Normalize(input);

    player.pos = Vector2Add(player.pos, Vector2Scale(input, player.speed*dt));

    // Draw
    //----------------------------------------------------------------------------------
    // Render game screen to a texture, 
    // it could be useful for scaling or further shader postprocessing
    BeginTextureMode(target);
        ClearBackground(PAL2);
        
        // TODO: Draw your game screen here
        // DrawText("Welcome to raylib NEXT gamejam!", 150, 140, 30, BLACK);
        // DrawRectangleLinesEx((Rectangle){ 0, 0, screenWidth, screenHeight }, 16, PAL5);

        Rect src = get_atlas(0,0);
        Rect dst = {player.pos.x, player.pos.y, TILE_SIZE, TILE_SIZE};
        DrawTexturePro(atlas, src, dst, (Vec2){0, 0}, 0, WHITE);

    EndTextureMode();
    
    // Render to screen (main framebuffer)
    BeginDrawing();
        ClearBackground(PAL2);

        // Draw render texture to screen, scaled if required
        DrawTexturePro(target.texture, 
            (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, 
            (Rectangle){ 0, 0, (float)target.texture.width, (float)target.texture.height }, 
            (Vector2){ 0, 0 }, 
            0.0f, 
            WHITE);

        // TODO: Draw everything that requires to be drawn at this point, maybe UI?
        if (show_atlas) {
            DrawTextureEx(atlas, (Vec2){16, 48}, 0, 4, WHITE);
            for (I32 i = 0; i < ARRAY_LEN(Color_Palette); i++ ) {
                Vec2 pos = {16.0f + i*32, 16.0f};
                Vec2 size = {32, 32};
                DrawRectangleV(pos, size, Color_Palette[i]);
                Color color = i == 5 ? PAL0 : PAL5;

                DrawText(TextFormat("%d", i), (int)pos.x + 4, (int)pos.y + 4, 24, color);
            }
        }

    EndDrawing();
    //----------------------------------------------------------------------------------  
}

Rect get_atlas(int col, int row) {
    return (Rect) {
        (F32) 0 + col * 16,
        (F32) 0 + row * 16,
        16,
        16,
    };
}