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

/*
 *  TODO: 
 *      - [] Render projectiles
 *      - [] Enemies
 */

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
#include "raylib_game.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const I32 screenWidth = 1024;
static const I32 screenHeight = 768;

static RenderTexture2D target = { 0 };  // Render texture to render our game

// TODO: Define global variables here, recommended to make them static

int frames_counter = 0;
static Texture atlas;
bool show_atlas = true;
Player player = {0};
Spell  spell  = {0};
Enemy  enemy  = {0};

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

    player = (Player){
        .pos = (Vec2){screenWidth/2.0f - TILE_SIZE/2, screenHeight/2.0f - TILE_SIZE/2},
        .speed = 200.0f,
        .current_mana = 0.0f,
        .max_mana = 100.0f,
        .mana_regen = 5.0f,
    };

    spell = (Spell) {
        .cooldown_timer = 0.0f,
        .is_on_cooldown = false,
        .mana_cost      = 25.0f,
    };

    enemy = (Enemy) {
        .pos = (Vec2){screenWidth/1.5f, screenHeight/1.5f},
        .speed = 200.0f,
    };

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
    frames_counter++;

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
        player.flip_texture = FLIP_X;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        input.x += 1;
        player.flip_texture = NO_FLIP;
    }

    input = Vector2Normalize(input);

    player.pos = Vector2Add(player.pos, Vector2Scale(input, player.speed*dt));
    player.pos = Vector2Clamp(player.pos, (Vec2){0, 0}, (Vec2){(F32)screenWidth - TILE_SIZE, (F32)screenHeight - TILE_SIZE});

    player.current_mana += player.mana_regen * dt;
    if (player.current_mana >= player.max_mana) player.current_mana = player.max_mana; 

    if (spell.is_on_cooldown) {
        spell.cooldown_timer -= dt;

        if (spell.cooldown_timer <= 0) {
            spell.is_on_cooldown = false;
            spell.cooldown_timer = 0.0f;
        }
    }

    if (!spell.is_on_cooldown && player.current_mana >= spell.mana_cost) {
        player.current_mana -= spell.mana_cost;

        spell.is_on_cooldown = true;
        spell.cooldown_timer = 5.0;

        TraceLog(LOG_INFO, "Shooting projectile");
    }

    // TODO: shoot projectile in the direction of enemy. 



    // Draw
    //----------------------------------------------------------------------------------
    // Render game screen to a texture,
    // it could be useful for scaling or further shader postprocessing
    BeginTextureMode(target);
        ClearBackground(PAL2);

        // TODO: Draw your game screen here
        Rect src = get_atlas(0,0);
        Rect dst = {player.pos.x, player.pos.y, TILE_SIZE, TILE_SIZE};
        draw_sprite(atlas, src, player.pos, player.flip_texture, WHITE);

    EndTextureMode();

    // Render to screen (main framebuffer)
    BeginDrawing();
        ClearBackground(PAL2);

        // Draw render texture to screen, scaled if required
        DrawTexturePro(target.texture,
            (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height },
            (Rectangle){ 0, 0, (float)target.texture.width, (float)target.texture.height  },
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
            DrawText(TextFormat("MANA: %.2f", player.current_mana), 16, screenHeight-64, 20, PAL4);

            if (spell.is_on_cooldown) {
                const char* text = TextFormat("Cooldown: %.1f", spell.cooldown_timer);
                DrawText(text, 16, screenHeight-32, 20, PAL4);
            } else {
                const char* text = "Ready!";
                DrawText(text, 16, screenHeight-32, 20, PAL7);
            }
        
        }

    EndDrawing();
    //----------------------------------------------------------------------------------
}

Rect get_atlas(int col, int row) {
    return (Rect) {
        (F32) 0 + col * TILE_SIZE_ORIGINAL,
        (F32) 0 + row * TILE_SIZE_ORIGINAL,
        TILE_SIZE_ORIGINAL,
        TILE_SIZE_ORIGINAL,
    };
}

void draw_sprite(Texture2D texture, Rectangle src, Vector2 position, Flip_Texture flip, Color tint)
{
    Rectangle dst = { position.x, position.y, fabsf(src.width)*4, fabsf(src.height)*4 };
    Vector2 origin = { 0.0f, 0.0f };

    switch(flip){
        case NO_FLIP: break;
        case FLIP_X:
            src.width *= -1;
            break;
        case FLIP_Y:
            src.height *= -1;
            break;
        case FLIP_XY:
            src.width *= -1;
            src.height *= -1;
            break;
    }
    DrawTexturePro(texture, src, dst, origin, 0.0f, tint);
}
