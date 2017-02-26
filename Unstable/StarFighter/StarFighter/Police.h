#ifndef POLICE_H_INCLUDED
#define POLICE_H_INCLUDED

#include "Agent.h"

class Police : public Agent
{
public :
	Police();
	Police(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Police(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Police();
	void update(sf::Time deltaTime) override;
};

#endif // POLICE_H_INCLUDED
