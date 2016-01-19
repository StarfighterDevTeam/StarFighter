#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "Fluxor.h"
#include "SFText.h"
#include "SFTextPop.h"

enum LinkStatus
{
	Link_Deactivated,
	Link_Activated,
	Link_Invalid,
};

struct Link
{
	Link(){};
	bool m_exists;
	LinkStatus m_activated;
};

enum LinkDirection
{
	LinkRight,
	LinkDown,
	LinkLeft,
	LinkUp,
};

class Module : public GameObject
{
public :
	Module();
	Module(ModuleType moduleType);
	Module(ModuleType moduleType, PlayerTeams team);
	Module(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	static Module* CreateModule(sf::Vector2u grid_index, ModuleType moduleType, PlayerTeams team, bool construction_finished = false, bool force_direction = false, int forced_link_direction = 0, unsigned flux = 0);
	static void EraseModule(sf::Vector2u grid_index);
	static void DebugFinishModule(sf::Vector2u grid_index);
	static void DebugRefillingModuleFlux(sf::Vector2u grid_index);
	void Initialize();
	Module* Clone();
	virtual ~Module();
	void update(sf::Time deltaTime) override;
	void GetFluxor(GameObject* object) override;
	void SetDirectionAutomatically();

	ModuleType m_moduleType;
	unsigned int m_flux;
	unsigned int m_flux_max;
	unsigned int m_flux_max_after_construction;
	unsigned int m_flux_max_under_construction;
	void FinishConstruction();
	sf::Vector2u m_curGridIndex;
	Glow* m_glow;
	void SetConstructionStatus(bool under_construction);
	sf::Clock m_construction_clock;
	GameObject* m_shield;

	//Module properties
	void ApplyModuleEffect(Fluxor* fluxor);
	bool m_isRefillingFlux;
	int m_add_speed;
	int m_add_flux;
	unsigned int m_turret_range;
	unsigned int m_shield_range;
	Fluxor* SearchNearbyAttackers(PlayerTeams team_not_to_target, float range);
	bool m_upgrade_player_stats;

	//Flux auto-generation
	bool m_isAutogeneratingFlux;
	float m_flux_autogeneration_time;
	sf::Clock m_flux_autogeneration_clock;
	void AutogenerateFlux();

	//Fluxor generation
	bool m_isGeneratingFluxor;
	FluxorType m_fluxor_generated_type;
	bool GenerateFluxor();
	float m_fluxor_generation_time;
	sf::Clock m_fluxor_spawn_clock;
	unsigned int m_fluxor_generation_cost;

	//Fluxor condensation
	bool m_isCondensatingFluxor;
	void CondensateFluxor(Fluxor* fluxor);

	//Flux transfer to Fluxors
	bool ConsummeFluxor(Fluxor* fluxor);
	bool AmplifyFluxor(Fluxor* fluxor);
	float m_flux_transfer_delay;
	sf::Clock m_flux_consumption_clock;

	//Module attacked by Fluxors
	void AttackModule(Fluxor* fluxor);

	//Spawn
	void ResolveProductionBufferList() override;
	vector<Fluxor*> m_fluxor_generation_buffer;

	//Links and circuit
	GameObject* m_arrow[4];
	Link m_link[4];
	Module* m_linked_modules[4];
	int GetMainLinkIndex();
	Module* GetMainLinkedModule();
	bool m_has_child_to_refill;
	void SwitchLinkDirection();
	bool UndockFluxor(Fluxor* fluxor);
	void UpdateLinks();
	void CheckCircuit();
	void UpdateFreeTileFeedbacks();
	bool UpdateFluxorDirection(Fluxor* fluxor);
	bool IsMainLinkActivated();

	SFRectangle* m_free_tile_feedback;
	vector <SFRectangle*> m_free_tile_condensator_feedbacks;
	int GetLinkIndexToFreeConnectedCell();
	bool m_is_connected_to_a_circuit;

	//HUD
	SFText* m_flux_text;
	void SetTeam(PlayerTeams team, TeamAlliances alliance);
	GameObject* m_team_marker;
	SFGauge* m_flux_gauge;
	void AddFluxGauge(GaugeStyles gauge, sf::Vector2f offset);
};

#endif // MODULE_H_INCLUDED
