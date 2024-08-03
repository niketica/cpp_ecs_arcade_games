#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include "ComponentManager.h"

class ECSManager : public EntityManager, public ComponentManager
{
public:
    ECSManager(Entity maxNrOfEntities);

    void removeEntity(Entity entity);

    void reset();
};