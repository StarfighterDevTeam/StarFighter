#ifndef FLUXOR_H_INCLUDED
#define FLUXOR_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"

class Fluxor : public GameObject
{
public :
	Fluxor();
	Fluxor(FluxorType FluxorType);
	Fluxor(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Fluxor* Clone();
	void Initialize();
	static Fluxor* CreateFluxor(FluxorType FluxorType);

	virtual ~Fluxor();
	void update(sf::Time deltaTime) override;
	bool ScreenBorderContraints();
	void UpdateRotation();

	static float RandomizeTurnDelay();
	static sf::Vector2f RandomizePosition();
	static sf::Vector2f RandomizeSpeed();
	void ChaosTurns();
	
	FluxorType m_FluxorType;
	float m_turn_delay;
	sf::Clock m_turn_clock;
	float m_absolute_speed;
	unsigned int m_flux;

	void Respawn();
	void Death();
	sf::Clock m_respawn_clock;
	float m_respawn_time;

	bool m_guided;

};

#endif // FLUXOR_H_INCLUDED
