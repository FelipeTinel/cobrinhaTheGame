#include "Button.hpp"

Button::Button(const char* imagePath, Vector2 position, float width, float height)
{
    texture = LoadTexture(imagePath);
    bounds = { position.x, position.y, width, height };
}

Button::~Button()
{
    UnloadTexture(texture);
}

void Button::Draw()
{
    DrawTexturePro(
        texture,
        { 0, 0, (float)texture.width, (float)texture.height },
        bounds,
        { 0, 0 },
        0,
        WHITE
    );
}

bool Button::IsClicked()
{
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec(mouse, bounds) &&
           IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
