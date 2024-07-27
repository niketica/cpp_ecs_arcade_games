#include "ECSManager.h"

ECSManager::ECSManager(Entity maxNrOfEntities) : EntityManager(maxNrOfEntities) {}

void ECSManager::removeEntity(Entity entity)
{
    removeComponentsForEntity(entity);
    availableEntities.push(entity);
}

void ECSManager::reset()
{
    resetComponentManager();
    resetEntityManager();
}