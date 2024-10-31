#ifndef RAYLIB_GAME_H
#define RAYLIB_GAME_H

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

typedef enum {
    NO_FLIP = 0,
    FLIP_X,
    FLIP_Y,
    FLIP_XY
} Flip_Texture;

typedef struct Player {
    Vec2 pos;
    F32  speed;

    F32  current_mana;
    F32  max_mana;
    F32  mana_regen;

    bool casting_mana_ray;

    Flip_Texture flip_texture;
} Player;

typedef struct Spell {
    F32 cooldown_timer;
    B32 is_on_cooldown;
    F32 mana_cost;
} Spell;

typedef struct Apprentice {
    Vec2 pos;
    F32 speed;

    bool following_player;

    Flip_Texture flip_texture;

} Apprentice;

typedef struct Enemy {
    Vec2 pos;
    F32  speed;

    Flip_Texture flip_texture;
} Enemy;

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
#define TILE_SIZE_ORIGINAL 16

#define SPRITE_CENTER(pos) ((Vec2){(pos).x + TILE_SIZE/2, (pos).y + TILE_SIZE/2})

#define MAX_ENEMIES 512
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame
Rect get_atlas(int row, int col);
void draw_sprite(Texture2D texture, Rectangle src, Vector2 position, Flip_Texture flip, Color tint);

#endif // RAYLIB_GAME_H