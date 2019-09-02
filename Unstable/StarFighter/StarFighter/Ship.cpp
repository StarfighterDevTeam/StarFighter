#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Ship::Ship()
{
	m_hit_feedback_timer = 0;
	m_isOrbiting = NULL;
	m_orbit_angle = 0;
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
	float current_inertia_angle = GetVectorAngleRad(m_speed);

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

void Ship::UpdateOrbit(sf::Time deltaTime)
{
	//gravity?
	for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[PlanetObject])
	{
		if (m_isOrbiting != NULL && m_isOrbiting != object)
			continue;

		Planet* planet = (Planet*)object;
		const float dx = m_position.x - planet->m_position.x;
		const float dy = m_position.y - planet->m_position.y;

		if (dx*dx + dy*dy <= (planet->m_gravity_range + m_size.y * 0.5) * (planet->m_gravity_range + m_size.y * 0.5))//range for leaving orbit
			if (m_isOrbiting != NULL || dx*dx + dy*dy <= planet->m_gravity_range * planet->m_gravity_range)//range for entering orbit
			{
				if (m_isOrbiting == NULL)
				{
					m_orbit_angle = GetVectorAngleRad(sf::Vector2f(dx, -dy)) - M_PI_2;
					m_orbit_cw = (m_speed.x > 0 && dy > 0) || (m_speed.x < 0 && dy < 0) ? 1 : -1;//clockwise gravitation
				}

				m_isOrbiting = planet;

				m_orbit_angle -= deltaTime.asSeconds() * m_orbit_cw * 2 * M_PI * 1 / planet->m_gravity_period;
				BoundAngle(m_orbit_angle, 2 * M_PI);

				//m_speed = sf::Vector2f(0, 0);
				m_position.x = planet->m_position.x + planet->m_gravity_range * cos(m_orbit_angle);
				m_position.y = planet->m_position.y + planet->m_gravity_range * sin(m_orbit_angle);
			}
		else if (m_isOrbiting != NULL)
			m_isOrbiting = NULL;

		if (m_isOrbiting != NULL)
			break;
	}
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

	UpdateOrbit(deltaTime);

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