#include "Scene.h"

Scene::Scene(Game* game) : game(game) {}

Game* Scene::getGame()
{
    return game;
}