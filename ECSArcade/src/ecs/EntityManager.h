#pragma once

#include <iostream>
#include <queue>
#include "Entity.h"

class EntityManager
{
public:
    EntityManager(Entity maxNrOfEntities);

    Entity& addEntity();
    virtual void removeEntity(Entity entity) = 0;

protected:
    Entity maxNrOfEntities;
    /* TODO add scale up / scale down effect of available entities */
    std::queue<Entity> availableEntities;

    void resetEntityManager();
};