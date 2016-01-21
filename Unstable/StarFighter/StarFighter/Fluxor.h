#ifndef FLUXOR_H_INCLUDED
#define FLUXOR_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "SFText.h"
#include "SFTextPop.h"
#include "FluxEntity.h"

enum FluxorInteractionWithModule
{
	Fluxor_NewInteraction,
	Fluxor_Consumption,
	Fluxor_OtherInteractions,
};

class Fluxor : public FluxEntity
{
public :
	Fluxor();
	Fluxor(FluxorType FluxorType, PlayerTeams team);
	Fluxor(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Fluxor* Clone();
	void Initialize();
	static Fluxor* CreateFluxor(FluxorType FluxorType, bool within_bounds = false, sf::FloatRect bounds = { 0, 0, 0, 0 });
	void CheckCondensation() override;

	virtual ~Fluxor();
	void update(sf::Time deltaTime) override;
	bool ScreenBorderContraints();
	void UpdateRotation();
	void GetFluxor(GameObject* object) override;

	static float RandomizeTurnDelay();
	static sf::Vector2f RandomizePosition(bool within_bounds = false, sf::FloatRect bounds = { 0, 0, 0, 0 });
	static sf::Vector2f RandomizeSpeed();
	void ChaosTurns();
	void CollisionResponse(GameObject* bumper, CollisionSide collision, bool bouncing) override;
	
	FluxorType m_FluxorType;
	float m_turn_delay;
	sf::Clock m_turn_clock;
	float m_absolute_speed;

	bool m_guided;
	bool m_docked;

	bool m_has_spawn_bounds;
	sf::FloatRect m_spawn_bounds;
	
	vector<Module*> m_modules_visited;
	GameObject* m_target;
	bool m_target_memory;

	//Fluxor properties
	bool m_consummable_by_players;
	bool m_consummable_by_modules;
	bool m_can_be_refilled_by_modules;
	bool m_needs_link_to_circulate;
	bool m_condensed_to_circulate;
	bool m_flux_attacker;
	bool m_fluxovore;
	bool m_flux_stealer;
	unsigned int m_flux_stolen;
	bool m_flux_attack_piercing;
	float m_flux_attack_delay;
	sf::Clock m_flux_attack_clock;
	void WastingFlux();
	void AttackFluxor(Fluxor* fluxor);
	void BringStealerBack();

	//flags
	FluxorInteractionWithModule m_module_interaction;
};

#endif // FLUXOR_H_INCLUDED
