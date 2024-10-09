#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

Ship::Ship() : Destructible()
{
	m_inputs_direction = sf::Vector2f(0.f, 0.f);
	m_isOrbiting = NULL;
	m_orbit_angle = 0;
	m_range_max = 0;
	m_isReflectingShots = false;
	m_shield_regen_buffer = 0;
	m_energy_regen_buffer = 0;
	m_shield_max = 0;
	m_energy_max = 0;
	m_gravitation_range = 0;

	//UI
	m_shield_circle = new CircleDisplay();
	m_shield_circle->setFillColor(sf::Color::Transparent);
	m_shield_circle->setOutlineThickness(3);
	m_shield_circle->m_target_object = this;
	m_shield_circle->m_circle_type = Circle_Shield;

	m_gravitation_circle = new CircleDisplay();
	m_gravitation_circle->setFillColor(sf::Color(255, 0, 255, 20));
	m_gravitation_circle->setOutlineThickness(3);
	m_gravitation_circle->setOutlineColor(sf::Color(255, 0, 255, 255));
	m_gravitation_circle->setPointCount(256);
	m_gravitation_circle->m_target_object = this;
	m_gravitation_circle->m_circle_type = Circle_Gravitation;

	m_health_container_rect.setFillColor(sf::Color(10, 10, 10, 255));
	m_health_container_rect.setSize(sf::Vector2f(150, 10));
	m_health_container_rect.setOrigin(sf::Vector2f(m_health_container_rect.getSize().x * 0.5f, m_health_container_rect.getSize().y * 0.5f));
	m_health_container_rect.setOutlineThickness(1);
	m_health_container_rect.setOutlineColor(sf::Color::White);

	m_health_rect.setFillColor(sf::Color(0, 200, 0, 255));
	m_health_rect.setOrigin(sf::Vector2f(m_health_container_rect.getSize().x * 0.5f, m_health_container_rect.getSize().y * 0.5f));

	m_shield_container_rect.setFillColor(sf::Color(10, 10, 10, 255));
	m_shield_container_rect.setSize(sf::Vector2f(150, 10));
	m_shield_container_rect.setOrigin(sf::Vector2f(m_shield_container_rect.getSize().x * 0.5f, m_shield_container_rect.getSize().y * 0.5f));
	m_shield_container_rect.setOutlineThickness(1);
	m_shield_container_rect.setOutlineColor(sf::Color::White);

	m_shield_rect.setFillColor(sf::Color(0, 0, 200, 255));
	m_shield_rect.setOrigin(sf::Vector2f(m_shield_container_rect.getSize().x * 0.5f, m_shield_container_rect.getSize().y * 0.5f));

	m_energy_container_rect.setFillColor(sf::Color(10, 10, 10, 255));
	m_energy_container_rect.setSize(sf::Vector2f(150, 10));
	m_energy_container_rect.setOrigin(sf::Vector2f(m_energy_container_rect.getSize().x * 0.5f, m_energy_container_rect.getSize().y * 0.5f));
	m_energy_container_rect.setOutlineThickness(1);
	m_energy_container_rect.setOutlineColor(sf::Color::White);

	m_energy_rect.setFillColor(sf::Color(255, 255, 0, 255));
	m_energy_rect.setOrigin(sf::Vector2f(m_energy_container_rect.getSize().x * 0.5f, m_energy_container_rect.getSize().y * 0.5f));
}

Ship::~Ship()
{
	for (Ammo* ammo : m_shots_fired)
		ammo->m_owner = NULL;

	delete m_shield_circle;
	delete m_gravitation_circle;
}

void Ship::ApplyFlightModel(sf::Time deltaTime, sf::Vector2f inputs_direction)
{
	m_acceleration = 0;
	if (inputs_direction.y < 0)//up
		m_acceleration = m_acceleration_max * inputs_direction.y;

	m_heading += inputs_direction.x * m_turn_speed * deltaTime.asSeconds();

	sf::Vector2f acceleration_vector = GetVectorFromLengthAndRadAngle(m_acceleration, m_heading * M_PI_F / 180.f);

	sf::Vector2f braking_vector = sf::Vector2f(0.f, 0.f);
	float current_inertia_angle = GetAngleRadFromVector(m_speed);

	if (inputs_direction.y > 0)
		braking_vector = GetVectorFromLengthAndRadAngle(m_braking_max, current_inertia_angle);
	else if (inputs_direction.y == 0)
		braking_vector = GetVectorFromLengthAndRadAngle(m_idle_decelleration, current_inertia_angle);

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

		if (dx *dx + dy *dy <= (planet->m_gravity_range + m_size.y * 0.5f) * (planet->m_gravity_range + m_size.y * 0.5f))//range for leaving orbit
		{
			if (m_isOrbiting != NULL || dx *dx + dy *dy <= planet->m_gravity_range * planet->m_gravity_range)//range for entering orbit
			{
				/*
				if (m_isOrbiting == NULL || m_acceleration != 0)
				{
					m_orbit_angle = GetAngleRadFromVector(sf::Vector2f(dx, -dy)) - M_PI_2;
					m_orbit_cw = (m_speed.x > 0 && dy > 0) || (m_speed.x < 0 && dy < 0) ? 1 : -1;//clockwise gravitation
				}
				*/

				m_isOrbiting = planet;
				planet->m_orbit_circle.setOutlineColor(sf::Color(128, 128, 128, 60));

				/*
				m_orbit_angle -= deltaTime.asSeconds() * m_orbit_cw * 2 * M_PI * 1 / planet->m_gravity_period;
				BoundAngle(m_orbit_angle, 2 * M_PI);

				//m_speed = sf::Vector2f(0.f, 0.f);
				sf::Vector2f position;
				position.x = planet->m_position.x + planet->m_gravity_range * cos(m_orbit_angle);
				position.y = planet->m_position.y + planet->m_gravity_range * sin(m_orbit_angle);

				sf::Vector2f speed = sf::Vector2f(3 * (position.x - m_position.x), 3 * (position.y - m_position.y));
				m_position += speed * deltaTime.asSeconds();
				*/
			}
		}
		else if (m_isOrbiting != NULL)
		{
			m_isOrbiting->m_orbit_circle.setOutlineColor(sf::Color(128, 128, 128, 25));
			m_isOrbiting = NULL;
			NormalizeVector(&m_speed, m_speed_max);
		}

		if (m_isOrbiting != NULL)
			break;
	}
}

float Ship::GetRadius(bool include_shield) const
{
	return include_shield == false || m_shield == 0 ? m_radius : m_shield_range;
}

void Ship::Update(sf::Time deltaTime)
{
	//Entering/Leaving planet orbit
	UpdateOrbit(deltaTime);

	//Regen shield
	UpdateShieldRegen(deltaTime);

	//Update health and shield bars size
	m_health_rect.setSize(sf::Vector2f(m_health_container_rect.getSize().x * m_health / m_health_max, m_health_container_rect.getSize().y));
	if (m_shield_max > 0)
		m_shield_rect.setSize(sf::Vector2f(m_shield_container_rect.getSize().x * m_shield / m_shield_max, m_shield_container_rect.getSize().y));

	Destructible::Update(deltaTime);
}

void Ship::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject, true);

		m_stroboscopic_effect_clock.restart();
	}
}

void Ship::GetHitByAmmo(GameObject* ammo, bool send_network_packet)
{
	//Apply damage
	int damage = ((Ammo*)ammo)->m_damage;

	if (m_shield > 0)
		//shield absorbing damage
		if (m_shield > damage)
		{
			m_shield -= damage;
			damage = 0;
			if (m_isReflectingShots == true)
			{
				Ammo* this_ammo = (Ammo*)ammo;
				this_ammo->Bounce(this);
				ammo->m_collider = ammo->m_collider == AllyFire ? EnemyFire : AllyFire;
			}
			else
			{
				//TODO: FX hit shield
				FX* new_FX = new FX(FX_HitShield, ammo->m_position);
				(*CurrentGame).addToScene(new_FX, FX_Layer, BackgroundObject, true);
				ammo->m_garbageMe = true;
			}
			
		}
		//shield destroyed
		else
		{
			damage -= m_shield;
			m_shield = 0;
		}

	//no shield
	if (m_shield <= 0)
		Destructible::GetHitByAmmo(ammo);
}

void Ship::GetHitByObject(GameObject* object)
{
	//Apply damage
	int damage = 0;// ((Destructible*)object)->m_collision_damage;

	if (m_shield > 0)
		//shield absorbing damage
		if (m_shield > damage)
		{
			m_shield -= damage;
			damage = 0;

			//calculate the "exact" point of collision for FX placement
			const float dx = m_position.x- object->m_position.x;
			const float dy = m_position.y -object->m_position.y;
			const float angle = GetAngleRadFromVector(sf::Vector2f(dx, dy));

			FX* new_FX = new FX(FX_HitShield, m_position + GetVectorFromLengthAndRadAngle(m_shield_range, angle));
			(*CurrentGame).addToScene(new_FX, FX_Layer, BackgroundObject, true);
		}
	//shield destroyed
		else
		{
			damage -= m_shield;
			m_shield = 0;
		}

	//no shield
	if (m_shield <= 0)
		Destructible::GetHitByObject(object);
}

void Ship::InitShip()
{
	//ship range and angle of fire max
	m_range_max = 0;
	for (Weapon* weapon : m_weapons)
		m_range_max = MaxBetweenValues(m_range_max, weapon->m_range);

	m_health = m_health_max;
	m_shield = m_shield_max;
	m_energy = m_energy_max;

	//shield circle
	m_shield_circle->setRadius(m_shield_range);
	m_shield_circle->setOrigin(sf::Vector2f(m_shield_range, m_shield_range));
	m_shield_circle->setOutlineColor(m_isReflectingShots == true ? sf::Color(0, 255, 0, 80) : sf::Color(0, 0, 255, 80));

	//gravitation circle
	m_gravitation_circle->setRadius(m_gravitation_range);
	m_gravitation_circle->setOrigin(sf::Vector2f(m_gravitation_range, m_gravitation_range));

	//position of UI bars (health, shield, energy)
	SetPosition(getPosition());
}

void Ship::UpdateShieldRegen(sf::Time deltaTime)
{
	if (m_shield_max == 0 || m_shield == 0)
		return;

	if (m_shield < m_shield_max)
	{
		m_shield_regen_buffer += m_shield_regen * deltaTime.asSeconds();

		double intpart;
		m_shield_regen_buffer = modf(m_shield_regen_buffer, &intpart);
		m_shield += (int)intpart;

		if (m_shield > m_shield_max)
			m_shield = m_shield_max;
	}
}

void Ship::UpdateEnergyRegen(sf::Time deltaTime)
{
	if (m_energy < m_energy_max)
	{
		m_energy_regen_buffer += m_energy_regen * deltaTime.asSeconds();

		double intpart;
		m_energy_regen_buffer = modf(m_energy_regen_buffer, &intpart);
		m_energy += (int)intpart;

		if (m_energy > m_energy_max)
			m_energy = m_energy_max;
	}
}

void Ship::SetPosition(sf::Vector2f position)
{
	GameObject::SetPosition(position);

	m_shield_circle->setPosition(getPosition());
	m_gravitation_circle->setPosition(getPosition());

	m_health_container_rect.setPosition(sf::Vector2f(getPosition().x, getPosition().y - 50.f));
	m_health_rect.setPosition(sf::Vector2f(m_health_container_rect.getPosition().x, m_health_container_rect.getPosition().y));
	
	m_shield_container_rect.setPosition(sf::Vector2f(getPosition().x, m_health_container_rect.getPosition().y - m_health_container_rect.getSize().y * 0.5f - m_health_container_rect.getOutlineThickness() - m_health_container_rect.getSize().y * 0.5f));
	m_shield_rect.setPosition(sf::Vector2f(m_shield_container_rect.getPosition().x, m_shield_container_rect.getPosition().y));
	
	m_energy_container_rect.setPosition(sf::Vector2f(getPosition().x, m_shield_container_rect.getPosition().y - m_shield_container_rect.getSize().y * 0.5f - m_shield_container_rect.getOutlineThickness() - m_shield_container_rect.getSize().y * 0.5f));
	m_energy_rect.setPosition(sf::Vector2f(m_energy_container_rect.getPosition().x, m_energy_container_rect.getPosition().y));
}

void Ship::Draw(RenderTarget& screen)
{
	GameObject::Draw(screen);

	//health and shield
	if (m_visible == true)
	{
		//if (m_shield > 0)
		//	screen.draw(*m_shield_circle);

		if (m_shield_max > 0)
		{
			screen.draw(m_shield_container_rect);
			screen.draw(m_shield_rect);
		}

		screen.draw(m_health_container_rect);
		screen.draw(m_health_rect);
	}
}

float Ship::GetGravitationRange()
{
	return m_gravitation_range;
}

void Ship::SetROE(RuleOfEngagement roe)
{
	if (roe != ROE_FireAtWill || m_weapons.empty() == false)//can't use ROE_FireAtWill without weapons
		SpatialObject::SetROE(roe);
}

bool Ship::HasWeapons()
{
	return m_weapons.empty() == false;
}

void Ship::AddAmmoToShotsFired(Ammo* ammo)
{
	m_shots_fired.push_back(ammo);
}

void Ship::HitWithGravitation(GameObject* object)
{
	SpatialObject* target = (SpatialObject*)object;

	const float dx = target->m_position.x - m_position.x;
	const float dy = target->m_position.y - m_position.y;
	const float angle = GetAngleRadFromVector(sf::Vector2f(dx, dy));
	const float speed_deg = (angle * 180.f / M_PI_F);

	//Apply gravitation only if object is trying to get away from the attractor
	if ((this == (*CurrentGame).m_playerShip || m_roe == ROE_FireAtWill) && m_radius > object->m_radius && abs(GetAngleDegToTargetPosition(object->m_position, speed_deg, m_position)) > 90)
	{
		const float dist_sqr = dx *dx + dy *dy;
		const float range = m_gravitation_range;

		const float strenght = Lerp(dist_sqr, (range * 0.5f) * (range * 0.5f), (range) * (range), 0.f, m_gravitation_strength);
		sf::Vector2f gravity = GetVectorFromLengthAndRadAngle(strenght, angle);

		target->m_speed += gravity;
		NormalizeVector(&target->m_speed, target->m_speed_max);

		//update Rule of engagement if object is an AIShip
		target->GetHitByGravitation();
	}
}

void Ship::Replenish()
{
	m_health = m_health_max;
	m_shield = m_shield_max;
	m_energy = m_energy_max;
}