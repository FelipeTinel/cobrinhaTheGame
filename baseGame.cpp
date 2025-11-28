#include <raylib.h>
#include <iostream>

using namespace std;

int main () {

int cellsize = 32;
int cellcount = 30;
cout << "Iniciando o jogo..." << endl;
InitWindow(cellsize*cellcount, cellsize*cellcount, "Jogo da Cobrinha");
SetTargetFPS(60);


while (!WindowShouldClose()) {
    BeginDrawing();
    EndDrawing();
}

CloseWindow();
return 0; 

}