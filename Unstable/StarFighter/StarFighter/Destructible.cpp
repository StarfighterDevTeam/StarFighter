#include "Destructible.h"

extern Game* CurrentGame;

using namespace sf;

Destructible::Destructible() : SpatialObject()
{
	m_collision_damage = 0;
	m_hit_feedback_timer = 0;
	m_hit_immunity_timer = 0;
}

Destructible::~Destructible()
{
	
}

void Destructible::Update(sf::Time deltaTime)
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

void Destructible::GetHitByAmmo(GameObject* ammo)
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

void Destructible::GetHitByObject(GameObject* object)
{
	int damage = ((Destructible*)object)->m_collision_damage;

	if (damage > 0 && m_hit_immunity_timer <= 0)
	{
		//calculate the "exact" point of collision for FX placement
		const float dx = object->m_position.x - m_position.x;
		const float dy = object->m_position.y - m_position.y;
		const float overlap = object->m_radius + m_radius - sqrt(dx*dx + dy*dy);
		sf::Vector2f collision_vector = sf::Vector2f(dx, dy);
		ScaleVector(&collision_vector, m_radius - overlap * 0.5);

		FX* new_FX = new FX(FX_Hit, m_position + collision_vector);
		(*CurrentGame).addToScene(new_FX, FX_Layer, BackgroundObject, true);

		//apply damage
		m_health -= damage;
		if (m_health < 0)
			m_health = 0;
		m_hit_feedback_timer = 0.05;
		m_hit_immunity_timer = 0.5;

		if (m_health <= 0)
			Death();
	}
}

void Destructible::Death()
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

	//create loot
	if (m_hostility == Hostility_Enemy)
		CreateLoot(10);
}

Loot* Destructible::CreateLoot(int money)
{
	Loot* loot = new Loot(money);
	loot->m_position = m_position;
	loot->m_sector_index = GameObject::GetStarSectorIndex(m_position);
	loot->m_speed = sf::Vector2f(m_speed.x * 0.2, m_speed.y * 0.2);

	if ((*CurrentGame).StoreObjectIfNecessary(loot) == false)
		(*CurrentGame).addToScene(loot, loot->m_layer, loot->m_collider, true);

	return loot;
}