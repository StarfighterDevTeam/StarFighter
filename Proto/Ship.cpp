#include "Ship.h"
#include "Inputs.h"

Ship::Ship()
	: SEShip()
{
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber)
	: SEShip(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size)
	: SEShip(position, speed, textureName, size)
{
}

void Ship::UpdateInputStates()
{
	Inputs* pInputs = (Inputs*)InputsManager;
	GetInputState(pInputs->isFiring(), Action_Firing);
}


void Ship::update(sf::Time deltaTime)
{
	sf::Vector2f inputs_direction = InputsManager->getDirections();

	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	ManageAcceleration(inputs_direction);
	
	//Action input
	UpdateInputStates();
	if (m_inputs_states[Action_Firing] == Input_Tap)
	{
		//do some action
		(*CurrentGame).CreateSFTextPop("action", Font_Arial, 20, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y/2 - 20);
		CurrentGame->PlaySFX(SFX_Laser);
	}

	MaxSpeedConstraints();
	IdleDecelleration(deltaTime);
	UpdateRotation();

	GameObject::update(deltaTime);

	//HUD
	m_is_asking_SFPanel = SFPanel_None;
	if (m_SFTargetPanel)
	{
		m_SFTargetPanel->Update(deltaTime);
	}

	ScreenBorderContraints();	
}