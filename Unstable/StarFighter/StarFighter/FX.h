#ifndef FX_H_INCLUDED
#define FX_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Independant.h"
#include "Game.h"

class FX : public Independant
{
public:
	FX(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, sf::Time m_duration);
	void update(sf::Time deltaTime) override;

	FX* Clone();
	int frameNumber;

private:
	sf::Clock deltaClockExploding;
	bool exploding;
	sf::Time duration;
};

class Aura : public FX
{
public:
	Aura(Independant* m_target, std::string textureName, sf::Vector2f size, int m_frameNumber);
	void Init(std::string m_textureName, sf::Vector2f dsize, int m_frameNumber);
	void update(sf::Time deltaTime) override;
	Aura* Clone();
	Independant* target;

private:
	sf::Clock deltaClockExploding;
	sf::Time duration;
};

#endif // FX_H_INCLUDED