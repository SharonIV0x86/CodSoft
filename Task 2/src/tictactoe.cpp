#define RAYGUI_IMPLEMENTATION
#include "./game.hpp"
int main(void)
{
    
    const int screenWidth = 600;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Tic Tac Toe");

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24); 
    sc.CURRENTSCREEN = UI::Screen::HOMESCREEN;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);
        switch (sc.CURRENTSCREEN)
        {
        case UI::Screen::HOMESCREEN:
            game.homeScreen(screenHeight, screenWidth);
            break;
        case UI::Screen::PLAYSCREEN:
            game.playScreen(screenHeight, screenWidth);
        default:
            break;
        }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
