#include <raylib.h>
#include <deque>
#include <iostream>
#include <raymath.h>
#include <optional>

using namespace std;

int cellsize = 32;
int cellcount = 30;
int offset = 50;

// - NOTA DE FELIPE TINEL, O CONQUISTADOR.
    //   Esse trecho do código é interessante aperfeiçoa-lo.
    //   O que fiz atualmente é uma forma funcional, mas não eficiente, de checar
    //   se uma fruta nascerá numa posição ocupada pela cobra ou não.
    //   A ideia, no futuro, é criar uma váriavel global relacionada a uma posição
    //   ocupada na matriz que sirva de parâmetro para a fruta.
    //   Os primeiros passos estão nessa função e variável comentada.

// ________________________________________________________________________________//

// enum GameMode 
// {
//     SinglePlayer,
//     Multiplayer
// }


bool IsRandomPosOnSnake(const deque<Vector2>& OccupiedCell, Vector2 pos)
{
    for (int i = 0; i < OccupiedCell.size(); i++)
    {
        if (Vector2Equals(OccupiedCell[i], pos)) return true; 
    }

    return false;
}

void AddSequenceInOccupiedCell (deque<Vector2>& sequence, deque<Vector2>& OccupiedCell)
{
    OccupiedCell.insert (
        OccupiedCell.end(),
        sequence.begin(),
        sequence.end()

    );

}


class Snake
{
public:

    deque<Vector2> body;
    Vector2 direction;
    bool addSegment = false;
    int player = 0;
    float scale;
    Texture2D texture;
    Image image;
    Color snakeColor;

    Snake (int identify)
    {
        switch (identify)
        {
            case 1:
            body = { {6, 9}, {5, 9}, {4, 9} };
            direction = {1, 0};
            player = 1;
            image = LoadImage("Gráficos/cobraAzul-removebg-preview.png");
            snakeColor = BLUE; 

            break;

            case 2:
            body = { {24, 21}, {25, 21}, {26, 21} };
            direction = {-1, 0};
            player = 2;
            image = LoadImage("Gráficos/cobraVermelha-removebg-preview.png");
            snakeColor = RED;

            break;


        }

        ImageResize(&image, cellsize, cellsize); 
        texture = LoadTextureFromImage(image);
        UnloadImage(image);


    }

    void Draw()
    {

        for(int i = 0; i < body.size(); i++){
            float x = body[i].x * cellsize;
            float y = body[i].y * cellsize;

            Rectangle segment = {
                x,
                y,
                (float)cellsize,
                (float)cellsize
            };

            scale = cellsize / texture.height;
  
            if (i == 0) {
                if (Vector2Equals(direction, {1, 0}))
                DrawTextureEx(texture, body[0] * cellsize, 90, scale, WHITE);
                else if (Vector2Equals(direction, {-1, 0}))
                DrawTextureEx(texture, body[0] * cellsize, 270, scale, WHITE);
                else if (Vector2Equals(direction, {0, -1}))
                DrawTextureEx(texture, body[0] * cellsize, 0, scale, WHITE);
                else if (Vector2Equals(direction, {0, 1}))
                DrawTextureEx(texture, body[0] * cellsize, 180, scale, WHITE);
                } else {
                    DrawRectangleRounded(segment, 0.5f, 6, snakeColor);
                }

              


        }
    }


    void Update()
    {
        
        body.push_front(Vector2Add(body[0], direction));

        if (addSegment) {
            
            addSegment = false;

        } else {

            body.pop_back();

        }

    }


    void Reset()
    {
        switch (player)
        {
        case 1:
            body = { {6, 9}, {5, 9}, {4, 9} };
            direction = {1, 0};
            break;
        case 2:
            body = { {24, 21}, {25, 21}, {26, 21} };
            direction = {-1, 0};
            break;
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
        ImageResize(&image, cellsize, cellsize); 
        texture = LoadTextureFromImage(image);
        UnloadImage(image);

    }

    ~Food(){
        UnloadTexture(texture);
    }

    void Draw(){
        DrawTexture(texture,position.x * cellsize,position.y * cellsize, WHITE);
     }

    Vector2 GenerateRandomPos(const deque<Vector2>& Occupied){
      Vector2 position;

        do {
            float x = (float)GetRandomValue(0, cellcount - 1);
            float y = (float)GetRandomValue(0, cellcount - 1);
            position = {x, y};
        }
        while (IsRandomPosOnSnake(Occupied, position));

        return position;
    }

};


class Game
{
public:
    
    // Principal objetivo (talvez não o melhor, mas o que eu melhor visualizo) da classe Game: Criar um construtor que recebe "Modo de Jogo"
    // como parâmetro e, a partir disso, utiliza ou não a váriavel opcional "snake2". A partir disso, modificar os métodos com "Multiplayer"
    // no nome, colocando para receber um parâmetro de modo também e utilizando-as de maneira diferente atráves de um switch case. A depender
    // do modo passado no parametro na main, o jogo mudaria. Olha que lindo.
    
    Snake snake1;
    Snake snake2;
    deque<Vector2> OccupiedCell;
    Food food;
    bool IsRunning = true;

    Game() : snake1(1), snake2(2), food() {
      
        MultiplayerOccupiedUpdate();
        food.position = food.GenerateRandomPos(OccupiedCell);
    }

    


 // Parte pra modificar:   
//__________________________________________________________
    
    // Isso aqui faz com que "Food" utilize aquele deque que te falei, que recebe todos
    // os corpos de cobra.
    void MultiplayerOccupiedUpdate()
    {
        OccupiedCell.clear();
        AddSequenceInOccupiedCell(snake1.body, OccupiedCell);
        AddSequenceInOccupiedCell(snake2.body, OccupiedCell);
    }

    void MultiplayerDraw(){
        snake2.Draw();
        snake1.Draw();
        food.Draw();
    }
 
    void MultiplayerUpdate(){
        if(IsRunning){
   
            
            Movement(snake1);
            Movement(snake2);
            snake1.Update();
            snake2.Update();
            MultiplayerOccupiedUpdate();
            CheckCollisionWithEdges(snake1);
            CheckCollisionWithEdges(snake2);
            CheckCollisionWithBody(snake1);
            CheckCollisionWithBody(snake2); 
            CheckCollisionWithOpp();            
            CheckBodyInOtherBody();
            CheckIfAte(snake1, food);
            CheckIfAte(snake2, food);
        }

        if (IsRunning == false && IsKeyPressed(KEY_SPACE))
        IsRunning = true;

    }

        void MupltiplayerGameOver(){
        snake1.Reset();
        snake2.Reset();
        food.position = food.GenerateRandomPos(OccupiedCell);
        IsRunning = false;
    }

//_________________________________________________________________________

    void CheckIfAte(Snake& snake, Food& food){

        if(Vector2Equals(snake.body[0], food.position)) {
            food.position = food.GenerateRandomPos(OccupiedCell);
            snake.addSegment = true;   
        }
    }

    void CheckCollisionWithEdges(Snake& snake){
        if(snake.body[0].x == cellcount || snake.body[0].x == -1
          || snake.body[0].y < 0 || snake.body[0].y >= cellcount){
            MupltiplayerGameOver();
        }
    }

    void CheckCollisionWithBody(Snake& snake) {
        for (int i = 1; i < snake.body.size(); i++) {
            if (Vector2Equals(snake.body[0], snake.body[i])) {
                MupltiplayerGameOver();
            }
        }
        
    }

    void CheckCollisionWithOpp () 
    {
        for (int i = 1; i < snake2.body.size(); i++) {
            if (Vector2Equals(snake1.body[0], snake2.body[i])) {
                MupltiplayerGameOver();
            }
        }
        
        for (int i = 1; i < snake1.body.size(); i++) {
            if (Vector2Equals(snake2.body[0], snake1.body[i])) {
                MupltiplayerGameOver();
            }
        }
    }

    void CheckBodyInOtherBody()
    {
        for (int i = 1; i < snake2.body.size(); i++) {
            for (int j = 1; j < snake1.body.size(); j++) {
                if (Vector2Equals(snake2.body[i], snake1.body[j])) {
                MupltiplayerGameOver();
            }
            }
            
        }
               
    }

    void Movement (Snake& snake)
    {

        switch (snake.player)
        {
            case 1:
            if (IsKeyDown(KEY_W) && snake.direction.y != 1)
            snake.direction = {0, -1};

            if (IsKeyDown(KEY_S) && snake.direction.y != -1)
            snake.direction = {0, 1};

            if (IsKeyDown(KEY_A) && snake.direction.x != 1)
            snake.direction = {-1, 0};

            if (IsKeyDown(KEY_D) && snake.direction.x != -1)
            snake.direction = {1, 0};
            break;

            case 2:
            if (IsKeyDown(KEY_UP) && snake.direction.y != 1)
            snake.direction = {0, -1};

            if (IsKeyDown(KEY_DOWN) && snake.direction.y != -1)
            snake.direction = {0, 1};

            if (IsKeyDown(KEY_LEFT) && snake.direction.x != 1)
            snake.direction = {-1, 0};

            if (IsKeyDown(KEY_RIGHT) && snake.direction.x != -1)
            snake.direction = {1, 0};
            
        }

       
}



};




int main () {

    cout << "Iniciando o jogo..." << endl;
    InitWindow(2 * offset + cellsize * cellcount, 2 * offset + cellsize * cellcount, "Jogo da Cobrinha");
    SetTargetFPS(60);

    Game game;

    double lastUpdateTime = 0;

    while (!WindowShouldClose()) {

        if (GetTime() - lastUpdateTime >= 0.2) { 
            game.MultiplayerUpdate();
            lastUpdateTime = GetTime();
        }


        BeginDrawing();
        ClearBackground(GREEN);

        game.MultiplayerDraw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
