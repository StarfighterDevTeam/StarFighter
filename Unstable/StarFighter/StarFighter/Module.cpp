#include "Module.h"

extern Game* CurrentGame;

using namespace sf;


void Module::Initialize()
{
	m_flux = 0;
	m_fluxor_generated = NULL;

	//Flux display
	m_flux_text.setFont(*(*CurrentGame).font2);
	m_flux_text.setCharacterSize(20);
	m_flux_text.setColor(sf::Color::White);
	m_flux_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + MODULE_FLUX_DISPLAY_OFFSET_Y));
	(*CurrentGame).addToFeedbacks(&m_flux_text);

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());

	m_activated = false;
}

Module::Module()
{
	Initialize();
}

Module::Module(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0,0), textureName, size, origin, frameNumber, animationNumber)
{
	Initialize();
}

Module::Module(ModuleType moduleType)
{
	std::string textureName;
	m_moduleType = moduleType;

	if (moduleType == ModuleType_O)
	{
		textureName = "Assets/2D/moduleO.png";
	}
	else if (moduleType == ModuleType_A)
	{
		textureName = "Assets/2D/moduleA.png";
	}
	else if (moduleType == ModuleType_B)
	{
		textureName = "Assets/2D/moduleB.png";
	}
	else
	{
		textureName = "Assets/2D/moduleC.png";
	}
	
	const unsigned int W = TILE_SIZE;
	const unsigned int H = TILE_SIZE;

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(W, H), 1, 1);
	setOrigin(sf::Vector2f(W / 2, H / 2));
}

Module* Module::Clone()
{
	Module* clone = new Module(this->getPosition(), this->m_textureName, this->m_size, sf::Vector2f(this->m_size.x / 2, this->m_size.y / 2), this->m_frameNumber, this->m_animationNumber);
	clone->m_speed = this->m_speed;
	clone->m_collider_type = this->m_collider_type;
	clone->m_layer = this->m_layer;
	clone->m_moduleType = this->m_moduleType;

	return clone;
}

Module* Module::CreateModule(sf::Vector2u grid_index, ModuleType moduleType)
{
	Module* new_module = (*CurrentGame).m_module_list[moduleType]->Clone();

	grid_index.x--;
	grid_index.y--;
	new_module->setPosition(sf::Vector2f(grid_index.x*TILE_SIZE + TILE_SIZE / 2, grid_index.y*TILE_SIZE + TILE_SIZE / 2));

	if (moduleType == ModuleType_O)
	{
		new_module->m_flux_max = MODULE_O_FLUX_MAX;
	}
	else if (moduleType == ModuleType_A)
	{
		new_module->m_flux_max = MODULE_A_FLUX_MAX;

		//HACK PROTO
		new_module->m_flux = 200;
		new_module->m_fluxor_generated = (*CurrentGame).m_fluxor_list[FluxorType_Green];
		new_module->m_fluxor_spawn_time = 1.f;
		new_module->m_fluxor_spawn_clock.restart();
	}
	else if (moduleType == ModuleType_B)
	{
		new_module->m_flux_max = MODULE_B_FLUX_MAX;
	}
	else
	{
		new_module->m_flux_max = MODULE_C_FLUX_MAX;
	}

	new_module->m_glow = new Glow(new_module, sf::Color::Blue, MODULE_GLOW_RADIUS, 1, MODULE_GLOW_ANIMATION_DURATION, MODULE_GLOW_MIN_RADIUS);

	(*CurrentGame).addToScene(new_module, ModuleLayer, ModuleObject);
	(*CurrentGame).addToScene(new_module->m_glow, GlowLayer, BackgroundObject);

	return new_module;
}


Module::~Module()
{
	(*CurrentGame).removeFromFeedbacks(&m_flux_text);
}

void Module::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());

	//update activation
	if (!m_parents.empty())
	{
		bool parent_activated = false;
		size_t parentsVectorSize = m_parents.size();
		for (size_t i = 0; i < parentsVectorSize; i++)
		{
			if (m_parents[i]->m_activated)
			{
				parent_activated = true;
				break;
			}
		}
		m_activated = m_flux == m_flux_max && parent_activated;
	}
	else
	{
		m_activated = m_flux == m_flux_max;
	}
	m_glow->visible = m_activated;

	//fluxor generation
	GenerateFluxor();
	
	//hud
	ostringstream ss;
	ss << m_flux << "/" << m_flux_max;
	m_flux_text.setString(ss.str());
	m_flux_text.setPosition(sf::Vector2f(getPosition().x - m_flux_text.getGlobalBounds().width / 2, getPosition().y + m_size.y / 2 + MODULE_FLUX_DISPLAY_OFFSET_Y));
}

bool Module::GenerateFluxor()
{
	if (m_fluxor_generated && m_activated)
	{
		if (m_fluxor_spawn_clock.getElapsedTime().asSeconds() > m_fluxor_spawn_time)
		{
			Fluxor* fluxor = m_fluxor_generated->Clone();
			fluxor->m_speed = sf::Vector2f(100, 0);
			fluxor->m_absolute_speed = 100;
			fluxor->setPosition(getPosition());
			fluxor->m_guided = true;

			(*CurrentGame).addToScene(fluxor, FluxorLayer, FluxorObject);

			m_fluxor_spawn_clock.restart();

			return true;
		}
	}

	return false;
}