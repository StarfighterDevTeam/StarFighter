#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "Fluxor.h"

struct Link
{
	Link(){};
	bool m_exists;
	bool m_activated;
};

class Module : public GameObject
{
public :
	Module();
	Module(ModuleType moduleType);
	Module(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	static Module* CreateModule(sf::Vector2u grid_index, ModuleType moduleType);
	static void EraseModule(sf::Vector2u grid_index);
	static void DebugFinishModule(sf::Vector2u grid_index);
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
	//bool m_activated;
	Glow* m_glow;
	bool m_under_construction;
	void SetConstructionStatus(bool under_construction);

	vector<Module*> m_parents;
	vector<Module*> m_children;

	//Activation
	//void UpdateActivation();

	//Module properties
	void ApplyModuleEffect(Fluxor* fluxor);
	bool m_isConsummingFlux;
	bool m_isRefillingFlux;
	int m_add_speed;

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

	//Flux transfer to Fluxors
	void ConsummeFluxor(Fluxor* fluxor);
	float m_flux_transfer_delay;

	//Spawn
	//void ResolveProductionBufferList() override;
	//vector<Fluxor*> m_fluxor_generation_buffer;

	//Links
	GameObject* m_arrow[4];
	Link m_link[4];
	int GetMainLinkIndex();
	void SwitchLinkDirection();
	bool UndockFluxor(Fluxor* fluxor);
	void UpdateLinks();
	bool UpdateFluxorDirection(Fluxor* fluxor);
	bool IsMainLinkActivated();

	//HUD
	sf::Text m_flux_text;
};

#endif // MODULE_H_INCLUDED
