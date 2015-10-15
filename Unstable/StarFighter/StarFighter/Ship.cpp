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
	wasFiringButtonReleased = false;
	isSwitchingButtonReleased = false;
	carry_again_clock.restart();

	isTackling = NOT_TACKLING;
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

	if (isTackling == NOT_TACKLING)
	{
		ManageAcceleration(inputs_direction);
		IdleDecelleration(deltaTime);
	}
	
	ManageTackle();
	
	//printf("speed : %f, %f \n", speed.x, speed.y);

	GameObject::update(deltaTime);

	if (ScreenBorderContraints())
	{
		if (isTackling != NOT_TACKLING)
		{
			isTackling = NOT_TACKLING;
		}
	}

	ManageFire();
	ManageSwitchRotation();
	ManageDiscoball(deltaTime);

	ManageKeyReleases();
}

bool Ship::ScreenBorderContraints()
{
	bool correction_made = false;

	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		speed.x = 0;
		correction_made = true;
	}

	if (this->getPosition().x > SCENE_SIZE_X - (m_size.x / 2))
	{
		this->setPosition(SCENE_SIZE_X - (m_size.x / 2), this->getPosition().y);
		speed.x = 0;
		correction_made = true;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		speed.y = 0;
		correction_made = true;
	}

	if (this->getPosition().y > SCENE_SIZE_Y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, SCENE_SIZE_Y - (m_size.y / 2));
		speed.y = 0;
		correction_made = true;
	}

	return correction_made;
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	//idle decceleration
	if (!movingX)
	{
		speed.x -= (speed.x) * deltaTime.asSeconds()* SHIP_DECELERATION_COEF / 100.f;

		if (abs(speed.x) < SHIP_MIN_SPEED)
			speed.x = 0;
	}

	if (!movingY)
	{
		speed.y -= (speed.y)*deltaTime.asSeconds()*SHIP_DECELERATION_COEF / 100.f;

		if (abs(speed.y) < SHIP_MIN_SPEED)
			speed.y = 0;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	speed.x += inputs_direction.x * SHIP_ACCELERATION;
	speed.y += inputs_direction.y * SHIP_ACCELERATION;

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
		if (InputGuy::isFiring() && wasFiringButtonReleased)
		{
			ReleaseDiscoball();

			isFiringButtonReleased = false;
			wasFiringButtonReleased = false;
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
		wasFiringButtonReleased = true;
	}
	if (!InputGuy::isSwitchingRotation())
	{
		isSwitchingButtonReleased = true;
	}
}

void Ship::ManageTackle()
{
	//State 0
	if (isTackling == NOT_TACKLING)
	{
		tackle_curSpeedBonus = 0;

		if (m_discoball == NULL)
		{
			if (tackle_again_clock.getElapsedTime().asSeconds() > TACKLE_AGAIN_COOLDOWN)
			{
				if ((speed.x * speed.x) + (speed.y * speed.y) > SHIP_MIN_SPEED_FOR_TACKLE * SHIP_MIN_SPEED_FOR_TACKLE)
				{
					if (InputGuy::isFiring() && wasFiringButtonReleased)
					{
						isTackling = INITIATE_TACLKE;

						isFiringButtonReleased = false;
						wasFiringButtonReleased = false;

						speed_on_tackling.x = speed.x;
						speed_on_tackling.y = speed.y;
					}
				}
			}
		}
	}

	//State 1
	if (isTackling == INITIATE_TACLKE)
	{
		//max value not reach = linear increase of speed
		if (tackle_curSpeedBonus + SHIP_TACKLE_ACCELERATION_RATIO < SHIP_MAX_TACKLE_SPEED_BONUS_RATIO)
		{
			speed.x += (speed_on_tackling.x * SHIP_TACKLE_ACCELERATION_RATIO);
			speed.y += (speed_on_tackling.y * SHIP_TACKLE_ACCELERATION_RATIO);

			//keeping memory of increases
			tackle_curSpeedBonus += SHIP_TACKLE_ACCELERATION_RATIO;
		}
		//max value reached = changing state
		else
		{
			//capping value
			tackle_curSpeedBonus = SHIP_MAX_TACKLE_SPEED_BONUS_RATIO - tackle_curSpeedBonus;

			speed.x += (speed_on_tackling.x * tackle_curSpeedBonus);
			speed.y += (speed_on_tackling.y * tackle_curSpeedBonus);

			isTackling = MAX_SPEED_TACKLE;
			tackle_min_clock.restart();

			printf("speed max : %f, %f \n", speed.x, speed.y);
		}
	}

	//State 2
	if (isTackling == MAX_SPEED_TACKLE)
	{
		if (tackle_min_clock.getElapsedTime().asSeconds() > SHIP_TACKLE_MIN_LASTING_TIME)
		{
			if (!wasFiringButtonReleased)
			{
				isTackling = HOLDING_TACKLE;
				tackle_max_hold_clock.restart();
				printf("speed holding");
			}
			else
			{
				isTackling = ENDING_TACKLE;

				printf("speed end of tackle");
			}
			speed.x *= SHIP_SPEED_PERCENTAGE_ON_HOLDING_TACKLE;
			speed.y *= SHIP_SPEED_PERCENTAGE_ON_HOLDING_TACKLE;

			printf(" : %f, %f \n", speed.x, speed.y);
		}
	}
		
	//State 3
	if (isTackling == HOLDING_TACKLE)
	{
		if (tackle_max_hold_clock.getElapsedTime().asSeconds() > SHIP_TACKLE_MAX_HOLD_TIME || wasFiringButtonReleased)
		{
			isTackling = ENDING_TACKLE;
			printf("speed end of tackle : %f, %f \n", speed.x, speed.y);
		}
		else
		{
			speed.x *= (1 - SHIP_TACKLE_HOLDDECELERATION_COFF);
			speed.y *= (1 - SHIP_TACKLE_HOLDDECELERATION_COFF);
		}
	}

	//State 4
	if (isTackling == ENDING_TACKLE)
	{
		speed.x *= (1 - SHIP_TACKLE_DECELERATION_COFF);
		speed.y *= (1 - SHIP_TACKLE_DECELERATION_COFF);

		//reached targeted speed for ending the tackle?
		if ((abs(speed.x) < SHIP_MIN_SPEED_AFTER_TACKLE && abs(speed.x) > 0) || (abs(speed.y) < SHIP_MIN_SPEED_AFTER_TACKLE && abs(speed.y) > 0))
		{
			isTackling = NOT_TACKLING;
			tackle_again_clock.restart();
			printf("speed normal : %f, %f \n", speed.x, speed.y);
		}
	}
}