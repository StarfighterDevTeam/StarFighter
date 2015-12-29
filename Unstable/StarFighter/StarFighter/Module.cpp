#include "Module.h"

extern Game* CurrentGame;

using namespace sf;

Module::Module()
{
	m_flux = 0;
}

Module::Module(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0,0), textureName, size, origin, frameNumber, animationNumber)
{
	m_flux = 0;
}

Module* Module::CreateModule(sf::Vector2u grid_index, ModuleType moduleType)
{
	grid_index.x--;
	grid_index.y--;
	std::string textureName;
	if (moduleType == ModuleType_A)
		textureName = "Assets/2D/moduleA.png";
	Module* new_module = new Module(sf::Vector2f(grid_index.x*TILE_SIZE + TILE_SIZE / 2, grid_index.y*TILE_SIZE + TILE_SIZE / 2), textureName, sf::Vector2f(TILE_SIZE, TILE_SIZE), sf::Vector2f(TILE_SIZE / 2, TILE_SIZE / 2), 1, 2);

	new_module->m_moduleType = moduleType;

	return new_module;
}


Module::~Module()
{
	
}

void Module::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}