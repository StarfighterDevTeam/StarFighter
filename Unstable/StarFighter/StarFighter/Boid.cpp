#include "Boid.h"

extern Game* CurrentGame;

using namespace sf;

Boid::Boid()
{
	Boid::Init();
}

Boid::Boid(sf::Vector2f position)
{
	GameObject::Init(position, sf::Vector2f(0, 0), "2D/boid.png", sf::Vector2f(32, 32));
	Boid::Init();
}

void Boid::Init()
{
	m_layer = BoidLayer;
	m_collider_type = BoidObject;

	m_state = Boid_Swimming_Solo;
	m_growth = 100;
	m_flock = NULL;

	//random speed, direction and color
	m_randomized_speed = RandomizeFloatBetweenValues(sf::Vector2f(BOID_BASE_SPEED_MIN, BOID_BASE_SPEED_MAX));
	float angle_ = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	SetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, angle_ * M_PI / 180);

	setRotation(angle_ * 180 / M_PI - 180);

	int r = 0;// RandomizeIntBetweenValues(50, 200);
	int g = 160;// RandomizeIntBetweenValues(50, 255);
	int b = 230;// RandomizeIntBetweenValues(50, 255);
	setColor(sf::Color(r, g, b, 255));
	m_eggs = RandomizeIntBetweenValues(EGG_NB_MIN, EGG_NB_MAX);
	m_egg_cooldown = RandomizeFloatBetweenValues(sf::Vector2f(EGG_LAYING_COOLDOWN_MIN, EGG_LAYING_COOLDOWN_MAX));
	m_change_dir_time = RandomizeFloatBetweenValues(sf::Vector2f(BOID_MIN_CHANGE_DIR_TIME, BOID_MAX_CHANGE_DIR_TIME));

	m_oscillation_offset.x = 0;
	m_oscillation_offset.y = 0;

	//debug circles
	m_debug_flocking_radius.setRadius(FLOCKING_RADIUS);
	m_debug_flocking_radius.setOrigin(sf::Vector2f(m_debug_flocking_radius.getRadius(), m_debug_flocking_radius.getRadius()));
	m_debug_flocking_radius.setOutlineColor(sf::Color::Blue);
	m_debug_flocking_radius.setOutlineThickness(2);
	m_debug_flocking_radius.setFillColor(sf::Color::Transparent);
	m_debug_flocking_radius.setPosition(getPosition());

	m_debug_separation_radius.setRadius(FLOCKING_SEPARATION_RADIUS);
	m_debug_separation_radius.setOrigin(sf::Vector2f(m_debug_separation_radius.getRadius(), m_debug_separation_radius.getRadius()));
	m_debug_separation_radius.setOutlineColor(sf::Color::Green);
	m_debug_separation_radius.setOutlineThickness(2);
	m_debug_separation_radius.setFillColor(sf::Color::Transparent);
	m_debug_separation_radius.setPosition(getPosition());
}

Boid::~Boid()
{
	for (Threat* threat : m_threats)
		delete threat;
}


void Boid::Draw(RenderTarget& screen)
{
	GameObject::Draw(screen);

	if (m_boid_neighbours.empty() == true)
		m_debug_flocking_radius.setOutlineColor(sf::Color::Blue);
	else
		m_debug_flocking_radius.setOutlineColor(sf::Color::Red);

	screen.draw(m_debug_flocking_radius);
	//screen.draw(m_debug_separation_radius);
}

void Boid::update(sf::Time deltaTime)
{
	if (m_state == Boid_Swimming_Solo || m_state == Boid_Swimming_Flocked)
	{
		m_state = m_boid_neighbours.empty() ? Boid_Swimming_Solo : Boid_Swimming_Flocked;
	}

	UpdateThreats();

	if (m_state == Boid_Swimming_Solo || m_state == Boid_Swimming_Flocked)
	{
		//bounce on screen borders
		bool bounced = false;
		if (getPosition().x - m_size.x / 2 < 0 && m_speed.x < 0)
		{
			m_speed.x *= -1;
			bounced = true;
		}
		if (getPosition().x + m_size.x / 2 > REF_WINDOW_RESOLUTION_X  && m_speed.x > 0)
		{
			m_speed.x *= -1;
			bounced = true;
		}
		if (getPosition().y - m_size.y / 2 < 0 && m_speed.y < 0)
		{
			m_speed.y *= -1;
			bounced = true;
		}
		if (getPosition().y + m_size.y / 2 > REF_WINDOW_RESOLUTION_Y && m_speed.y > 0)
		{
			m_speed.y *= -1;
			bounced = true;
		}

		if (!bounced)
		{
			//Flee threats
			sf::Vector2f flee_vector = sf::Vector2f(0, 0);
			float flee_angle = 0;
			if (!m_threats.empty() && IsGrown())//babies don't know how to flee threats
			{
				for (Threat* threat : m_threats)
				{
					//Flee all possible threats at the same time
					flee_vector += Flee(threat->m_pos);
					//flee_angle += FleeByAngle(threat.m_pos);
				}

				//flee_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(FLEEING_MAX_SPEED, flee_angle);
				ScaleSpeed(&flee_vector, FLEEING_MAX_SPEED);
				m_speed = flee_vector;

				//m_speed.x = m_speed.x + avoid_borders.x * FLOCKING_AVOID_BORDERS_WEIGHT;
				//m_speed.y = m_speed.y + avoid_borders.y * FLOCKING_AVOID_BORDERS_WEIGHT;
				NormalizeSpeed(&m_speed, FLEEING_MAX_SPEED);

				//cosmetical: we don't want to change direction straight away after fleeing
				m_change_dir_clock.restart();
			}
			else
			{
				//Flocking
				if (m_state == Boid_Swimming_Flocked)
				{
					//1. cohesion
					m_avg_position = GetAveragePosition();
					sf::Vector2f cohesion_vector = sf::Vector2f(m_avg_position.x - getPosition().x, m_avg_position.y - getPosition().y);
					NormalizeSpeed(&cohesion_vector, m_randomized_speed);

					//2. alignment
					m_avg_speed = GetAverageSpeed();
					sf::Vector2f alignment_vector = sf::Vector2f(m_avg_speed.x - m_speed.x, m_avg_speed.y - m_speed.y);
					NormalizeSpeed(&alignment_vector, m_randomized_speed);

					//3. separation
					sf::Vector2f separation_vector = Separate();
					NormalizeSpeed(&separation_vector, m_randomized_speed);

					//TOTAL
					float new_speed_x = m_speed.x * FLOCKING_PREVIOUS_SPEED_WEIGHT + cohesion_vector.x * FLOCKING_COHESION_WEIGHT + alignment_vector.x * FLOCKING_ALIGNMENT_WEIGHT + separation_vector.x * FLOCKING_SEPARATION_WEIGHT;
					float new_speed_y = m_speed.y * FLOCKING_PREVIOUS_SPEED_WEIGHT + cohesion_vector.y * FLOCKING_COHESION_WEIGHT + alignment_vector.y * FLOCKING_ALIGNMENT_WEIGHT + separation_vector.y * FLOCKING_SEPARATION_WEIGHT;

					//smoothing
					if (GetAngleDegToTargetPosition(getPosition(), getRotation(), sf::Vector2f(getPosition().x + new_speed_x, getPosition().y + new_speed_y)) > 5)
						m_speed = sf::Vector2f(new_speed_x, new_speed_y);

					//applying
					NormalizeSpeed(&m_speed, m_randomized_speed);
				}

				//Change direction randomly
				sf::Vector2f change_dir = sf::Vector2f(0, 0);
				if (m_state == Boid_Swimming_Solo && m_change_dir_clock.getElapsedTime().asSeconds() > m_change_dir_time)
				{
					m_change_dir_time = RandomizeFloatBetweenValues(sf::Vector2f(BOID_MIN_CHANGE_DIR_TIME, BOID_MAX_CHANGE_DIR_TIME));
					m_change_dir_clock.restart();

					int dir = RandomizeIntBetweenValues(0, 1);
					dir = dir == 0 ? -1 : 1;
					float angle_increment = RandomizeFloatBetweenValues(sf::Vector2f(dir*BOID_MIN_CHANGE_DIR_ANGLE, dir*BOID_MAX_CHANGE_DIR_ANGLE));

					//correction to avoid screen borders
					float border_size = 200.0f;
					if (getPosition().x < border_size)
					{
						if (m_speed.y < 0)
							if (angle_increment < 0)
								angle_increment = -angle_increment;

						if (m_speed.y > 0)
							if (angle_increment > 0)
								angle_increment = -angle_increment;
					}

					if (getPosition().x > REF_WINDOW_RESOLUTION_X - border_size)
					{
						if (m_speed.y < 0)
							if (angle_increment > 0)
								angle_increment = -angle_increment;

						if (m_speed.y > 0)
							if (angle_increment < 0)
								angle_increment = -angle_increment;
					}

					if (getPosition().y < border_size)
					{
						if (m_speed.x < 0)
							if (angle_increment > 0)
								angle_increment = -angle_increment;

						if (m_speed.x > 0)
							if (angle_increment < 0)
								angle_increment = -angle_increment;
					}

					if (getPosition().y > REF_WINDOW_RESOLUTION_Y - border_size)
					{
						if (m_speed.x < 0)
							if (angle_increment < 0)
								angle_increment = -angle_increment;

						if (m_speed.x > 0)
							if (angle_increment > 0)
								angle_increment = -angle_increment;
					}
					
					float angle_ = getRotation() + angle_increment;
					change_dir = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, angle_ / 180 * M_PI);

					m_speed.x = change_dir.x;
					m_speed.y = change_dir.y;
				}
			}
		}
	}

	//EggLaying();
	Growing();

	//AvoidBorders(m_speed, deltaTime);

	//oscillation
	bool isOscillating = m_state != Boid_Laying_Egg;// && m_state != Boid_Swimming_Flocked;
	if (isOscillating)
	{
		float growth_ratio = m_growth / 100.f;
		float oscillation_speed = GetAbsoluteSpeed() * BOID_OSCILLATION_FREQUENCY * growth_ratio;
		float angle = GetAngleRadForSpeed(m_speed);
		m_oscillation_offset.x = BOID_OSCILLATION_AMPLITUDE * growth_ratio * sin(m_oscillation_clock.getElapsedTime().asSeconds() * oscillation_speed) * cos(angle);// cos(m_oscillation_clock.getElapsedTime().asSeconds() * oscillation_speed) * cos(angle) + sin(m_oscillation_clock.getElapsedTime().asSeconds() * oscillation_speed) * sin(angle);
		m_oscillation_offset.y = BOID_OSCILLATION_AMPLITUDE * growth_ratio * sin(m_oscillation_clock.getElapsedTime().asSeconds() * oscillation_speed) * sin(angle);// *oscillation_speed);// *sin(angle);// +BOID_OSCILLATION_AMPLITUDE * cos(m_oscillation_clock.getElapsedTime().asSeconds() * oscillation_speed) * cos(angle);

		m_speed += m_oscillation_offset;
	}
	else
	{
		m_oscillation_clock.restart();
	}

	UpdateRotation();

	GameObject::update(deltaTime);

	if (isOscillating)
	{
		m_speed -= m_oscillation_offset;//oscillations are just cosmetical, so they must be ignored from further speed calculations
	}

	//debug circles
	m_debug_flocking_radius.setPosition(getPosition());
	m_debug_separation_radius.setPosition(getPosition());
}

void Boid::AddToBoidNeighbours(GameObject* boid)
{
	if (boid != NULL)
	{
		Boid* new_boid = (Boid*)boid;
			m_boid_neighbours.push_back(new_boid);
	}
}

void Boid::ClearBoidNeighbours()
{
	m_boid_neighbours.clear();
}

void Boid::AddToBoidThreats(GameObject* predator)
{
	bool found = false;
	for (Threat* threat : m_threats)
	{
		//threat already known? then simply refresh the clock
		if (threat->m_object == predator)
		{
			threat->m_clock.restart();
			found = true;
			continue;
		}
	}
	//new threat identified
	if (!found)
	{
		Threat* new_threat = new Threat();
		new_threat->m_object = predator;
		new_threat->m_pos = predator->getPosition();
		new_threat->m_angle = predator->getRotation();

		m_threats.push_back(new_threat);
	}
}

void Boid::AddToBoidThreats(sf::Vector2f pos)
{
	Threat* new_threat = new Threat();
	new_threat->m_object = NULL;
	new_threat->m_pos = pos;
	new_threat->m_object = NULL;

	m_threats.push_back(new_threat);
}

Flock* Boid::GetFlock()
{
	return m_flock;
}

void Boid::SetFlock(Flock* flock)
{
	m_flock = flock;
}

sf::Vector2f Boid::GetAveragePosition()
{
	int size = m_boid_neighbours.size();
	float sum_x = 0;
	float sum_y = 0;
	
	if (m_boid_neighbours.empty())
	{
		return getPosition();
	}

	for (Boid* boid : m_boid_neighbours)
	{
		if (boid)
		{
			sum_x += boid->getPosition().x;
			sum_y += boid->getPosition().y;
		}
	}

	sum_x /= 1.0f * size;
	sum_y /= 1.0f * size;

	return sf::Vector2f(sum_x, sum_y);
}

sf::Vector2f Boid::GetAverageSpeed()
{
	int size = m_boid_neighbours.size();
	float sum_x = 0;
	float sum_y = 0;

	if (m_boid_neighbours.empty())
	{
		return m_speed;
	}

	for (Boid* boid : m_boid_neighbours)
	{
		if (boid)
		{
			sum_x += boid->m_speed.x;
			sum_y += boid->m_speed.y;
		}
	}

	sum_x /= 1.0f * size;
	sum_y /= 1.0f * size;

	return sf::Vector2f(sum_x, sum_y);
}

sf::Vector2f Boid::Separate()
{
	if (m_boid_neighbours.empty())
	{
		return m_speed;
	}

	int count = 0;
	float sum_x = 0;
	float sum_y = 0;

	for (Boid* boid : m_boid_neighbours)
	{
		if (boid)
		{
			if (GetDistanceBetweenObjects(this, boid) < FLOCKING_SEPARATION_RADIUS)
			{
				sf::Vector2f vector = sf::Vector2f(boid->getPosition().x - getPosition().x, boid->getPosition().y - getPosition().y);
				sum_x -= vector.x;
				sum_y -= vector.y;
				count++;
			}
		}
	}

	if (count == 0)
	{
		return m_speed;
	}

	sum_x /= 1.f * count;
	sum_y /= 1.f * count;

	return sf::Vector2f(sum_x, sum_y);
}

sf::Vector2f Boid::Flee(sf::Vector2f threat_pos)
{
	sf::Vector2f flee_vector = sf::Vector2f(getPosition().x - threat_pos.x, getPosition().y - threat_pos.y);

	ScaleSpeed(&flee_vector, FLEEING_MAX_SPEED);
	return flee_vector;
}

float Boid::FleeByAngle(sf::Vector2f threat_pos)
{
	float flee_angle = GetAngleRadBetweenPositions(getPosition(), threat_pos);

	return flee_angle;
}

void Boid::UpdateThreats()
{
	//remanence of threat
	vector<Threat*> updated_threats;
	for (Threat* threat : m_threats)
	{
		if (threat->m_clock.getElapsedTime().asSeconds() < FLEEING_DURATION)
			updated_threats.push_back(threat);
		else
			delete threat;
	}

	m_threats.clear();

	for (Threat* threat : updated_threats)
		m_threats.push_back(threat);
}

void Boid::Growing()
{
	if (m_growth < 100)
	{
		m_growth = 100 * BABY_BOID_SCALE + 100 * (m_growth_clock.getElapsedTime().asSeconds() / BABY_GROWTH_DURATION) * (100 * BABY_BOID_SCALE / 100);

		if (m_growth < 100)
		{
			setScale(sf::Vector2f(1.f * m_growth / 100, 1.f * m_growth / 100));
			m_size.x = 32.f * m_growth / 100;
			m_size.y = 32.f * m_growth / 100;
			//m_diag = (float)sqrt(((m_size.x / 2)*(m_size.x / 2)) + ((m_size.y / 2)*(m_size.y / 2)));
		}
		else
		{
			m_growth = 100;
			setScale(sf::Vector2f(1.f , 1.f));
			m_size.x = 32;
			m_size.y = 32;
			m_diag = (float)sqrt(((m_size.x / 2)*(m_size.x / 2)) + ((m_size.y / 2)*(m_size.y / 2)));

			m_egg_clock.restart();
		}
	}
}

bool Boid::IsGrown()
{
	return m_growth > 75;
}

void Boid::EggLaying()
{
	if (m_growth < 100 || m_eggs == 0 || !m_threats.empty())//condition to start laying egggs
	{
		m_egg_clock.restart();
		return;
	}

	if ((*CurrentGame).m_living_count[Living_Boid] < BOID_MAX_POPULATION)
	{
		if (m_egg_clock.getElapsedTime().asSeconds() > m_egg_cooldown || m_state == Boid_Laying_Egg)//the second condition has purpose when laying egg has already started, then we want to ignore threats
		{
			m_state = Boid_Laying_Egg;
			ScaleSpeed(&m_speed, m_randomized_speed * 0.5);

			if (m_between_two_eggs_clock.getElapsedTime().asSeconds() > EGG_LAYING_TIME_BETWEEN_TWO_EGGS)
			{
				Boid* baby_boid = new Boid(getPosition());
				baby_boid->ScaleObject(BABY_BOID_SCALE);
				baby_boid->setColor(sf::Color(m_color.r, m_color.g, m_color.b, 255));
				baby_boid->m_growth = 100 * BABY_BOID_SCALE;
				float angle = (getRotation() + 180) * M_PI / 180;
				baby_boid->SetSpeedVectorFromAbsoluteSpeedAndAngle(baby_boid->m_randomized_speed, angle);

				(*CurrentGame).addToScene(baby_boid, BoidLayer, BoidObject);

				m_between_two_eggs_clock.restart();
				m_eggs--;

				if (m_eggs == 0)
				{
					m_change_dir_clock.restart();
					SetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, getRotation() * M_PI / 180);
					m_state = Boid_Swimming_Solo;
					m_egg_clock.restart();
				}
			}
		}
		else
		{
			m_between_two_eggs_clock.restart();
		}
	}
}