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
 *          - [] Add projectile to spell struct
 *          - [] Put projectiles inside dynamic array
 *
 *      - [] Enemies
 *          - [x] Put enemies inside dynamic array
 *          - [x] Find algorithm to space out enemies
 *      - [] Web build
 *          - [] modify build script to use emcc and compile locally
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
#include "atlas.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const I32 screenWidth = 675;
static const I32 screenHeight = 900;

static RenderTexture2D target = { 0 };  // Render texture to render our game

// TODO: Define global variables here, recommended to make them static

int frames_counter = 0;
static Texture atlas;
bool show_atlas = true;

Player player = {0};
Spell  spell  = {0};
Apprentice apprentice = {0};

Enemy *enemies = NULL;

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
    Image atlas_image = {0};

    atlas_image.format = ATLAS_FORMAT;
    atlas_image.width = ATLAS_WIDTH;
    atlas_image.height = ATLAS_HEIGHT;
    atlas_image.mipmaps = 1;
    atlas_image.data = ATLAS_DATA;

    atlas = LoadTextureFromImage(atlas_image);

    player = (Player){
        .pos = (Vec2){screenWidth/2.0f - TILE_SIZE/2, screenHeight/2.0f - TILE_SIZE/2},
        .speed = 200.0f,

        .health = 100.0f,
        .is_invincible = false,
        .invincibility_timer = 5.0f, 

        .current_mana = 0.0f,
        .max_mana = 100.0f,
        .mana_regen = 5.0f,
    };

    apprentice = (Apprentice) {
        .pos = (Vec2){screenWidth/2.0f - TILE_SIZE/2 - 30, screenHeight/2.0f - TILE_SIZE/2 - 30},
        .speed = 150.0f,
        .following_player = false,
    };

    spell = (Spell) {
        .cooldown_timer = 0.0f,
        .is_on_cooldown = false,
        .mana_cost      = 25.0f,
    };

    int number_of_enemies = 10;
    for (int i = 0; i < number_of_enemies; i++) {
        F32 angle = (2.0f * PI * i) / number_of_enemies;
        F32 radius = 500.0f;
        Enemy enemy = {
            .pos = (Vec2){
                (F32)screenWidth/2  + radius * cosf(angle),
                (F32)screenHeight/2 + radius * sinf(angle)},
            .speed = 50.0f,
        };
        arrput(enemies, enemy);
    }

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

    arrfree(enemies);

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

    // PLAYER

    if (IsKeyPressed(KEY_E)) {
        player.current_spell = (player.current_spell + 1) % SPELL_COUNT;
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

    player.invincibility_timer -= dt;
    if (player.invincibility_timer <= 0) player.is_invincible = false;

    player.current_mana += player.mana_regen * dt;
    if (player.current_mana >= player.max_mana) player.current_mana = player.max_mana;

    if (spell.is_on_cooldown) {
        spell.cooldown_timer -= dt;

        if (spell.cooldown_timer <= 0) {
            spell.is_on_cooldown = false;
            spell.cooldown_timer = 0.0f;
        }
    }

    // PLAYER SPELL

    if (!spell.is_on_cooldown && player.current_mana >= spell.mana_cost) {
        player.current_mana -= spell.mana_cost;

        spell.is_on_cooldown = true;
        spell.cooldown_timer = 5.0;

        TraceLog(LOG_INFO, "Shooting projectile");
    }

    // ENEMIES
    // TODO: shoot projectile in the direction of enemy.

    for (int i = 0; i < arrlen(enemies); i++) {
        Vec2 separation = {0, 0};
        int  neighbours = 0;

        // calculate separation force
        for (int j = 0; j < arrlen(enemies); j++) {
            if (i==j) continue;

            F32 distance = Vector2Distance(enemies[i].pos, enemies[j].pos);

            if (distance < TILE_SIZE) {
                Vec2 diff = Vector2Normalize(Vector2Subtract(enemies[i].pos, enemies[j].pos));
                separation = Vector2Add(separation, Vector2Scale(diff, TILE_SIZE/1.5/distance));
                neighbours++;
            }
        }

        // average and limit separation force
        if (neighbours > 0) {
            separation = Vector2Scale(separation, 1.0f/neighbours);
            F32 min_force = enemies[i].speed * 0.0005f;
            F32 max_force = enemies[i].speed * 0.5f;

            separation = Vector2ClampValue(separation, min_force, max_force);
        }

        Enemy* enemy = &enemies[i];
        Vec2 enemy_to_player_dist_vec = Vector2Subtract(player.pos, enemy->pos);
        Vec2 enemy_to_player_vel      = Vector2Add(Vector2Normalize(enemy_to_player_dist_vec), separation);
        F32  enemy_to_player_dist     = Vector2Distance(SPRITE_CENTER(player.pos), SPRITE_CENTER(enemy->pos));
        // TODO: add enemy struct field for distance to player comparison
        if (enemy_to_player_dist > TILE_SIZE) {
            enemy->pos = Vector2Add(enemy->pos, Vector2Scale(enemy_to_player_vel, enemy->speed*dt));
        }

        if (enemy_to_player_dist_vec.x < 0) {
            enemy->flip_texture = FLIP_X;
        } else {
            enemy->flip_texture = NO_FLIP;
        }

        Rect player_rect = (Rect){player.pos.x, player.pos.y, TILE_SIZE, TILE_SIZE};
        Rect enemy_rect  = (Rect){enemy->pos.x, enemy->pos.y, TILE_SIZE, TILE_SIZE};

        if (!player.is_invincible && CheckCollisionRecs(player_rect, enemy_rect)) {
            player.health -= 1;
            player.is_invincible = true;
            player.invincibility_timer = 0.2f;
        }
    }

    // Apprentice

    if (IsKeyPressed(KEY_Q)) {
        apprentice.following_player = !apprentice.following_player;
    }

    Vec2 appr_to_player_diff = Vector2Subtract(player.pos, apprentice.pos);
    if (apprentice.following_player) {
        Vec2 appr_to_player_vel  = Vector2Normalize(appr_to_player_diff);
        F32  appr_to_player_dist = Vector2Distance(SPRITE_CENTER(player.pos), SPRITE_CENTER(apprentice.pos));

        if (appr_to_player_dist > TILE_SIZE*1.5f) {
            apprentice.pos = Vector2Add(apprentice.pos, Vector2Scale(appr_to_player_vel, apprentice.speed*dt));
        }
    }

    if (appr_to_player_diff.x < 0) {
        apprentice.flip_texture = FLIP_X;
    } else {
        apprentice.flip_texture = NO_FLIP;
    }

    // Draw
    //----------------------------------------------------------------------------------
    // Render game screen to a texture,
    // it could be useful for scaling or further shader postprocessing
    BeginTextureMode(target);
        ClearBackground(PAL2);

        // TODO: Draw your game screen here
        Rect src = get_atlas(0,0);
        draw_sprite(atlas, src, player.pos, player.flip_texture, WHITE);
        src = get_atlas(0,2);
        draw_sprite(atlas, src, apprentice.pos, apprentice.flip_texture, WHITE);

        for (int i=0; i < arrlen(enemies); i++) {
            src = get_atlas(i%4,4);
            draw_sprite(atlas, src, enemies[i].pos, enemies[i].flip_texture, WHITE);
            if (show_atlas) DrawLineV(SPRITE_CENTER(player.pos), SPRITE_CENTER(enemies[i].pos), PAL4);
        }

        Rect player_health_rect = (Rect) {
            player.pos.x, 
            player.pos.y + TILE_SIZE + 10, 
            (player.health/100.0f)*TILE_SIZE, 
            10.0f
        };
        DrawRectangleRec(player_health_rect, PAL4);

        switch (player.current_spell) {
        case NO_SPELL: break;
        case MANA_RAY:
            Vec2 p1 = Vector2Add(SPRITE_CENTER(player.pos), (Vec2){0, 24});
            Vec2 p2 = Vector2Add(SPRITE_CENTER(apprentice.pos), (Vec2){0, 24});
            DrawLineEx(p1, p2, 16, PAL0);
            DrawLineEx(p1, p2, 10, PAL1);
            break;
        case DEATH_RAY:
            p1 = Vector2Add(SPRITE_CENTER(player.pos), (Vec2){0, 24});
            p2 = Vector2Add(SPRITE_CENTER(apprentice.pos), (Vec2){0, 24});
            DrawLineEx(p1, p2, 16, PAL3);
            DrawLineEx(p1, p2, 10, PAL4);

            break;
        }


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
            DrawTextureEx(atlas, (Vec2){16, 64}, 0, 3, WHITE);
            for (I32 i = 0; i < ARRAY_LEN(Color_Palette); i++ ) {
                Vec2 pos = {16.0f + i*32, 32.0f};
                Vec2 size = {32, 32};
                DrawRectangleV(pos, size, Color_Palette[i]);
                Color color = i == 5 ? PAL0 : PAL5;

                DrawText(TextFormat("%d", i), (int)pos.x + 4, (int)pos.y + 4, 24, color);
            }

            if (spell.is_on_cooldown) {
                const char* text = TextFormat("Cooldown: %.1f", spell.cooldown_timer);
                DrawText(text, 16, screenHeight-20, 20, PAL4);
            } else {
                const char* text = "Ready!";
                DrawText(text, 16, screenHeight-20, 20, PAL7);
            }
            DrawText(TextFormat("MANA: %.2f", player.current_mana), 16, screenHeight-40, 20, PAL4);
            DrawText(TextFormat("dt: %f", GetFrameTime()), 16, screenHeight-60, 20, PAL4);
            DrawText(TextFormat("Spell: %i", player.current_spell), 16, screenHeight-80, 20, PAL4);

            DrawFPS(10,10);

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
