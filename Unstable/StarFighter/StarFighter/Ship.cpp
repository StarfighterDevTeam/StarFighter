#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Ship::Ship()
{
	
}

void Ship::Init()
{
	m_collider_type = GameObjectType::PlayerShip;
	moving = false;
	movingX = movingY = false;
	m_disable_inputs = false;
	m_controllerType = AllControlDevices;
	m_flux = 0;
	m_flux_max = SHIP_MAX_FLUX;

	//Flux display
	m_flux_text.setFont(*(*CurrentGame).font2);
	m_flux_text.setCharacterSize(20);
	m_flux_text.setColor(sf::Color::White);
	m_flux_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + FLUX_DISPLAY_OFFSET_Y));
	(*CurrentGame).addToFeedbacks(&m_flux_text);
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
	sf::Vector2f inputs_direction = InputGuy::getDirections();

	if (!m_disable_inputs)
	{
		moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		movingX = inputs_direction.x != 0;
		movingY = inputs_direction.y != 0;
	}

	ManageAcceleration(inputs_direction);
	MaxSpeedConstraints();
	IdleDecelleration(deltaTime);
	UpdateRotation();

	GameObject::update(deltaTime);

	ScreenBorderContraints();	

	//hud
	ostringstream ss;
	ss << m_flux << "/" << m_flux_max;
	m_flux_text.setString(ss.str());
	m_flux_text.setPosition(sf::Vector2f(getPosition().x - m_flux_text.getGlobalBounds().width / 2, getPosition().y + m_size.y / 2 + FLUX_DISPLAY_OFFSET_Y));

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());
}

void Ship::ScreenBorderContraints()
{
	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().x > (*CurrentGame).map_size.x - (m_size.x / 2))
	{
		this->setPosition((*CurrentGame).map_size.x - (m_size.x / 2), this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		speed.y = 0;
	}

	if (this->getPosition().y > (*CurrentGame).map_size.y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, (*CurrentGame).map_size.y - (m_size.y / 2));
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

void Ship::MaxSpeedConstraints()
{
	float ship_max_speed = SHIP_MAX_SPEED;

	//max speed constraints
	NormalizeSpeed(&speed, ship_max_speed);
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

void Ship::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		stroboscopic_effect_clock.restart();
	}
}

//FLUX SPECIFIC
void Ship::GetFluxor(GameObject* object)
{
	if (object)
	{
		if (m_flux < m_flux_max)
		{
			Fluxor* fluxor = (Fluxor*)object;

			m_flux += fluxor->m_flux;
			if (m_flux > m_flux_max)
			{
				m_flux = m_flux_max;
			}

			fluxor->Death();
		}
	}
}

void Ship::GetModule(GameObject* object)
{
	if (object)
	{
		Module* module = (Module*)object;

		if (m_curGridIndex.x == module->m_curGridIndex.x && m_curGridIndex.y == module->m_curGridIndex.y)
		{
			if (InputGuy::isFiring())
			{
				if (module->m_flux < module->m_flux_max && m_flux > 0)
				{
					m_flux--;
					module->m_flux++;
				}
			}
		}
	}
}