#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Ship::Ship()
{
	
}

void Ship::Init()
{
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
	isDodgingButtonReleased = false;

	isTackling = NOT_TACKLING;
	isThrowing = NOT_THROWING;
	isBrawling = NOT_BRAWLING;
	isDodging = NOT_DODGING;
	isRecovering = NOT_HIT;

	throw_curSpeedBonus = 0.f;

	m_team = BlueTeam;
	isLaunchingScript = false;
	m_character = Natalia;

	arrived_at_destination = true;

	//IA SPECIFIC
	m_target_opponent = NULL;
	m_target_team_mate = NULL;
	m_isUnmarked = true;
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
	//IA SPECIFIC
	SetTargetOpponent();
	m_isUnmarked = IsUnmarked();

	//ManageHitRecovery();

	m_input_direction = sf::Vector2f(0, 0);

	if (!disable_inputs)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_controllerType == AllControlDevices)
		{
			sf::Vector2i click_position = sf::Mouse::getPosition(*(*CurrentGame).getMainWindow());
			sf::Vector2f target_position = (*CurrentGame).mainScreen.mapPixelToCoords(click_position);
			target_position.x /= (*CurrentGame).scale_factor.x;
			target_position.y /= (*CurrentGame).scale_factor.y;
			m_destination = target_position;
			//m_input_direction = GetInputsToGetPosition(m_destination, deltaTime);
		}
		else
		{
			if (!arrived_at_destination)
			{
				m_input_direction = GetInputsToGetPosition(m_destination, deltaTime);
			}
			else
			{
				m_input_direction = InputGuy::getDirections(m_controllerType);
			}
		}

		moving = m_input_direction.x != 0 || m_input_direction.y != 0;
		movingX = m_input_direction.x != 0;
		movingY = m_input_direction.y != 0;
	}

	if (isTackling == NOT_TACKLING && isBrawling == NOT_BRAWLING && isRecovering == NOT_HIT && isDodging != INITIATE_DODGING)
	{
		GetDirectionInputs(m_input_direction);
		MaxSpeedConstraints();
		IdleDecelleration(deltaTime);
	}

	//ManageDodge();
	
	//if (!InputGuy::isStraffing(m_controllerType))
	//{
	UpdateRotation();
	//}
	
	ManageTackle();
	//ManageBrawl();
	
	//printf("speed : %f, %f \n", speed.x, speed.y);

	GameObject::update(deltaTime);

	if (ScreenBorderContraints())
	{
		ResetStatus();
	}

	if (isDodging == NOT_DODGING)
	{
		ManageFire();
	}

	ManageSwitchRotation();
	
	ManageDiscoball(deltaTime);
	ManageKeyReleases();
	ManageFeedbacks();

	//stroboscopic effect
	if (GetAbsoluteSpeed() > SHIP_MAX_SPEED)
	{
		PlayStroboscopicEffect(seconds(TACKLE_STROBO_EFFECT_DURATION * GetAbsoluteSpeed() / SHIP_TACKLE_MAX_SPEED), seconds(STROBO_EFFECT_TIME_BETWEEN_POSES));
	}	
}

void Ship::ResetStatus()
{
	isTackling = NOT_TACKLING;
	isBrawling = NOT_BRAWLING;
	isDodging = NOT_DODGING;
	ghost = false;
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
}

void Ship::UpdateRotation()
{
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
		setRotation((SpeedToPolarAngle(speed) * 180 / (float)M_PI));
	}
}

float Ship::GetMaxSpeed()
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

	return ship_max_speed;
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
	NormalizeSpeed(&speed, ship_max_speed);
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

					//game mode specific
					if ((*CurrentGame).cur_GameRules == CarryToAbleInputs)
					{
						m_discoball->carrier->disable_inputs = true;
						m_discoball->carrier->speed.x = 0;
						m_discoball->carrier->speed.y = 0;
						disable_inputs = false;
						(*CurrentGame).playerShip = this;
					}

					m_discoball->carrier->m_discoball = NULL;
				}
				else
				{
					m_discoball->discoball_curAngularSpeed -= CARRY_CATCH_ACCELERATION_MALUS;
				}

				//acquisition of the discoball
				m_discoball->carried = true;
				m_discoball->carrier = this;
				discoball_curAngle = angle_collision;
				carrier_clock.restart();

				//checking min and max cap values
				//printf("Discoball catched (speed: %f", m_discoball->discoball_curAngularSpeed);
				DiscoballSpeedConstraints();
				//printf(" | correction: %f)\n", m_discoball->discoball_curAngularSpeed);

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
				if (m_team == BlueTeam)
				{
					m_discoball->SetDiscoballStatus(DiscoballCarriedBlueTeam);
				}
				if (m_team == RedTeam)
				{
					m_discoball->SetDiscoballStatus(DiscoballCarriedRedTeam);
				}
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
		else if (m_discoball->is_touching_bumper)
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

				//printf("Discoball decelerated. (speed: %f", m_discoball->discoball_curAngularSpeed);
				DiscoballSpeedConstraints();
				//printf(" | correction: %f)\n", m_discoball->discoball_curAngularSpeed);
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
		//printf("Discoball thrown. (speed: %f)\n", m_discoball->discoball_curAngularSpeed);
		carry_again_clock.restart();

		m_discoball->SetDiscoballStatus(DiscoballFree);

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

		//cancel possible ghost status
		m_discoball->ghost = false;

		//m_discoball->discoball_curAngularSpeed += CARRY_THROW_ACCELERATION_BONUS;
		m_discoball->discoball_curAngularSpeed *= DISCOBALL_RELEASE_SPEED_RATIO;
		m_discoball->discoball_curAngularSpeed += angularSpeedBonus;
		m_discoball->speed.x = -m_discoball->discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE * sin(discoball_curAngle);
		m_discoball->speed.y = m_discoball->discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE * cos(discoball_curAngle);
		m_discoball->carried = false;
		//printf("Discoball released. (speed: %f)\n", m_discoball->discoball_curAngularSpeed);
		carry_again_clock.restart();

		m_discoball->SetDiscoballStatus(DiscoballLost);

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
				SwitchRotation();
			}
		}
	}
}

void Ship::SwitchRotation()
{
	discoball_clockwise = !discoball_clockwise;
	carrier_clock.restart();
	isSwitchingButtonReleased = false;

	(*CurrentGame).PlaySFX(SFX_Switch);
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
	if (!InputGuy::isDodging(m_controllerType))
	{
		isDodgingButtonReleased = true;
		wasDodgingButtonReleased = true;
	}
}

void Ship::ManageTackle()
{
	//State 0
	if (isTackling == NOT_TACKLING && isRecovering == NOT_HIT)
	{
		if (m_discoball == NULL || m_discoball != NULL)
		{
			if (tackle_again_clock.getElapsedTime().asSeconds() > TACKLE_AGAIN_COOLDOWN)
			{
				if (moving)
				{
					if (InputGuy::isDodging(m_controllerType) && wasDodgingButtonReleased)
					{
						isTackling = INITIATE_TACLKE;

						//isFiringButtonReleased = false;
						//wasFiringButtonReleased = false;
						isDodgingButtonReleased = false;
						wasDodgingButtonReleased = false;

						MaxSpeedConstraints();//avoids to cumulate multiple bonuses

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
		float new_absolute_speed = GetAbsoluteSpeed();
		if (new_absolute_speed + SHIP_TACKLE_ACCELERATION < SHIP_TACKLE_MAX_SPEED)
		{
			new_absolute_speed += SHIP_TACKLE_ACCELERATION;
		}
		else
		{
			new_absolute_speed = SHIP_TACKLE_MAX_SPEED;
			isTackling = HOLDING_TACKLE;
			tackle_max_hold_clock.restart();
		}

		SetSpeedVectorFromAbsoluteSpeed(new_absolute_speed, SpeedToPolarAngle(speed) + M_PI);
		
		//printf("speed : %f, %f \n", speed.x, speed.y);
	}

	//State 3
	if (isTackling == HOLDING_TACKLE)
	{
		if (tackle_max_hold_clock.getElapsedTime().asSeconds() > SHIP_TACKLE_MAX_HOLD_TIME || wasDodgingButtonReleased)
		{
			isTackling = ENDING_TACKLE;
			//printf("speed end of tackle : %f, %f \n", speed.x, speed.y);
		}
		else
		{
			SetSpeedVectorFromAbsoluteSpeed(GetAbsoluteSpeed() - SHIP_TACKLE_DECELERATION_WHILE_HOLDING, SpeedToPolarAngle(speed) + M_PI);
		}
	}

	//State 4
	if (isTackling == ENDING_TACKLE)
	{
		float new_absolute_speed = GetAbsoluteSpeed();
		if (new_absolute_speed - SHIP_TACKLE_DECELERATION > 0)
		{
			new_absolute_speed -= SHIP_TACKLE_DECELERATION;
		}
		else
		{
			new_absolute_speed = 0;
		}

		SetSpeedVectorFromAbsoluteSpeed(new_absolute_speed, SpeedToPolarAngle(speed) + M_PI);

		if (new_absolute_speed < SHIP_MIN_SPEED_AFTER_TACKLE)
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
	//if (isTackling != NOT_TACKLING || isBrawling != NOT_BRAWLING)
	//{
	//	setColor(Color(255, 0, 0, 255));
	//}
	//else if (isRecovering != NOT_HIT)
	if (isRecovering != NOT_HIT)
	{
		setColor(Color(0, 0, 255, 255));
	}
	//else if (isThrowing != NOT_THROWING)
	//{
	//	setColor(Color(0, 255, 0, 255));
	//}
	//else if (isDodging != NOT_DODGING)
	//{
	//	setColor(Color(255, 0, 0, 129));
	//	if (m_discoball != NULL)
	//	{
	//		m_discoball->setColor(Color(255, 0, 0, 129));
	//	}
	//}
	//else if (m_discoball != NULL)
	//{
	//	setColor(Color(255, 255, 255, 200));
	//	m_discoball->setColor(Color(255, 255, 255, 255));
	//}
	//else
	//{
	//	setColor(Color(255, 255, 255, 255));
	//}
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

			if (player2->isTackling != NOT_TACKLING || player2->isBrawling != NOT_BRAWLING)
			{
				if (isRecovering == NOT_HIT)
				{
					if (m_discoball != NULL)
					{
						ReleaseDiscoball();
					}

					isTackling = NOT_TACKLING;
					//isRecovering = RECOVERING_FROM_TACKLE;
					hit_recovery_clock.restart();
					speed.x = 0;
					speed.y = 0;
				}
			}

			player2->isTackling = NOT_TACKLING;
			//player2->isRecovering = RECOVERING_FROM_TACKLE;
			player2->hit_recovery_clock.restart();
			player2->speed.x = 0;
			player2->speed.y = 0;
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

void Ship::ManageDodge()
{
	//State 0
	if (isDodging == NOT_DODGING)
	{
		if (m_discoball != NULL)
		{
			if (dodge_again_clock.getElapsedTime().asSeconds() > DODGE_AGAIN_COOLDOWN)
			{
				if (isDodgingButtonReleased)
				{
					if ((speed.x * speed.x) + (speed.y * speed.y) > SHIP_MIN_SPEED_FOR_DODGE * SHIP_MIN_SPEED_FOR_DODGE)
					{
						if (InputGuy::isDodging(m_controllerType))
						{
							isDodging = INITIATE_DODGING;
							dodge_duration_clock.restart();
							isDodgingButtonReleased = false;

							float angle = SpeedToPolarAngle(speed);
							SetSpeedVectorFromAbsoluteSpeed(-SHIP_SPEED_BONUS_ON_DODGING * SHIP_MAX_SPEED, angle);

							ghost = true;

							if (m_discoball != NULL)
							{
								m_discoball->ghost = true;
							}

							(*CurrentGame).PlaySFX(SFX_Tackle);
						}
					}
				}
			}
		}
	}

	if (isDodging == INITIATE_DODGING)
	{
		if (dodge_duration_clock.getElapsedTime().asSeconds() > SHIP_DODGE_DURATION)
		{
			isDodging = NOT_DODGING;
			dodge_again_clock.restart();
			ghost = false;

			if (m_discoball != NULL)
			{
				m_discoball->ghost = false;
			}
		}
	}
}

void Ship::GetPortal(GameObject* portal)
{
	LevelPortal* getportal = (LevelPortal*)portal;
	m_script = getportal->m_script;
	isLaunchingScript = true;
}

//IA
sf::Vector2f Ship::GetInputsToGetPosition(sf::Vector2f position, sf::Time deltaTime)
{
	sf::Vector2f input_direction = sf::Vector2f(0, 0);
	
	//compute max speed available
	float cur_MaxSpeed = GetAbsoluteSpeed() + SHIP_ACCELERATION;
	const float cur_MaxSpeed_limit = GetMaxSpeed();
	if (cur_MaxSpeed > cur_MaxSpeed_limit)
	{
		cur_MaxSpeed = cur_MaxSpeed_limit;
	}

	//compute distance
	const float diff_x = position.x - getPosition().x;
	const float diff_y = position.y - getPosition().y;
	float distance = sqrt(diff_x * diff_x + diff_y * diff_y);

	//U-turn?
	/*
	float angle = SpeedToPolarAngle(sf::Vector2f(diff_x, diff_y)) * 180 / M_PI;
	if (abs(getRotation() - angle) > SHIP_CLICK_ANGLE_FOR_UTURN)
	{
		printf("\ngetrot : %f, angle: %f\n", getRotation(), angle);
		input_direction.x = diff_x / distance;
		input_direction.y = diff_y / distance;

		// normalize
		if (input_direction.x*input_direction.x + input_direction.y*input_direction.y > SHIP_CLICK_UTURN_INPUT_VALUE)
		{
			float p = (1.f / SHIP_CLICK_UTURN_INPUT_VALUE / sqrt((input_direction.x*input_direction.x) + (input_direction.y*input_direction.y)));
			input_direction.x *= p;
			input_direction.y *= p;
		}

		printf("U-TURN\n ");

		return input_direction;
	}
	*/

	cur_MaxSpeed *= deltaTime.asSeconds();

	if (distance < SHIP_CLICK_INPUT_PRECISION)//arrived at destination
	{
		//printf("input calculated: (%f, %f) #ARRIVED#\n\n", input_direction.x, input_direction.y);
		arrived_at_destination = true;
		return sf::Vector2f(0, 0);
	}
	else if (distance < cur_MaxSpeed)//last frame before arrival at destination
	{
		input_direction.x = diff_x / cur_MaxSpeed;//*deltaTime.asSeconds();
		input_direction.y = diff_y / cur_MaxSpeed;//*deltaTime.asSeconds();

		//printf("input calculated: (%f, %f) #LAST MOVE#\n\n", input_direction.x, input_direction.y);

		arrived_at_destination = false;
		return input_direction;
	}
	else//not arrived yet (need to travel at max speed)
	{		
		input_direction.x = diff_x / distance;
		input_direction.y = diff_y / distance;

		//printf("input calculated: (%f, %f) #MAX SPEED#\n\n", input_direction.x, input_direction.y);

		arrived_at_destination = false;
		return input_direction;
	}
}

void Ship::PlayerBumper(GameObject* bumper, Time deltaTime)
{
	Bumper* bumper_ = (Bumper*)bumper;

	if ((bumper_->m_type == OnlyBlueTeamThrough && m_team == RedTeam) || (bumper_->m_type == OnlyRedTeamThrough && m_team == BlueTeam))
	{
		ResetStatus();

		bool is_vertical_bumper = bumper->m_size.x < bumper->m_size.y;

		const float correction_x = (bumper->m_size.x / 2) + (m_size.x / 2);
		const float correction_y = (bumper->m_size.y / 2) + (m_size.y / 2);

		const float speed_x = speed.x * deltaTime.asSeconds();
		const float speed_y = speed.y * deltaTime.asSeconds();

		if (is_vertical_bumper)
		{
			//if (speed.x > 0)
			//{
			//	setPosition(sf::Vector2f(bumper->getPosition().x - correction_x, getPosition().y));
			//}
			//else if (speed.x < 0)
			//{
			//
			//	setPosition(sf::Vector2f(bumper->getPosition().x + correction_x, getPosition().y));
			//}
			//else
			//{

				//this detection system is not perfect. It's good enough.
				if (getPosition().x < bumper->getPosition().x)
				{
					if (speed_x > -m_size.x)
						setPosition(sf::Vector2f(bumper->getPosition().x - correction_x, getPosition().y));
					else // in this situation, the player most likely passed through the detection because of a very high speed... but we busted him :) look at him, he looks like a suspect.
						setPosition(sf::Vector2f(bumper->getPosition().x + correction_x, getPosition().y));
				}
				else
				{
					if (speed_x < m_size.x)
  						setPosition(sf::Vector2f(bumper->getPosition().x + correction_x, getPosition().y));
					else
						setPosition(sf::Vector2f(bumper->getPosition().x - correction_x, getPosition().y));
				}
			//}

			speed.x = 0;
		}
		else //horizontal bumper
		{
			//if (speed.y > 0)
			//{
			//	setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y - correction_y));
			//}
			//else if (speed.y < 0)
			//{
			//	setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y + correction_y));
			//}
			//else
			//{
				if (getPosition().y < bumper->getPosition().y)
				{
					if (speed_y > -m_size.y)
 						setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y - correction_y));
					else
						setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y + correction_y));
				}
				else
				{
					if (speed_y < m_size.y)
						setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y + correction_y));
					else
						setPosition(sf::Vector2f(getPosition().x, bumper->getPosition().y - correction_y));
				}
			//}

			speed.y = 0;
		}
	}
}

void Ship::LoadPlayerShipWithScript(IngameScript script)
{
	if (this == (*CurrentGame).playerShip)
	{
		m_script = script;
		isLaunchingScript = true;
	}
}

void Ship::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		stroboscopic_effect_clock.restart();
	}
}

bool Ship::IsUnmarked()
{
	if (m_target_opponent)
	{
		sf::Vector2f diff_position = sf::Vector2f(getPosition().x - m_target_opponent->getPosition().x, getPosition().y - m_target_opponent->getPosition().y);
		if (diff_position.x * diff_position.x + diff_position.y * diff_position.y > IA_DISTANCE_FOR_UNMARKED * IA_DISTANCE_FOR_UNMARKED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

bool Ship::SetTargetOpponent()
{
	GameObjectType type = m_team == BlueTeam ? PlayerRedShip : PlayerBlueShip;

	//find closest opponent
	m_target_opponent = (Ship*)FindClosestGameObjectTyped(type, false);

	return m_target_opponent;
}

GameObject* Ship::FindClosestGameObjectTyped(GameObjectType type, bool needs_to_be_unmarked)
{
	std::vector<GameObject*> sceneGameObjectsTyped = (*CurrentGame).GetSceneGameObjectsTyped(type);
	size_t sceneGameObjectsTypedSize = sceneGameObjectsTyped.size();

	sf::Vector2f pos;
	float shortest_distance = -1;
	GameObject* returned_obj = NULL;
	for (size_t j = 0; j < sceneGameObjectsTypedSize; j++)
	{
		if (sceneGameObjectsTyped[j] == NULL)
			continue;

		if (sceneGameObjectsTyped[j]->GarbageMe)
			continue;

		if (sceneGameObjectsTyped[j]->isOnScene && !sceneGameObjectsTyped[j]->ghost)
		{
			//specific for unmarked players
			if (needs_to_be_unmarked && (type == PlayerBlueShip || type == PlayerRedShip))
			{
				Ship* ship = (Ship*)(sceneGameObjectsTyped[j]);
				if (!ship->m_isUnmarked || ship == this)
				{
					continue;
				}
			}

			const float a = getPosition().x - sceneGameObjectsTyped[j]->getPosition().x;
			const float b = getPosition().y - sceneGameObjectsTyped[j]->getPosition().y;

			float distance_to_ref = (a * a) + (b * b);
			//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance_to_ref;
				returned_obj = sceneGameObjectsTyped[j];
			}
		}
	}

	return returned_obj;
}