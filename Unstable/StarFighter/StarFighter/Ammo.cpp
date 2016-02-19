#include "Ammo.h"

extern Game* CurrentGame;

Ammo::Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_damage, FX* m_explosion) : GameObject(position, speed,  textureName, size)
{
	damage = m_damage;
	armor = 1;
	armor_max = 1;
	shield = 0;
	shield_max = 0;
	shield_regen = 0;
	startPattern = false;
	ref_speed = sqrt((speed.x*speed.x) + (speed.y*speed.y));
	explosion = m_explosion;
	radius = 0;
	angspeed = 0;
	range = 0;
	current_range = 0;
	shot_angle = 0;
	display_name = "Ammo";
}

Ammo* Ammo::Clone()
{
	Ammo* m_ammo = new Ammo(this->getPosition(),this->speed,this->textureName,this->m_size,this->damage, this->explosion);
	m_ammo->display_name = this->display_name;

	m_ammo->Pattern = this->Pattern;
	m_ammo->radius = this->radius;
	m_ammo->angspeed = this->angspeed;
	m_ammo->range = this->range;
	m_ammo->rotation_speed = this->rotation_speed;

	return m_ammo;
}

void Ammo::Death()
{
	FX* myFX = this->explosion->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
    (*CurrentGame).addToScene(myFX, LayerType::ExplosionLayer, GameObjectType::Neutral);

	this->visible = false;
	this->isOnScene = false;
	this->GarbageMe = true;
}

void Ammo::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	sf::Vector2f newspeed = this->speed;
	float new_ref_speed = ref_speed;
	if (hyperspeedMultiplier > 1)
	{
		newspeed.x += GameObject::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier -1) * (*CurrentGame).vspeed).x;
		newspeed.y += GameObject::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier - 1) * (*CurrentGame).vspeed).y;
		new_ref_speed *= hyperspeedMultiplier;
	}
	else if (hyperspeedMultiplier < 1)
	{
		newspeed.x = this->speed.x * hyperspeedMultiplier;
		newspeed.y = this->speed.y * hyperspeedMultiplier;
		new_ref_speed *= hyperspeedMultiplier;
	}

	this->setGhost(hyperspeedMultiplier > 1.0f);

	//range before bullet extinction (optional. put "0" not to use)
	if (this->range > 0)
	{
		this->current_range += (new_ref_speed*deltaTime.asSeconds());
		if (this->current_range > this->range)
		{
			this->visible = false;
			this->GarbageMe = true;
		}
	}

	//if not disappeared, move it
	if (!this->GarbageMe)
	{
		static sf::Vector2f newposition, offset, pattern_offset;
		
		newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

		//call bobbyPattern
		pattern_offset = Pattern.GetOffset(deltaTime.asSeconds());
		offset.x = pattern_offset.x * cos(this->shot_angle) + pattern_offset.y * sin(this->shot_angle);
		offset.y = pattern_offset.x * sin(this->shot_angle) + pattern_offset.y * cos(this->shot_angle);
		//offset = GameObject::getSpeed_for_Direction((*CurrentGame).direction, offset);
		newposition.x += offset.x;
		newposition.y += offset.y;

		this->setPosition(newposition.x, newposition.y);

		//rotation
		this->rotate(this->rotation_speed*deltaTime.asSeconds());

		AnimatedSprite::update(deltaTime);
	}
}