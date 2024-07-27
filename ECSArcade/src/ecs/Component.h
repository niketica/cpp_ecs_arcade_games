#pragma once

struct PositionComponent {
    float x;
    float y;
};

struct VelocityComponent {
    float vx;
    float vy;
};

struct GameStatus
{
public:
    bool running = false;
    GameStatus() {}
    GameStatus(bool running) : running(running) {}
};