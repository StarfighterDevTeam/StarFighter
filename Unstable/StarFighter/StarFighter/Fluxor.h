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
	void GetFluxor(GameObject* object) override;

	static float RandomizeTurnDelay();
	static sf::Vector2f RandomizePosition();
	static sf::Vector2f RandomizeSpeed();
	void ChaosTurns();
	
	FluxorType m_FluxorType;
	float m_turn_delay;
	sf::Clock m_turn_clock;
	float m_absolute_speed;
	unsigned int m_flux;
	unsigned int m_flux_max;

	void Respawn();
	void Death();
	sf::Clock m_respawn_clock;
	float m_respawn_time;

	bool m_guided;
	bool m_docked;
	
	vector<Module*> m_modules_visited;
	GameObject* m_target;
	GameObject* m_target_memory;

	//Fluxor properties
	bool m_displaying_flux;
	bool m_consummable_by_players;
	bool m_consummable_by_modules;
	bool m_can_be_refilled_by_modules;
	bool m_needs_link_to_circulate;
	bool m_flux_attacker;
	bool m_fluxovore;
	bool m_flux_stealer;
	unsigned int m_flux_stolen;
	bool m_flux_attack_piercing;
	float m_flux_attack_delay;
	sf::Clock m_flux_attack_clock;
	bool m_wasting_flux;
	int m_flux_waste;
	float m_flux_waste_delay;
	sf::Clock m_flux_waste_clock;
	void WastingFlux();
	void AttackFluxor(Fluxor* fluxor);
	void BringStealerBack();

	unsigned int m_transfer_buffer;
	sf::Clock m_flux_transfer_clock;

	//HUD
	sf::Text m_flux_text;
};

#endif // FLUXOR_H_INCLUDED
