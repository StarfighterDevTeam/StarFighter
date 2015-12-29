#include "Fluxor.h"

extern Game* CurrentGame;

using namespace sf;

Fluxor::Fluxor()
{
	
}

Fluxor::Fluxor(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{

}

Fluxor* Fluxor::CreateFluxor(FluxorType FluxorType)
{
	//texture
	std::string textureName;
	if (FluxorType == FluxorType_Blue)
		textureName = "Assets/2D/fluxor_blue.png";

	const unsigned int W = FLUXOR_WIDTH;
	const unsigned int H = FLUXOR_HEIGHT;

	//speed and direction
	float absolute_speed = RandomizeFloatBetweenValues(sf::Vector2f(FLUXOR_SPEED_MIN, FLUXOR_SPEED_MAX));
	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	sf::Vector2f speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(absolute_speed, angle);

	//position
	float position_x = RandomizeFloatBetweenValues(sf::Vector2f(W / 2, (*CurrentGame).map_size.x - W / 2));
	float position_y = RandomizeFloatBetweenValues(sf::Vector2f(H / 2, (*CurrentGame).map_size.y - H / 2));

	Fluxor* new_Fluxor = new Fluxor(sf::Vector2f(position_x, position_y), speed, textureName, sf::Vector2f(W, H), sf::Vector2f(W / 2, H / 2), 1, 1);

	//turn delay
	new_Fluxor->m_turn_delay = RandomizeTurnDelay();
	new_Fluxor->m_absolute_speed = absolute_speed;

	new_Fluxor->m_FluxorType = FluxorType;
	new_Fluxor->m_flux = FLUXOR_FLUX_VALUE;

	(*CurrentGame).addToScene(new_Fluxor, FluxorLayer, FluxorObject);

	return new_Fluxor;
}


Fluxor::~Fluxor()
{
	
}

void Fluxor::update(sf::Time deltaTime)
{
	if (visible)
	{
		//chaos turns
		if (m_turn_clock.getElapsedTime().asSeconds() > m_turn_delay)
		{
			ChaosTurns();
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

void Fluxor::ChaosTurns()
{
	m_turn_delay = RandomizeTurnDelay();
	m_turn_clock.restart();
	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_absolute_speed, angle);
}

bool Fluxor::ScreenBorderContraints()
{
	bool touching_screen_border = false;
	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		speed.x *= -1;
		touching_screen_border = true;
	}

	if (this->getPosition().x >(*CurrentGame).map_size.x - (m_size.x / 2))
	{
		this->setPosition((*CurrentGame).map_size.x - (m_size.x / 2), this->getPosition().y);
		speed.x *= -1;
		touching_screen_border = true;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		speed.y *= -1;
		touching_screen_border = true;
	}

	if (this->getPosition().y >(*CurrentGame).map_size.y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, (*CurrentGame).map_size.y - (m_size.y / 2));
		speed.y *= -1;
		touching_screen_border = true;
	}

	return touching_screen_border;
}

void Fluxor::UpdateRotation()
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
		setRotation((GetAngleRadForSpeed(speed) * 180 / (float)M_PI) - 90);
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
	float position_x = RandomizeFloatBetweenValues(sf::Vector2f(W / 2, (*CurrentGame).map_size.x - W / 2));
	float position_y = RandomizeFloatBetweenValues(sf::Vector2f(H / 2, (*CurrentGame).map_size.y - H / 2));

	visible = true;
	m_turn_clock.restart();
}