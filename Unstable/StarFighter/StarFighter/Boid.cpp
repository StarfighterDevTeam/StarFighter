#include "Boid.h"

extern Game* CurrentGame;

using namespace sf;

Boid::Boid()
{
	Boid::Init();
}

Boid::Boid(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	Boid::Init();
}

void Boid::Init()
{
	m_state = Boid_Swimming_Solo;
	m_growth = 100;

	//random speed, direction and color
	m_randomized_speed = RandomizeFloatBetweenValues(sf::Vector2f(FLOCKING_BASE_SPEED_MIN, FLOCKING_BASE_SPEED_MAX));
	float angle_ = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	SetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, angle_ * M_PI / 180);
	setRotation(angle_ * 180 / M_PI - 180);
	int r = RandomizeIntBetweenValues(50, 255);
	int g = RandomizeIntBetweenValues(50, 255);
	int b = RandomizeIntBetweenValues(50, 255);
	setColor(sf::Color(r, g, b, 255));
	m_eggs = RandomizeIntBetweenValues(EGG_NB_MIN, EGG_NB_MAX);
	m_change_dir_time = RandomizeFloatBetweenValues(sf::Vector2f(BOID_MIN_CHANGE_DIR_TIME, BOID_MAX_CHANGE_DIR_TIME));
	
}

Boid::~Boid()
{
	m_boid_neighbours.clear();
}

void Boid::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);

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
			//Avoid Borders (if possible in the same way as boid neighbours)
			for (Boid* boid : m_boid_neighbours)
			{
				if (m_avoiding_x != boid->m_avoiding_x && boid->m_avoiding_x != 0)
				{
					m_avoiding_x = boid->m_avoiding_x;
				}
				if (m_avoiding_y != boid->m_avoiding_y && boid->m_avoiding_y != 0)
				{
					m_avoiding_y = boid->m_avoiding_y;
				}
			}
			sf::Vector2f avoid_borders = AvoidBorders();

			//Flee threats
			sf::Vector2f flee_vector = sf::Vector2f(0, 0);
			if (!m_threats.empty() && IsGrown())//babies don't know how to flee threats
			{
				for (Threat threat : m_threats)
				{
					//Flee all possible threats at the same time
					flee_vector += Flee(threat.m_pos);
				}

				ScaleSpeed(&flee_vector, FLEEING_MAX_SPEED);
				m_speed = flee_vector;

				m_speed.x = m_speed.x + avoid_borders.x * FLOCKING_AVOID_BORDERS_WEIGHT;
				m_speed.y = m_speed.y + avoid_borders.y * FLOCKING_AVOID_BORDERS_WEIGHT;
				NormalizeSpeed(&m_speed, FLEEING_MAX_SPEED);

				//cosmetical: we don't want to change direction straight away after fleeing
				m_change_dir_clock.restart();
			}
			else
			{
				//Flocking
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
				m_speed.x = m_speed.x * FLOCKING_PREVIOUS_SPEED_WEIGHT + cohesion_vector.x * FLOCKING_COHESION_WEIGHT + alignment_vector.x * FLOCKING_ALIGNMENT_WEIGHT + separation_vector.x * FLOCKING_SEPARATION_WEIGHT;
				m_speed.y = m_speed.y * FLOCKING_PREVIOUS_SPEED_WEIGHT + cohesion_vector.y * FLOCKING_COHESION_WEIGHT + alignment_vector.y * FLOCKING_ALIGNMENT_WEIGHT + separation_vector.y * FLOCKING_SEPARATION_WEIGHT;

				NormalizeSpeed(&m_speed, m_randomized_speed);

				m_speed.x = m_speed.x + avoid_borders.x * FLOCKING_AVOID_BORDERS_WEIGHT;
				m_speed.y = m_speed.y + avoid_borders.y * FLOCKING_AVOID_BORDERS_WEIGHT;
				NormalizeSpeed(&m_speed, m_randomized_speed);

				//Change direction randomly
				sf::Vector2f change_dir = sf::Vector2f(0, 0);
				if (m_state == Boid_Swimming_Solo && m_change_dir_clock.getElapsedTime().asSeconds() > m_change_dir_time)
				{
					m_change_dir_time = RandomizeFloatBetweenValues(sf::Vector2f(BOID_MIN_CHANGE_DIR_TIME, BOID_MAX_CHANGE_DIR_TIME));
					m_change_dir_clock.restart();

					int dir = RandomizeIntBetweenValues(0, 1);
					dir = dir == 0 ? -1 : 1;
					float angle_ = RandomizeFloatBetweenValues(sf::Vector2f(getRotation() + dir*BOID_MIN_CHANGE_DIR_ANGLE, getRotation() + dir*BOID_MAX_CHANGE_DIR_ANGLE)) - 180;

					change_dir = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, angle_ / 180 * M_PI);

					m_speed.x = change_dir.x;
					m_speed.y = change_dir.y;
				}
			}
		}

		float angle = GetAngleRadForSpeed(m_speed);
		setRotation(angle * 180 / M_PI);
	}

	EggLaying();
	Growing();
}

void Boid::AddToBoidNeighbours(GameObject* boid)
{
	if (boid)
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
	for (Threat threat : m_threats)
	{
		//threat already known? then simply refresh the clock
		if (threat.m_object == predator)
		{
			threat.m_clock.restart();
			found = true;
			continue;
		}
	}
	//new threat identified
	if (!found)
	{
		Threat new_threat;
		new_threat.m_object = predator;
		new_threat.m_pos = predator->getPosition();
		new_threat.m_angle = predator->getRotation();

		m_threats.push_back(new_threat);
	}
}

void Boid::AddToBoidThreats(sf::Vector2f pos)
{
	Threat new_threat;
	new_threat.m_object = NULL;
	new_threat.m_pos = pos;

	m_threats.push_back(new_threat);
	
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

void Boid::UpdateThreats()
{
	//remanence of threat
	vector<Threat> updated_threats;
	for (Threat threat : m_threats)
	{
		if (threat.m_clock.getElapsedTime().asSeconds() < FLEEING_DURATION)
		{
			updated_threats.push_back(threat);
		}
	}
	m_threats = updated_threats;
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
	if (m_growth < 100 || m_eggs == 0 || !m_threats.empty())
	{
		m_egg_clock.restart();
		return;
	}

	if (m_egg_clock.getElapsedTime().asSeconds() > EGG_LAYING_COOLDOWN || m_state == Boid_Laying_Egg)
	{
		if (RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) < EGG_LAYING_CHANCE || m_state == Boid_Laying_Egg)
		{
			m_state = Boid_Laying_Egg;
			m_speed = sf::Vector2f(0, 0);

			if (m_between_two_eggs_clock.getElapsedTime().asSeconds() > EGG_LAYING_TIME_BETWEEN_TWO_EGGS)
			{
				Boid* baby_boid = new Boid(getPosition(), "2D/boid.png", sf::Vector2f(32, 32), sf::Vector2f(16, 16));
				baby_boid->ScaleObject(BABY_BOID_SCALE);
				baby_boid->setColor(sf::Color(m_color.r, m_color.g, m_color.b, 255));
				baby_boid->m_growth = 100 * BABY_BOID_SCALE;
				float angle = (getRotation()) * M_PI / 180;
				baby_boid->SetSpeedVectorFromAbsoluteSpeedAndAngle(baby_boid->m_randomized_speed, angle);

				(*CurrentGame).addToScene(baby_boid, BoidLayer, BoidObject);

				m_between_two_eggs_clock.restart();
				m_eggs--;

				if (m_eggs == 0)
				{
					m_change_dir_clock.restart();
					SetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, (getRotation() - 180) * M_PI / 180);
					m_state = Boid_Swimming_Solo;
				}
			}
		}
		else
		{
			m_egg_clock.restart();
		}
	}
	else
	{
		m_between_two_eggs_clock.restart();
	}

}