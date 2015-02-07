#include "Ammo.h"

extern Game* CurrentGame;

Ammo::Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_damage, FX* m_explosion) : Independant(position, speed,  textureName, size)
{
	damage = m_damage;
	armor = 1;
	shield = 0;
	shield_max = 0;
	shield_regen = 0;
	startPattern = false;
	ref_speed = sqrt(pow(speed.x,2)+pow(speed.y,2));
	this->explosion = m_explosion;
	this->radius = 0;
	this->angspeed = 0;
}

Ammo* Ammo::Clone()
{
	Ammo* m_ammo = new Ammo(this->getPosition(),this->speed,this->textureName,this->m_size,this->damage, this->explosion);
	m_ammo->display_name = this->display_name;

	m_ammo->Pattern = this->Pattern;
	m_ammo->radius = this->radius;
	m_ammo->angspeed = this->angspeed;

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
	Independant::update(deltaTime);
}