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
	m_target_previous_position = sf::Vector2f(0, 0);
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

	if (!disable_inputs && m_IA_activated)
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
			
			//2. Try to defend it
			if (m_target_discoball != NULL)
			{
				if ((GetAngleVariationToObject(m_target_discoball) > IA_ANGLERAD_VARIATION_FOR_DISCOBALL_GUARD_STANCE
					&& (GetDistanceBetweenObjects(this, m_target_discoball) > IA_DISTANCE_FOR_DISCOBALL_GUARD_STANCE))
					|| m_target_discoball->carried)
				{
					//Passive defense: move around own goal
					float distance_guard = GetDistanceBetweenObjects(this, m_target_goal);
					float distance_discoball_to_goal = GetDistanceBetweenObjects(m_target_discoball, m_target_goal);
					//are we "behind" the ball?
					if (distance_discoball_to_goal < distance_guard)
					{
						distance_guard = distance_discoball_to_goal;
					}
					IA_GuardPosition(m_target_goal->getPosition(), distance_guard, deltaTime);
				}
				else
				{
					//Aggresive defense: move to the ball
					IA_MoveToObject(m_target_discoball, deltaTime, true);
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
			
			//2. Shoot at it
			IA_MoveToPosition(m_target_goal->getPosition(), deltaTime);
			//if (m_isUnmarked)
				IA_ShootToPosition(m_target_goal->getPosition());
			//else
			//{
			//	if (m_target_team_mate && m_target_team_mate->m_isUnmarked)
			//	{
			//		IA_ShootToPosition(m_target_team_mate->getPosition());
			//	}
			//		
			//}

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
		m_input_direction = GetInputsToGetPosition(sf::Vector2f(object->getPosition().x + anticipation * object->speed.x, object->getPosition().y + anticipation * object->speed.y), deltaTime);
	}
}


bool ShipIA::SetTargetDiscoball()
{
	//find closest discoball
	m_target_discoball = (Discoball*)FindClosestGameObjectTyped(DiscoballObject);

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
	m_target_goal = (Goal*)FindClosestGameObjectTyped(type);

	return m_target_goal;
}

bool ShipIA::SetTargetTeamMate(bool only_unmarked)
{
	GameObjectType type = m_team == BlueTeam ? PlayerBlueShip : PlayerRedShip;

	//find closest unmarked team mate
	m_target_team_mate = (Ship*)FindClosestGameObjectTyped(type, only_unmarked);

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
		const sf::Vector2f diff = sf::Vector2f(position.x - getPosition().x, position.y - getPosition().y);
		float target_angle = SpeedToPolarAngle(diff) + M_PI;
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

	const sf::Vector2f diff = sf::Vector2f(position.x - (object_to_intercept->getPosition().x + anticipation * object_to_intercept->speed.x), 
		position.y - (object_to_intercept->getPosition().y + anticipation * object_to_intercept->speed.y));
	float target_angle = SpeedToPolarAngle(diff);

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
		//align with discoball's carrier
		if (m_target_discoball->carrier)
		{
			target_pos = GetDefensivePosition(position, distance, m_target_discoball->carrier, false);
		}
		else//align with discoball and anticipate on its movement
		{
			target_pos = GetDefensivePosition(position, distance, m_target_discoball, true);
		}
			
		IA_MoveToPosition(target_pos, deltaTime);
	}
}

float ShipIA::GetAngleVariationToObject(GameObject* object_to_intercept)
{
	assert(object_to_intercept != NULL);

	const sf::Vector2f current_diff = sf::Vector2f(getPosition().x - object_to_intercept->getPosition().x, getPosition().y - object_to_intercept->getPosition().y);
	const float current_angle = SpeedToPolarAngle(current_diff);

	
	//printf("cur angle: %f | ", current_angle);

	//const sf::Vector2f previous_object_pos = sf::Vector2f(object_to_intercept->getPosition().x - object_to_intercept->speed.x, object_to_intercept->getPosition().y - object_to_intercept->speed.y);
	//const sf::Vector2f previous_diff = sf::Vector2f(getPosition().x - previous_object_pos.x, getPosition().y - previous_object_pos.y);
	const sf::Vector2f previous_diff = sf::Vector2f(getPosition().x - m_target_previous_position.x, getPosition().y - m_target_previous_position.y);
	const float previous_angle = SpeedToPolarAngle(previous_diff);

	m_target_previous_position = object_to_intercept->getPosition();

	//printf("prev angle: %f | ", previous_angle);

	float angle_variation = abs(current_angle - previous_angle);

	printf("var: %f\n ", angle_variation);
	
	return angle_variation;
}