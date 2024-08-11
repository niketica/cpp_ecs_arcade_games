#pragma once

#include <array>
#include <format>
#include <ranges>

#include "game/common/Vec2.h"
#include "game/engine/Game.h"
#include "game/scene/common/Scene.h"
#include "game/scene/common/RandomNumberGenerator.h"
#include "game/scene/titlescreen/SceneTitleScreen.h"

enum class TetrisAction
{
    TETRIS_DOWN,
    TETRIS_LEFT,
    TETRIS_RIGHT,
    TETRIS_ROTATE
};

enum class TetrisTag
{
    TETROMINO,
    TETRIS_BLOCK,
    TETRIS_STATUS
};

struct Block
{
    Vec2 position { 0, 0 };
    sf::Color color{ 0, 0, 0 };
};

struct GridBlock
{
    sf::VertexArray& vArray;
    int width = 1;
    int x = 0;
    int y = 0;
    int xOffset = 0;
    int yOffset = 0;
    int r = 0;
    int g = 0;
    int b = 0;
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

class SceneTetris : public Scene
{
public:
    explicit SceneTetris(Game* game);

    void init() override;
    void input() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    const sf::Color windowClearColor = sf::Color(0, 0, 0);

    const RandomNumberGenerator rand = {};

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

    Entity getEntityWithTag(TetrisTag tag);
    std::vector<Entity> getEntitiesWithTag(TetrisTag tag);

    void createActionList();
    void createNextTetromino();
    void activateNextTetromino();
    bool isCollisionBottom(const Tetromino& activeTetromino);
    bool isCollisionHorizontal(const Tetromino& activeTetromino, const int xOffset);
    bool blockOccupiesPosition(Vec2 pos);
    void rotate(Tetromino& tetromino);
    void trimFirstRow(Tetromino& tetromino);
    void trimFirstCol(Tetromino& tetromino);
    void updateGrid(const Tetromino& tetromino);
    void processClearRow();
    bool isRowFilled(int y);
    void clearRow(int y);
    void processActions();
    void setBlockColor(const GridBlock& block) const;

    void renderGridLines(sf::RenderWindow& window) const;
    void renderGrid(sf::RenderWindow& window);
    void renderNextTetromino(sf::RenderWindow& window);
    void renderScore(sf::RenderWindow& window);
};