#pragma once

#include "../common/Scene.h"
#include "../../engine/Game.h"
#include "../../common/Vec2.h"
#include "../titlescreen/SceneTitleScreen.h"

enum TetrisAction
{
    TETRIS_DOWN,
    TETRIS_LEFT,
    TETRIS_RIGHT,
    TETRIS_ROTATE
};

struct Block
{
    Vec2 position { 0, 0 };
    sf::Color color{ 0, 0, 0 };
};

struct Shape
{
    int value[4][4];
};

struct Tetromino {
    sf::Color color;
    int shapeIndex = 0;
    Vec2 topLeftPos;
    bool next = false;
    bool active = false;
    Shape shape;
};

struct TetrisStatus
{
    int startingCooldown = 20;
    int movementCooldown = startingCooldown;
    int currentMovementCooldown = 0;
    int rowsCleared = 0;
    bool running = true;
};

enum TetrisTag
{
    TETROMINO,
    TETRIS_BLOCK,
    TETRIS_STATUS
};

class SceneTetris : public Scene
{
public:
    SceneTetris(Game* game);

    void init();
    void input();
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

private:
    const sf::Color windowClearColor = sf::Color(0, 0, 0);

    const static int columns = 10;
    const static int rows = 20;
    const int cellSize = 23;

    const static int nrOfTetrominos = 7;
    const static int shapeSize = 4;
    const int shapes[nrOfTetrominos][shapeSize][shapeSize] = {
        { { 0, 1, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 1, 0, 0 } },

        { { 0, 1, 1, 0 },
          { 0, 1, 1, 0 },
          { 0, 0, 0, 0 },
          { 0, 0, 0, 0 } },

        { { 0, 0, 0, 0 },
          { 1, 1, 1, 0 },
          { 0, 1, 0, 0 },
          { 0, 0, 0, 0 } },

        { { 0, 0, 1, 0 },
          { 0, 0, 1, 0 },
          { 0, 1, 1, 0 },
          { 0, 0, 0, 0 } },

        { { 0, 1, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 1, 1, 0 },
          { 0, 0, 0, 0 } },

        { { 0, 1, 1, 0 },
          { 1, 1, 0, 0 },
          { 0, 0, 0, 0 },
          { 0, 0, 0, 0 } },

        { { 1, 1, 0, 0 },
          { 0, 1, 1, 0 },
          { 0, 0, 0, 0 },
          { 0, 0, 0, 0 } }
    };

    const Tetromino tetrominos[nrOfTetrominos]{
        Tetromino { sf::Color(  0, 255, 255), 0 },
        Tetromino { sf::Color(255, 255,   0), 1 },
        Tetromino { sf::Color(255,   0, 255), 2 },
        Tetromino { sf::Color(  0,   0, 255), 3 },
        Tetromino { sf::Color(255, 129,   0), 4 },
        Tetromino { sf::Color(  0, 255,   0), 5 },
        Tetromino { sf::Color(255,   0,   0), 6 }
    };

    void createActionList();
    void renderBlocks(sf::RenderWindow& window);
    void renderGrid(sf::RenderWindow& window);
    void createNextTetromino();
    void activateNextTetromino();
    bool isCollisionBottom(Tetromino& activeTetromino);
    bool isCollisionHorizontal(Tetromino& activeTetromino, int xOffset);
    bool blockOccupiesPosition(Vec2 pos);
    void rotate(Tetromino& tetromino);
    bool trimFirstRow(Tetromino& tetromino);
    bool trimFirstCol(Tetromino& tetromino);
    void updateGrid(Tetromino& tetromino);
    void processClearRow();
    bool isRowFilled(int y);
    void clearRow(int y);
    void processActions();
};