#ifndef LEVELPORTAL_H_INCLUDED
#define LEVELPORTAL_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

enum PortalAnimations
{
	PortalOn,
	PortalOff,
};

class LevelPortal : public GameObject
{
public :
	LevelPortal(IngameScript script, bool togglable = false, bool toggled = false);
	LevelPortal(IngameScript script, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1, bool togglable = false, bool toggled = false);
	LevelPortal(IngameScript script, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, bool togglable = false, bool toggled = true);
	void Init();
	virtual ~LevelPortal();
	IngameScript m_script;
	LevelPortal* m_destination;
	bool m_togglable;
	bool m_toggled;
};

#endif // LEVELPORTAL_H_INCLUDED
