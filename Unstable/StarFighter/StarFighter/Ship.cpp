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
	discoball_clockwise = true;

	isFiringButtonReleased = false;
	isSwitchingButtonReleased = false;
	carry_again_clock.restart();
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
	ManageSwitchRotation();
	ManageDiscoball(deltaTime);

	ManageKeyReleases();
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
		if (carry_again_clock.getElapsedTime().asSeconds() > CARRY_AGAIN_COOLDOWN)
		{
			m_discoball = (Discoball*)discoball;

			//deleting old carrier's discoball ownership
			if (m_discoball->carried)
			{
				m_discoball->carrier->carry_again_clock.restart();
				m_discoball->carrier->m_discoball = NULL;
			}

			//acquisition of the discoball
			m_discoball->carried = true;
			m_discoball->carrier = this;
			discoball_curAngle = angle_collision;
			carrier_clock.restart();

			//1/ reset speed on catch?
			//discoball_curAngularSpeed = DISCOBALL_BASE_ANGULAR_SPEED;

			//2/ or translate cartesian speed into polar speed?
			discoball_curAngularSpeed = m_discoball->cartesian_speed / DISCOBALL_GRAVITATION_DISTANCE;
			discoball_curAngularSpeed -= CARRY_CATCH_ACCELERATION_MALUS;
			printf("Discoball catched (speed: %f", angle_collision, discoball_curAngularSpeed);
			DiscoballSpeedConstraints();
			printf(" | correction: %f)\n", discoball_curAngularSpeed);

			//setting the sense of rotation (clockwise or counter-clockwise)
			if (abs(angle_collision) < M_PI_4)
			{
				m_discoball->speed.x < 0 ? discoball_clockwise = true : discoball_clockwise = false;
			}
			else if (angle_collision >= M_PI_4 && angle_collision < 3.f * M_PI_4)
			{
				m_discoball->speed.y < 0 ? discoball_clockwise = true : discoball_clockwise = false;
			}
			else if (angle_collision >= 3.f * M_PI_4 && angle_collision < 5.f * M_PI_4)
			{
				m_discoball->speed.x > 0 ? discoball_clockwise = true : discoball_clockwise = false;
			}
			else //if (angle_collision >= 5.f * M_PI_4 && angle_collision < 7.f * M_PI_4)
			{
				m_discoball->speed.y > 0 ? discoball_clockwise = true : discoball_clockwise = false;
			}
		}
	}
}

void Ship::ManageDiscoball(sf::Time deltaTime)
{
	if (m_discoball != NULL)
	{
		int cc = discoball_clockwise ? cc = -1 : cc = 1;
		discoball_curAngle = fmod(discoball_curAngle - (discoball_curAngularSpeed * cc * deltaTime.asSeconds()), 2 * M_PI);
		m_discoball->carrier_curAngle = discoball_curAngle;
		m_discoball->carrier_curPosition = getPosition();

		//angular speed acceleration
		if (carrier_clock.getElapsedTime().asSeconds() > 1 && discoball_curAngularSpeed < DISCOBALL_BASE_ANGULAR_SPEED + (CARRY_MAX_TIME_FOR_ACCELERATION * CARRY_ANGULAR_ACCELERATION))
		{
			discoball_curAngularSpeed += CARRY_ANGULAR_ACCELERATION;
			printf("Discoball accelerated. (speed: %f", discoball_curAngularSpeed);
			DiscoballSpeedConstraints();
			printf(" | correction: %f)\n", discoball_curAngularSpeed);
			carrier_clock.restart();
		}

		//printf("angle: %f\n", discoball_curAngle * 180.f / M_PI);
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
		discoball_curAngularSpeed += CARRY_THROW_ACCELERATION_BONUS;
		m_discoball->speed.x = - discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE * sin(discoball_curAngle);
		m_discoball->speed.y = discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE * cos(discoball_curAngle);
		m_discoball->carried = false;
		printf("Discoball released. (speed: %f)\n", discoball_curAngularSpeed);
		carry_again_clock.restart();
		m_discoball->carrier = NULL;
		m_discoball = NULL;

	}
}

void Ship::DiscoballSpeedConstraints()
{
	//apply min and max speed caps
	if (discoball_curAngularSpeed < DISCOBALL_BASE_ANGULAR_SPEED)
	{
		discoball_curAngularSpeed = DISCOBALL_BASE_ANGULAR_SPEED;
	}
	else if (discoball_curAngularSpeed > DISCOBALL_BASE_ANGULAR_SPEED + (CARRY_MAX_TIME_FOR_ACCELERATION * CARRY_ANGULAR_ACCELERATION))
	{
		discoball_curAngularSpeed = DISCOBALL_BASE_ANGULAR_SPEED + (CARRY_MAX_TIME_FOR_ACCELERATION * CARRY_ANGULAR_ACCELERATION);
	}
}

void Ship::ManageSwitchRotation()
{
	if (m_discoball != NULL)
	{
		if (isSwitchingButtonReleased)
		{
			if (InputGuy::isSwitchingRotation())
			{
				discoball_clockwise = !discoball_clockwise;
				carrier_clock.restart();
				isSwitchingButtonReleased = false;
			}
		}
	}
}

void Ship::ManageKeyReleases()
{
	if (!InputGuy::isFiring())
	{
		isFiringButtonReleased = true;
	}
	if (!InputGuy::isSwitchingRotation())
	{
		isSwitchingButtonReleased = true;
	}
}