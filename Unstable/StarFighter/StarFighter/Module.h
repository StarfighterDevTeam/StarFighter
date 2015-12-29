#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"

enum ModuleType
{
	ModuleType_A,
};

class Module : public GameObject
{
public :
	Module();
	Module(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	static Module* CreateModule(sf::Vector2u grid_index, ModuleType moduleType);

	virtual ~Module();
	void update(sf::Time deltaTime) override;

	ModuleType m_moduleType;
	unsigned int m_flux;
};

#endif // MODULE_H_INCLUDED
