#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::unordered_map<std::string, EntityVec> EntityMap;

class EntityManager {
private:
	EntityVec	m_entities;
	EntityVec	m_toAdd;
	EntityMap	m_entityMap;
	size_t		m_totalEntities = 0;

public:
	EntityVec& getEntities();
	EntityVec& getEntities(const std::string& tag);
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	void update();

private:
	void removeDeadEntities(EntityVec&);
};
