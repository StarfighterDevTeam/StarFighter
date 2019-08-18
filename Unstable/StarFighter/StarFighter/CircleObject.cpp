#include "CircleObject.h"

CircleObject::CircleObject(AllianceType alliance, CircleType circle_type)
{
	m_alliance = alliance;
	m_circle_type = circle_type;
	m_color = GameObject::GetAllianceColor(alliance);
	m_speed = sf::Vector2f(0, 0);
	m_garbageMe = false;
	m_visible = true;// alliance == PlayerAlliance;

	setPointCount(CIRCLE_POINTS_COUNT);
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


Wave* CircleObject::CreateWaveBounce(sf::Vector2f position, float radius, float direction, Wave* wave)
{
	//see override function in class L16Entity
	return NULL;
}

void CircleObject::WaveReception(Wave* wave)
{
	//see override function in class L16Entity
}

void CircleObject::Draw(RenderTarget& screen)
{
	//see override function in class Wave
	screen.draw(*this);
}

bool CircleObject::IsColliding(Wave* wave, float direction)
{
	//see override function in class Node
	return false;
}

bool CircleObject::HasBouncedOnEntity(L16Entity* entity)
{
	//see override function in class Wave
	return false;
}

bool CircleObject::IsEvadedEntity(L16Entity* entity)
{
	//see override function in class Wave
	return false;
}

void CircleObject::AddToEvadedEntities(L16Entity* entity)
{
	//see override function in class Wave
}

void CircleObject::RemoveEntity(L16Entity* entity)
{
	//see override function in class Wave
}

void CircleObject::SelectEntity()
{
	//see override function in L16Entity
}

void CircleObject::HoverEntity()
{
	//see override function in L16Entity
}