#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "Fluxor.h"

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
	Module(ModuleType moduleType, PlayerTeams team);
	Module(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	static Module* CreateModule(sf::Vector2u grid_index, ModuleType moduleType, PlayerTeams team, bool construction_finished = false, int link_activation = 0, unsigned flux = 0);
	static void EraseModule(sf::Vector2u grid_index);
	static void DebugFinishModule(sf::Vector2u grid_index);
	static void DebugRefillingModuleFlux(sf::Vector2u grid_index);
	void Initialize();
	Module* Clone();
	virtual ~Module();
	void update(sf::Time deltaTime) override;
	void GetFluxor(GameObject* object) override;

	ModuleType m_moduleType;
	unsigned int m_flux;
	unsigned int m_flux_max;
	unsigned int m_flux_max_after_construction;
	unsigned int m_flux_max_under_construction;
	void FinishConstruction();
	sf::Vector2u m_curGridIndex;
	Glow* m_glow;
	void SetConstructionStatus(bool under_construction);

	//vector<Module*> m_parents;
	//vector<Module*> m_children;

	//Module properties
	void ApplyModuleEffect(Fluxor* fluxor);
	bool m_isRefillingFlux;
	int m_add_speed;
	int m_add_flux;
	unsigned int m_turret_range;
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
	void ConsummeFluxor(Fluxor* fluxor);
	void AmplifyFluxor(Fluxor* fluxor);
	float m_flux_transfer_delay;
	sf::Clock m_flux_consumption_clock;

	//Module attacked by Fluxors
	void AttackModule(Fluxor* fluxor);

	//Spawn
	void ResolveProductionBufferList() override;
	vector<Fluxor*> m_fluxor_generation_buffer;

	//Links
	GameObject* m_arrow[4];
	Link m_link[4];
	Module* m_linked_modules[4];
	int GetMainLinkIndex();
	Module* GetMainLinkedModule();
	bool m_has_child_to_refill;
	void SwitchLinkDirection();
	bool UndockFluxor(Fluxor* fluxor);
	void UpdateLinks();
	bool UpdateFluxorDirection(Fluxor* fluxor);
	bool IsMainLinkActivated();

	//HUD
	SFText* m_flux_text;
	void SetTeam(PlayerTeams team, TeamAlliances alliance);
};

#endif // MODULE_H_INCLUDED
