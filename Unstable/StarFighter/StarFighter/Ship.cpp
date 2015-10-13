#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Ship::Ship()
{

}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Independant(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	this->collider_type = IndependantType::PlayerShip;
	this->moving = false;
	this->movingX = movingY = false;
	this->visible = true;
	this->disable_inputs = false;
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

	ManageAcceleration(inputs_direction);
	IdleDecelleration(deltaTime);

	Independant::update(deltaTime);

	ScreenBorderContraints();	
}

void Ship::ScreenBorderContraints()
{
	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().x > SCENE_SIZE_X - (m_size.x / 2))
	{
		this->setPosition(SCENE_SIZE_X - (m_size.x / 2), this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		speed.y = 0;
	}

	if (this->getPosition().y > SCENE_SIZE_Y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, SCENE_SIZE_Y - (m_size.y / 2));
		speed.y = 0;
	}
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	//idle decceleration
	if (!movingX)
	{
		speed.x -= (speed.x) * deltaTime.asSeconds()* SHIP_DECCELERATION_COEF / 100.f;

		if (abs(speed.x) < SHIP_MIN_SPEED)
			speed.x = 0;
	}

	if (!movingY)
	{
		speed.y -= (speed.y)*deltaTime.asSeconds()*SHIP_DECCELERATION_COEF / 100.f;

		if (abs(speed.y) < SHIP_MIN_SPEED)
			speed.y = 0;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	speed.x += inputs_direction.x* SHIP_ACCELERATION;
	speed.y += inputs_direction.y*SHIP_ACCELERATION;

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