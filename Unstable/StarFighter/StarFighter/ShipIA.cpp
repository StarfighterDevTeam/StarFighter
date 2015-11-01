#include "ShipIA.h"

extern Game* CurrentGame;

using namespace sf;

ShipIA::ShipIA()
{
	Init();
}

ShipIA::ShipIA(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Ship(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Init();
}

ShipIA::ShipIA(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Ship(position, speed, textureName, size)
{
	Init();
}

void ShipIA::Init()
{
	m_target_discoball = NULL;
	m_target_goal = NULL;
	m_IA_activated = true;
	m_IA_level = IAMedium;
	m_disable_inputs = true;
	m_hold_tackle_duration = 0;
	m_tackle_activated = false;
}

ShipIA::~ShipIA()
{
	
}

void ShipIA::update(sf::Time deltaTime)
{
	m_input_direction = sf::Vector2f(0, 0);

	//0. Acknowledge players' positions relative to me. Am I unmarked? Where is the closest threat? Where is my closest support?
	SetTargetOpponent();
	SetTargetTeamMate();
	m_isUnmarked = IsUnmarked();//note: even a non playing IA needs to update its  situation in order for team mates to know if it's unmarked or not

	if (m_IA_activated)
	{
		//DEFENSE
		if (m_discoball == NULL)
		{
			//1. Define where is own goal
			if (m_target_goal == NULL)
			{
				SetTargetGoal(true);
			}
			else if (m_target_goal->m_team != m_team)
			{
				SetTargetGoal(true);
			}
			
			bool isGoalGuarded = m_target_goal ? isTargetGoalGuarded() : false;

			//2. Define discoball to play
			if (m_target_discoball != NULL)
			{
				//printf("angle variation: %f | distance to ball: %f\n", GetAngleVariationToObject(m_target_discoball), (GetDistanceBetweenObjects(this, m_target_discoball)));
				
				//Are we "behind" the ball?
				if (isOffside())
				{
					//Run back to the ball
					IA_MoveToObject(m_target_discoball, deltaTime, true);

					//Seems safe to use tackle
					if (GetAngleVariationToObject(m_target_discoball) * deltaTime.asSeconds() < IA_DISCOBALL_ANGLERAD_VARIATION_IS_SMALL || m_target_discoball->m_carrier)
					{
						printf("angle variation: %f -> TACKLE !!", GetAngleVariationToObject(m_target_discoball) * deltaTime.asSeconds());
						Tackle(SHIP_TACKLE_MAX_HOLD_TIME);
					}
					else
					{
						printf("angle variation: %f | distance to ball: %f\n", GetAngleVariationToObject(m_target_discoball) * deltaTime.asSeconds(), (GetDistanceBetweenObjects(this, m_target_discoball)));
					}
				}
				//Ball is uncontested? let's go get it.
				else if (!isTargetBallContested())
				{
					IA_MoveToObject(m_target_discoball, deltaTime, true);
				}
				//Ok, we are not offside, ball is contested, let's think: should we go contest it or just wait in defense?
				else
				{
					//ball is on our trajectory, or is carried by the player? Let's go, but carefully.
					if (GetAngleVariationToObject(m_target_discoball) < IA_DISCOBALL_ANGLERAD_VARIATION_IS_SMALL || m_target_discoball->m_carrier)
					{
						//Aggresive defense: move to the ball
						if (m_target_discoball->m_carrier != NULL)
						{
							IA_MoveToObject(m_target_discoball->m_carrier, deltaTime, false);
						}
						else
						{
							IA_MoveToObject(m_target_discoball, deltaTime, true);
						}
					}
					//Ball is changing trajectory too much, let's just wait here, adjusting position to intercept.
					else
					{
						//Passive defense: move around own goal at current distance
						float distance_guard = GetDistanceBetweenObjects(this, m_target_goal);
						IA_GuardPosition(m_target_goal->getPosition(), distance_guard, deltaTime);
					}
				}
			}
			else
			{
				SetTargetDiscoball();
			}
		}
		//ATTAQUE
		else
		{
			//1. Define where is opponent's goal
			if (m_target_goal == NULL)
			{
				SetTargetGoal(false);
			}
			else if (m_target_goal->m_team == m_team)
			{
				SetTargetGoal(false);
			}
			
			bool isGoalGuarded = m_target_goal ? isTargetGoalGuarded() : false;

			//2. Shoot at it
			IA_MoveToPosition(m_target_goal->getPosition(), deltaTime);
			IA_ShootToPosition(m_target_goal->getPosition());
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

	ManageTackle(m_tackle_activated);

	GameObject::update(deltaTime);

	if (ScreenBorderContraints())
	{
		ResetStatus();
	}

	ManageDiscoball(deltaTime);
	ManageKeyReleases();
	ManageFeedbacks();

	//stroboscopic effect
	if (GetAbsoluteSpeed() > SHIP_MAX_SPEED)
	{
		PlayStroboscopicEffect(seconds(TACKLE_STROBO_EFFECT_DURATION * GetAbsoluteSpeed() / SHIP_TACKLE_MAX_SPEED), seconds(STROBO_EFFECT_TIME_BETWEEN_POSES));
	}
}

void ShipIA::IA_MoveToPosition(sf::Vector2f position, sf::Time deltaTime)
{
	m_input_direction = GetInputsToGetPosition(position, deltaTime);
}

void ShipIA::IA_MoveToObject(GameObject* object, sf::Time deltaTime, bool anticipation)
{
	if (object)
	{
		m_input_direction = GetInputsToGetPosition(sf::Vector2f(object->getPosition().x + anticipation * object->speed.x * deltaTime.asSeconds(), object->getPosition().y + anticipation * object->speed.y * deltaTime.asSeconds()), deltaTime);
	}

	if (GetDistanceBetweenObjects(this, m_target_discoball) > IA_DISTANCE_FOR_SAFE_TACKLE)
		Tackle(SHIP_TACKLE_MAX_HOLD_TIME);
}

void ShipIA::Tackle(float hold_tackle_duration)
{
	if (m_isTackling == NOT_TACKLING)
	{
		m_hold_tackle_duration = hold_tackle_duration;
		m_tackle_activated = true;
	}
}

void ShipIA::ManageTackle(bool force_input)
{
	if (force_input)
		Ship::ManageTackle(true);
	else if (tackle_max_hold_clock.getElapsedTime().asSeconds() < m_hold_tackle_duration || m_isTackling == INITIATE_TACLKE)
	
		//isTacklingButtonReleased = false;
		//wasTacklingButtonReleased = false;
		Ship::ManageTackle(true);
	else if (m_isTackling == ENDING_TACKLE)
	{
		Ship::ManageTackle(false);
	}
	else
	{
		m_hold_tackle_duration = 0;
	}

	//reset flag
	m_tackle_activated = false;
}

bool ShipIA::SetTargetDiscoball()
{
	//find closest discoball
	m_target_discoball = (Discoball*)FindClosestGameObjectTyped(this, DiscoballObject);

	return m_target_discoball;	
}

bool ShipIA::SetTargetGoal(bool own_goal)
{
	GameObjectType type;
	if (m_team == BlueTeam)
		type = own_goal ? GoalBlueObject : GoalRedObject;
	else if (m_team == RedTeam)
		type = own_goal ? GoalRedObject : GoalBlueObject;
	else
		type = GoalGreenObject;

	//find closest goal
	m_target_goal = (Goal*)FindClosestGameObjectTyped(this, type);

	return m_target_goal;
}

bool ShipIA::SetTargetTeamMate(bool only_unmarked)
{
	GameObjectType type = m_team == BlueTeam ? PlayerBlueShip : PlayerRedShip;

	//find closest unmarked team mate
	m_target_team_mate = (Ship*)FindClosestGameObjectTyped(this, type, only_unmarked);

	return m_target_team_mate;
}

void ShipIA::SetIADifficultyLevel(IADifficultyLevel IA_level)
{
	m_IA_level = IA_level;
}

sf::Time ShipIA::GetReactionTime(IADifficultyLevel IA_level)
{
	if (IA_level == IAEasy)
	{
		return sf::seconds(IA_REACTION_TIME_EASY);
	}
	else if (IA_level == IAMedium)
	{
		return sf::seconds(IA_REACTION_TIME_MEDIUM);
	}
	else if (IA_level == IAHard)
	{
		return sf::seconds(IA_REACTION_TIME_HARD);
	}

	return sf::seconds(0.f);
}

float ShipIA::GetThrowPrecision(IADifficultyLevel IA_level)
{
	if (IA_level == IAEasy)
	{
		return M_PI * IA_PRECISION_EASY / 180;
	}
	else if (IA_level == IAMedium)
	{
		return M_PI * IA_PRECISION_MEDIUM / 180;
	}
	else if (IA_level == IAHard)
	{
		return M_PI * IA_PRECISION_HARD / 180;
	}

	return 0.f;
}

void ShipIA::IA_ShootToPosition(sf::Vector2f position)
{
	if (m_discoball != NULL)
	{
		float target_angle = GetAngleRadBetweenPositions(getPosition(), position);
		target_angle -= discoball_clockwise ? M_PI_2 : -M_PI_2;

		float diff_angle = target_angle - discoball_curAngle;
		while (diff_angle < -M_PI)
			diff_angle += 2 * M_PI;
		while (diff_angle > M_PI)
			diff_angle -= 2 * M_PI;

		if (abs(diff_angle) <= GetThrowPrecision(m_IA_level))
		{
			ThrowDiscoball();
		}
		else
		{
			if (diff_angle > 0 && diff_angle < M_PI_2 && !discoball_clockwise)
			{
				SwitchRotation();
			}
			else if (diff_angle < 0 && diff_angle > -M_PI_2 && discoball_clockwise)
			{
				SwitchRotation();
			}
		}
	}
}

void ShipIA::ActivateIA(bool activate)
{
	m_IA_activated = activate;
}

sf::Vector2f ShipIA::GetDefensivePosition(sf::Vector2f position, float distance, GameObject* object_to_intercept, bool anticipation)
{
	assert(object_to_intercept != NULL);

	const sf::Vector2f position_to_intercept = sf::Vector2f(object_to_intercept->getPosition().x + anticipation * object_to_intercept->speed.x,
		object_to_intercept->getPosition().y + anticipation * object_to_intercept->speed.y);
	float target_angle = GetAngleRadBetweenPositions(position, position_to_intercept);

	sf::Vector2f target_position;
	target_position.x = position.x - distance * sin(target_angle);
	target_position.y = position.y + distance * cos(target_angle);

	return target_position;
}

void ShipIA::IA_GuardPosition(sf::Vector2f position, float distance, sf::Time deltaTime)
{
	if (m_target_discoball)
	{
		sf::Vector2f target_pos;
		//align with discoball's m_carrier
		if (m_target_discoball->m_carrier)
		{
			target_pos = GetDefensivePosition(position, distance, m_target_discoball->m_carrier, false);
		}
		else//align with discoball and anticipate on its movement
		{
			target_pos = GetDefensivePosition(position, distance, m_target_discoball, true);
		}
			
		IA_MoveToPosition(target_pos, deltaTime);
	}

	printf("Guard\n");
}

float ShipIA::GetAngleVariationToObject(GameObject* object_to_intercept)
{
	assert(object_to_intercept != NULL);

	const float current_angle = GetAngleRadBetweenObjects(this, object_to_intercept);

	const sf::Vector2f future_position = sf::Vector2f(getPosition().x + speed.x,
		getPosition().y + speed.y); 
	const sf::Vector2f future_position_to_intercept = sf::Vector2f(object_to_intercept->getPosition().x + object_to_intercept->speed.x,
		object_to_intercept->getPosition().y + object_to_intercept->speed.y);
	const float previous_angle = GetAngleRadBetweenPositions(future_position, future_position_to_intercept);

	float angle_variation = abs(current_angle - previous_angle);
	
	return angle_variation;
}

bool ShipIA::isTargetGoalGuarded()
{
	if (m_target_goal)
	{
		GameObjectType type = m_target_goal->m_team == BlueTeam ? PlayerBlueShip : PlayerRedShip;

		GameObject* closest_defender = Ship::FindClosestGameObjectTyped(m_target_goal, type);

		if (closest_defender)
		{
			return GetDistanceBetweenObjects(closest_defender, m_target_goal) < IA_DISTANCE_FOR_UNGUARDED;
		}
		else
		{
			return false;
		}
	}

	LOGGER_WRITE(Logger::Priority::DEBUG, "IsTargetGoalGuarded() calls a NULL target goal.\n");
	return false;
}

bool ShipIA::isOffside()
{
	//we don't carry the discoball
	if (m_discoball == NULL)
	{
		if (m_target_goal)
		{
			//the targeted goal exists and is our own goal
			if (m_target_goal->m_team == m_team)
			{
				const float distance_to_own_goal = GetDistanceBetweenObjects(this, m_target_goal);
				const float distance_discoball_to_goal = GetDistanceBetweenObjects(m_target_discoball, m_target_goal);

				return distance_to_own_goal > distance_discoball_to_goal;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool ShipIA::isTargetBallContested()
{
	//if we are closer to the ball than the opponent, or if the ball is really far from anyone anyway, then it's "uncontested" (... and we should probably go get it)
	if (m_target_discoball)
	{
		const float distance_to_target = GetDistanceBetweenObjects(this, m_target_discoball);
		GameObject* opponent = FindClosestGameObjectTyped(m_target_discoball, GetOpponentGameObjectType(this));
		const float distance_discoball_to_opponent = GetDistanceBetweenObjects(opponent, m_target_discoball);

		if (distance_discoball_to_opponent > IA_DISTANCE_FOR_UNCONTESTED || (distance_to_target < distance_discoball_to_opponent))
			return false;
		else
			return true;
	}
	else
		return false;
}

GameObjectType ShipIA::GetOpponentGameObjectType(Ship* player)
{
	assert(player != NULL);
	GameObjectType type = player->m_team == BlueTeam ? PlayerRedShip : PlayerBlueShip;
	return type;
}

GameObjectType ShipIA::GetOwnGameObjectType(Ship* player)
{
	assert(player != NULL);
	GameObjectType type = player->m_team == BlueTeam ? PlayerBlueShip : PlayerRedShip;
	return type;
}