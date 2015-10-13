#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Ship::Ship()
{
	
}

void Ship::Init()
{
	collider_type = GameObjectType::PlayerShip;
	moving = false;
	movingX = movingY = false;
	disable_inputs = false;

	//TRON SPECIFIC
	m_discoball = NULL;
	discoball_curAngle = -1.f;
	discoball_curAngularSpeed = DISCOBALL_BASE_ANGULAR_SPEED;
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	this->Init();
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	this->Init();
}

Ship::~Ship()
{
	
}

void Ship::update(sf::Time deltaTime)
{
	sf::Vector2f inputs_direction = InputGuy::getDirections();

	if (!disable_inputs)
	{
		moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		movingX = inputs_direction.x != 0;
		movingY = inputs_direction.y != 0;
	}


	ManageAcceleration(inputs_direction);
	IdleDecelleration(deltaTime);

	GameObject::update(deltaTime);

	ScreenBorderContraints();

	ManageFire();
	ManageDiscoball(deltaTime);
}

void Ship::ScreenBorderContraints()
{
	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().x > SCENE_SIZE_X - (m_size.x / 2))
	{
		this->setPosition(SCENE_SIZE_X - (m_size.x / 2), this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		speed.y = 0;
	}

	if (this->getPosition().y > SCENE_SIZE_Y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, SCENE_SIZE_Y - (m_size.y / 2));
		speed.y = 0;
	}
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	//idle decceleration
	if (!movingX)
	{
		speed.x -= (speed.x) * deltaTime.asSeconds()* SHIP_DECCELERATION_COEF / 100.f;

		if (abs(speed.x) < SHIP_MIN_SPEED)
			speed.x = 0;
	}

	if (!movingY)
	{
		speed.y -= (speed.y)*deltaTime.asSeconds()*SHIP_DECCELERATION_COEF / 100.f;

		if (abs(speed.y) < SHIP_MIN_SPEED)
			speed.y = 0;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	speed.x += inputs_direction.x* SHIP_ACCELERATION;
	speed.y += inputs_direction.y*SHIP_ACCELERATION;

	//max speed constraints
	if (abs(speed.x) > SHIP_MAX_SPEED)
	{
		speed.x = speed.x > 0 ? SHIP_MAX_SPEED : -SHIP_MAX_SPEED;
	}
	if (abs(speed.y) > SHIP_MAX_SPEED)
	{
		speed.y = speed.y > 0 ? SHIP_MAX_SPEED : -SHIP_MAX_SPEED;
	}
}

//TRON SPECIFIC
void Ship::GetDiscoball(GameObject* discoball, float angle_collision)
{
	if (m_discoball == NULL)
	{
		m_discoball = (Discoball*)discoball;
		m_discoball->carried = true;
		discoball_curAngle = angle_collision;
		discoball_curAngularSpeed = DISCOBALL_BASE_ANGULAR_SPEED;
		carrier_clock.restart();

		LOGGER_WRITE(Logger::Priority::DEBUG, "Discoball catch.\n");
	}
}

void Ship::ManageDiscoball(sf::Time deltaTime)
{
	if (m_discoball != NULL)
	{
		discoball_curAngle = fmod(discoball_curAngle - (discoball_curAngularSpeed * deltaTime.asSeconds()), 2 * M_PI);
		m_discoball->carrier_curAngle = discoball_curAngle;
		m_discoball->carrier_curPosition = getPosition();

		if (carrier_clock.getElapsedTime().asSeconds() > 1 && carrier_clock.getElapsedTime().asSeconds() < CARRY_MAX_TIME_FOR_ACCELERATION)
		{
			discoball_curAngularSpeed += CARRY_ANGULAR_ACCELERATION;
			carrier_clock.restart();
		}

		printf("angle: %f\n", discoball_curAngle * 180.f / M_PI);
	}
}

void Ship::ManageFire()
{
	if (m_discoball != NULL)
	{
		if (InputGuy::isFiring())
		{
			ReleaseDiscoball();
		}
	}
}

void Ship::ReleaseDiscoball()
{
	if (m_discoball != NULL)
	{
		m_discoball->speed.x = - discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE * sin(discoball_curAngle);
		m_discoball->speed.y = discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE * cos(discoball_curAngle);
		m_discoball->carried = false;
		m_discoball = NULL;
	}

	LOGGER_WRITE(Logger::Priority::DEBUG, "Discoball release with speed (%f, %f).\n", m_discoball->speed.x, m_discoball->speed.y);
}