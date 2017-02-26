#ifndef WALKER_H_INCLUDED
#define WALKER_H_INCLUDED

#include "Agent.h"

class Walker : public Agent
{
public :
	Walker();
	Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Walker();
	void update(sf::Time deltaTime) override;
};

#endif // WALKER_H_INCLUDED
