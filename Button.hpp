#pragma once
#include <raylib.h>

class Button
{
public:
    Button(const char* imagePath, Vector2 position, float width, float height);
    ~Button();

    void Draw();
    bool IsClicked();

private:
    Texture2D texture;
    Rectangle bounds;
};
