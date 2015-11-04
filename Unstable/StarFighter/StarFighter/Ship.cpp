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
	m_disable_inputs = false;
	m_controllerType = AllControlDevices;
	m_tackle_cooldown_init = true;

	//TRON SPECIFIC
	m_discoball = NULL;
	discoball_curAngle = -1.f;
	discoball_clockwise = true;

	isFiringButtonReleased = false;
	wasFiringButtonReleased = false;
	isSwitchingButtonReleased = false;
	isTacklingButtonReleased = false;

	m_isTackling = NOT_TACKLING;
	m_isThrowing = NOT_THROWING;
	m_isRecovering = NOT_HIT;

	m_throw_curSpeedBonus = 0.f;

	m_team = BlueTeam;
	m_isLaunchingScript = false;
	m_isUsingPortal = false;

	m_character = Natalia;

	m_arrived_at_destination = true;

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

	if (!m_disable_inputs)
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
			if (!m_arrived_at_destination)
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

	if (m_isTackling == NOT_TACKLING && m_isRecovering == NOT_HIT)
	{
		GetDirectionInputs(m_input_direction);
		MaxSpeedConstraints();
		IdleDecelleration(deltaTime);
	}
	
	UpdateRotation();
	
	ManageTackle();

	GameObject::update(deltaTime);

	if (ScreenBorderContraints())
	{
		//ResetStatus();
	}

	ManageFire();
	
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
	m_isTackling = NOT_TACKLING;
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
		setRotation((GetAngleRadForSpeed(speed) * 180 / (float)M_PI));
	}
}

float Ship::GetMaxSpeed()
{
	float ship_max_speed = SHIP_MAX_SPEED;

	//bonus given after throwing the discoball
	if (throw_bonus_speed_clock.getElapsedTime().asSeconds() < THROW_BONUS_SPEED_TIMER)
	{
		if (m_isThrowing == AFTER_THROW)
		{
			if (m_isTackling == NOT_TACKLING && m_discoball == NULL) //for safety
			{
				ship_max_speed = SHIP_MAX_SPEED + m_throw_curSpeedBonus;
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
		if (m_isThrowing == AFTER_THROW)
		{
			if (m_isTackling == NOT_TACKLING && m_discoball == NULL) //for safety
			{
				ship_max_speed = SHIP_MAX_SPEED + m_throw_curSpeedBonus;
			}
		}
	}
	else
	{
		m_isThrowing = NOT_THROWING;
		m_throw_curSpeedBonus = 0;
	}

	//max speed constraints
	NormalizeSpeed(&speed, ship_max_speed);
}

//TRON SPECIFIC
void Ship::GetDiscoball(GameObject* discoball, float angle_collision)
{
	if (m_discoball == NULL)
	{
		if (m_isRecovering == NOT_HIT)
		{
			if (carry_again_clock.getElapsedTime().asSeconds() > CARRY_AGAIN_COOLDOWN)
			{
				m_discoball = (Discoball*)discoball;

				//deleting old m_carrier's discoball ownership
				if (m_discoball->m_carried)
				{
					m_discoball->m_carrier->carry_again_clock.restart();

					//game mode specific
					if ((*CurrentGame).cur_GameRules == CarryToAbleInputs)
					{
						m_discoball->m_carrier->m_disable_inputs = true;
						m_discoball->m_carrier->speed.x = 0;
						m_discoball->m_carrier->speed.y = 0;
						m_disable_inputs = false;
						(*CurrentGame).playerShip = this;
					}

					m_discoball->m_carrier->m_discoball = NULL;
				}
				else
				{
					m_discoball->discoball_curAngularSpeed -= CARRY_CATCH_ACCELERATION_MALUS;
				}

				//acquisition of the discoball
				m_discoball->m_carried = true;
				m_discoball->m_carrier = this;
				discoball_curAngle = angle_collision;
				m_carrier_clock.restart();

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
				m_isThrowing = NOT_THROWING;

				(*CurrentGame).PlaySFX(SFX_Catch);
				if (m_team == BlueTeam)
				{
					m_discoball->SetDiscoballStatus(Discoballm_carriedBlueTeam);
				}
				if (m_team == RedTeam)
				{
					m_discoball->SetDiscoballStatus(Discoballm_carriedRedTeam);
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
		else if (m_discoball->m_isTouchingBumper)
		{
			ReleaseDiscoball();
		}
		else
		{
			int cc = discoball_clockwise ? cc = -1 : cc = 1;
			discoball_curAngle = discoball_curAngle - (m_discoball->discoball_curAngularSpeed * cc * deltaTime.asSeconds());

			//angular speed deceleration
			if (m_carrier_clock.getElapsedTime().asSeconds() > CARRY_TIME_FOR_DECELERATION && m_discoball->discoball_curAngularSpeed > CARRY_BASE_ANGULAR_SPEED)
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
				m_carrier_clock.restart();
			}

			//transmitting values to the discoball
			discoball_curAngle = fmod(discoball_curAngle, 2 * M_PI);
			m_discoball->m_carrier_curAngle = discoball_curAngle;
			m_discoball->m_carrier_curPosition = getPosition();
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

		m_isThrowing = AFTER_THROW;
		throw_bonus_speed_clock.restart();
		m_throw_curSpeedBonus = m_discoball->discoball_curAngularSpeed * SHIP_MAX_SPEED_MULTIPLIER_AFTER_THROW;

		m_discoball->discoball_curAngularSpeed += CARRY_THROW_ACCELERATION_BONUS;

		//orthogonal launch
		if (USE_ORTHOGONAL_THROW)
			discoball_curAngle += discoball_clockwise ? M_PI_2 : - M_PI_2;

		m_discoball->SetSpeedVectorFromAbsoluteSpeed(m_discoball->discoball_curAngularSpeed * DISCOBALL_GRAVITATION_DISTANCE, discoball_curAngle);

		m_discoball->m_carried = false;
		//printf("Discoball thrown. (speed: %f)\n", m_discoball->discoball_curAngularSpeed);
		carry_again_clock.restart();

		m_discoball->SetDiscoballStatus(DiscoballFree);

		m_discoball->m_carrier = NULL;
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
		m_discoball->m_carried = false;
		//printf("Discoball released. (speed: %f)\n", m_discoball->discoball_curAngularSpeed);
		carry_again_clock.restart();

		m_discoball->SetDiscoballStatus(DiscoballLost);

		m_discoball->m_carrier = NULL;
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
	m_carrier_clock.restart();
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
	if (!InputGuy::isTackling(m_controllerType))
	{
		isTacklingButtonReleased = true;
		wasTacklingButtonReleased = true;
	}
}

void Ship::ManageTackle(bool force_input)
{
	//State 0
	if (m_isTackling == NOT_TACKLING && m_isRecovering == NOT_HIT)
	{
		if (m_discoball == NULL || m_discoball != NULL)
		{
			if (m_tackle_cooldown_init || tackle_again_clock.getElapsedTime().asSeconds() > TACKLE_AGAIN_COOLDOWN)
			{
				if (moving)
				{
					if (((InputGuy::isTackling(m_controllerType) && !m_disable_inputs) || force_input)
						&& wasTacklingButtonReleased)
					{
						m_isTackling = INITIATE_TACLKE;
						m_tackle_cooldown_init = false;

						isTacklingButtonReleased = false;
						wasTacklingButtonReleased = false;

						MaxSpeedConstraints();//avoids to cumulate multiple bonuses

						(*CurrentGame).PlaySFX(SFX_Tackle);
					}
				}
			}
		}
	}

	//State 1
	if (m_isTackling == INITIATE_TACLKE)
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
			m_isTackling = HOLDING_TACKLE;
			tackle_max_hold_clock.restart();
		}

		SetSpeedVectorFromAbsoluteSpeed(new_absolute_speed, GetAngleRadForSpeed(speed) + M_PI);
	}

	//State 3
	if (m_isTackling == HOLDING_TACKLE)
	{
		if ((force_input || (tackle_max_hold_clock.getElapsedTime().asSeconds() > SHIP_TACKLE_MAX_HOLD_TIME || wasTacklingButtonReleased)))
		{
			m_isTackling = ENDING_TACKLE;
		}
		else
		{
			SetSpeedVectorFromAbsoluteSpeed(GetAbsoluteSpeed() - SHIP_TACKLE_DECELERATION_WHILE_HOLDING, GetAngleRadForSpeed(speed) + M_PI);
		}
	}

	//State 4
	if (m_isTackling == ENDING_TACKLE)
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

		SetSpeedVectorFromAbsoluteSpeed(new_absolute_speed, GetAngleRadForSpeed(speed) + M_PI);

		if (new_absolute_speed < SHIP_MIN_SPEED_AFTER_TACKLE)
		{
			m_isTackling = NOT_TACKLING;
			tackle_again_clock.restart();
			//printf("speed normal : %f, %f \n", speed.x, speed.y);
		}
	}
}

void Ship::ManageHitRecovery()
{
	if (m_isRecovering == RECOVERING_FROM_TACKLE)
	{
		if (hit_recovery_clock.getElapsedTime().asSeconds() > RECOVERING_FROM_TACKLE_COOLDOWN)
		{
			m_isRecovering = NOT_HIT;
		}
	}
}

void Ship::ManageFeedbacks()
{
	//if (m_isTackling != NOT_TACKLING || isBrawling != NOT_BRAWLING)
	//{
	//	setColor(Color(255, 0, 0, 255));
	//}
	//else if (m_isRecovering != NOT_HIT)
	if (m_isRecovering != NOT_HIT)
	{
		setColor(Color(0, 0, 255, 255));
	}
	//else if (m_isThrowing != NOT_THROWING)
	//{
	//	setColor(Color(0, 255, 0, 255));
	//}
	//else if (isTackling != NOT_DODGING)
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

	if (m_isTackling != NOT_TACKLING)
	{
		if (player2->m_isRecovering == NOT_HIT)
		{
			if (player2->m_discoball != NULL)
			{
				player2->ReleaseDiscoball();
			}

			if (player2->m_isTackling != NOT_TACKLING)
			{
				if (m_isRecovering == NOT_HIT)
				{
					if (m_discoball != NULL)
					{
						ReleaseDiscoball();
					}

					m_isTackling = NOT_TACKLING;
					//m_isRecovering = RECOVERING_FROM_TACKLE;
					hit_recovery_clock.restart();
					speed.x = 0;
					speed.y = 0;
				}
			}

			player2->m_isTackling = NOT_TACKLING;
			player2->hit_recovery_clock.restart();
			player2->speed.x = 0;
			player2->speed.y = 0;
		}
	}
}

void Ship::GetPortal(GameObject* portal)
{
	if (!m_isUsingPortal)
	{
		LevelPortal* getportal = (LevelPortal*)portal;
		if (getportal->m_destination)
		{
			setPosition(getportal->m_destination->getPosition());
			//m_isUsingPortal = true; -> set in Game::CollisionCheckV2 method
		}
		else
		{
			m_script = getportal->m_script;
			m_isLaunchingScript = true;
		}
	}
}

void Ship::UsingPortal(bool is_using)
{
	m_isUsingPortal = is_using;
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

	cur_MaxSpeed *= deltaTime.asSeconds();

	if (distance < SHIP_CLICK_INPUT_PRECISION)//arrived at destination
	{
		//printf("input calculated: (%f, %f) #ARRIVED#\n\n", input_direction.x, input_direction.y);
		m_arrived_at_destination = true;
		return sf::Vector2f(0, 0);
	}
	else if (distance < cur_MaxSpeed)//last frame before arrival at destination
	{
		input_direction.x = diff_x / cur_MaxSpeed;//*deltaTime.asSeconds();
		input_direction.y = diff_y / cur_MaxSpeed;//*deltaTime.asSeconds();

		//printf("input calculated: (%f, %f) #LAST MOVE#\n\n", input_direction.x, input_direction.y);

		m_arrived_at_destination = false;
		return input_direction;
	}
	else//not arrived yet (need to travel at max speed)
	{		
		input_direction.x = diff_x / distance;
		input_direction.y = diff_y / distance;

		//printf("input calculated: (%f, %f) #MAX SPEED#\n\n", input_direction.x, input_direction.y);

		m_arrived_at_destination = false;
		return input_direction;
	}
}

void Ship::PlayerBumper(GameObject* bumper, Time deltaTime)
{
	Bumper* bumper_ = (Bumper*)bumper;

	if ((bumper_->m_type == OnlyBlueTeamThrough && m_team == RedTeam) || (bumper_->m_type == OnlyRedTeamThrough && m_team == BlueTeam))
	{
		//ResetStatus();

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
	m_target_opponent = (Ship*)FindClosestGameObjectTyped(this, type, false);

	return m_target_opponent;
}

GameObject* Ship::FindClosestGameObjectTyped(GameObject* this_object, GameObjectType type, bool needs_to_be_unmarked)
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
				if (!ship->m_isUnmarked || ship == this_object)
				{
					continue;
				}
			}

			const float a = this_object->getPosition().x - sceneGameObjectsTyped[j]->getPosition().x;
			const float b = this_object->getPosition().y - sceneGameObjectsTyped[j]->getPosition().y;

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