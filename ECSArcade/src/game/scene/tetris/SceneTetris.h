#pragma once

#include "../common/Scene.h"
#include "../../engine/Game.h"
#include "../../common/Vec2.h"
#include "../titlescreen/SceneTitleScreen.h"

enum TetrisAction
{
    TETRIS_UP,
    TETRIS_DOWN,
    TETRIS_LEFT,
    TETRIS_RIGHT
};

struct Block
{
    sf::Color color{ 30, 30, 30 };
    Vec2 position;
};

struct Tetromino {
    sf::Color color;
    int shapeIndex = 0;
    Vec2 topLeftPos;
    bool next = false;
    bool active = false;
};

enum TetrisTag
{
    TETROMINO
};

class SceneTetris : public Scene
{
public:
    SceneTetris(Game* game);

    void init();
    void input();
    void update();
    void render(sf::RenderWindow& window);

private:
    const sf::Color windowClearColor = sf::Color(0, 0, 0);

    const static int columns = 10;
    const static int rows = 20;
    const int cellSize = 23;
    const int movementCooldown = 10;
    int currentMovementCooldown = 0;

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

        { { 1, 1, 1, 0 },
          { 0, 1, 0, 0 },
          { 0, 0, 0, 0 },
          { 0, 0, 0, 0 } },

        { { 0, 0, 1, 0 },
          { 0, 0, 1, 0 },
          { 0, 0, 1, 0 },
          { 0, 1, 1, 0 } },

        { { 0, 1, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 1, 1, 0 } },

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
    bool tetrominoOccupiesPosition(Vec2 pos);
    bool tetrominoOccupiesPosition(Tetromino& tetromino, Vec2 pos);
};