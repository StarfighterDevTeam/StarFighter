#include "Fluxor.h"

extern Game* CurrentGame;

using namespace sf;

Fluxor::Fluxor()
{
	
}

Fluxor::Fluxor(FluxorType FluxorType)
{
	//texture
	std::string textureName;
	if (FluxorType == FluxorType_Blue)
		textureName = "Assets/2D/fluxor_blue.png";
	if (FluxorType == FluxorType_Green)
		textureName = "Assets/2D/fluxor_green.png";
	if (FluxorType == FluxorType_Red)
		textureName = "Assets/2D/fluxor_red.png";

	const unsigned int W = FLUXOR_WIDTH;
	const unsigned int H = FLUXOR_HEIGHT;

	m_FluxorType = FluxorType;
	m_flux = FLUXOR_FLUX_VALUE;
	m_guided = false;

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(W, H), 1, 1);
	setOrigin(sf::Vector2f(W / 2, H / 2));
}

Fluxor::Fluxor(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{

}

Fluxor* Fluxor::CreateFluxor(FluxorType FluxorType)
{
	//texture
	//std::string textureName;
	//if (FluxorType == FluxorType_Blue)
	//	textureName = "Assets/2D/fluxor_blue.png";
	//if (FluxorType == FluxorType_Green)
	//	textureName = "Assets/2D/fluxor_green.png";
	//if (FluxorType == FluxorType_Red)
	//	textureName = "Assets/2D/fluxor_red.png";
	//
	//const unsigned int W = FLUXOR_WIDTH;
	//const unsigned int H = FLUXOR_HEIGHT;

	//speed and direction
	float absolute_speed = RandomizeFloatBetweenValues(sf::Vector2f(FLUXOR_SPEED_MIN, FLUXOR_SPEED_MAX));
	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	sf::Vector2f speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(absolute_speed, angle);

	//position
	sf::Vector2f position = RandomizePosition();

	Fluxor* new_Fluxor = (*CurrentGame).m_fluxor_list[FluxorType_Blue]->Clone();
	new_Fluxor->setPosition(position);
	new_Fluxor->m_speed = speed;

	//turn delay
	new_Fluxor->m_turn_delay = RandomizeTurnDelay();
	new_Fluxor->m_absolute_speed = absolute_speed;

	new_Fluxor->m_flux = FLUXOR_FLUX_VALUE;

	(*CurrentGame).addToScene(new_Fluxor, FluxorLayer, FluxorObject);

	return new_Fluxor;
}

Fluxor::~Fluxor()
{
	
}

Fluxor* Fluxor::Clone()
{
	Fluxor* clone = new Fluxor(this->getPosition(), this->m_speed, this->m_textureName, this->m_size, sf::Vector2f(this->m_size.x / 2, this->m_size.y / 2), this->m_frameNumber, this->m_animationNumber);
	clone->m_collider_type = this->m_collider_type;
	clone->m_layer = this->m_layer;
	clone->m_FluxorType = this->m_FluxorType;

	return clone;
}

void Fluxor::update(sf::Time deltaTime)
{
	if (visible)
	{
		//chaos turns
		if (!m_guided)
		{
			if (m_turn_clock.getElapsedTime().asSeconds() > m_turn_delay)
			{
				ChaosTurns();
			}
		}

		if (ScreenBorderContraints())
		{
			m_turn_clock.restart();
		}

		UpdateRotation();

		GameObject::update(deltaTime);
	}
	else//if dead
	{
		if (m_respawn_clock.getElapsedTime().asSeconds() > m_respawn_time)
		{
			Respawn();
		}
	}
	
}

float Fluxor::RandomizeTurnDelay()
{
	return RandomizeFloatBetweenValues(sf::Vector2f(FLUXOR_TURN_MIN_DELAY, FLUXOR_TURN_MAX_DELAY));
}

sf::Vector2f Fluxor::RandomizePosition()
{
	const unsigned int W = FLUXOR_WIDTH;
	const unsigned int H = FLUXOR_HEIGHT;

	sf::Vector2f position;
	bool position_is_valid = false;

	while (!position_is_valid)
	{
		position_is_valid = true;

		float position_x = RandomizeFloatBetweenValues(sf::Vector2f(W / 2, (*CurrentGame).map_size.x - W / 2));
		float position_y = RandomizeFloatBetweenValues(sf::Vector2f(H / 2, (*CurrentGame).map_size.y - H / 2));

		position = sf::Vector2f(position_x, position_y);

		//checking if position is valid
		if ((*CurrentGame).GetClosestObject(position, PlayerShip))
		{
			float distance_to_player = GameObject::GetDistanceBetweenPositions(position, (*CurrentGame).GetClosestObject(position, PlayerShip)->getPosition());
			if (distance_to_player < TILE_SIZE)
			{
				position_is_valid = false;
			}
		}

		if (!(*CurrentGame).isCellFree(position))
		{
			position_is_valid = false;
		}
	}

	return position;
}

sf::Vector2f Fluxor::RandomizeSpeed()
{
	float absolute_speed = RandomizeFloatBetweenValues(sf::Vector2f(FLUXOR_SPEED_MIN, FLUXOR_SPEED_MAX));
	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	sf::Vector2f speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(absolute_speed, angle);

	return speed;
}

void Fluxor::ChaosTurns()
{
	m_turn_delay = RandomizeTurnDelay();
	m_turn_clock.restart();
	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	m_speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_absolute_speed, angle);
}

bool Fluxor::ScreenBorderContraints()
{
	bool touching_screen_border = false;
	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		m_speed.x *= -1;
		touching_screen_border = true;
	}

	if (this->getPosition().x >(*CurrentGame).map_size.x - (m_size.x / 2))
	{
		this->setPosition((*CurrentGame).map_size.x - (m_size.x / 2), this->getPosition().y);
		m_speed.x *= -1;
		touching_screen_border = true;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		m_speed.y *= -1;
		touching_screen_border = true;
	}

	if (this->getPosition().y >(*CurrentGame).map_size.y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, (*CurrentGame).map_size.y - (m_size.y / 2));
		m_speed.y *= -1;
		touching_screen_border = true;
	}

	return touching_screen_border;
}

void Fluxor::UpdateRotation()
{
	//turning toward targeted position
	if (m_speed.x == 0 && m_speed.y == 0)
	{
		//do nothing
	}
	else if (m_speed.x == 0 && m_speed.y > 0)
	{
		setRotation(180);
	}
	else if (m_speed.x == 0 && m_speed.y < 0)
	{
		setRotation(90);
	}
	else if (m_speed.y == 0 && m_speed.x > 0)
	{
		setRotation(0);
	}
	else if (m_speed.y == 0 && m_speed.x < 0)
	{
		setRotation(270);
	}
	else
	{
		setRotation((GetAngleRadForSpeed(m_speed) * 180 / (float)M_PI) - 90);
	}
}

void Fluxor::Death()
{
	visible = false;
	m_respawn_time = RandomizeFloatBetweenValues(sf::Vector2f(FLUXOR_RESPAWN_MIN_TIME, FLUXOR_RESPAWN_MAX_TIME));
	m_respawn_clock.restart();
}

void Fluxor::Respawn()
{
	const unsigned int W = FLUXOR_WIDTH;
	const unsigned int H = FLUXOR_HEIGHT;

	//position
	setPosition(RandomizePosition());

	visible = true;
	m_turn_clock.restart();
}