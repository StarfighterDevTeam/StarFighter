#include "Discoball.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Discoball::Discoball()
{
	
}

Discoball::Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	this->Init();
}

Discoball::Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	this->Init();
}

void Discoball::Init()
{
	collider_type = DiscoballObject;
	carrier = NULL;
	carried = false;
	carrier_curAngle = -1.f;
	carrier_curPosition = sf::Vector2f(-1.f, -1.f);
	DontGarbageMe = true;
	discoball_curAngularSpeed = 0;
	coeff_friction = 0;
	m_status = DiscoballFree;
	setAnimationLine(DiscoballFree);
	is_touching_bumper = false;
}

Discoball::~Discoball()
{
	
}

void Discoball::update(sf::Time deltaTime)
{
	is_touching_bumper = false;

	if (!carried)
	{
		static sf::Vector2f newposition;

		//Basic movement (initial vector)
		newposition.x = this->getPosition().x + speed.x*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + speed.y*deltaTime.asSeconds();

		this->setPosition(newposition.x, newposition.y);

		//Bouncing against screen borders
		bool isBouncing_ = false;
		if (newposition.x < this->m_size.x / 2)
		{
			this->speed.x *= -1;
			this->setPosition(this->m_size.x / 2, newposition.y);
			isBouncing_ = true;
			
		}
		else if (newposition.x > (*CurrentGame).map_size.x - this->m_size.x / 2)
		{
			this->speed.x *= -1;
			this->setPosition((*CurrentGame).map_size.x - this->m_size.x / 2, newposition.y);
			isBouncing_ = true;
		}
		else if (newposition.y < this->m_size.y / 2)
		{
			this->speed.y *= -1;
			this->setPosition(newposition.x, this->m_size.y / 2);
			isBouncing_ = true;
		}
		else if (newposition.y > (*CurrentGame).map_size.y - this->m_size.y / 2)
		{
			this->speed.y *= -1;
			this->setPosition(newposition.x, (*CurrentGame).map_size.y - this->m_size.y / 2);
			isBouncing_ = true;
		}
		//SFX feedback
		if (isBouncing_)
		{
			(*CurrentGame).PlaySFX(SFX_Bounce);
		}

		//friction
		if (USE_FRICTION_ON_DISCOBALL)
		{
			coeff_friction += COEFF_FRICTION_INCREASE_RATIO * COEFF_FRICTION_INCREASE_RATIO;
			speed.x *= (1 - COEFF_FRICTION_INCREASE_RATIO);
			speed.y *= (1 - COEFF_FRICTION_INCREASE_RATIO);
			if (GetAbsoluteSpeed() < DISCOBALL_MIN_SPEED)
			{
				speed.x = 0;
				speed.y = 0;
			}
		}

		setColor(sf::Color(255, 255, 255, 255));
	}

	else //get carrier position
	{
		static sf::Vector2f new_position, new_offset;
		static sf::Vector2f previous_position = getPosition();

		new_offset.x = -DISCOBALL_GRAVITATION_DISTANCE * sin(carrier_curAngle);
		new_offset.y = DISCOBALL_GRAVITATION_DISTANCE * cos(carrier_curAngle);
		//new_offset.x = 0;
		//new_offset.y = 0;

		new_position.x = carrier_curPosition.x + new_offset.x;
		new_position.y = carrier_curPosition.y + new_offset.y;

		setPosition(new_position);

		//useful info to record
		speed.x = new_position.x - previous_position.x;
		speed.y = new_position.y - previous_position.y;

		coeff_friction = 0;
	}

	AnimatedSprite::update(deltaTime);
}

void Discoball::SetDiscoballStatus(DiscoballStatus status)
{
	m_status = status;
	setAnimationLine(m_status, true);
}


void Discoball::DiscoballBumper(GameObject* bumper)
{
	is_touching_bumper = true;

	bool is_vertical_bumper = bumper->m_size.x < bumper->m_size.y;

	const float correction_x = (bumper->m_size.x / 2) + (m_size.x / 2);
	const float correction_y = (bumper->m_size.y / 2) + (m_size.y / 2);

	if (!carried)
	{
		if (is_vertical_bumper)
		{
			if (speed.x >= 0)
			{
				setPosition(sf::Vector2f(bumper->getPosition().x - correction_x, getPosition().y));
			}
			else
			{
				setPosition(sf::Vector2f(bumper->getPosition().x + correction_x, getPosition().y));
			}

			speed.x *= -1;
		}
		else //horizontal bumper
		{
			if (speed.y >= 0)
			{
				setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y - correction_y));
			}
			else
			{
				setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y + correction_y));
			}

			speed.y *= -1;
		}
	}
	else
	{
		if (is_vertical_bumper)
		{
			if (speed.x >= 0)
			{
				setPosition(sf::Vector2f(bumper->getPosition().x + correction_x, getPosition().y));
			}
			else
			{
				setPosition(sf::Vector2f(bumper->getPosition().x - correction_x, getPosition().y));
			}

			speed.x *= -1;
		}
		else //horizontal bumper
		{
			if (speed.y >= 0)
			{
				setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y + correction_y));
			}
			else
			{
				setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y - correction_y));
			}

			speed.y *= -1;
		}
	}
}