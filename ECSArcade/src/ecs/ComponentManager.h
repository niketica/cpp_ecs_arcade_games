#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include "Entity.h"

class ComponentManager {
public:
    template<typename T>
    void addComponent(Entity entity, T component) noexcept
    {
        auto shared = std::make_shared<T>(std::move(component));
        components[std::type_index(typeid(T))].emplace(entity, std::move(shared));
    }

    template<typename T>
    void removeComponent(Entity entity) noexcept
    {
        components[std::type_index(typeid(T))].erase(entity);
    }

    void removeComponentsForEntity(Entity entity) noexcept
    {
        for (auto& component : components) {
            component.second.erase(entity);
        }
    }

    template<typename T>
    std::shared_ptr<T> getComponent(Entity entity)
    {
        const auto typeIndex = std::type_index(typeid(T));
        auto it = components.find(typeIndex);
        if (it != components.end()) {
            auto& entityMap = it->second;
            auto entityIt = entityMap.find(entity);
            if (entityIt != entityMap.end()) {
                return std::static_pointer_cast<T>(entityIt->second);
            }
        }
        throw std::invalid_argument("Entity does not have component: " + std::string(typeid(T).name()));
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> getComponents()
    {
        auto entities = getEntitiesWithComponent<T>();
        std::vector<std::shared_ptr<T>> componentsOfType;
        componentsOfType.reserve(entities.size());
        for (Entity e : entities) {
            componentsOfType.push_back(getComponent<T>(e));
        }
        return componentsOfType;
    }

    template<typename T>
    std::vector<Entity> getEntitiesWithComponent() noexcept
    {
        std::vector<Entity> entities;
        const auto typeIndex = std::type_index(typeid(T));
        auto it = components.find(typeIndex);
        if (it != components.end()) {
            const auto& componentMap = it->second;
            entities.reserve(componentMap.size());
            for (const auto& pair : componentMap) {
                entities.push_back(pair.first);
            }
        }
        return entities;
    }

    template<typename T>
    Entity getFirstEntityWithComponent()
    {
        const auto typeIndex = std::type_index(typeid(T));
        auto it = components.find(typeIndex);
        if (it != components.end()) {
            const auto& componentsOfType = it->second;
            if (!componentsOfType.empty()) {
                return componentsOfType.begin()->first;
            }
        }
        throw std::invalid_argument("No entity found with component: " + std::string(typeid(T).name()));
    }

    template<typename T>
    std::shared_ptr<T> getAnyComponent()
    {
        return getComponent<T>(getFirstEntityWithComponent<T>());
    }

protected:
    void resetComponentManager() noexcept
    {
        components.clear();
    }

private:
    using ComponentMap = std::unordered_map<Entity, std::shared_ptr<void>>;
    std::unordered_map<std::type_index, ComponentMap> components;
};
