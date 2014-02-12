#ifndef FX_H_INCLUDED
#define FX_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Ammo.h"

class FX : public Independant
{

public:
	FX(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, sf::Time m_duration);
	void update(sf::Time deltaTime, sf::Clock polarClock) override;

private:
	sf::Clock deltaClockExploding;
	bool exploding;
	sf::Time duration;
};


#endif // FX_H_INCLUDED