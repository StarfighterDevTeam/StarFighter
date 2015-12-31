#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "Fluxor.h"

class Module : public GameObject
{
public :
	Module();
	Module(ModuleType moduleType);
	Module(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	static Module* CreateModule(sf::Vector2u grid_index, ModuleType moduleType);
	void Initialize();
	Module* Clone();
	virtual ~Module();
	void update(sf::Time deltaTime) override;
	void GetFluxor(GameObject* object) override;

	ModuleType m_moduleType;
	unsigned int m_flux;
	unsigned int m_flux_max;
	sf::Vector2u m_curGridIndex;
	bool m_activated;
	Glow* m_glow;

	vector<Module*> m_parents;
	vector<Module*> m_children;

	//Fluxor generation
	Fluxor* m_fluxor_generated;
	bool GenerateFluxor();
	float m_fluxor_spawn_time;
	sf::Clock m_fluxor_spawn_clock;

	//Activation
	void UpdateActivation();

	//Module properties
	void ApplyModuleEffect(Fluxor* fluxor);

	//Spawn
	void ResolveProductionBufferList() override;
	vector<Fluxor*> m_fluxor_generation_buffer;

	//HUD
	sf::Text m_flux_text;
};

#endif // MODULE_H_INCLUDED
