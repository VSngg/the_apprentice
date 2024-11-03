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
    SCREEN_TITLE = 0, 
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

typedef enum {
    NO_SPELL = 0,
    MANA_RAY,
    DEATH_RAY,
    SPELL_KIND_COUNT,
} Spell_Kind;

typedef struct Player {
    Vec2 pos;
    F32  speed;

    F32  health;
    F32  max_health;
    bool is_invincible;
    F32  invincibility_timer;

    Spell_Kind active_spell;
    F32        mana;
    F32        max_mana;
    F32        mana_regen;
    bool       is_casting;

    Vec2 ray_anchor;

    Flip_Texture flip_texture;
} Player;

typedef struct Spell{
    F32 initial_cost;
    F32 cost_per_second;
    F32 activation_distance;
} Spell;

typedef struct Apprentice {
    Vec2 pos;
    F32 speed;

    F32  health;
    F32  max_health;
    bool is_invincible;
    F32  invincibility_timer;

    F32        mana;
    F32        max_mana;
    F32        mana_regen;


    bool following_player;

    Vec2 ray_anchor;

    Flip_Texture flip_texture;

} Apprentice;

typedef struct Enemy {
    int  id;
    bool alive;
    Vec2 pos;
    F32  speed;

    F32 health;
    F32 max_health;

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

const Spell SPELLS[] = {
    [NO_SPELL]  = {0.0f, 0.0f, 0.0f},
    [MANA_RAY]  = {20.0f, 1.0f, 500.0f},
    [DEATH_RAY] = {20.0f, 2.0f, 500.0f},
};


#define PAL0 Color_Palette[0]
#define PAL1 Color_Palette[1]
#define PAL2 Color_Palette[2]
#define PAL3 Color_Palette[3]
#define PAL4 Color_Palette[4]
#define PAL5 Color_Palette[5]
#define PAL6 Color_Palette[6]
#define PAL7 Color_Palette[7]

#define TILE_SIZE_ORIGINAL 16
#define TILE_UPSCALE_FACTOR 3
#define TILE_SIZE TILE_SIZE_ORIGINAL*TILE_UPSCALE_FACTOR

#define SPRITE_CENTER(pos) ((Vec2){(pos).x + TILE_SIZE/2, (pos).y + TILE_SIZE/2})

#define MAX_ENEMIES 512

#define DEATH_RAY_DAMAGE 0.5f
#define MANA_RAY_MANA_PER_SECOND 2.0f
#define ENEMY_DAMAGE 1.0f
#define ENEMY_MANA_BURN 0.1f
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame
void init_gameplay(void);
void update_gameplay(void);
void draw_gameplay(void);
void draw_ui(void);
void draw_debug_ui(void);
Rect get_atlas(int row, int col);
void draw_sprite(Texture2D texture, Rectangle src, Vector2 position, Flip_Texture flip, Color tint);

#endif // RAYLIB_GAME_H