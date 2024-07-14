#include "EntityManager.h"

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	for (auto& ent : m_entitiesToAdd)
	{
		m_entities.push_back(ent);
		m_entityMap[ent->tag()].push_back(ent);
	}
	m_entitiesToAdd.clear();
	//TODO: add entities from m_entitiesToAdd to the proper location(s)
	// - add them to the vector of all entities
	// - add them to the vector inside the map, with the tag as a key probably

	// remove dead entities form the vector of all entities
	removeDeadEntities(m_entities);

	//remove dead entities from each vector in the entity map
	//c++20 way of iterating through [key, value] pairs in a map
	for (auto& pair : m_entityMap)
	{
		removeDeadEntities(pair.second);
	}
}

void EntityManager::removeDeadEntities(EntityVec& vec) {
	for (auto it = vec.begin(); it != vec.end();) {
		if (!(*it)->isActive()) {
			it = vec.erase(it); // Remove the dead entity and update the iterator
		}
		else {
			++it; // Move to the next entity
		}
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

	m_entitiesToAdd.push_back(entity);

	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string & tag)
{
	//return the correct vector from the map
	for (auto& search : m_entityMap)
	{
		if (search.first == tag) {
			return search.second;
		}
	}

}

const std::map<std::string, EntityVec>& EntityManager::getEntityMap()
{
	return m_entityMap;
}