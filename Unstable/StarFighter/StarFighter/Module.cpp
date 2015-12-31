#include "Module.h"

extern Game* CurrentGame;

using namespace sf;


void Module::Initialize()
{
	m_flux = 0;
	m_flux_max = 1;
	m_activated = false;
	m_glow = new Glow(this, sf::Color::Blue, MODULE_GLOW_RADIUS, 1, MODULE_GLOW_ANIMATION_DURATION, MODULE_GLOW_MIN_RADIUS);
	m_glow->visible = false;

	m_isGeneratingFluxor = false;
	m_isConsummingFlux = false;
	m_isRefillingFlux = false;
	m_flux_transfer_delay = 0.1f;

	//Flux display
	m_flux_text.setFont(*(*CurrentGame).font2);
	m_flux_text.setCharacterSize(20);
	m_flux_text.setColor(sf::Color::White);
	m_flux_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + MODULE_FLUX_DISPLAY_OFFSET_Y));
	(*CurrentGame).addToFeedbacks(&m_flux_text);

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());
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

	switch (moduleType)
	{
		case ModuleType_Generator:
		{
			textureName = "Assets/2D/module_generator.png";
			break;
		}
		case ModuleType_Armory:
		{
			textureName = "Assets/2D/module_armory.png";
			break;
		}
		case ModuleType_Battery:
		{
			textureName = "Assets/2D/module_battery.png";
			break;
		}
		case ModuleType_Relay:
		{
			textureName = "Assets/2D/module_relay.png";
			break;
		}
		case ModuleType_Factory:
		{
			textureName = "Assets/2D/module_factory.png";
			break;
		}
		case ModuleType_Shield:
		{
			textureName = "Assets/2D/module_shield.png";
			break;
		}
		case ModuleType_Turret:
		{
			textureName = "Assets/2D/module_turret.png";
			break;
		}
		case ModuleType_Switch:
		{
			textureName = "Assets/2D/module_switch.png";
			break;
		}
		case ModuleType_Amplifier:
		{
			textureName = "Assets/2D/module_amplifier.png";
			break;
		}
		case ModuleType_Accelerator:
		{
			textureName = "Assets/2D/module_accelerator.png";
			break;
		}
	}
	
	const unsigned int W = TILE_SIZE;
	const unsigned int H = TILE_SIZE;

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(W, H), 1, 1);
	setOrigin(sf::Vector2f(W / 2, H / 2));

	Initialize();

	//type specific parameters
	switch (moduleType)
	{
		case ModuleType_Generator:
		{
			m_flux_max = 100;
			m_isGeneratingFluxor = true;
			m_fluxor_generated_type = FluxorType_Green;
			m_fluxor_generation_time = 3.f;
			break;
		}
		case ModuleType_Armory:
		{
			m_flux_max = 100;
			break;
		}
		case ModuleType_Battery:
		{
			m_flux_max = 1500;
			m_isConsummingFlux = true;
			break;
		}
		case ModuleType_Relay:
		{
			m_flux_max = 5;
			m_isRefillingFlux = true;
			break;
		}
		case ModuleType_Factory:
		{
			m_flux_max = 50;
			break;
		}
		case ModuleType_Shield:
		{
			m_flux_max = 100;
			break;
		}
		case ModuleType_Turret:
		{
			m_flux_max = 200;
			break;
		}
		case ModuleType_Switch:
		{
			m_flux_max = 1;
			break;
		}
		case ModuleType_Amplifier:
		{
			m_flux_max = 1;
			break;
		}
		case ModuleType_Accelerator:
		{
			 m_flux_max = 1;
			break;
		}
	}
}

Module* Module::Clone()
{
	Module* clone = new Module(this->getPosition(), this->m_textureName, this->m_size, sf::Vector2f(this->m_size.x / 2, this->m_size.y / 2), this->m_frameNumber, this->m_animationNumber);
	clone->m_speed = this->m_speed;
	clone->m_collider_type = this->m_collider_type;
	clone->m_layer = this->m_layer;
	clone->m_moduleType = this->m_moduleType;
	clone->m_flux = this->m_flux;
	clone->m_flux_max = this->m_flux_max;

	return clone;
}

Module* Module::CreateModule(sf::Vector2u grid_index, ModuleType moduleType)
{
	Module* new_module = new Module(moduleType);

	grid_index.x--;
	grid_index.y--;
	new_module->setPosition(sf::Vector2f(grid_index.x*TILE_SIZE + TILE_SIZE / 2, grid_index.y*TILE_SIZE + TILE_SIZE / 2));
	if (new_module->m_glow)
	{
		new_module->m_glow->setPosition(new_module->getPosition());
	}

	(*CurrentGame).addToScene(new_module, ModuleLayer, ModuleObject);
	(*CurrentGame).addToScene(new_module->m_glow, GlowLayer, BackgroundObject);

	//update game grid knownledge
	if (!(*CurrentGame).isCellFree(grid_index))
	{
		(*CurrentGame).m_module_grid[grid_index.x][grid_index.y]->GarbageMe = true;
	}
	(*CurrentGame).m_module_grid[grid_index.x][grid_index.y] = (GameObject*)new_module;


	return new_module;
}


Module::~Module()
{
	(*CurrentGame).removeFromFeedbacks(&m_flux_text);
	if (m_glow)
	{
		m_glow->visible = false;
		m_glow->GarbageMe = true;
	}
}

void Module::update(sf::Time deltaTime)
{
	if (m_flux > m_flux_max && m_flux_max > 0)
	{
		m_flux = m_flux_max;
	}

	GameObject::update(deltaTime);

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());

	//update activation
	UpdateActivation();

	//fluxor generation
	GenerateFluxor();
	
	//hud
	ostringstream ss;
	ss << m_flux << "/" << m_flux_max;
	m_flux_text.setString(ss.str());
	m_flux_text.setPosition(sf::Vector2f(getPosition().x - m_flux_text.getGlobalBounds().width / 2, getPosition().y + m_size.y / 2 + MODULE_FLUX_DISPLAY_OFFSET_Y));
}

void Module::GetFluxor(GameObject* object)
{
	if (object)
	{
		Fluxor* fluxor = (Fluxor*)object;
		ApplyModuleEffect(fluxor);	
	}
}

void Module::UpdateActivation()
{
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
}

bool Module::GenerateFluxor()
{
	if (m_isGeneratingFluxor && m_activated)
	{
		if (m_fluxor_spawn_clock.getElapsedTime().asSeconds() > m_fluxor_generation_time)
		{
			Fluxor* fluxor = new Fluxor(m_fluxor_generated_type);

			//HACK
			fluxor->m_speed = sf::Vector2f(100, 0);
			fluxor->m_absolute_speed = 100;
			fluxor->setPosition(getPosition());
			fluxor->m_guided = true;

			(*CurrentGame).addToScene(fluxor, FluxorLayer, FluxorObject);
			//flux display
			if (fluxor->m_isDisplayingFlux)
			{
				(*CurrentGame).addToFeedbacks(&fluxor->m_flux_text);
			}

			m_fluxor_spawn_clock.restart();

			return true;
		}
	}

	return false;
}

void Module::ApplyModuleEffect(Fluxor* fluxor)
{
	if (fluxor)
	{
		if (fluxor->m_FluxorType == FluxorType_Green)
		{
			if (m_moduleType == ModuleType_Relay)
			{
				//example
		
			}

			//module "consumption"
			if (m_isConsummingFlux)
			{
				if (m_flux < m_flux_max && fluxor->m_flux > 0)
				{
					m_flux++;
					fluxor->m_flux--;
					fluxor->m_docked = true;
				}
				else
				{
					fluxor->m_docked = false;
				}
			}

			//module "refill fluxor"
			if (m_isRefillingFlux)
			{
				ManageTransfer(fluxor, m_flux);
			}

			//module "factory"
			if (m_isGeneratingFluxor)
			{
				Fluxor* new_fluxor = new Fluxor(m_fluxor_generated_type);
				new_fluxor->m_speed = (sf::Vector2f(0, 100));
				m_fluxor_generation_buffer.push_back(new_fluxor);
			}
		}
	}
}

void Module::ManageTransfer(Fluxor* fluxor, unsigned int flux_to_transfer)
{
	//Fluxor already exists in known transfers?
	map<Fluxor*, unsigned int>::iterator it = m_module_to_fluxor_transfer_buffer.find(fluxor);
	if (it != this->m_module_to_fluxor_transfer_buffer.end())
	{
		//Conditions for transfer are valid?
		if (fluxor->m_flux < fluxor->m_flux_max && m_flux > 0)
		{
			//manage transfer
			if (m_flux_transfer_limiter_clock.getElapsedTime().asSeconds() > m_flux_transfer_delay)
			{
				map<Fluxor*, unsigned int>::iterator it(m_module_to_fluxor_transfer_buffer.begin());
				it->first->m_flux++;//transfer flux to Fluxor
				it->second--;//substract from amount remaining to transfer
				m_flux_transfer_limiter_clock.restart();
				it->first->m_flux_waste_clock.restart();

				//transfer finished?
				if (it->second == 0)
				{
					//thanks bye
					m_module_to_fluxor_transfer_buffer.erase(fluxor);
					fluxor->m_docked = false;
				}
			}
		}
		//Conditions for transfer are invalid
		else
		{
			//thanks bye
			m_module_to_fluxor_transfer_buffer.erase(fluxor);
			fluxor->m_docked = false;
		}
	}
	//New Fluxor transfer
	else
	{
		//Conditions for transfer are valid?
		if (fluxor->m_flux < fluxor->m_flux_max && m_flux > 0)
		{
			//welcome
			m_module_to_fluxor_transfer_buffer[fluxor] = flux_to_transfer;
			fluxor->m_docked = true;
			map<Fluxor*, unsigned int>::iterator it = m_module_to_fluxor_transfer_buffer.find(fluxor);

			//manage transfer
			if (m_flux_transfer_limiter_clock.getElapsedTime().asSeconds() > m_flux_transfer_delay)
			{
				it->first->m_flux++;//transfer flux to Fluxor
				it->second--;//substract from amount remaining to transfer
				m_flux_transfer_limiter_clock.restart();
				it->first->m_flux_waste_clock.restart();

				//transfer finished?
				if (it->second == 0)
				{
					//thanks bye
					m_module_to_fluxor_transfer_buffer.erase(fluxor);
					fluxor->m_docked = false;
				}
			}
		}
		else
		{
			//thanks bye, no need to subscribe
			fluxor->m_docked = false;
		}
	}
}

void Module::ResolveProductionBufferList()
{
	size_t fluxorGenerationBufferSize = m_fluxor_generation_buffer.size();
	if (fluxorGenerationBufferSize > 0)
	{
		for (size_t i = 0; i < fluxorGenerationBufferSize; i++)
		{
			(*CurrentGame).addToScene(m_fluxor_generation_buffer[i], FluxorLayer, FluxorObject);
		}

		m_fluxor_generation_buffer.clear();
	}
}