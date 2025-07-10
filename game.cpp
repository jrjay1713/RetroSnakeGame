#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;


Color blue = {0,0,225,225};
Color darkGreen = {41,53,21,225};
Color green = {173,204,96,225};

int cellSize = 30;
int cellCount = 25;
int offset = 75;


double lastUpdateTime = 0;



bool elementInDique(Vector2 element, deque<Vector2> deque){
    for(int i = 0; i < deque.size(); i++){
        if(Vector2Equals(deque[i], element)){
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}


class Snake{
    public:
        deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        Vector2 direction = {1,0};

        bool addSegment = false;

        void draw()
        {
            for(unsigned int i = 0; i < body.size(); i++){
                float x = body[i].x;
                float y = body[i].y;
                Rectangle segments = Rectangle{offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};
                DrawRectangleRounded(segments, 0.5, 0.6, darkGreen);

            }
        }

        void update(){
            body.push_front(Vector2Add(body[0], direction));
            if(addSegment == true ){
                
                addSegment = false;
            }else{
                body.pop_back();
            }
        }

        void Reset(){
            body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
            direction = {1,0};
        }

};

class Food{

    public:
        Vector2 position;
        Texture2D texture;

        Food(deque<Vector2> snakeBody){
            Image image = LoadImage("Assets/food.png");
            ImageResize(&image, cellSize, cellSize);
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = GenerateRandomPos(snakeBody);
        }

        ~Food(){
            UnloadTexture(texture);
        }

        void draw()
        {
            DrawTexture(texture, offset + position.x * cellSize, offset + position.y* cellSize, WHITE);
            // DrawRectangle(position.x*cellSize, position.y*cellSize, cellSize, cellSize, darkGreen);
        }

        Vector2 GenerateRandomCell(){
            float x = GetRandomValue(0, cellCount - 1);
            float y = GetRandomValue(0, cellCount - 1);
            return Vector2{x,y};
        }

        Vector2 GenerateRandomPos(deque<Vector2> snakeBody){
            position = GenerateRandomCell();
            while(elementInDique(position, snakeBody)){
                position = GenerateRandomCell();
            }
            return position;
        }

};

class Game
{
    public:
    Snake snake;
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;

    Sound eatSound;
    Sound wallSound;

    Game(){
        InitAudioDevice();
        eatSound = LoadSound("Sounds/eatSound.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }

    ~Game(){
        UnloadSound(wallSound);
        UnloadSound(eatSound);
        CloseAudioDevice();
    }

    void draw(){
        snake.draw();
        food.draw();
    }

    void update()
    {
        if(running)
        {
            snake.update();
            collisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    void collisionWithFood(){
        if(Vector2Equals(snake.body[0], food.position ))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;  
            PlaySound(eatSound); 
            score++ ;
        }
    }

    void CheckCollisionWithEdges()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if(snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
        
    }
    void GameOver()
    {    
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if(elementInDique(snake.body[0], headlessBody))
        {
            PlaySound(wallSound);
            GameOver();
        }

    }

};



int main() {

    InitWindow(2*offset + cellCount*cellSize, 2*offset + cellCount*cellSize, "SnakeGame");
    SetTargetFPS(60);
    cout<<"game started......"<<endl;

    Game game = Game();
    

    while(WindowShouldClose() == false)
    {
        BeginDrawing();
        
        if(eventTriggered(0.2))
        {
            game.update();
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0,1};
            game.running = true;
        }
        else if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0,-1};
            game.running = true;
        }
        else if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1,0};
            game.running = true;
        }
        else if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1,0};
            game.running = true;
        }

        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount + 10, (float)cellSize*cellCount + 10}, 5, darkGreen);
        game.draw();
        DrawText("Retro Snake", offset-5, 20, 40, darkGreen);
        DrawText(TextFormat("%i",game.score), offset - 5, offset + cellSize*cellCount + 10, 40, darkGreen);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
