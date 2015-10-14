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
	collider_type = GameObjectType::DiscoballObject;
	carried = false;
	carrier_curAngle = -1.f;
	carrier_curPosition = sf::Vector2f(-1.f, -1.f);
	DontGarbageMe = true;
}

Discoball::~Discoball()
{
	
}

void Discoball::update(sf::Time deltaTime)
{
	if (!carried)
	{
		static sf::Vector2f newposition;

		//Basic movement (initial vector)
		newposition.x = this->getPosition().x + speed.x*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + speed.y*deltaTime.asSeconds();

		this->setPosition(newposition.x, newposition.y);

		//Bouncing against screen borders
		if (newposition.x < this->m_size.x / 2)
		{
			this->speed.x *= -1;
			this->setPosition(this->m_size.x / 2, newposition.y);
		}
		else if (newposition.x > SCENE_SIZE_X - this->m_size.x / 2)
		{
			this->speed.x *= -1;
			this->setPosition(SCENE_SIZE_X - this->m_size.x / 2, newposition.y);
		}
		else if (newposition.y < this->m_size.y / 2)
		{
			this->speed.y *= -1;
			this->setPosition(newposition.x, this->m_size.y / 2);
		}
		else if (newposition.y > SCENE_SIZE_Y - this->m_size.y / 2)
		{
			this->speed.y *= -1;
			this->setPosition(newposition.x, SCENE_SIZE_Y - this->m_size.y / 2);
		}

		//calculations
		const float a = speed.x;
		const float b = speed.y;
		float s = (a * a) + (b * b);
		s = sqrt(s);
		cartesian_speed = s;

		polar_angle = atan(a / b);

		//printf("cartesian speed: %f, polar angle: %f\n", cartesian_speed, polar_angle);
	}

	else // get carrier position
	{
		static sf::Vector2f new_position, new_offset;

		new_offset.x = -DISCOBALL_GRAVITATION_DISTANCE * sin(carrier_curAngle);
		new_offset.y = DISCOBALL_GRAVITATION_DISTANCE * cos(carrier_curAngle);

		new_position.x = carrier_curPosition.x + new_offset.x;
		new_position.y = carrier_curPosition.y + new_offset.y;

		setPosition(new_position);

	}

	AnimatedSprite::update(deltaTime);
}