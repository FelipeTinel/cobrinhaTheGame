#include <raylib.h>
#include <deque>
#include <iostream>

using namespace std;

int cellsize = 32;
int cellcount = 30;


class Snake
{
public:
    deque<Vector2> body = { {6, 9}, {5, 9}, {4, 9} };

    void Draw(){
        for(int i = 0; i < body.size(); i++){
            int x = body[i].x;
            int y = body[i].y;
            DrawRectangle(x*cellsize, y*cellsize, cellsize, cellsize, ...);
        }
    }

};

class Food
{


public:   
    Vector2 position = {6,7};
    Texture2D texture;

    Food(){
    Image image = LoadImage("Gráficos/Maçã.png");
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    position = GenerateRandomPos();
    }

    ~Food(){
    UnloadTexture(texture);
    }


    void Draw(){
    DrawTexture(texture, position.x * cellsize, position.y * cellsize, WHITE);
    }

    Vector2 GenerateRandomPos(){
        float x = GetRandomValue(0, cellcount - 1);
        float y = GetRandomValue(0, cellcount - 1);
        return Vector2{x,y};
    }
};


int main () {


cout << "Iniciando o jogo..." << endl;
InitWindow(cellsize*cellcount, cellsize*cellcount, "Jogo da Cobrinha");
SetTargetFPS(60);

Snake snake = Snake();
Food food = Food();

while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(...);
    food.Draw();

    EndDrawing();
}

CloseWindow();
return 0; 

}