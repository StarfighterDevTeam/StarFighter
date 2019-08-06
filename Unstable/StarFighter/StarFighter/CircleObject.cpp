#include "CircleObject.h"

CircleObject::CircleObject(AllianceType alliance)
{
	m_alliance = alliance;
	m_speed = sf::Vector2f(0, 0);
	m_garbageMe = false;
	setPointCount(64);
}

CircleObject::~CircleObject()
{

}

void CircleObject::update(sf::Time deltaTime)
{
	m_previous_speed = m_speed;

	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;

	//Basic movement (initial vector)
	newposition.x = getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	setPosition(newposition.x, newposition.y);
}