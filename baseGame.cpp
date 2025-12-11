#include <raylib.h>
#include <deque>
#include <iostream>
#include <raymath.h>

using namespace std;

int cellsize = 32;
int cellcount = 30;


// - NOTA DE FELIPE TINEL, O CONQUISTADOR.
    //   Esse trecho do código é interessante aperfeiçoa-lo.
    //   O que fiz atualmente é uma forma funcional, mas não eficiente, de checar
    //   se uma fruta nascerá numa posição ocupada pela cobra ou não.
    //   A ideia, no futuro, é criar uma váriavel global relacionada a uma posição
    //   ocupada na matriz que sirva de parâmetro para a fruta.
    //   Os primeiros passos estão nessa função e variável comentada.

// ________________________________________________________________________________//

// deque<Vector2> posOccupied;
// void OccupiedCell(Vector2 pos, const deque<Vector2>& snakeBody)
// {
//     for (int i = 0; i < snakeBody.size(); i++)
//     {
//         if (Vector2Equals(snakeBody[i], pos)) return; 
//     }
//     ;
// }

// ________________________________________________________________________________//



bool IsRandomPosOnSnake(Vector2 pos, const deque<Vector2>& snakeBody1, const deque<Vector2>& snakeBody2)
{
    for (int i = 0; i < snakeBody1.size(); i++)
    {
        if (Vector2Equals(snakeBody1[i], pos)) return true; 
    }

    for (int i = 0; i < snakeBody2.size(); i++)
        {
            if (Vector2Equals(snakeBody2[i], pos)) return true; 
        }
    return false;
}


class Snake
{
public:

    // deque<Vector2> body = { {6, 9}, {5, 9}, {4, 9} };
    // Vector2 direction = {1,0};
    deque<Vector2> body;
    Vector2 direction;
    bool addSegment = false;
    int player = 0;

    Snake (int identify)
    {
        switch (identify)
        {
            case 1:
            body = { {6, 9}, {5, 9}, {4, 9} };
            direction = {1, 0};
            player = 1;
            break;

            case 2:
            body = { {9, 6}, {9, 5}, {9, 4} };
            direction = {-1, 0};
            player = 2;
            break;
        }

    }

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

            if (player == 1)
            DrawRectangleRounded(segment, 0.5f, 6, GREEN);
            else if (player == 2)
            DrawRectangleRounded(segment, 0.5f, 6, BLUE);
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
        switch (player)
        {
        case 1:
            body = { {6, 9}, {5, 9}, {4, 9} };
            direction = {1, 0};
            break;
        case 2:
            body = { {9, 6}, {9, 5}, {9, 4} };
            direction = {-1, 0};
            break;
    };
    };
    

};

class Food
{
public:   
    Vector2 position = {6,7};
    Texture2D texture;

    Food(deque<Vector2>& snakeBody1, deque<Vector2>& snakeBody2){
        Image image = LoadImage("Gráficos/Maçã.png");
        ImageResize(&image, cellsize, cellsize); 
        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        position = GenerateRandomPos(snakeBody1, snakeBody2);
    }

    ~Food(){
        UnloadTexture(texture);
    }

    void Draw(){
        DrawTexture(texture,position.x * cellsize,position.y * cellsize, WHITE);
     }

    Vector2 GenerateRandomPos(const deque<Vector2>& snakeBody1, const deque<Vector2>& snakeBody2){
      Vector2 position;

        do {
            float x = (float)GetRandomValue(0, cellcount - 1);
            float y = (float)GetRandomValue(0, cellcount - 1);
            position = {x, y};
        }
        while (IsRandomPosOnSnake(position, snakeBody1, snakeBody2));

        return position;
    }

};


class Game
{
public:
    Snake snake1;
    Snake snake2;
    Food food = Food(snake1.body, snake2.body);
    bool IsRunning = true;


    Game() : snake1(1), snake2(2), food(snake1.body, snake2.body) {}


    void Draw(){
        snake1.Draw();
        snake2.Draw();
        food.Draw();
    }

    void Update(){
        if(IsRunning){
            snake1.Update();
            snake2.Update();
            CheckIfAte();
            CheckCollisionWithEdges();
            CheckCollisionWithBody();           
        }

    }

    void CheckIfAte(){

        if(Vector2Equals(snake1.body[0], food.position)) {
            food.position = food.GenerateRandomPos(snake1.body, snake2.body);
            snake1.addSegment = true;   
        }

        if(Vector2Equals(snake2.body[0], food.position)) {
            food.position = food.GenerateRandomPos(snake1.body, snake2.body);
            snake2.addSegment = true;   
        }
    }

    void CheckCollisionWithEdges(){
        if(snake1.body[0].x == cellcount || snake1.body[0].x == -1){
            GameOver();
        }
        if(snake1.body[0].y == cellcount || snake1.body[0].y == -1){
            GameOver();
        }
        if(snake2.body[0].x == cellcount || snake2.body[0].x == -1){
            GameOver();
        }
        if(snake2.body[0].y == cellcount || snake2.body[0].y == -1){
            GameOver();
        }
    }

    void CheckCollisionWithBody() {
        for (int i = 1; i < snake1.body.size(); i++) {
            if (Vector2Equals(snake1.body[0], snake1.body[i]) && Vector2Equals(snake2.body[0], snake2.body[i])) {
                GameOver();
            }
        }
        for (int i = 1; i < snake2.body.size(); i++) {
            if (Vector2Equals(snake2.body[0], snake2.body[i]) && Vector2Equals(snake1.body[0], snake1.body[i])) {
                GameOver();
            }
        }
    }

    void GameOver(){
        snake1.Reset();
        snake2.Reset();
        food.position = food.GenerateRandomPos(snake1.body, snake2.body);
        IsRunning = false;
    }

    void Movement ()
    {

        if (IsKeyDown(KEY_UP) && snake2.direction.y != 1)
        snake2.direction = {0, -1};

        if (IsKeyDown(KEY_DOWN) && snake2.direction.y != -1)
        snake2.direction = {0, 1};

        if (IsKeyDown(KEY_LEFT) && snake2.direction.x != 1)
        snake2.direction = {-1, 0};

        if (IsKeyDown(KEY_RIGHT) && snake2.direction.x != -1)
        snake2.direction = {1, 0};

        if (IsKeyDown(KEY_W) && snake1.direction.y != 1)
        snake1.direction = {0, -1};

        if (IsKeyDown(KEY_S) && snake1.direction.y != -1)
        snake1.direction = {0, 1};

        if (IsKeyDown(KEY_A) && snake1.direction.x != 1)
        snake1.direction = {-1, 0};

        if (IsKeyDown(KEY_D) && snake1.direction.x != -1)
        snake1.direction = {1, 0};

    IsRunning = true;
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


        game.Movement();


        BeginDrawing();
        ClearBackground(RED);

        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
