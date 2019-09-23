#include "DestructibleObject.h"

extern Game* CurrentGame;

using namespace sf;

DestructibleObject::DestructibleObject() : SpatialObject()
{
	m_collision_damage = 0;
	m_hit_feedback_timer = 0;
	m_hit_immunity_timer = 0;
}

DestructibleObject::~DestructibleObject()
{
	
}

void DestructibleObject::Update(sf::Time deltaTime)
{
	//hit feedback update
	if (m_hit_feedback_timer > 0)
		m_hit_feedback_timer -= deltaTime.asSeconds();

	if (m_hit_feedback_timer > 0)
		setColor(sf::Color::Yellow);
	else
		setColor(sf::Color::White);

	//hit immunity timer
	if (m_hit_immunity_timer > 0)
		m_hit_immunity_timer -= deltaTime.asSeconds();

	SpatialObject::Update(deltaTime);
}

void DestructibleObject::GetHitByAmmo(GameObject* ammo)
{
	int damage = ((Ammo*)ammo)->m_damage;

	//FX hit
	ammo->m_garbageMe = true;
	FX* new_FX = new FX(FX_Hit, ammo->m_position);
	(*CurrentGame).addToScene(new_FX, FX_Layer, BackgroundObject, true);

	if (damage > 0)
	{
		m_health -= damage;
		if (m_health < 0)
			m_health = 0;
		m_hit_feedback_timer = 0.05;

		//Death?
		if (m_health <= 0)
			Death();
	}
}

void DestructibleObject::GetHitByObject(GameObject* object)
{
	int damage = ((DestructibleObject*)object)->m_collision_damage;

	if (damage > 0 && m_hit_immunity_timer <= 0)
	{
		FX* new_FX = new FX(FX_Hit, m_position);
		(*CurrentGame).addToScene(new_FX, FX_Layer, BackgroundObject, true);

		m_health -= damage;
		if (m_health < 0)
			m_health = 0;
		m_hit_feedback_timer = 0.05;
		m_hit_immunity_timer = 0.5;

		if (m_health <= 0)
			Death();
	}
}

void DestructibleObject::Death()
{
	//garbage object
	m_garbageMe = true;

	//destroyed outside of screen view? => delete from storage
	if (m_removeMe == true)
	{
		(*CurrentGame).m_garbageObjects.push_back(this);

		int id = (*CurrentGame).GetSectorId(m_sector_index);
		vector<GameObject*> tmp_stored_objects;
		for (GameObject* object : (*CurrentGame).m_sceneGameObjectsStored[id])
			if (object != this)
				tmp_stored_objects.push_back(object);

		(*CurrentGame).m_sceneGameObjectsStored[id].clear();
		if (tmp_stored_objects.empty() == false)
			for (GameObject* object : tmp_stored_objects)
				(*CurrentGame).m_sceneGameObjectsStored[id].push_back(object);
	}

	//FX death
	FX* new_FX = new FX(FX_Death, m_position);
	(*CurrentGame).addToScene(new_FX, FX_Layer, BackgroundObject, true);
}