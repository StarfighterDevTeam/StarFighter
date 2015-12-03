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

	//IA scripted
	m_force_discoball_uncontested = false;
	m_force_tackle_disabled = false;
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
			if (m_target_goal)
			{
				if (SetTargetDiscoball())
				{
					//printf("angle variation: %f | distance to ball: %f\n", GetAngleVariationToObject(m_target_discoball), (GetDistanceBetweenObjects(this, m_target_discoball)));

					//Are we "behind" the ball?
					if (isOffside())
					{
						//Run back to the discoball carrier if any...
						if (m_target_discoball->m_carrier)
						{
							IA_MoveToObject(m_target_discoball->m_carrier, deltaTime, true);
							//Seems safe to use tackle
							if (GetAngleVariationToObject(m_target_discoball->m_carrier) < IA_DISCOBALL_ANGLERAD_VARIATION_IS_SMALL)
							{
								Tackle(SHIP_TACKLE_MAX_HOLD_TIME);
							}

						}
						//...otherwise, run back to the discoball itself
						else
						{
							IA_MoveToObject(m_target_discoball, deltaTime, true);
							//Seems safe to use tackle
							if (GetAngleVariationToObject(m_target_discoball) < IA_DISCOBALL_ANGLERAD_VARIATION_IS_SMALL)
							{
								Tackle(SHIP_TACKLE_MAX_HOLD_TIME);
							}
						}
					}
					//Ball is uncontested? let's go get it.
					else if (!isTargetBallContested())
					{
						IA_MoveToObject(m_target_discoball, deltaTime, true);
						//Seems safe to use tackle
						if (GetAngleVariationToObject(m_target_discoball) < IA_DISCOBALL_ANGLERAD_VARIATION_IS_SMALL)
						{
							Tackle(SHIP_TACKLE_MAX_HOLD_TIME);
						}
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
			}
			//We haven't found any goal to defend, but perhaps there is a ball to play nevertheless?
			else
			{
				if (SetTargetDiscoball())
				{
					IA_MoveToObject(m_target_discoball, deltaTime, true);
					//Seems safe to use tackle
					//if (GetAngleVariationToObject(m_target_discoball) < IA_DISCOBALL_ANGLERAD_VARIATION_IS_SMALL)
					//{
					//	Tackle(SHIP_TACKLE_MAX_HOLD_TIME);
					//}
				}
			}
		}
		//ATTACK
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

			if (m_target_goal)
			{
				//2. Attack it
				//Move toward target
				IA_MoveToPosition(m_target_goal->getPosition(), deltaTime);
				if (!isGoalGuarded)
					Tackle(SHIP_TACKLE_MAX_HOLD_TIME);

				//Shoot (or pass)
				sf::Vector2f target_position;
				if (EvaluateFireTrajectory(m_target_goal))
				{
					IA_ShootToPosition(m_target_goal->getPosition());
					//IA_ShootToPosition(GetBouncingShotCoordinate(m_target_goal->getPosition(), false, false));
					printf("direct shoot\n");
				}
				else if (m_target_team_mate && EvaluateFireTrajectory(m_target_team_mate))
				{
					IA_ShootToPosition(m_target_team_mate->getPosition());
					printf("direct pass\n");
				}
				else if (EvaluateBouncingFireTrajectory(m_target_goal, &target_position))
				{
					printf("bounce shot\n");
					IA_ShootToPosition(target_position);
				}
				else if (m_target_team_mate && EvaluateBouncingFireTrajectory(m_target_team_mate, &target_position))
				{
					printf("bounce pass\n");
					IA_ShootToPosition(target_position);
				}
				else
				{
					printf("direct shoot (last resort)\n");
					IA_ShootToPosition(m_target_goal->getPosition());
				}
			}
		}
	}
	else
	{
		m_input_direction = InputGuy::getDirections(m_controllerType);
	}

	moving = m_input_direction.x != 0 || m_input_direction.y != 0;
	movingX = m_input_direction.x != 0;
	movingY = m_input_direction.y != 0;

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
		if (m_isTackling == INITIATE_TACLKE)
			m_isTackling = HOLDING_TACKLE;
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
	if (m_isTackling == NOT_TACKLING && !m_force_tackle_disabled)
	{
		m_hold_tackle_duration = hold_tackle_duration;
		m_tackle_activated = true;
	}
}

void ShipIA::ManageTackle(bool force_input)
{
	if (force_input)
	{
		Ship::ManageTackle(true);
	}
	else if (tackle_max_hold_clock.getElapsedTime().asSeconds() < m_hold_tackle_duration || m_isTackling == INITIATE_TACLKE)
	{
		Ship::ManageTackle(true);
	}
	else if (m_isTackling == HOLDING_TACKLE || m_isTackling == ENDING_TACKLE)
	{
		Ship::ManageTackle(false);
	}
	else
	{
		m_hold_tackle_duration = 0.f;
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

float ShipIA::GetReactionTime(IADifficultyLevel IA_level)
{
	if (IA_level == IAEasy)
	{
		return IA_REACTION_TIME_EASY;
	}
	else if (IA_level == IAMedium)
	{
		return IA_REACTION_TIME_MEDIUM;
	}
	else if (IA_level == IAHard)
	{
		return IA_REACTION_TIME_HARD;
	}

	return 0.f;
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
		
		if (USE_ORTHOGONAL_THROW)
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

	//printf("Guard\n");
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

		if (distance_discoball_to_opponent > IA_DISTANCE_FOR_UNCONTESTED || (distance_to_target <= distance_discoball_to_opponent) || m_force_discoball_uncontested)
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

void ShipIA::ForceDiscoballUncontested(bool forced_value)
{
	m_force_discoball_uncontested = forced_value;
}

void ShipIA::ForceTackleDisabled(bool forced_value)
{
	m_force_tackle_disabled = forced_value;
}

bool ShipIA::EvaluateFireTrajectory(sf::Vector2f firing_position, sf::Vector2f target_position)
{
	//looking for closest enemy able to intercept the ball: distance to segment of trajectory
	GameObjectType type = m_team == BlueTeam ? PlayerRedShip : PlayerBlueShip;
	std::vector<GameObject*> sceneGameObjectsTyped = (*CurrentGame).GetSceneGameObjectsTyped(type);
	size_t sceneGameObjectsTypedSize = sceneGameObjectsTyped.size();
	
	float shortest_distance = -1;
	sf::Vector2f threat_position;
	GameObject* returned_obj = NULL;
	for (size_t j = 0; j < sceneGameObjectsTypedSize; j++)
	{
		if (sceneGameObjectsTyped[j] == NULL)
			continue;

		if (sceneGameObjectsTyped[j]->GarbageMe)
			continue;

		if (sceneGameObjectsTyped[j]->isOnScene && !sceneGameObjectsTyped[j]->ghost)
		{
			sf::Vector2f p_0 = sceneGameObjectsTyped[j]->getPosition();
			sf::Vector2f p_1 = firing_position;
			sf::Vector2f p_2 = target_position;
			float distance_to_ref = GameObject::DistancePointToSement(p_0.x, p_0.y, p_1.x, p_1.y, p_2.x, p_2.y) - sceneGameObjectsTyped[j]->diag;
			if (distance_to_ref < 0)
				distance_to_ref = 0;
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance_to_ref;
				threat_position = p_0;
			}
		}
	}

	//distance of threat is sufficient -> trajectory is clear
	bool threat_is_far_enough = shortest_distance > IA_DISTANCE_FOR_CLEAR_LINE_OF_FIRE || shortest_distance < 0;
	if (threat_is_far_enough)
		return true;

	//opponent could be close but "behind" the shooter, hence not a threat to the trajectory
	bool threat_is_behind = abs(GetAngleRadBetweenPositions(firing_position, target_position) - GetAngleRadBetweenPositions(firing_position, threat_position)) > M_PI_2;
	return threat_is_behind;
}

bool ShipIA::EvaluateFireTrajectory(sf::Vector2f target_position)
{
	if (m_discoball)
		return EvaluateFireTrajectory(m_discoball->getPosition(), target_position);
	else
		return EvaluateFireTrajectory(getPosition(), target_position);
}

bool ShipIA::EvaluateFireTrajectory(GameObject* target_object)
{
	if (target_object == NULL)
		return false;
	else
		return EvaluateFireTrajectory(target_object->getPosition());
}

sf::Vector2f ShipIA::GetBouncingShotCoordinate(sf::Vector2f target_position, bool vertical_bounce, bool solution_top_or_left)
{
	//no bounce needed
	if ((target_position.x == 0 && target_position.x == 0) || (target_position.x == 0 && target_position.y == (*CurrentGame).map_size.y)
		|| (target_position.x == (*CurrentGame).map_size.x && target_position.y == 0) || (target_position.x == (*CurrentGame).map_size.x && target_position.y == (*CurrentGame).map_size.y))
		return target_position;

	sf::Vector2f bounce_position;
	float dx = getPosition().x - target_position.x;
	float dy = getPosition().y - target_position.y;
	if (dx < 0)
		dx = (*CurrentGame).map_size.x - dx;
	if (dy < 0)
		dy = (*CurrentGame).map_size.y - dy;

	if (vertical_bounce)//discoball hits the top or bottom border
	{
		if (solution_top_or_left)
		{
			bounce_position.x = target_position.y * dx / (getPosition().y + target_position.y);
			bounce_position.y = 0;
		}
		else
		{
			bounce_position.x = ((*CurrentGame).map_size.y - target_position.y) * dx / ((*CurrentGame).map_size.y - getPosition().y + (*CurrentGame).map_size.y - target_position.y);
			bounce_position.y = (*CurrentGame).map_size.y;
		}
	}
	else//discoball hits the left or right border
	{
		if (solution_top_or_left)
		{
			bounce_position.x = 0;
			bounce_position.y = target_position.x * dy / (getPosition().x + target_position.x);
		}
		else
		{
			bounce_position.x = (*CurrentGame).map_size.x;
			bounce_position.y = ((*CurrentGame).map_size.x - target_position.x) * dy / ((*CurrentGame).map_size.x - getPosition().x + (*CurrentGame).map_size.x - target_position.x);
		}
	}
	
	return bounce_position;
}

bool ShipIA::EvaluateBouncingFireTrajectory(GameObject* target_object, sf::Vector2f* target_position)
{
	if (target_object == NULL)
		return false;

	//acquisition of bouncing shot solutions
	sf::Vector2f solution[4];
	bool valid_solution[4] = { true, true, true, true };

	if (target_object->collider_type == GoalBlueObject || target_object->collider_type == GoalRedObject)
	{
		if (target_object->m_size.x < target_object->m_size.y)
		{
			solution[0] = GetBouncingShotCoordinate(target_object->getPosition(), true, true);
			solution[1] = GetBouncingShotCoordinate(target_object->getPosition(), true, false);
			valid_solution[2] = false;
			valid_solution[3] = false;
		}
		else
		{
			solution[2] = GetBouncingShotCoordinate(target_object->getPosition(), false, true);
			solution[3] = GetBouncingShotCoordinate(target_object->getPosition(), false, false);
			valid_solution[0] = false;
			valid_solution[1] = false;
		}
	}
	else
	{
		solution[0] = GetBouncingShotCoordinate(target_object->getPosition(), true, true);
		solution[1] = GetBouncingShotCoordinate(target_object->getPosition(), true, false);
		solution[2] = GetBouncingShotCoordinate(target_object->getPosition(), false, true);
		solution[3] = GetBouncingShotCoordinate(target_object->getPosition(), false, false);
	}

	for (int i = 0; i < 4; i++)
	{
		if (!valid_solution[i])
			continue;

		//checking if both trajectories are safe: fire to screen border and bounce toward final target
		valid_solution[i] = EvaluateFireTrajectory(solution[i]) && EvaluateFireTrajectory(solution[i], target_object->getPosition());
	}

	//any solution still valid at this stage?
	if (!valid_solution[0] && !valid_solution[1] && !valid_solution[2] && !valid_solution[3])
		return false;
	//any unique solution at this stage?
	else if (valid_solution[0] + valid_solution[1] + valid_solution[2] + valid_solution[3] == 1)
	{
		for (int i = 0; i < 4; i++)
		{
			if (valid_solution[i])
			{
				*target_position = solution[i];
				return true;
			}
		}
	}

	//choosing the best option among the remaining: shortest distance to travel for the discoball
	float shortest_distance = -1;
	for (int i = 0; i < 4; i++)
	{
		if (!valid_solution[i])
			continue;

		float dx, dy;
		switch (i)
		{
			case 0: {//top
						dx = getPosition().x - target_object->getPosition().x;
						dy = getPosition().y + target_object->getPosition().y;
						break;
			}
			case 1: {//bottom
						dx = getPosition().x - target_object->getPosition().x;
						dy = (*CurrentGame).map_size.y - getPosition().y + target_object->getPosition().y;
						break;
			}
			case 2: {//left
						dx = getPosition().x + target_object->getPosition().x;
						dy = getPosition().y;
						break;
			}
			case 3: {//right
						dx = (*CurrentGame).map_size.x - getPosition().x + (*CurrentGame).map_size.x - target_object->getPosition().x;
						dy = getPosition().y;
						break;
			}
		}

		float distance = sqrt(dx*dx + dy*dy);
		if (distance < shortest_distance || shortest_distance < 0)
		{
			shortest_distance = distance;
			for (int j = 0; j < i; j++)
				valid_solution[j] = false;//elimination of previously checked solutions, for they are longer
		}
		else
			valid_solution[i] = false;
	}

	//return the best valid solution remaining
	for (int i = 0; i < 4; i++)
	{
		if (valid_solution[i])
		{
			*target_position = solution[i];
			return true;
		}
	}

	return false;
}