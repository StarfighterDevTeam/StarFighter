#ifndef LEVELPORTAL_H_INCLUDED
#define LEVELPORTAL_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

enum PortalAnimations
{
	PortalActivated,
	PortalDeactivated,
};

class LevelPortal : public GameObject
{
public :
	LevelPortal(IngameScript script);
	LevelPortal(IngameScript script, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	LevelPortal(IngameScript script, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~LevelPortal();
	IngameScript m_script;
	LevelPortal* m_destination;
};

#endif // LEVELPORTAL_H_INCLUDED
