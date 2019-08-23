#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_hit_feedback_timer = 0;
}

Ship::~Ship()
{
	
}

void Ship::ApplyFlightModel(sf::Time deltaTime, sf::Vector2f inputs_direction)
{
	m_acceleration = 0;
	if (inputs_direction.y < 0)//up
		m_acceleration = m_max_acceleration * inputs_direction.y;

	m_heading += inputs_direction.x * m_turn_speed * deltaTime.asSeconds();

	sf::Vector2f acceleration_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_acceleration, m_heading * M_PI / 180);

	sf::Vector2f braking_vector = sf::Vector2f(0, 0);
	float current_inertia_angle = GetAngleRadForVector(m_speed);

	if (inputs_direction.y > 0)
		braking_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_max_braking, current_inertia_angle);

	else if (inputs_direction.y == 0)
		braking_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_idle_decelleration, current_inertia_angle);

	braking_vector.x = abs(m_speed.x) > abs(braking_vector.x) ? braking_vector.x : -m_speed.x;//braking cannot exceed speed (that would make us go backward)
	braking_vector.y = abs(m_speed.y) > abs(braking_vector.y) ? braking_vector.y : -m_speed.y;

	m_speed += (acceleration_vector + braking_vector) * deltaTime.asSeconds();
	NormalizeVector(&m_speed, m_speed_max);

	//thruster animation
	m_currentFrame = inputs_direction.y < 0 ? 1 : 0;
}

void Ship::Update(sf::Time deltaTime)
{
	if (m_hit_feedback_timer > 0)
		m_hit_feedback_timer -= deltaTime.asSeconds();

	if (m_hit_feedback_timer > 0)
		setColor(sf::Color::Yellow);
	else
		setColor(sf::Color::White);

	GameObject::Update(deltaTime);
}


void Ship::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		m_stroboscopic_effect_clock.restart();
	}
}

void Ship::GetHitByAmmo(GameObject* ammo)
{
	ammo->m_garbageMe = true;
	m_hit_feedback_timer = 0.05;
}