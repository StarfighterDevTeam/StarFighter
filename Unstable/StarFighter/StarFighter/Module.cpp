#include "Module.h"

extern Game* CurrentGame;

using namespace sf;


void Module::Init()
{
	m_flux = 0;

	//Flux display
	m_flux_text.setFont(*(*CurrentGame).font2);
	m_flux_text.setCharacterSize(20);
	m_flux_text.setColor(sf::Color::White);
	m_flux_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + FLUX_DISPLAY_OFFSET_Y));
	(*CurrentGame).addToFeedbacks(&m_flux_text);

	//update grid index
	m_curGridIndex = GetGridIndex();
}

Module::Module()
{
	Init();
}

Module::Module(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0,0), textureName, size, origin, frameNumber, animationNumber)
{
	Init();
}

Module* Module::CreateModule(sf::Vector2u grid_index, ModuleType moduleType)
{
	grid_index.x--;
	grid_index.y--;
	std::string textureName;

	if (moduleType == ModuleType_A)
	{
		textureName = "Assets/2D/moduleA.png";
	}
		
	Module* new_module = new Module(sf::Vector2f(grid_index.x*TILE_SIZE + TILE_SIZE / 2, grid_index.y*TILE_SIZE + TILE_SIZE / 2), textureName, sf::Vector2f(TILE_SIZE, TILE_SIZE), sf::Vector2f(TILE_SIZE / 2, TILE_SIZE / 2), 1, 2);

	new_module->m_moduleType = moduleType;

	if (moduleType == ModuleType_A)
	{
		new_module->m_flux_max = MODULE_A_FLUX_MAX;
	}

	return new_module;
}


Module::~Module()
{
	
}

void Module::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);

	//hud
	ostringstream ss;
	ss << m_flux << "/" << m_flux_max;
	m_flux_text.setString(ss.str());
	m_flux_text.setPosition(sf::Vector2f(getPosition().x - m_flux_text.getGlobalBounds().width / 2, getPosition().y + m_size.y / 2 + FLUX_DISPLAY_OFFSET_Y));
}