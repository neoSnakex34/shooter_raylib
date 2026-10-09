#include "bullet.h"
#include "entity.h"
#include "glob_macros.h"
#include "player.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

#define INT_TO_STRING_BUF_SIZE 32
#ifdef PLATFORM_WEB
#define SCREEN_W 1280
#define SCREEN_H 720
#else
#define SCREEN_W 1500
#define SCREEN_H 1500
#endif
#define CENTER                                                                 \
    (Vector2) { .x = SCREEN_W / 2.0, .y = SCREEN_H / 2.0 }
#define ENEMY_COUNT 10

/*
  file-scope game state
  needed for the emscripten frame callback which takes no arguments
*/
static int score;
static bool gameOver;
static float dt;
static Bullet bullet;
static Entity enemies[ENEMY_COUNT];
static int time_;
static Player player;
static char scoreBuf[INT_TO_STRING_BUF_SIZE];
static Sound shootFx;
static Sound enemyDeadFx;
static Music theme;
static int dirX;
static int dirY;
static float playerStepFactor;

void scoreToString(int score, char *buf, size_t bufSize)
{
    snprintf(buf, bufSize, "score: %d", score);
}

/*
  init game
  this should be designed in a sense
  in which one could easily restart the game by calling it
  now is just a stub initializer
  should be refactored asap
*/
void init(int *score, bool *gameOver, float *dt, Entity *enemies,
          Player *player)
{
    *score = 0;
    *gameOver = false;
    *dt = 1;
    player->entity.position = CENTER;
    player->speed = 360.0f;
    player->entity.active = true;

    for (size_t i = 0; i < ENEMY_COUNT; ++i)
    {
        enemies[i].position = getRandomPositionAvoidingCircle(
            SCREEN_W, SCREEN_H, &player->entity, PLAYER_RADIUS + 100);
        enemies[i].active = true;
    }
};

/*
  per-frame callback
  extracted from the original while loop so emscripten can drive it
*/
void frameCallback(void)
{
    time_ = (time_ + 1) % 60;
    BeginDrawing();
    printf("time: %d\n", time_);
    scoreToString(score, scoreBuf, INT_TO_STRING_BUF_SIZE);
    DrawText(scoreBuf, 20, 20, 60, RAYWHITE);
    UpdateMusicStream(theme);
    dt = GetFrameTime();
    playerStepFactor = player.speed * dt;
    ClearBackground(BLACK);

    /*
        exit if needed
    */
    if (IsKeyDown(KEY_ESCAPE))
    {
#ifdef PLATFORM_WEB
        emscripten_cancel_main_loop();
        CloseAudioDevice();
        UnloadMusicStream(theme);
        UnloadSound(shootFx);
        UnloadSound(enemyDeadFx);
        CloseWindow();
        return;
#else
        return;
#endif
    }

    /*
        player movement
    */
    dirX = 0;
    dirY = 0;
    if (IsKeyDown(KEY_W))
    {
        dirY = NEGATIVE;
    }
    if (IsKeyDown(KEY_S))
    {
        dirY = POSITIVE;
    }
    if (IsKeyDown(KEY_D))
    {
        dirX = POSITIVE;
    }
    if (IsKeyDown(KEY_A))
    {
        dirX = NEGATIVE;
    }

    /*
        draw enemies type one as stated in enemy count
    */
    for (size_t i = 0; i < ENEMY_COUNT; ++i)
    {
        if (enemies[i].active)
            DrawRectangle(enemies[i].position.x, enemies[i].position.y,
                          ENEMY_TYPE_ONE_SIZE, ENEMY_TYPE_ONE_SIZE,
                          RAYWHITE);
    }

    /*
        player position updated
        based on the movement selected
    */
    updatePlayerPosition(&player, &dt, &dirX, &dirY, SCREEN_W, SCREEN_H);
    if (player.entity.active)
    {
        // DrawRectangle(player.entity.position.x,
        // player.entity.position.y-33, 33, 99, GREEN);
        // DrawRectangle(player.entity.position.x-33,
        // player.entity.position.y, 99, 33, GREEN);
        // DrawRectangle(player.entity.position.x-8.65,
        // player.entity.position.y-8.65, 49, 49, GREEN);
        DrawTexture(player.entity.texture,
                    player.entity.position.x - PLAYER_RADIUS,
                    player.entity.position.y - PLAYER_RADIUS, WHITE);
        if (time_ > 30)
        {
            DrawCircleV(player.entity.position, PLAYER_RADIUS, DARKGREEN);
            DrawTexture(player.entity.texture,
                        player.entity.position.x - PLAYER_RADIUS,
                        player.entity.position.y - PLAYER_RADIUS, WHITE);
        }
    }

    if (!bullet.entity.active && !gameOver)
    {
        if (IsKeyPressed(KEY_UP))
        {
            // draw bullet
            initShootFromEntity(&bullet, &player.entity, "up");
        }

        if (IsKeyPressed(KEY_DOWN))
        {
            initShootFromEntity(&bullet, &player.entity, "down");
        }

        if (IsKeyPressed(KEY_RIGHT))
        {
            initShootFromEntity(&bullet, &player.entity, "right");
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            initShootFromEntity(&bullet, &player.entity, "left");
        }

        /*
            play sound
        */
        if (bullet.entity.active)
            PlaySound(shootFx);
    }

    /*
        actually move the bullet
    */
    if (bullet.entity.active)
    {
        updateBullet(&bullet, SCREEN_W, SCREEN_H);
        DrawRectangle(bullet.entity.position.x - BULLET_TYPE_ONE_SIZE / 2.0,
                      bullet.entity.position.y - BULLET_TYPE_ONE_SIZE / 2.0,
                      BULLET_TYPE_ONE_SIZE, BULLET_TYPE_ONE_SIZE, YELLOW);
    }

    /*
        player collision check with enemies
    */
    if (atLeastOneIsActive(enemies, ENEMY_COUNT))
    {
        for (size_t i = 0; i < ENEMY_COUNT; ++i)
        {
            Entity *enemy = &enemies[i];

            if (enemy->active &&
                CheckCollisionCircleRec(
                    player.entity.position, PLAYER_RADIUS,
                    (Rectangle){enemy->position.x, enemy->position.y,
                                ENEMY_TYPE_ONE_SIZE, ENEMY_TYPE_ONE_SIZE}))
            {
                gameOver = true;
            }
        }
    }

    if (atLeastOneIsActive(enemies, ENEMY_COUNT))
    {

        for (size_t i = 0; i < ENEMY_COUNT; ++i)
        {
            Entity *enemy = &enemies[i];
            if (enemy->active &&
                CheckCollisionRecs(
                    (Rectangle){enemy->position.x, enemy->position.y,
                                ENEMY_TYPE_ONE_SIZE, ENEMY_TYPE_ONE_SIZE},
                    (Rectangle){
                        bullet.entity.position.x, bullet.entity.position.y,
                        BULLET_TYPE_ONE_SIZE, BULLET_TYPE_ONE_SIZE}))
            {
                printf("shoot!\n");
                PlaySound(enemyDeadFx);
                // printf("active: %d\n",getActiveEntity(enemies,
                // ENEMY_COUNT));
                bullet.entity.active = false;
                enemy->active = false;
                score++;
                enemy->position = getRandomPositionAvoidingCircle(
                    SCREEN_W, SCREEN_H, &player.entity,
                    PLAYER_RADIUS + 100);
                enemy->active = true;
            }
        }
    }

    if (gameOver)
    {
        StopMusicStream(theme);
        ClearBackground(WHITE);
        player.entity.active = false;
        for (size_t i = 0; i < ENEMY_COUNT; ++i)
        {
            enemies[i].active = false;
        }

        bullet.entity.active = false;

        char *gameOverText =
            "Game Over\npress esc to exit\ntype r to restart";
        int textWidth = MeasureText(gameOverText, 50);

        int textStartX = (SCREEN_W / 2) - textWidth / 2;
        int textStartY = (SCREEN_H / 2) - 50 / 2;

        DrawText(gameOverText, textStartX, textStartY, 50, BLACK);
        if (IsKeyPressed(KEY_R))
        {
            init(&score, &gameOver, &dt, enemies, &player);
            PlayMusicStream(theme);
        }
    }

    EndDrawing();
}

/*
  entry point
*/
int main(void)
{

    // SetRandomSeed(time(NULL));
    srand(time(NULL));
    bullet.entity.active = false;
    bullet.entity.speed = BULLET_SPEED;
    time_ = 0;
    init(&score, &gameOver, &dt, enemies, &player);

    // TODO CHANGE
    InitAudioDevice();
    shootFx = LoadSound("assets/fx/shoot.ogg");
    enemyDeadFx = LoadSound("assets/fx/enemy_dead.ogg");
    theme = LoadMusicStream("assets/music/main_track.ogg");
    SetMusicVolume(theme, 0.4f);
    PlayMusicStream(theme);

    InitWindow(SCREEN_W, SCREEN_H, "SHOOT!");
    player.entity.texture = LoadTexture("assets/sprites/player_b.png");

    dirX = 0;
    dirY = 0;

    SetTargetFPS(60);

#ifdef PLATFORM_WEB
    emscripten_set_main_loop(frameCallback, 0, 1);
#else
    while (!WindowShouldClose())
    {
        frameCallback();
        if (IsKeyDown(KEY_ESCAPE))
            break;
    }

    CloseAudioDevice();
    UnloadMusicStream(theme);
    UnloadSound(shootFx);
    UnloadSound(enemyDeadFx);
    CloseWindow();
#endif

    return 0;
}
