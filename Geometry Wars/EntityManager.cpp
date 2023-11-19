#include "EntityManager.h"

EntityVec& EntityManager::getEntities() {
    return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag) {
    return m_entityMap[tag];
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
    std::shared_ptr<Entity> entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_toAdd.push_back(entity);
    return entity;
}

void EntityManager::update() {
    for (std::shared_ptr<Entity> e : m_toAdd) {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }
    m_toAdd.clear();

    removeDeadEntities(m_entities);

    for (auto& [tag, entityVec]: m_entityMap) {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec& entityVec) {
    EntityVec newVec;

    for (std::shared_ptr<Entity> e : entityVec) {
        if (e->isActive()) {
            newVec.push_back(e);
        }
    }

    entityVec = newVec;
}
