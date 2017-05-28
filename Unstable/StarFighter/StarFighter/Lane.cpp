#include "Lane.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Lane::Lane(GameObject* spawner) : GameObject(sf::Vector2f(990, 740), sf::Vector2f(0, 0), sf::Color::Blue, sf::Vector2f(16, 16))
{
	m_spawner = spawner;
	m_lane_angle = 0.f;
}

void Lane::update(sf::Time deltaTime)
{
	sf::Vector2f spawner_pos = m_spawner->getPosition();
	sf::Vector2f offset;
	float rad_angle = -m_lane_angle * M_PI / 180.f;
	rad_angle -= M_PI_2;
	offset.x = LANE_OFFSET_Z * cos(rad_angle);// +LANE_OFFSET_Z * sin(m_lane_angle);
	offset.y = -LANE_OFFSET_Z * sin(rad_angle);// +LANE_OFFSET_Z * cos(m_lane_angle);
	offset.x = offset.x < 0 ? ceil(offset.x) : floor(offset.x);
	offset.y = offset.y < 0 ? ceil(offset.y) : floor(offset.y);

	setPosition(sf::Vector2f(spawner_pos.x + offset.x, spawner_pos.y + offset.y));

	AnimatedSprite::update(deltaTime);
}