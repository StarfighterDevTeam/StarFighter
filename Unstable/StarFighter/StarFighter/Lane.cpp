#include "Lane.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Lane::Lane(GameObject* spawner) : GameObject(sf::Vector2f(990, 740), sf::Vector2f(0, 0), "2D/lane.png", sf::Vector2f(400, 32))
								//GameObject(sf::Vector2f(990, 740), sf::Vector2f(0, 0), sf::Color::Blue, sf::Vector2f(400, 32))
{
	m_spawner = spawner;
	m_lane_angle = 0.f;
	m_lane_width = m_size.x;
}

void Lane::update(sf::Time deltaTime)
{
	sf::Vector2f default_pos = sf::Vector2f(m_spawner->getPosition().x, m_spawner->getPosition().y );
	sf::Vector2f offset;
	float rad_angle = -m_lane_angle * M_PI / 180.f;
	offset.x = LANE_OFFSET_Z * sin(rad_angle);
	offset.y = LANE_OFFSET_Z * cos(rad_angle);
	offset.x = offset.x < 0 ? ceil(offset.x) : floor(offset.x);
	offset.y = offset.y < 0 ? ceil(offset.y) : floor(offset.y);

	setPosition(sf::Vector2f(default_pos.x + offset.x, default_pos.y + offset.y));
	setRotation(m_lane_angle);

	AnimatedSprite::update(deltaTime);
}