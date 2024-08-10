#pragma once

#include "ecs/Entity.h"
#include "ecs/EntityManager.h"
#include "ecs/ComponentManager.h"

class ECSManager : public EntityManager, public ComponentManager
{
public:
    explicit ECSManager(Entity maxNrOfEntities);
    virtual ~ECSManager() = default;

    void removeEntity(Entity entity) override;

    void reset();
};