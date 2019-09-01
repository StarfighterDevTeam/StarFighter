#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : SpatialObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
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
	//hit feedback
	if (m_hit_feedback_timer > 0)
		m_hit_feedback_timer -= deltaTime.asSeconds();

	if (m_hit_feedback_timer > 0)
		setColor(sf::Color::Yellow);
	else
		setColor(sf::Color::White);

	//gravity?
	for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[PlanetObject])
	{
		Planet* planet = (Planet*)object;
		float dx = m_position.x - planet->m_position.x;
		float dy = m_position.y - planet->m_position.y;
		if (dx*dx + dy*dy < planet->m_gravity_range * planet->m_gravity_range)
		{
			/*
			bool clockwise = (m_speed.x > 0 && dy < 0) || (m_speed.x < 0 && dy > 0);

			float angle = GetAngleRadForVector(sf::Vector2f(dx, dy));
			angle = deltaTime.asSeconds();
			m_speed.x = planet->m_gravity_range * cos(angle);
			m_speed.y = planet->m_gravity_range * sin(angle);
			*/

			//centrifuge
			/*
			sf::Vector2f centrifuge_force = clockwise == true ? sf::Vector2f(m_speed.y, -m_speed.x) : sf::Vector2f(-m_speed.y, m_speed.x);
			float centrifuge_strenght = planet->m_gravity_strength;
			ScaleVector(&centrifuge_force, centrifuge_strenght);
			m_speed += centrifuge_force * deltaTime.asSeconds();

			//gravity
			float angle = GetAngleRadForVector(sf::Vector2f(dx, dy));
			sf::Vector2f gravity_speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(planet->m_gravity_strength, angle);
			gravity_speed.y = -gravity_speed.y;
			m_speed += gravity_speed * deltaTime.asSeconds();
			printf("gravity");
			*/
		}
	}

	SpatialObject::Update(deltaTime);

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

bool Ship::GetHitByAmmo(GameObject* ammo)
{
	ammo->m_garbageMe = true;
	m_hit_feedback_timer = 0.05;

	return true;
}