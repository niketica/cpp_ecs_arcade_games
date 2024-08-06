#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include "ComponentManager.h"

class ECSManager : public EntityManager, public ComponentManager
{
public:
    explicit ECSManager(Entity maxNrOfEntities);
    virtual ~ECSManager() = default;

    void removeEntity(Entity entity) override;

    void reset();
};