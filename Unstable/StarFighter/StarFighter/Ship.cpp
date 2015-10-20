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
	m_controllerType = AllControlDevices;

	//TRON SPECIFIC
	m_discoball = NULL;
	discoball_curAngle = -1.f;
	discoball_clockwise = true;

	isFiringButtonReleased = false;
	wasFiringButtonReleased = false;
	isSwitchingButtonReleased = false;

	isTackling = NOT_TACKLING;
	isThrowing = NOT_THROWING;
	isBrawling = NOT_BRAWLING;
	isRecovering = NOT_HIT;

	throw_curSpeedBonus = 0.f;
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

void Ship::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void Ship::update(sf::Time deltaTime)
{
	ManageHitRecovery();

	sf::Vector2f inputs_direction = InputGuy::getDirections(m_controllerType);

	if (!disable_inputs)
	{
		moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		movingX = inputs_direction.x != 0;
		movingY = inputs_direction.y != 0;
	}

	if (isTackling == NOT_TACKLING && isBrawling == NOT_BRAWLING && isRecovering == NOT_HIT)
	{
		GetDirectionInputs(inputs_direction);
		MaxSpeedConstraints();
		IdleDecelleration(deltaTime);
	}
	
	ManageTackle();
	ManageBrawl();
	
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
	ManageFeedbacks();
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

	if (this->getPosition().x > (*CurrentGame).map_size.x - (m_size.x / 2))
	{
		this->setPosition((*CurrentGame).map_size.x - (m_size.x / 2), this->getPosition().y);
		speed.x = 0;
		correction_made = true;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		speed.y = 0;
		correction_made = true;
	}

	if (this->getPosition().y >(*CurrentGame).map_size.y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, (*CurrentGame).map_size.y - (m_size.y / 2));
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
		speed.x -= (speed.x) * deltaTime.asSeconds() * SHIP_DECELERATION_COEF / 100.f;

		if (abs(speed.x) < SHIP_MIN_SPEED)
			speed.x = 0;
	}

	if (!movingY)
	{
		speed.y -= (speed.y) * deltaTime.asSeconds() * SHIP_DECELERATION_COEF / 100.f;

		if (abs(speed.y) < SHIP_MIN_SPEED)
			speed.y = 0;
	}
}

void Ship::GetDirectionInputs(sf::Vector2f inputs_direction)
{
	speed.x += inputs_direction.x * SHIP_ACCELERATION;
	speed.y += inputs_direction.y * SHIP_ACCELERATION;

	//turning toward targeted position
	if (speed.x == 0 && speed.y == 0)
	{
		//do nothing
	}
	else if (speed.x == 0 && speed.y > 0)
	{
		setRotation(180);
	}
	else if (speed.x == 0 && speed.y < 0)
	{
		setRotation(0);
	}
	else if (speed.y == 0 && speed.x > 0)
	{
		setRotation(90);
	}
	else if (speed.y == 0 && speed.x < 0)
	{
		setRotation(270);
	}
	else
	{
		setRotation(SpeedToPolarAngle(speed) * 180 / M_PI);
	}
}

void Ship::MaxSpeedConstraints()
{
	float ship_max_speed = SHIP_MAX_SPEED;

	//bonus given after throwing the discoball
	if (throw_bonus_speed_clock.getElapsedTime().asSeconds() < THROW_BONUS_SPEED_TIMER)
	{
		if (isThrowing == AFTER_THROW)
		{
			if (isTackling == NOT_TACKLING && m_discoball == NULL) //for safety
			{
				ship_max_speed = SHIP_MAX_SPEED + throw_curSpeedBonus;
			}
		}
	}
	else
	{
		isThrowing = NOT_THROWING;
		throw_curSpeedBonus = 0;
	}

	//max speed constraints
	if (abs(speed.x) > ship_max_speed)
	{
		speed.x = speed.x > 0 ? ship_max_speed : -ship_max_speed;
	}
	if (abs(speed.y) > ship_max_speed)
	{
		speed.y = speed.y > 0 ? ship_max_speed : -ship_max_speed;
	}

	//diagonal movement?
	if (abs(speed.x) + abs(speed.y) > ship_max_speed)
	{
		float p = (ship_max_speed / sqrt((speed.x*speed.x) + (speed.y*speed.y)));
		speed.x *= p;
		speed.y *= p;
	}
}

//TRON SPECIFIC
void Ship::GetDiscoball(GameObject* discoball, float angle_collision)
{
	if (m_discoball == NULL)
	{
		if (isRecovering == NOT_HIT)
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

				//setting discoball speed after catch
				//1/ reset speed on catch?
				//m_discoball->discoball_curAngularSpeed = DISCOBALL_BASE_ANGULAR_SPEED;

				//2/ apply a malus?
				m_discoball->discoball_curAngularSpeed -= CARRY_CATCH_ACCELERATION_MALUS;

				//acquisition of the discoball
				m_discoball->carried = true;
				m_discoball->carrier = this;
				discoball_curAngle = angle_collision;
				carrier_clock.restart();

				//checking min and max cap values
				printf("Discoball catched (speed: %f", m_discoball->discoball_curAngularSpeed);
				DiscoballSpeedConstraints();
				printf(" | correction: %f)\n", m_discoball->discoball_curAngularSpeed);

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

				//canceling speed bonus on receiving the ball
				isThrowing = NOT_THROWING;

				(*CurrentGame).PlaySFX(SFX_Catch);
			}
		}
	}
}

void Ship::ManageDiscoball(sf::Time deltaTime)
{
	if (m_discoball != NULL)
	{
		//touching map border results in dropping discoball
		if (m_discoball->getPosition().y > (*CurrentGame).map_size.y - (m_discoball->m_size.y / 2) || m_discoball->getPosition().y < m_discoball->m_size.y / 2
			|| m_discoball->getPosition().x > (*CurrentGame).map_size.x - (m_discoball->m_size.x / 2) || m_discoball->getPosition().x < m_discoball->m_size.x / 2)
		{
			ReleaseDiscoball();
		}
		else
		{
			int cc = discoball_clockwise ? cc = -1 : cc = 1;
			discoball_curAngle = discoball_curAngle - (m_discoball->discoball_curAngularSpeed * cc * deltaTime.asSeconds());

			//angular speed deceleration
			if (carrier_clock.getElapsedTime().asSeconds() > CARRY_TIME_FOR_DECELERATION && m_discoball->discoball_curAngularSpeed > CARRY_BASE_ANGULAR_SPEED)
			{
				if (m_discoball->discoball_curAngularSpeed > CARRY_BASE_ANGULAR_SPEED + CARRY_ANGULAR_DECELERATION)
				{
					m_discoball->discoball_curAngularSpeed -= CARRY_ANGULAR_DECELERATION;
				}
				else
				{
					m_discoball->discoball_curAngularSpeed = CARRY_BASE_ANGULAR_SPEED;
				}

				printf("Discoball decelerated. (speed: %f", m_discoball->discoball_curAngularSpeed);
				DiscoballSpeedConstraints();
				printf(" | correction: %f)\n", m_discoball->discoball_curAngularSpeed);
				carrier_clock.restart();
			}

			//transmitting values to the discoball
			discoball_curAngle = fmod(discoball_curAngle, 2 * M_PI);
			m_discoball->carrier_curAngle = discoball_curAngle;
			m_discoball->carrier_curPosition = getPosition();
		}
	}
}

void Ship::ManageFire()
{
	if (m_discoball != NULL)
	{
		if (InputGuy::isFiring(m_controllerType) && wasFiringButtonReleased)
		{
			ThrowDiscoball();

			isFiringButtonReleased = false;
			wasFiringButtonReleased = false;
		}
	}
}

void Ship::ThrowDiscoball()
{
	if (m_discoball != NULL)
	{
		//throw in rotation direction #optional line
		//discoball_curAngle = (getRotation() + 180.f) * M_PI / 180.f;

		isThrowing = AFTER_THROW;
		throw_bonus_speed_clock.restart();
		throw_curSpeedBonus = m_discoball->discoball_curAngularSpeed * SHIP_MAX_SPEED_MULTIPLIER_AFTER_THROW;

		m_discoball->discoball_curAngularSpeed += CARRY_THROW_ACCELERATION_BONUS;

		//orthogonal launch
		discoball_curAngle += discoball_clockwise ? M_PI_2 : - M_PI_2;

		m_discoball->SetSpeedVectorFromAbsoluteSpeed(m_discoball->discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE, discoball_curAngle);

		m_discoball->carried = false;
		printf("Discoball thrown. (speed: %f)\n", m_discoball->discoball_curAngularSpeed);
		carry_again_clock.restart();

		m_discoball->carrier = NULL;
		m_discoball = NULL;

		(*CurrentGame).PlaySFX(SFX_Throw);
	}
}

void Ship::ReleaseDiscoball(float angularSpeedBonus)
{
	//DISCOBALL_RELEASE_SPEED_RATIO
	if (m_discoball != NULL)
	{
		carry_again_clock.restart();

		//m_discoball->discoball_curAngularSpeed += CARRY_THROW_ACCELERATION_BONUS;
		m_discoball->discoball_curAngularSpeed *= DISCOBALL_RELEASE_SPEED_RATIO;
		m_discoball->discoball_curAngularSpeed += angularSpeedBonus;
		m_discoball->speed.x = -m_discoball->discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE * sin(discoball_curAngle);
		m_discoball->speed.y = m_discoball->discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE * cos(discoball_curAngle);
		m_discoball->carried = false;
		printf("Discoball released. (speed: %f)\n", m_discoball->discoball_curAngularSpeed);
		carry_again_clock.restart();

		m_discoball->carrier = NULL;
		m_discoball = NULL;
	}
}

void Ship::DiscoballSpeedConstraints()
{
	//apply min and max speed caps
	if (m_discoball->discoball_curAngularSpeed < CARRY_BASE_ANGULAR_SPEED)
	{
		m_discoball->discoball_curAngularSpeed = CARRY_BASE_ANGULAR_SPEED;
	}
	else if (m_discoball->discoball_curAngularSpeed > CARRY_MAX_ANGULAR_SPEED)
	{
		m_discoball->discoball_curAngularSpeed = CARRY_MAX_ANGULAR_SPEED;
	}
}

void Ship::ManageSwitchRotation()
{
	if (m_discoball != NULL)
	{
		if (isSwitchingButtonReleased)
		{
			if (InputGuy::isSwitchingRotation(m_controllerType))
			{
				discoball_clockwise = !discoball_clockwise;
				carrier_clock.restart();
				isSwitchingButtonReleased = false;

				(*CurrentGame).PlaySFX(SFX_Switch);
			}
		}
	}
}

void Ship::ManageKeyReleases()
{
	if (!InputGuy::isFiring(m_controllerType))
	{
		isFiringButtonReleased = true;
		wasFiringButtonReleased = true;
	}
	if (!InputGuy::isSwitchingRotation(m_controllerType))
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
					if (InputGuy::isFiring(m_controllerType) && wasFiringButtonReleased)
					{
						isTackling = INITIATE_TACLKE;

						isFiringButtonReleased = false;
						wasFiringButtonReleased = false;

						MaxSpeedConstraints();//avoids to cumulate multiple bonuses
						speed_on_tackling.x = speed.x;
						speed_on_tackling.y = speed.y;

						(*CurrentGame).PlaySFX(SFX_Tackle);
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

			//printf("speed max : %f, %f \n", speed.x, speed.y);
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
				//printf("speed holding");
			}
			else
			{
				isTackling = ENDING_TACKLE;

				//printf("speed end of tackle");
			}
			speed.x *= SHIP_SPEED_PERCENTAGE_ON_HOLDING_TACKLE;
			speed.y *= SHIP_SPEED_PERCENTAGE_ON_HOLDING_TACKLE;

			//printf(" : %f, %f \n", speed.x, speed.y);
		}
	}
		
	//State 3
	if (isTackling == HOLDING_TACKLE)
	{
		if (tackle_max_hold_clock.getElapsedTime().asSeconds() > SHIP_TACKLE_MAX_HOLD_TIME || wasFiringButtonReleased)
		{
			isTackling = ENDING_TACKLE;
			//printf("speed end of tackle : %f, %f \n", speed.x, speed.y);
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
		if ((abs(speed.x) < SHIP_MIN_SPEED_AFTER_TACKLE && abs(speed.x) > 0) || (abs(speed.y) < SHIP_MIN_SPEED_AFTER_TACKLE && abs(speed.y) > 0) || (speed.x == 0 && speed.y == 0))
		{
			isTackling = NOT_TACKLING;
			tackle_again_clock.restart();
			//printf("speed normal : %f, %f \n", speed.x, speed.y);
		}
	}
}

void Ship::ManageHitRecovery()
{
	if (isRecovering == RECOVERING_FROM_BRAWL)
	{
		if (hit_recovery_clock.getElapsedTime().asSeconds() > RECOVERING_FROM_BRAWL_COOLDOWN)
		{
			isRecovering = NOT_HIT;
		}
	}

	if (isRecovering == RECOVERING_FROM_TACKLE)
	{
		if (hit_recovery_clock.getElapsedTime().asSeconds() > RECOVERING_FROM_TACKLE_COOLDOWN)
		{
			isRecovering = NOT_HIT;
		}
	}
}

void Ship::ManageFeedbacks()
{
	if (isTackling != NOT_TACKLING || isBrawling != NOT_BRAWLING)
	{
		setColor(Color(255, 0, 0, 255));
	}
	else if (isRecovering != NOT_HIT)
	{
		setColor(Color(0, 0, 255, 255));
	}
	else if (isThrowing != NOT_THROWING)
	{
		setColor(Color(0, 255, 0, 255));
	}
	else if (m_discoball != NULL)
	{
		setColor(Color(255, 255, 255, 200));
	}
	else
	{
		setColor(Color(255, 255, 255, 255));
	}
}

void Ship::PlayerContact(GameObject* player, float angle_collision)
{
	Ship* player2 = (Ship*)player;

	if (isTackling != NOT_TACKLING || isBrawling != NOT_BRAWLING)
	{
		if (player2->isRecovering == NOT_HIT)
		{
			if (player2->m_discoball != NULL)
			{
				player2->ReleaseDiscoball();
			}

			player2->isRecovering = isTackling != NOT_TACKLING ? RECOVERING_FROM_TACKLE : RECOVERING_FROM_BRAWL;
			player2->hit_recovery_clock.restart();
			player2->speed.x = 0;
			player2->speed.y = 0;
		}
	}

	else if (player2->isTackling != NOT_TACKLING || player2->isBrawling != NOT_BRAWLING)
	{
		if (isRecovering == NOT_HIT)
		{
			if (m_discoball != NULL)
			{
				ReleaseDiscoball();
			}

			isRecovering = player2->isTackling != NOT_TACKLING ? RECOVERING_FROM_TACKLE : RECOVERING_FROM_BRAWL;
			hit_recovery_clock.restart();
			speed.x = 0;
			speed.y = 0;
		}
	}
}

void Ship::ManageBrawl()
{
	//State 0
	if (isBrawling == NOT_BRAWLING)
	{
		if (m_discoball == NULL)
		{
			if (brawl_again_clock.getElapsedTime().asSeconds() > BRAWL_AGAIN_COOLDOWN)
			{
				if (isSwitchingButtonReleased)
				{
					if (InputGuy::isSwitchingRotation(m_controllerType))
					{
						isBrawling = INITIATE_BRAWL;
						brawl_duration_clock.restart();
						isSwitchingButtonReleased = false;

						float angle = getRotation() / 180.f * M_PI;
						SetSpeedVectorFromAbsoluteSpeed(- SHIP_SPEED_ON_BRAWLING, angle);

						(*CurrentGame).PlaySFX(SFX_Switch);
					}
				}
			}
		}
	}

	if (isBrawling == INITIATE_BRAWL)
	{
		if (brawl_duration_clock.getElapsedTime().asSeconds() > SHIP_BRAWL_DURATION)
		{
			speed.x = 0;
			speed.y = 0;

			isBrawling = ENDING_BRAWL;
			brawl_again_clock.restart();
		}
	}

	if (isBrawling == ENDING_BRAWL)
	{
		if (brawl_again_clock.getElapsedTime().asSeconds() > BRAWL_AGAIN_COOLDOWN)
		{
			isBrawling = NOT_BRAWLING;
		}
	}
}