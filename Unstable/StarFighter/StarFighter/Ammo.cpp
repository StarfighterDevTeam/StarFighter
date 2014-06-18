#include "Ammo.h"

Ammo::Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_damage, FX* m_explosion) : Independant(position, speed,  textureName, size)
{
	damage = m_damage;
	armor = 1;
	shield = 0;
	shield_regen = 0;
	startPattern = false;

	this->explosion = m_explosion;

	//TODO: remove this
	/*
	if (m_ammo_type == AmmoType::LaserBeam)
	{

	sf::Vector2f m_size_texture;
	m_size_texture.x = LASER_WIDTH;
	m_size_texture.x = LASER_HEIGHT;
	Independant(position, speed, LASER_FILENAME, m_size_texture);
	}

	if (m_ammo_type == AmmoType::LaserBeamBlue)
	{
	armor = 1;
	shield = 0;
	shield_regen = 0;
	sf::Vector2f m_size_texture;
	m_size_texture.x = LASERBLUE_WIDTH;
	m_size_texture.x = LASERBLUE_HEIGHT;
	Independant(position, speed, LASERBLUE_FILENAME, m_size_texture);
	}
	*/


}

Ammo* Ammo::Clone()
{
	Ammo* m_ammo = new Ammo(this->getPosition(),this->speed,this->textureName,this->m_size,this->damage, this->explosion);
	return m_ammo;
}

Independant* Ammo::explode()
{
	FX* myFX = this->explosion->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	return (Independant*)myFX;
}

