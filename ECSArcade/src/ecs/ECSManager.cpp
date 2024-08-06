#include "ECSManager.h"

ECSManager::ECSManager(Entity maxNrOfEntities) : EntityManager(maxNrOfEntities) {}

void ECSManager::removeEntity(Entity entity)
{
    removeComponentsForEntity(entity);
    makeEntityAvailable(entity);
}

void ECSManager::reset()
{
    resetComponentManager();
    resetEntityManager();
}