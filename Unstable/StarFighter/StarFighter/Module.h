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

	ModuleType m_moduleType;
	unsigned int m_flux;
	unsigned int m_flux_max;
	sf::Vector2u m_curGridIndex;
	bool m_activated;
	Glow* m_glow;

	vector<Module*> m_parents;
	vector<Module*> m_children;

	Fluxor m_fluxor_generated;

	//HUD
	sf::Text m_flux_text;
};

#endif // MODULE_H_INCLUDED
