#include <raylib.h>
#include <deque>
#include <iostream>
#include <raymath.h>

using namespace std;

int cellsize = 32;
int cellcount = 30;



bool IsRandomPosOnSnake(Vector2 pos, const deque<Vector2>& snakeBody)
{
    for (int i = 0; i < snakeBody.size(); i++)
    {
        if (Vector2Equals(snakeBody[i], pos)) return true; 
    }
    return false;
}



class Snake
{
public:
    deque<Vector2> body = { {6, 9}, {5, 9}, {4, 9} };
    Vector2 direction = {1,0};
    bool addSegment = false;

    void Draw(){
        for(int i = 0; i < body.size(); i++){
            float x = body[i].x * cellsize;
            float y = body[i].y * cellsize;

            Rectangle segment = {
                x,
                y,
                (float)cellsize,
                (float)cellsize
            };

            DrawRectangleRounded(segment, 0.5f, 6, GREEN);
        }
    }

    void Update(){
        
        body.push_front(Vector2Add(body[0], direction));

        if (addSegment) {
            
            addSegment = false;

        } else {

            body.pop_back();

        }

    }

    void Reset(){
        body = { {6, 9}, {5, 9}, {4, 9} };
        direction = {1, 0};
    }

};

class Food
{
public:   
    Vector2 position = {6,7};
    Texture2D texture;

    Food(deque<Vector2>& snakeBody){
        Image image = LoadImage("Gráficos/Maçã.png");
        ImageResize(&image, cellsize, cellsize); 
        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        position = GenerateRandomPos(snakeBody);
    }

    ~Food(){
        UnloadTexture(texture);
    }

    void Draw(){
        DrawTexture(texture,position.x * cellsize,position.y * cellsize, WHITE);
     }

    Vector2 GenerateRandomPos(const deque<Vector2>& snakeBody){
      Vector2 position;

        do {
            float x = (float)GetRandomValue(0, cellcount - 1);
            float y = (float)GetRandomValue(0, cellcount - 1);
            position = {x, y};
        }
        while (IsRandomPosOnSnake(position, snakeBody));

        return position;
    }

};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool IsRunning = true;

    void Draw(){
        snake.Draw();
        food.Draw();
    }

    void Update(){
        if(IsRunning){
            snake.Update();
            CheckIfAte();
            CheckCollisionWithEdges();
            CheckCollisionWithBody();           
        }

    }

    void CheckIfAte(){

        if(Vector2Equals(snake.body[0], food.position)) {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;   
        }
    }

    void CheckCollisionWithEdges(){
        if(snake.body[0].x == cellcount || snake.body[0].x == -1){
            GameOver();
        }
        if(snake.body[0].y == cellcount || snake.body[0].y == -1){
            GameOver();
        }
    }

    void CheckCollisionWithBody() {
        for (int i = 1; i < snake.body.size(); i++) {
            if (Vector2Equals(snake.body[0], snake.body[i])) {
                GameOver();
            }
        }
    }

    void GameOver(){
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        IsRunning = false;
    }
};




int main () {

    cout << "Iniciando o jogo..." << endl;
    InitWindow(cellsize*cellcount, cellsize*cellcount, "Jogo da Cobrinha");
    SetTargetFPS(60);

    Game game;

    double lastUpdateTime = 0;

    while (!WindowShouldClose()) {

        if (GetTime() - lastUpdateTime >= 0.2) { 
            game.Update();
            lastUpdateTime = GetTime();
        }


        switch (GetKeyPressed()){
            case KEY_UP:
                if (game.snake.direction.y != 1) { 
                    game.snake.direction = {0, -1};
                }
                game.IsRunning = true;
                break;

            case KEY_DOWN:
                if (game.snake.direction.y != -1) {
                    game.snake.direction = {0, 1};
                }
                game.IsRunning = true;
                break;

            case KEY_LEFT:
                if (game.snake.direction.x != 1) { 
                    game.snake.direction = {-1, 0};
                }
                game.IsRunning = true;
                break;

            case KEY_RIGHT:
                if (game.snake.direction.x != -1) { 
                    game.snake.direction = {1, 0};
                }
                game.IsRunning = true;
                break;
         }


        BeginDrawing();
        ClearBackground(RED);

        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
