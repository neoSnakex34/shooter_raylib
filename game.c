#include "bullet.h"
#include "entity.h"
#include "glob_macros.h"
#include "player.h"
#include "raylib-5.5_linux_amd64/include/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INT_TO_STRING_BUF_SIZE 32
#define SCREEN_W 1500
#define SCREEN_H 1500
#define CENTER                                                                 \
    (Vector2) { .x = SCREEN_W / 2.0, .y = SCREEN_H / 2.0 }
#define ENEMY_COUNT 10

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
  entry point
*/
int main(void)
{

    // SetRandomSeed(time(NULL));
    srand(time(NULL));
    int score;
    bool gameOver;
    float dt;
    Bullet bullet = {.entity.active = false, .entity.speed = BULLET_SPEED};
    Entity enemies[ENEMY_COUNT];
    int time = 0;
    Player player = {0};
    init(&score, &gameOver, &dt, enemies, &player);
    char scoreBuf[INT_TO_STRING_BUF_SIZE];

    // TODO CHANGE
    InitAudioDevice();
    Sound shootFx = LoadSound("assets/fx/shoot.ogg");
    Sound enemyDeadFx = LoadSound("assets/fx/enemy_dead.ogg");
    Music theme = LoadMusicStream("assets/music/main_track.ogg");
    SetMusicVolume(theme, 0.4f);
    PlayMusicStream(theme);

    InitWindow(SCREEN_W, SCREEN_H, "SHOOT!");
    player.entity.texture = LoadTexture("assets/sprites/player_b.png");

    // TODO move this
    int dirX = 0;
    int dirY = 0;
    float playerStepFactor;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        time = (time + 1) % 60;
        BeginDrawing();
        printf("time: %d\n", time);
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
            break;
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
            if (time > 30)
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

    CloseAudioDevice();
    UnloadMusicStream(theme);
    UnloadSound(shootFx);
    UnloadSound(enemyDeadFx);
    CloseWindow();

    return 0;
}
