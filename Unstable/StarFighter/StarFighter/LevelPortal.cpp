#include "LevelPortal.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

LevelPortal::LevelPortal(IngameScript script, bool togglable, bool toggled)
{
	m_script = script;
	m_togglable = togglable;
	m_toggled = toggled;
}

LevelPortal::LevelPortal(IngameScript script, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber, bool togglable, bool toggled) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_script = script;
	m_togglable = togglable;
	m_toggled = toggled;
	this->Init();
}

LevelPortal::LevelPortal(IngameScript script, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, bool togglable, bool toggled) : GameObject(position, speed, textureName, size)
{
	m_script = script;
	m_togglable = togglable;
	m_toggled = toggled;
	this->Init();
}

void LevelPortal::Init()
{
	m_collider_type = LevelPortalObject;
	DontGarbageMe = true;
	m_destination = NULL;

	if (m_togglable && !m_toggled)
		setAnimationLine(PortalOff);
	else
		setAnimationLine(PortalOn);
}

LevelPortal::~LevelPortal()
{
	
}