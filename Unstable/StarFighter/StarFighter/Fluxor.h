#ifndef FLUXOR_H_INCLUDED
#define FLUXOR_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"

enum FluxorType
{
	FluxorType_Blue,
};

class Fluxor : public GameObject
{
public :
	Fluxor();
	Fluxor(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	static Fluxor* CreateFluxor(FluxorType FluxorType);

	virtual ~Fluxor();
	void update(sf::Time deltaTime) override;
	bool ScreenBorderContraints();
	void UpdateRotation();

	static float RandomizeTurnDelay();
	void ChaosTurns();
	
	FluxorType m_FluxorType;
	float m_turn_delay;
	sf::Clock m_turn_clock;
	float m_absolute_speed;
	int m_flux;
};

#endif // FLUXOR_H_INCLUDED
