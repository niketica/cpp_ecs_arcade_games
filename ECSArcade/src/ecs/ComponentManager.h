#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include <iostream>

#include "Entity.h"

class ComponentManager {
public:
    template<typename T>
    void addComponent(Entity entity, T component) {
        auto shared = std::make_shared<T>(component);
        components[std::type_index(typeid(T))][entity] = shared;
    }

    template<typename T>
    void removeComponent(Entity entity) {
        components[std::type_index(typeid(T))].erase(entity);
    }

    void removeComponentsForEntity(Entity entity) {
        for (auto& component : components)
        {
            component.second.erase(entity);
        }
    }

    template<typename T>
    std::shared_ptr<T> getComponent(Entity entity) {
        auto typeIndex = std::type_index(typeid(T));

        auto it = components.find(typeIndex);
        if (it != components.end()) {
            auto& entityMap = it->second;
            auto entityIt = entityMap.find(entity);
            if (entityIt != entityMap.end()) {
                return std::static_pointer_cast<T>(entityIt->second);
            }
        }

        std::cerr << "Entity does not have component: " << typeid(T).name() << std::endl;
        exit(-1);
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> getComponents() {
        auto entities = getEntitiesWithComponent<T>();

        std::vector<std::shared_ptr<T>> components;
        for (Entity e : entities)
        {
            auto component = getComponent<T>(e);
            components.push_back(component);
        }

        return components;
    }

    template<typename T>
    std::vector<Entity> getEntitiesWithComponent() {
        std::vector<Entity> entities;
        auto it = components.find(std::type_index(typeid(T)));

        if (it != components.end()) {
            const auto& componentMap = it->second;
            entities.reserve(componentMap.size());

            for (const auto& pair : componentMap) {
                if (pair.second != nullptr) {
                    entities.push_back(pair.first);
                }
            }
        }

        return entities;
    }

    template<typename T>
    Entity getFirstEntityWithComponent() {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            for (const auto& pair : it->second) {
                return pair.first;
            }
        }
        std::cerr << "No entity found with component: " << typeid(T).name() << std::endl;
        exit(-1);
    }

    template<typename T>
    std::shared_ptr<T> getAnyComponent() {
        Entity e = getFirstEntityWithComponent<T>();
        return getComponent<T>(e);
    }

protected:
    void resetComponentManager()
    {
        components.clear();
    }

private:
    std::unordered_map<std::type_index, std::unordered_map<Entity, std::shared_ptr<void>>> components;
};
