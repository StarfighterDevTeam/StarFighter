#include "Boid.h"

extern Game* CurrentGame;

using namespace sf;

Boid::Boid()
{

}

Boid::Boid(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	//random speed, direction and color
	float speed_ = RandomizeFloatBetweenValues(sf::Vector2f(FLOCKING_BASE_SPEED_MIN, FLOCKING_BASE_SPEED_MAX));
	float angle_ = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	SetSpeedVectorFromAbsoluteSpeedAndAngle(speed_, angle_);
	m_previous_speed = m_speed;
	setRotation(angle_ * 180 / M_PI - 180);
	int r = RandomizeIntBetweenValues(0, 255);
	int g = RandomizeIntBetweenValues(0, 255);
	int b = RandomizeIntBetweenValues(0, 255);
	setColor(sf::Color(r, g, b, 255));
}

Boid::~Boid()
{
	
}

void Boid::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);

	//bounce on screen borders
	if (getPosition().x - m_size.x / 2 < 0)
	{
		m_speed.x *= -1;
	}
	if (getPosition().x + m_size.x / 2 > REF_WINDOW_RESOLUTION_X)
	{
		m_speed.x *= -1;
	}
	if (getPosition().y - m_size.y / 2 < 0)
	{
		m_speed.y *= -1;
	}
	if (getPosition().y + m_size.y / 2 > REF_WINDOW_RESOLUTION_Y)
	{
		m_speed.y *= -1;
	}

	//Flocking
	//1. cohesion
	m_avg_position = GetAveragePosition();
	sf::Vector2f cohesion_vector = sf::Vector2f(m_avg_position.x - getPosition().x, m_avg_position.y - getPosition().y);
	NormalizeSpeed(&cohesion_vector, FLOCKING_MAX_SPEED);
	

	//2. alignment
	m_avg_speed = GetAverageSpeed();
	sf::Vector2f alignment_vector = sf::Vector2f(m_avg_speed.x - m_speed.x, m_avg_speed.y - m_speed.y);
	NormalizeSpeed(&alignment_vector, FLOCKING_MAX_SPEED);

	//3. separation
	sf::Vector2f separation_vector = Separate();
	NormalizeSpeed(&separation_vector, FLOCKING_MAX_SPEED);

	//Sum up vectors and normalize speed
	m_previous_speed.x = m_speed.x;
	m_previous_speed.y = m_speed.y;

	m_speed.x = m_speed.x * FLOCKING_PREVIOUS_SPEED_WEIGHT + cohesion_vector.x * FLOCKING_COHESION_WEIGHT + alignment_vector.x * FLOCKING_ALIGNMENT_WEIGHT + separation_vector.x * FLOCKING_SEPARATION_WEIGHT;
	m_speed.y = m_speed.y * FLOCKING_PREVIOUS_SPEED_WEIGHT + cohesion_vector.y * FLOCKING_COHESION_WEIGHT + alignment_vector.y * FLOCKING_ALIGNMENT_WEIGHT + separation_vector.y * FLOCKING_SEPARATION_WEIGHT;
	//m_speed.x /= 1.0f + FLOCKING_PREVIOUS_SPEED_WEIGHT + FLOCKING_COHESION_WEIGHT + FLOCKING_ALIGNMENT_WEIGHT + FLOCKING_SEPARATION_WEIGHT;
	//m_speed.y /= 1.0f + FLOCKING_PREVIOUS_SPEED_WEIGHT + FLOCKING_COHESION_WEIGHT + FLOCKING_ALIGNMENT_WEIGHT + FLOCKING_SEPARATION_WEIGHT;

	NormalizeSpeed(&m_speed, FLOCKING_MAX_SPEED);

	float angle = GetAngleRadForSpeed(m_speed);
	setRotation(angle * 180 / M_PI);
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