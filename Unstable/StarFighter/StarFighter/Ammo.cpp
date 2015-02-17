#include "Ammo.h"

extern Game* CurrentGame;

Ammo::Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_damage, FX* m_explosion) : Independant(position, speed,  textureName, size)
{
	damage = m_damage;
	armor = 1;
	armor_max = 1;
	shield = 0;
	shield_max = 0;
	shield_regen = 0;
	startPattern = false;
	ref_speed = sqrt(pow(speed.x,2)+pow(speed.y,2));
	explosion = m_explosion;
	radius = 0;
	angspeed = 0;
	range = 0;
	current_range = 0;
}

Ammo* Ammo::Clone()
{
	Ammo* m_ammo = new Ammo(this->getPosition(),this->speed,this->textureName,this->m_size,this->damage, this->explosion);
	m_ammo->display_name = this->display_name;

	m_ammo->Pattern = this->Pattern;
	m_ammo->radius = this->radius;
	m_ammo->angspeed = this->angspeed;
	m_ammo->range = this->range;

	return m_ammo;
}

void Ammo::Death()
{
	FX* myFX = this->explosion->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
    (*CurrentGame).addToScene(myFX, LayerType::ExplosionLayer, IndependantType::Neutral);
}

void Ammo::update(sf::Time deltaTime)
{
	//range before bullet extinction (optional. put "0" not to use)
	if (this->range > 0)
	{
		this->current_range += (ref_speed*deltaTime.asSeconds());
		if (this->current_range > this->range)
		{
			this->visible = false;
			this->GarbageMe;
		}
	}

	//if not disappeared, move it
	if (!this->GarbageMe)
	{
		static sf::Vector2f newposition, offset;

		newposition.x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();

		//call bobbyPattern
		offset = Pattern.GetOffset(deltaTime.asSeconds());
		offset = Independant::getSpeed_for_Direction((*CurrentGame).direction, offset);
		newposition.x += offset.x;
		newposition.y += offset.y;

		this->setPosition(newposition.x, newposition.y);

		AnimatedSprite::update(deltaTime);
	}
}