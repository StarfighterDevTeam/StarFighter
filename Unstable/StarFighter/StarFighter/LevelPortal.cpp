#include "LevelPortal.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

LevelPortal::LevelPortal(IngameScript script)
{
	m_script = script;
}

LevelPortal::LevelPortal(IngameScript script, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_script = script;
	this->Init();
}

LevelPortal::LevelPortal(IngameScript script, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	m_script = script;
	this->Init();
}

void LevelPortal::Init()
{
	collider_type = LevelPortalObject;
	DontGarbageMe = true;
}

LevelPortal::~LevelPortal()
{
	
}