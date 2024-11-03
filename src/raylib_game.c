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
static const I32 screenWidth = 720;
static const I32 screenHeight = 960;
static const F32 map_width = 30*TILE_SIZE;
static const F32 map_height = 30*TILE_SIZE;

static RenderTexture2D target = { 0 };  // Render texture to render our game

// TODO: Define global variables here, recommended to make them static

int frames_counter = 0;
static Texture atlas;
bool gameplay_paused = false;
bool should_draw_debug_ui = false;

Music music = {0};

Player player = {0};
Apprentice apprentice = {0};

Enemy *enemies = NULL;

Camera2D camera = {0};

bool touch_active = false;
Vec2 touch_start = {0};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
// #if !defined(_DEBUG)
//     SetTraceLogLevel(LOG_NONE);         // Disable raylib trace log messages
// #endif

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "The Apprentice");
    SetExitKey(0);

    InitAudioDevice();
    music = LoadMusicStream("resources/vandalorum-folly_of_man.wav");
    PlayMusicStream(music);

    // TODO: Load resources / Initialize variables at this point
    Image atlas_image = {0};

    atlas_image.format = ATLAS_FORMAT;
    atlas_image.width = ATLAS_WIDTH;
    atlas_image.height = ATLAS_HEIGHT;
    atlas_image.mipmaps = 1;
    atlas_image.data = ATLAS_DATA;

    atlas = LoadTextureFromImage(atlas_image);

    player = (Player){
        .pos = (Vec2){map_height/2.0f - TILE_SIZE/2, map_width/2.0f - TILE_SIZE/2},
        .speed = TILE_SIZE*3,

        .health = 100.0f,
        .max_health = 100.0f,
        .is_invincible = false,
        .invincibility_timer = 5.0f, 

        .mana = 100.0f,
        .max_mana = 100.0f,
        .mana_regen = 10.0f,
    };

    apprentice = (Apprentice) {
        .pos = (Vec2){map_width/2.0f - TILE_SIZE/2 - 30, map_height/2.0f - TILE_SIZE/2 - 30},
        .speed = TILE_SIZE*2.8f,
        .following_player = true,

        .health = 100.0f,
        .max_health = 100.0f,
        .is_invincible = false,
        .invincibility_timer = 5.0f, 

        .mana = 0.0f,
        .max_mana = 100.0f,
        .mana_regen = 1.0f,
    };

    int number_of_enemies = 10;
    for (int i = 0; i < number_of_enemies; i++) {
        F32 angle = (2.0f * PI * i) / number_of_enemies;
        F32 radius = 500.0f;
        Enemy enemy = {
            .id = i,
            .alive = true,
            .pos = (Vec2){
                (F32)map_width/2  + radius * cosf(angle),
                (F32)map_height/2 + radius * sinf(angle)},
            .speed = TILE_SIZE*1.5f,

            .health = 100.0f,
            .max_health = 100.0f,
        };
        arrput(enemies, enemy);
    }

    camera.target = player.pos;
    camera.offset = (Vec2) {screenWidth/2 - TILE_SIZE/2, screenHeight/2 - TILE_SIZE/2};
    camera.zoom = 1.0f;

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
    UnloadMusicStream(music);
    UnloadRenderTexture(target);
    UnloadTexture(atlas);

    arrfree(enemies);

    // TODO: Unload all loaded resources at this point

    CloseAudioDevice();
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
    UpdateMusicStream(music);

    if (IsKeyPressed(KEY_ESCAPE)) {
        gameplay_paused = !gameplay_paused;
        if (gameplay_paused) PauseMusicStream(music);
        else ResumeMusicStream(music);
    }

    if (!gameplay_paused) update_gameplay();

    // Draw
    //----------------------------------------------------------------------------------
    // Render game screen to a texture,
    // it could be useful for scaling or further shader postprocessing
    BeginTextureMode(target);
        ClearBackground(PAL2);
        BeginMode2D(camera);

        // TODO: Draw your game screen here
        DrawRectangleLinesEx((Rect){0, 0, map_width, map_height}, TILE_SIZE, PAL5);

        Rect src = get_atlas(0,0);
        draw_sprite(atlas, src, player.pos, player.flip_texture, WHITE);

        src = get_atlas(0,2);
        draw_sprite(atlas, src, apprentice.pos, apprentice.flip_texture, WHITE);

        for (int i=0; i < arrlen(enemies); i++) {
            src = get_atlas(enemies[i].id%4,4);

            draw_sprite(atlas, src, enemies[i].pos, enemies[i].flip_texture, WHITE);

            if (should_draw_debug_ui) {
                DrawLineV(SPRITE_CENTER(player.pos), SPRITE_CENTER(enemies[i].pos), PAL4);

                Rect enemy_health_rect = (Rect) {
                    enemies[i].pos.x, 
                    enemies[i].pos.y + TILE_SIZE + 10, 
                    (enemies[i].health/100.0f)*TILE_SIZE, 
                    10.0f
                };
                DrawRectangleRec(enemy_health_rect, PAL4);
                DrawRectangleLinesEx(enemy_health_rect, 2, PAL5);
            }

        }

        if (player.is_casting) {
            F32 ad;
            switch (player.active_spell) {
            case NO_SPELL: break;
            case MANA_RAY:
                ad = SPELLS[MANA_RAY].activation_distance;
                DrawRing(apprentice.ray_anchor, ad-2, ad+2, 0, 360, 48, PAL0);

                DrawCircleV(player.ray_anchor, 8, PAL0);
                DrawCircleV(apprentice.ray_anchor, 8, PAL0);
                DrawLineEx(player.ray_anchor, apprentice.ray_anchor, 16, PAL0);

                DrawCircleV(player.ray_anchor, 5, PAL1);
                DrawCircleV(apprentice.ray_anchor, 5, PAL1);
                DrawLineEx(player.ray_anchor, apprentice.ray_anchor, 10, PAL1);
                break;
            case DEATH_RAY:
                ad = SPELLS[MANA_RAY].activation_distance;
                DrawRing(apprentice.ray_anchor, ad-2, ad+2, 0, 360, 48, PAL3);

                DrawCircleV(player.ray_anchor, 8, PAL3);
                DrawCircleV(apprentice.ray_anchor, 8, PAL3);
                DrawLineEx(player.ray_anchor, apprentice.ray_anchor, 16, PAL3);

                DrawCircleV(player.ray_anchor, 5, PAL4);
                DrawCircleV(apprentice.ray_anchor, 5, PAL4);
                DrawLineEx(player.ray_anchor, apprentice.ray_anchor, 10, PAL4);
                break;            
            }
        }

        Rect player_health_rect = (Rect) {
            player.pos.x, 
            player.pos.y + TILE_SIZE + 8, 
            (player.health/100.0f)*TILE_SIZE, 
            8.0f
        };
        Rect player_mana_rect = (Rect) {
            player.pos.x, 
            player.pos.y + TILE_SIZE + 16, 
            (player.mana/100.0f)*TILE_SIZE, 
            8.0f
        };
        Rect appr_health_rect = (Rect) {
            apprentice.pos.x, 
            apprentice.pos.y + TILE_SIZE + 8, 
            (apprentice.health/100.0f)*TILE_SIZE, 
            8.0f
        };
        Rect appr_mana_rect = (Rect) {
            apprentice.pos.x, 
            apprentice.pos.y + TILE_SIZE + 16, 
            (apprentice.mana/100.0f)*TILE_SIZE, 
            8.0f
        };
        DrawRectangleRec(player_health_rect, PAL4);
        DrawRectangleLinesEx(player_health_rect, 2, PAL5);

        DrawRectangleRec(player_mana_rect, PAL0);
        DrawRectangleLinesEx(player_mana_rect, 2, PAL5);

        DrawRectangleRec(appr_health_rect, PAL4);
        DrawRectangleLinesEx(appr_health_rect, 2, PAL5);
        DrawRectangleRec(appr_mana_rect, PAL0);
        DrawRectangleLinesEx(appr_mana_rect, 2, PAL5);

        EndMode2D();
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
        if (should_draw_debug_ui) draw_debug_ui();

        draw_ui();

    EndDrawing();
    //----------------------------------------------------------------------------------
}

void update_gameplay(void) {
    F32 dt = GetFrameTime();
    frames_counter++;

    if (IsKeyPressed(KEY_TAB)) {
        const char* text = should_draw_debug_ui ? "Hiding atlas" : "Showing atlas";
        TraceLog(LOG_INFO, text);
        should_draw_debug_ui = !should_draw_debug_ui;
    }

    // PLAYER

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

    // Touch controls

    if (GetGestureDetected() == GESTURE_TAP) {
        touch_active = true;
        touch_start = GetTouchPosition(0);
    }

    if (touch_active && GetTouchPointCount() > 0) {
        Vec2 current_touch = GetTouchPosition(0);
        Vec2 direction = Vector2Subtract(current_touch, touch_start);
        
        float min_distance = 10.0f;
        if (Vector2Length(direction) > min_distance) {
            input = Vector2Normalize(direction);
            
            if (input.x < 0) {
                player.flip_texture = FLIP_X;
            } else if (input.x > 0) {
                player.flip_texture = NO_FLIP;
            }
        }
    }

    if (GetTouchPointCount() == 0) {
        touch_active = false;
    }

    input = Vector2Normalize(input);

    player.pos = Vector2Add(player.pos, Vector2Scale(input, player.speed*dt));
    player.pos = Vector2Clamp(player.pos, (Vec2){0, 0}, (Vec2){map_width, map_height});

    camera.target = player.pos;

    player.health = Clamp(player.health, 0.0f, player.max_health);
    player.invincibility_timer -= dt;
    if (player.invincibility_timer <= 0) player.is_invincible = false;

    apprentice.health = Clamp(apprentice.health, 0.0f, apprentice.max_health);
    apprentice.invincibility_timer -= dt;
    if (apprentice.invincibility_timer <= 0) apprentice.is_invincible = false;
    
    player.ray_anchor     = Vector2Add(SPRITE_CENTER(player.pos), (Vec2){0, 24});
    apprentice.ray_anchor = Vector2Add(SPRITE_CENTER(apprentice.pos), (Vec2){0, 24});
    F32 spellray_distance = Vector2Distance(player.ray_anchor, apprentice.ray_anchor);

    if (IsKeyPressed(KEY_Q)) {
        player.active_spell = NO_SPELL; 

        // Try casting if enough mana
        if (player.active_spell != NO_SPELL &&
            player.mana >= SPELLS[player.active_spell].initial_cost &&
            spellray_distance <= SPELLS[player.active_spell].activation_distance) {
            player.is_casting = true;
            player.mana -= SPELLS[player.active_spell].initial_cost;
        } else {
            player.is_casting = false;
        }
    }

    if (IsKeyPressed(KEY_ONE)) {
        player.active_spell = MANA_RAY;
    }
    if (IsKeyPressed(KEY_TWO)) {
        player.active_spell = DEATH_RAY;
    }

    if (player.active_spell != NO_SPELL) {
        // Initial cast
        if (!player.is_casting && 
            player.mana >= SPELLS[player.active_spell].initial_cost &&
            spellray_distance <= SPELLS[player.active_spell].activation_distance) {
            player.is_casting = true;
            player.mana -= SPELLS[player.active_spell].initial_cost;
        }

        // Continue cast
        if (player.is_casting) {
            if (spellray_distance >= SPELLS[player.active_spell].activation_distance) {
                player.is_casting = false;
            }

            F32 mana_cost = SPELLS[player.active_spell].cost_per_second * dt;

            if (player.mana >= mana_cost) {
                player.mana -= mana_cost;
            } else {
                player.is_casting = false;
            }
        }
    }

    if (!player.is_casting) {
        player.mana += player.mana_regen * dt;
        player.mana = Clamp(player.mana, 0.0f, player.max_mana);

        if (player.active_spell != NO_SPELL && 
            player.mana >= SPELLS[player.active_spell].initial_cost &&
            spellray_distance <= SPELLS[player.active_spell].activation_distance) {
            player.is_casting = true;
            player.mana -= SPELLS[player.active_spell].initial_cost;
        }
    }

    // Apprentice

    if (player.is_casting && player.active_spell == MANA_RAY) {
        apprentice.mana += MANA_RAY_MANA_PER_SECOND * dt;
    }
    if (player.is_casting && player.active_spell == DEATH_RAY) {
        apprentice.mana -= SPELLS[DEATH_RAY].cost_per_second * dt;
    }
    if (apprentice.mana <= 0.0f) {
        player.is_casting = false;
    }

    apprentice.mana += apprentice.mana_regen * dt;
    apprentice.mana = Clamp(apprentice.mana, 0.0f, apprentice.max_mana);

    if (IsKeyPressed(KEY_E)) {
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

    // ENEMIES
    // TODO: shoot projectile in the direction of enemy.

    for (int i = 0; i < arrlen(enemies); i++) {
        Enemy* enemy = &enemies[i];
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

        Rect player_rect     = (Rect){player.pos.x, player.pos.y, TILE_SIZE, TILE_SIZE};
        Rect apprentice_rect = (Rect){apprentice.pos.x, apprentice.pos.y, TILE_SIZE, TILE_SIZE};
        Rect enemy_rect      = (Rect){enemy->pos.x, enemy->pos.y, TILE_SIZE, TILE_SIZE};

        if (!player.is_invincible && CheckCollisionRecs(player_rect, enemy_rect)) {
            player.health -= ENEMY_DAMAGE;
            player.is_invincible = true;
            player.invincibility_timer = 0.2f;
        }

        if (!apprentice.is_invincible && CheckCollisionRecs(apprentice_rect, enemy_rect)) {
            apprentice.health -= ENEMY_DAMAGE;
            apprentice.is_invincible = true;
            apprentice.invincibility_timer = 0.5f;
        }

        enemy->health = Clamp(enemy->health, 0.0f, enemy->max_health);
        if (!enemy->alive) arrdel(enemies, i); 

        if (enemy->health == 0.0f) {
            enemy->alive = false;
        }

        if (player.active_spell == DEATH_RAY && player.is_casting) {
            if (CheckCollisionPointLine(SPRITE_CENTER(enemy->pos), player.ray_anchor, apprentice.ray_anchor, 16*3)) {
                enemy->health -= DEATH_RAY_DAMAGE;
            }
        }

        // Burn mana if enemies touch mana ray
        if (player.active_spell == MANA_RAY && player.is_casting) {
            if (CheckCollisionPointLine(SPRITE_CENTER(enemy->pos), player.ray_anchor, apprentice.ray_anchor, 16*3)) {
                player.mana -= ENEMY_MANA_BURN;
            }
        }

    }
}

void draw_ui(void) {
    Rect follow_icon = get_atlas(3,9);
    if (apprentice.following_player) {
        follow_icon = get_atlas(4,9);
    }
    draw_sprite(atlas, follow_icon, (Vec2){screenWidth - 150, screenHeight - 80}, NO_FLIP, WHITE);

    Rect spell_icon_rect = {0};
    switch (player.active_spell) {
    case NO_SPELL: 
        spell_icon_rect = get_atlas(0,9);
        break;
    case MANA_RAY:
        spell_icon_rect = get_atlas(1,9);
        break;
    case DEATH_RAY:
        spell_icon_rect = get_atlas(2,9);
        break;
    }

    Rect player_health_rect = (Rect) {
        20, 
        screenHeight - 100, 
        (player.health/100.0f)*200.0f, 
        20.0f
    };
    Rect player_mana_rect = (Rect) {
        20, 
        screenHeight - 82, 
        (player.mana/100.0f)*200.0f, 
        20.0f
    };
    Rect apprentice_health_rect = (Rect) {
        20, 
        screenHeight - 42, 
        (apprentice.health/100.0f)*200.0f, 
        20.0f
    };
    Rect apprentice_mana_rect = (Rect) {
        20, 
        screenHeight - 22, 
        (apprentice.mana/100.0f)*200.0f, 
        20.0f
    };

    draw_sprite(atlas, spell_icon_rect, (Vec2){screenWidth - 80, screenHeight - 80}, NO_FLIP, WHITE);

    DrawRectangle(0, screenHeight-130, TILE_SIZE*5, TILE_SIZE*3, PAL3);

    DrawText("MAGE", 20, screenHeight-120, 20, PAL5);
    DrawRectangleRec(player_health_rect, PAL4);
    DrawRectangleLinesEx(player_health_rect, 2, PAL5);
    DrawRectangleRec(player_mana_rect, PAL0);
    DrawRectangleLinesEx(player_mana_rect, 2, PAL5);
    DrawText("APPRENTICE", 20, screenHeight-60, 20, PAL5);
    DrawRectangleRec(apprentice_health_rect, PAL4);
    DrawRectangleLinesEx(apprentice_health_rect, 2, PAL5);
    DrawRectangleRec(apprentice_mana_rect, PAL0);
    DrawRectangleLinesEx(apprentice_mana_rect, 2, PAL5);


    if (gameplay_paused) {
        F32 fontsize = 40;
        const char* text = "GAME PAUSED";
        DrawRectangle(screenWidth/2 - MeasureText(text, fontsize)/2, screenHeight/2 - fontsize/2, MeasureText(text,fontsize), fontsize, PAL1);
        DrawText(text, screenWidth/2 - MeasureText(text, fontsize)/2, screenHeight/2 - fontsize/2, fontsize, PAL5);
    }

}

void draw_debug_ui(void) {
    DrawTextureEx(atlas, (Vec2){16, 64}, 0, 3, WHITE);

    for (I32 i = 0; i < ARRAY_LEN(Color_Palette); i++ ) {
        Vec2 pos = {16.0f + i*32, 32.0f};
        Vec2 size = {32, 32};
        DrawRectangleV(pos, size, Color_Palette[i]);
        Color color = i == 5 ? PAL0 : PAL5;

        DrawText(TextFormat("%d", i), (int)pos.x + 4, (int)pos.y + 4, 24, color);
    }

    DrawText(TextFormat("MANA: %.2f", player.mana), 16, screenHeight-40, 20, PAL4);
    DrawText(TextFormat("dt: %f", GetFrameTime()), 16, screenHeight-60, 20, PAL4);
    DrawText(TextFormat("Spell: %i", player.active_spell), 16, screenHeight-80, 20, PAL4);

    DrawFPS(10,10);
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
    Rectangle dst = { 
        position.x, 
        position.y, 
        fabsf(src.width)*TILE_UPSCALE_FACTOR,
        fabsf(src.height)*TILE_UPSCALE_FACTOR
    };
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
