#ifndef PORTAL_H_INCLUDED
#define PORTAL_H_INCLUDED

#include "Phase.h"

enum PortalState
{
	PortalInvisible,//0
	PortalClose,//1
	PortalGhost,//2
	PortalOpen,//3
};

enum PortalAnimation
{
	PortalOpenIdle,//0
	PortalOpening,//1
	PortalClosing,//2
	PortalCloseIdle,//3
};

class Portal : public GameObject
{
public:
	Portal(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber);
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void Open();
	void Close();

	std::string m_destination_name;
	int m_max_unlocked_hazard_level;
	sf::Vector2f m_offset;//respect to the background
	PortalState m_state;
	Directions m_direction;
	int m_level;
};

#endif //PORTAL_H_INCLUDED