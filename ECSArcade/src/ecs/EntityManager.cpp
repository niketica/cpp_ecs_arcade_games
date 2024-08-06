#include "EntityManager.h"

EntityManager::EntityManager(Entity maxNrOfEntities) : maxNrOfEntities(maxNrOfEntities)
{
    resetEntityManager();
}

void EntityManager::resetEntityManager()
{
    std::queue<Entity> newEntityQueue;
    for (Entity e = 0; e < maxNrOfEntities; e++)
    {
        newEntityQueue.push(e);
    }

    std::swap(availableEntities, newEntityQueue);
}

Entity EntityManager::addEntity()
{
    if (availableEntities.empty())
    {
        std::cerr << "Max number of entities reached!" << std::endl;
        exit(1);
    }

    Entity e = availableEntities.front();
    availableEntities.pop();

    return e;
}

void EntityManager::makeEntityAvailable(Entity entity)
{
    availableEntities.push(entity);
}