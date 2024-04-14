#define maxInputNumbers 2
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
#include "../include/raylib.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Color normalColor = GRAY;
Color hoverColor = SKYBLUE;
Color borderColor = DARKGRAY;
int framesCounter = 0;
int status = 0;

int randGen()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    int lowerBound = 1;
    int upperBound = 99;
    int targetNumber = rand() % (upperBound - lowerBound + 1) + lowerBound;
    return targetNumber;
}

class UI
{
public:
    struct Screen
    {
        enum
        {
            HOMESCREEN,
            PLAYSCREEN,
        };
        int CURRENTSCREEN;
    };
    bool guessed = false;
};

class Game
{
public:
    void playScreen(int, int);
    void homeScreen(int, int);
};
Game game;
UI::Screen sc;
UI u;
int letterCount = 0;
bool mouseOnBox = false;
int tries = 0;
int targetNumber;
void Game::homeScreen(int screenHeight, int screenWidth)
{
    ClearBackground(WHITE);

    const int padding = 70;
    const int buttonWidth = 150;
    const int buttonHeight = 100;

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    DrawText(TextFormat("Number Guessing Game "), 80, 80, 40, BLUE);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(RED));
    if (GuiButton((Rectangle){screenWidth / 2 - 75, screenHeight / 2 - 50, 150, 100}, "Play"))
    {
        std::cout << "Button Clicked! " << std::endl;
        sc.CURRENTSCREEN = sc.PLAYSCREEN;
        tries = 10;
        status = 0;
        targetNumber = randGen();
    }
}
char name[maxInputNumbers + 1] = "";
Rectangle inputBoxRect = {600 / 3, 600 / 2 - 15, 225, 50};
bool hitEnter = false;
void clearCoords(int x, int y)
{
    DrawText(TextFormat(" "), x, y, 20, WHITE);
}
void Game::playScreen(int screenHeight, int screenWidth)
{
    bool mouseOnBox = CheckCollisionPointRec(GetMousePosition(), inputBoxRect);

    if (mouseOnBox)
    {
        SetMouseCursor(MOUSE_CURSOR_ARROW);
        int key = GetCharPressed();
        while (key > 0)
        {
            if ((key >= 48) && (key <= 57) && (strlen(name) < maxInputNumbers))
            {
                char pressedKey[2] = {(char)key, '\0'};
                strncat(name, pressedKey, 1);
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            tries--;

            hitEnter = true;
            int guess = atoi(name);
            strcpy(name, "");
            if (guess > targetNumber)
            {
                status = 1;
            }
            else if (guess < targetNumber)
            {
                status = 2;
            }
            else if (guess == targetNumber)
            {
                status = 3;
            }
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            size_t len = strlen(name);
            if (len > 0)
            {
                name[len - 1] = '\0';
            }
        }
    }
    else
    {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    DrawRectangleRec(inputBoxRect, LIGHTGRAY);
    if (mouseOnBox)
        DrawRectangleLines((int)inputBoxRect.x, (int)inputBoxRect.y, (int)inputBoxRect.width, (int)inputBoxRect.height, RED);
    else
        DrawRectangleLines((int)inputBoxRect.x, (int)inputBoxRect.y, (int)inputBoxRect.width, (int)inputBoxRect.height, DARKGRAY);

    DrawText(TextFormat("%s", name), inputBoxRect.x + 5, inputBoxRect.y + 8, 20, BLACK);

    DrawText(TextFormat("Tries: %02i", tries), 200, 10, 40, BLUE);
    if (!u.guessed)
    {
        DrawText(TextFormat("Number: "), 200, 90, 40, RED);
    }
    else
    {
        DrawText(TextFormat("Number: %d", targetNumber), 200, 90, 40, RED);
    }

    if (status == 3)
    {
        clearCoords(inputBoxRect.x, inputBoxRect.y + 50);
        clearCoords(inputBoxRect.x, inputBoxRect.y + 100);
        DrawText(TextFormat("You Guessed Correctly! %d", targetNumber), inputBoxRect.x, inputBoxRect.y + 100, 20, GREEN);
        if (!u.guessed)
        {
            DrawText(TextFormat("Number: %d", targetNumber), 200, 90, 40, GREEN);
            if (GuiButton((Rectangle){inputBoxRect.x, inputBoxRect.y, inputBoxRect.width, inputBoxRect.height}, "Play Again"))
            {
                std::cout << "HOME! " << std::endl;
                sc.CURRENTSCREEN = sc.PLAYSCREEN;
                tries = 10;
                status = 0;
                targetNumber = randGen();
            }
        }
    }
    if (status == 2)
    {
        clearCoords(inputBoxRect.x, inputBoxRect.y + 50);
        clearCoords(inputBoxRect.x, inputBoxRect.y + 100);
        clearCoords(inputBoxRect.x, inputBoxRect.y + 50);
        clearCoords(inputBoxRect.x, inputBoxRect.y + 100);
        DrawText(TextFormat("Guess Higher! "), inputBoxRect.x, inputBoxRect.y + 100, 20, RED);
    }
    if (status == 1)
    {
        clearCoords(inputBoxRect.x, inputBoxRect.y + 50);
        clearCoords(inputBoxRect.x, inputBoxRect.y + 100);
        DrawText(TextFormat("Guess Lower! "), inputBoxRect.x, inputBoxRect.y + 100, 20, RED);
    }

    if (tries <= 0)
    {
        clearCoords(inputBoxRect.x, inputBoxRect.y + 50);
        clearCoords(inputBoxRect.x, inputBoxRect.y + 100);
        DrawText(TextFormat("No Attempts Left!"), inputBoxRect.x, inputBoxRect.y + 150, 20, RED);
        if (!u.guessed)
        {
            DrawText(TextFormat("Number: %d", targetNumber), 200, 90, 40, GREEN);
            if (GuiButton((Rectangle){inputBoxRect.x, inputBoxRect.y, inputBoxRect.width, inputBoxRect.height}, "Play Again"))
            {
                std::cout << "HOME! " << std::endl;
                sc.CURRENTSCREEN = sc.PLAYSCREEN;
                tries = 10;
                status = 0;
                targetNumber = randGen();
            }
        }
    }
}
