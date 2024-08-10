#pragma once

#include <iostream>
#include <queue>

#include "ecs/Entity.h"

class EntityManager
{
public:
    explicit EntityManager(Entity maxNrOfEntities);
    virtual ~EntityManager() = default;

    Entity addEntity();
    virtual void removeEntity(Entity entity) = 0;

    void resetEntityManager();
    void makeEntityAvailable(Entity entity);

private:
    Entity maxNrOfEntities;
    std::queue<Entity> availableEntities;
};