#include <raylib.h>
#include <deque>
#include <iostream>
#include <raymath.h>
#include <optional> 
#include "Button.hpp"

using namespace std;

float cellsize = 24.0f;
int cellcount = 25;
float offset = 40.0f;

bool IsRandomPosOnSnake(const deque<Vector2>& OccupiedCell, Vector2 pos)
{
    for (int i = 0; i < OccupiedCell.size(); i++)
        if (Vector2Equals(OccupiedCell[i], pos)) return true;
    return false;
}

void AddSequenceInOccupiedCell(deque<Vector2>& sequence, deque<Vector2>& OccupiedCell)
{
    OccupiedCell.insert(OccupiedCell.end(), sequence.begin(), sequence.end());
}

class Snake
{
public:
    deque<Vector2> body;
    Vector2 direction;
    bool addSegment = false;
    int player = 0;
    Texture2D texture;
    Image image;
    Color snakeColor;

    Snake(int identify)
    {
        if (identify == 1)
        {
            body = {{6, 9}, {5, 9}, {4, 9}};
            direction = {1, 0};
            player = 1;
            image = LoadImage("Gráficos/cobraAzul-removebg-preview.png");
            snakeColor = BLUE;
        }
        else
        {
            body = {{24, 21}, {25, 21}, {26, 21}};
            direction = {-1, 0};
            player = 2;
            image = LoadImage("Gráficos/cobraVermelha-removebg-preview.png");
            snakeColor = RED;
        }

        ImageResize(&image, (int)cellsize, (int)cellsize);
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment) addSegment = false;
        else body.pop_back();
    }

    void Reset()
    {
        addSegment = false;
        if (player == 1)
        {
            body = {{6, 9}, {5, 9}, {4, 9}};
            direction = {1, 0};
        }
        else
        {
            body = {{24, 21}, {25, 21}, {26, 21}};
            direction = {-1, 0};
        }
    }

    void Draw()
    {
        for (int i = 0; i < body.size(); i++)
        {
            Rectangle r = 
            {
                offset + body[i].x * cellsize,
                offset + body[i].y * cellsize,
                cellsize,
                cellsize
            };

            if (i == 0)
                DrawRectangleRounded(r, 0.4f, 6, WHITE);
            else
                DrawRectangleRounded(r, 0.5f, 6, snakeColor);
        }
    }
};

class Food
{
public:
    Vector2 position;
    Texture2D texture;

    Food()
    {
        Image image = LoadImage("Gráficos/Maçã.png");
        ImageResize(&image, (int)cellsize, (int)cellsize);
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    void Draw()
    {
        DrawTexture(texture, offset + position.x * cellsize, offset + position.y * cellsize, WHITE);
    }

    Vector2 GenerateRandomPos(const deque<Vector2>& Occupied)
    {
        Vector2 pos;
        do
        {
            pos = {(float)GetRandomValue(0, cellcount - 1), (float)GetRandomValue(0, cellcount - 1)};
        } while (IsRandomPosOnSnake(Occupied, pos));
        return pos;
    }
};

enum class GameMode
{
    SINGLE,
    MULTI
};

enum class GameAction
{
    NONE,
    RESTART,
    BACK_TO_MENU
};

class Game
{
public:
    Snake snake1{1};
    optional<Snake> snake2;
    deque<Vector2> OccupiedCell;
    Food food;
    bool IsRunning = true;
    GameMode mode;

    Game(GameMode gm) : mode(gm)
    {
        ResetGame();
    }

    void GameOver()
    {
        IsRunning = false;
    }

    void ResetGame()
    {
        snake1.Reset();

        if (mode == GameMode::MULTI)
            snake2 = Snake(2);
        else
            snake2.reset();

        MultiplayerOccupiedUpdate();
        food.position = food.GenerateRandomPos(OccupiedCell);
        IsRunning = true;
    }

    void MultiplayerOccupiedUpdate()
    {
        OccupiedCell.clear();
        AddSequenceInOccupiedCell(snake1.body, OccupiedCell);

        if (snake2)
            AddSequenceInOccupiedCell(snake2->body, OccupiedCell);
    }

    void UpdateGame()
    {
        if (!IsRunning) return;

        Movement(snake1);
        snake1.Update();
        CheckCollisionWithEdges(snake1);
        if (!IsRunning) return;
        CheckCollisionWithBody(snake1);
        if (!IsRunning) return;

        if (snake2)
        {
            Movement(*snake2);
            snake2->Update();
            CheckCollisionWithEdges(*snake2);
            if (!IsRunning) return;
            CheckCollisionWithBody(*snake2);
            if (!IsRunning) return;

            CheckCollisionWithOpp();
            if (!IsRunning) return;

            if (Vector2Equals(snake1.body[0], snake2->body[0]))
            {
                GameOver();
                return;
            }
        }

        MultiplayerOccupiedUpdate();

        CheckIfAte(snake1, food);
        if (snake2) CheckIfAte(*snake2, food);
    }

    void CheckIfAte(Snake& snake, Food& food)
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            snake.addSegment = true;
            food.position = food.GenerateRandomPos(OccupiedCell);
        }
    }

    void CheckCollisionWithEdges(Snake& snake)
    {
        int x = (int)snake.body[0].x;
        int y = (int)snake.body[0].y;

        if (x < 0 || x >= cellcount || y < 0 || y >= cellcount) 
        {
            GameOver();

            if (snake.body.size() > 1) {
                snake.body[0] = snake.body[1];
            } else {
                if (x < 0) snake.body[0].x = 0;
                if (x >= cellcount) snake.body[0].x = cellcount - 1;
                if (y < 0) snake.body[0].y = 0;
                if (y >= cellcount) snake.body[0].y = cellcount - 1;
            }
        }
    }


    void CheckCollisionWithBody(Snake& snake)
    {
        for (int i = 1; i < snake.body.size(); i++)
            if (Vector2Equals(snake.body[0], snake.body[i])) GameOver();
    }

    void CheckCollisionWithOpp()
    {
        if (!snake2) return;

        for (int i = 1; i < snake2->body.size(); i++)
            if (Vector2Equals(snake1.body[0], snake2->body[i])) GameOver();

        for (int i = 1; i < snake1.body.size(); i++)
            if (Vector2Equals(snake2->body[0], snake1.body[i])) GameOver();
    }

    void Movement(Snake& snake)
    {
        if (snake.player == 1)
        {
            if (IsKeyDown(KEY_W) && snake.direction.y != 1) snake.direction = {0, -1};
            if (IsKeyDown(KEY_S) && snake.direction.y != -1) snake.direction = {0, 1};
            if (IsKeyDown(KEY_A) && snake.direction.x != 1) snake.direction = {-1, 0};
            if (IsKeyDown(KEY_D) && snake.direction.x != -1) snake.direction = {1, 0};
        }
        else
        {
            if (IsKeyDown(KEY_UP) && snake.direction.y != 1) snake.direction = {0, -1};
            if (IsKeyDown(KEY_DOWN) && snake.direction.y != -1) snake.direction = {0, 1};
            if (IsKeyDown(KEY_LEFT) && snake.direction.x != 1) snake.direction = {-1, 0};
            if (IsKeyDown(KEY_RIGHT) && snake.direction.x != -1) snake.direction = {1, 0};
        }
    }

    GameAction Draw()
    {
        snake1.Draw();
        if (snake2) snake2->Draw();
        food.Draw();

        if (!IsRunning)
        {
            DrawRectangle(0, 0,
                2 * offset + cellsize * cellcount,
                2 * offset + cellsize * cellcount,
                Fade(BLACK, 0.6f));

            DrawText("GAME OVER", 220, 120, 40, RED);

            Rectangle restartBtn = {200, 200, 250, 50};
            Rectangle menuBtn = {200, 280, 250, 50};

            DrawRectangleRec(restartBtn, LIGHTGRAY);
            DrawRectangleRec(menuBtn, LIGHTGRAY);

            DrawText("Jogar novamente", restartBtn.x + 20, restartBtn.y + 15, 20, BLACK);
            DrawText("Voltar ao menu", menuBtn.x + 40, menuBtn.y + 15, 20, BLACK);

            Vector2 mouse = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (CheckCollisionPointRec(mouse, restartBtn))
                    return GameAction::RESTART;

                if (CheckCollisionPointRec(mouse, menuBtn))
                    return GameAction::BACK_TO_MENU;
            }
        }

        return GameAction::NONE;
    }
};

int main()
{
    InitWindow(2 * offset + cellsize * cellcount,
               2 * offset + cellsize * cellcount,
               "Jogo da Cobrinha");

    SetTargetFPS(60);

voltar_menu:
    GameMode mode;
    bool selected = false;

    while (!selected && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GREEN);

        DrawText("Escolha o modo de jogo", 100, 50, 30, WHITE);

        Rectangle singleBtn = {100, 150, 300, 50};
        Rectangle multiBtn = {100, 250, 300, 50};

        DrawRectangleRec(singleBtn, LIGHTGRAY);
        DrawRectangleRec(multiBtn, LIGHTGRAY);

        DrawText("Singleplayer", singleBtn.x + 50, singleBtn.y + 10, 20, BLACK);
        DrawText("Multiplayer", multiBtn.x + 50, multiBtn.y + 10, 20, BLACK);

        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointRec(mouse, singleBtn))
            {
                mode = GameMode::SINGLE;
                selected = true;
            }
            if (CheckCollisionPointRec(mouse, multiBtn))
            {
                mode = GameMode::MULTI;
                selected = true;
            }
        }

        EndDrawing();
    }

    Game game(mode);
    double lastUpdateTime = GetTime();

    while (!WindowShouldClose())
    {

        if (game.IsRunning && GetTime() - lastUpdateTime >= 0.2)
        {
            game.UpdateGame();
            lastUpdateTime = GetTime();
        }

        BeginDrawing();
        ClearBackground(GREEN);

        DrawRectangleLinesEx(
            Rectangle{offset - 5, offset - 5,
                    cellsize * cellcount + 10,
                    cellsize * cellcount + 10},
            5, DARKGREEN
        );

        GameAction action = game.Draw();
        EndDrawing();

        if (!game.IsRunning)
        {
            if (action == GameAction::RESTART)
            {
                game.ResetGame();
                lastUpdateTime = GetTime();
            }
            else if (action == GameAction::BACK_TO_MENU)
            {
                goto voltar_menu;
            }
        }
    }

    CloseWindow();
    return 0;
}