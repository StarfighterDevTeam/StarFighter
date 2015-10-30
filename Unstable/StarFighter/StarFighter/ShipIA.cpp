#include "ShipIA.h"

extern Game* CurrentGame;

using namespace sf;

ShipIA::ShipIA()
{
	m_target_discoball = NULL;
	m_IA_activated = true;
	m_IA_level = IAMedium;
}

ShipIA::ShipIA(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Ship(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_target_discoball = NULL;
	m_IA_activated = true;
	m_IA_level = IAMedium;
}

ShipIA::ShipIA(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Ship(position, speed, textureName, size)
{
	m_target_discoball = NULL;
	m_IA_activated = true;
	m_IA_level = IAMedium;
}

ShipIA::~ShipIA()
{
	
}

void ShipIA::update(sf::Time deltaTime)
{
	m_input_direction = sf::Vector2f(0, 0);

	if (!disable_inputs && m_IA_activated)
	{
		//DEFENSE
		if (m_discoball == NULL)
		{
			if (m_target_discoball != NULL)
			{
				IA_MoveToPosition(m_target_discoball->getPosition(), deltaTime);
			}
			else
			{
				SetTargetDiscoball();
			}
		}

		//ATTAQUE
		else
		{
			IA_ShootToPosition(sf::Vector2f(0, 540));
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

void ShipIA::IA_MoveToPosition(sf::Vector2f position, sf::Time deltaTime)
{
	m_input_direction = GetInputsToGetPosition(position, deltaTime);
}

bool ShipIA::SetTargetDiscoball()
{
	m_target_discoball = (Discoball*)(*CurrentGame).GetClosestObject(this, DiscoballObject);

	return m_target_discoball;	
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