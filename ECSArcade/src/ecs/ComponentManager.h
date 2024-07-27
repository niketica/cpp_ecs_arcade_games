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
        // Find the type index of the requested type T
        auto typeIndex = std::type_index(typeid(T));

        // Check if the type index exists in the components map
        auto it = components.find(typeIndex);
        if (it != components.end()) {
            // Find the entity in the nested map
            auto entityMap = it->second;
            auto entityIt = entityMap.find(entity);
            if (entityIt != entityMap.end()) {
                // Cast the stored shared_ptr<void> to shared_ptr<T> and return it
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
            for (const auto& pair : it->second) {
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

    void printComponents()
    {
        std::cout << "####################################################" << std::endl;
        std::cout << "### ECS ComponentManager - printComponents START ###" << std::endl;
        std::cout << "####################################################" << std::endl;

        for (const auto& componentPair : components) {
            std::cout << "Component - " << componentPair.first.name() << std::endl;
            auto map = componentPair.second;
            printComponentValues(map);
            std::cout << std::endl;
        }

        std::cout << "##################################################" << std::endl;
        std::cout << "### ECS ComponentManager - printComponents END ###" << std::endl;
        std::cout << "##################################################" << std::endl;
    }

    void printComponentValues(const std::unordered_map<Entity, std::shared_ptr<void>>& entityComponents) {
        for (const auto& entityComponentPair : entityComponents) {
            Entity entity = entityComponentPair.first;
            std::shared_ptr<void> component = entityComponentPair.second;

            // Example type check and cast; extend this logic as needed for other types
            if (component) {
                if (auto intComponent = std::static_pointer_cast<int>(component)) {
                    std::cout << "Entity " << entity << " has int component with value: " << *intComponent << std::endl;
                }
                else if (auto doubleComponent = std::static_pointer_cast<double>(component)) {
                    std::cout << "Entity " << entity << " has double component with value: " << *doubleComponent << std::endl;
                }
                else {
                    std::cout << "Entity " << entity << " has component of unknown type" << std::endl;
                }
            }
            else {
                std::cout << "Entity " << entity << " has a null component" << std::endl;
            }
        }
    }

protected:
    void resetComponentManager()
    {
        components.clear();
    }

private:
    std::unordered_map<std::type_index, std::unordered_map<Entity, std::shared_ptr<void>>> components;
};
