// Breakout.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "raylib.h"

struct Ball {
    Vector2 m_pos;
    Vector2 m_acceleration{ 1.0f, 1.0f};
    float m_velocity;
    float m_radius = 5.0f;
};

struct Brick {
    Rectangle m_rect;
    Color m_color;
    float m_width{75.f};
    float m_height{45.f};
};

struct Player {
    Rectangle m_rect;
    float m_velocity{ 250.f };
    size_t m_score{ 0 };
    float m_paddle_width{ 75.f };
    float m_paddle_height{ 10.f };
};

namespace globals {
    constexpr size_t window_width{ 800 };
    constexpr size_t window_height{ 900 };

    Image background;
    Texture2D texture_background;
    Player player;
    Ball ball;

    std::vector<Brick> bricks;
    const float brick_width{ 75.f };
    const float brick_height{ 45.f };
    const float bricks_per_row{ 8 };
    const float horizontal_spacing{ 5.0f };
    const float verticle_spacing{ 6.0f };
    float total_bricks_width = (bricks_per_row * brick_width) + ((bricks_per_row - 1) * horizontal_spacing);
    float start_x = (window_width - total_bricks_width) / 2.0f;

    Sound boop;
}

void game_startup() {
    globals::background = LoadImage("textures/bg.png");
    globals::texture_background = LoadTextureFromImage(globals::background);
    UnloadImage(globals::background);

    //Load player
    globals::player.m_rect = Rectangle{ 250.f, 750.f, globals::player.m_paddle_width, globals::player.m_paddle_height };
    globals::player.m_score = 0;

    //Load ball
    globals::ball.m_pos = Vector2{ 600, 600 };
    globals::ball.m_velocity = 300.f;

    std::vector<Color> colors{ RAYWHITE, RED, ORANGE, PURPLE, BROWN, PINK, MAGENTA, DARKGRAY, GREEN };

    Brick new_brick;
    for (size_t i = 0; i < globals::bricks_per_row; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            //new_brick.m_rect = Rectangle{ static_cast<float>(40 + (i * 55)),static_cast<float>(50 + (j * 26)), new_brick.m_width, new_brick.m_height };
            float x = globals::start_x + i * (globals::brick_width + globals::horizontal_spacing);
            float y = 50.0f + j * (globals::brick_height + globals::verticle_spacing);
            new_brick.m_rect = Rectangle{ x, y, new_brick.m_width, new_brick.m_height };
            new_brick.m_color = colors.at(rand() % colors.size());
            globals::bricks.emplace_back(new_brick);
        }
    }

    globals::boop = LoadSound("sounds/boop.wav");
}

void game_update() {
    float frame_time = GetFrameTime();
    if (IsKeyDown(KEY_LEFT)) {
        globals::player.m_rect.x -= globals::player.m_velocity * frame_time;
    }
    else if (IsKeyDown(KEY_RIGHT)) {
        globals::player.m_rect.x += globals::player.m_velocity * frame_time;
    }

    //Update ball position
    globals::ball.m_pos.x = globals::ball.m_pos.x + ((globals::ball.m_velocity * globals::ball.m_acceleration.x) * frame_time);
    globals::ball.m_pos.y = globals::ball.m_pos.y + ((globals::ball.m_velocity * globals::ball.m_acceleration.y) * frame_time);

    //Ball collision between each brick and the ball
    Brick brick;
    for (size_t i = 0; i < globals::bricks.size(); ++i) {
        brick = globals::bricks.at(i);
        if (CheckCollisionCircleRec(globals::ball.m_pos, globals::ball.m_radius, brick.m_rect)) {
            globals::ball.m_acceleration.y *= -1;
            globals::bricks.erase(globals::bricks.begin() + i);
            globals::player.m_score += 10;
            PlaySound(globals::boop);
            break;
        }
    }

    //Player collision detection with window
    if (globals::player.m_rect.x < 0) {
        globals::player.m_rect.x = 0;
    }
    else if (globals::player.m_rect.x > (globals::window_width - globals::player.m_rect.width)) {
        globals::player.m_rect.x = (globals::window_width - globals::player.m_rect.width);
    }

    //Ball collision with window
    if (globals::ball.m_pos.x > globals::window_width || globals::ball.m_pos.x < 10) {
        globals::ball.m_acceleration.x *= -1; // Flip the direction
    }
    else if (globals::ball.m_pos.y > globals::window_height || globals::ball.m_pos.y < 10) {
        globals::ball.m_acceleration.y *= -1; // Flip the direction
    }

    //Ball collision with player
    if (CheckCollisionCircleRec(globals::ball.m_pos, globals::ball.m_radius, globals::player.m_rect)) {
        globals::ball.m_acceleration.x *= -1;
        globals::ball.m_acceleration.y *= -1;
        PlaySound(globals::boop);
    }
}

void game_render() {
    //Render the background and player
    DrawTexture(globals::texture_background, 0, 0, RAYWHITE);
    DrawRectangle(globals::player.m_rect.x, globals::player.m_rect.y, globals::player.m_rect.width, globals::player.m_rect.height, RED);

    //Render the bricks
    Brick brick;
    for (size_t i = 0; i < globals::bricks.size(); ++i) {
        brick = globals::bricks.at(i);
        DrawRectangle(brick.m_rect.x, brick.m_rect.y, brick.m_rect.width, brick.m_rect.height, brick.m_color);
    }

    //Render the ball
    DrawCircle(globals::ball.m_pos.x, globals::ball.m_pos.y, globals::ball.m_radius, RAYWHITE);

    // Render the score
    std::string str_score{ "SCORE: " + std::to_string(globals::player.m_score) };
    DrawText(str_score.c_str(), 10, 10, 30, YELLOW);
}

void game_shutdown() {
    UnloadSound(globals::boop);
    UnloadTexture(globals::texture_background);
    CloseAudioDevice();
}

int main()
{
    srand(time(NULL));

    InitWindow(globals::window_width, globals::window_height, "Breakout");
    InitAudioDevice();
    SetTargetFPS(60);

    game_startup();

    while (!WindowShouldClose()) {

        game_update();

        BeginDrawing();
        ClearBackground(BLUE);

        game_render();

        EndDrawing();
    }

    game_shutdown();
    CloseWindow();

    return EXIT_SUCCESS;
}
