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
	m_fluxor_generation_cost = 0;
	m_add_speed = 0;

	//Flux display
	m_flux_text.setFont(*(*CurrentGame).font2);
	m_flux_text.setCharacterSize(20);
	m_flux_text.setColor(sf::Color::White);
	m_flux_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + MODULE_FLUX_DISPLAY_OFFSET_Y));
	(*CurrentGame).addToFeedbacks(&m_flux_text);

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());

	//links
	GameObject arrow = GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "Assets/2D/arrow.png", sf::Vector2f(12, 18), sf::Vector2f(6, 9), 1, 2);
	for (int i = 0; i < 4; i++)
	{
		m_link[i].m_exists = i == 0;
		m_link[i].m_activated = false;

		m_arrow[i] = arrow.Clone();
		m_arrow[i]->visible = m_link[i].m_exists;
		float angle = i * 90;
		m_arrow[i]->rotate(i * 90);
	}
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
			m_fluxor_generated_type = FluxorType_Blue;
			m_fluxor_generation_time = 3.f;
			//m_fluxor_generation_cost = m_flux_max;
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
			m_flux_max = 10;
			m_isGeneratingFluxor = true;
			m_fluxor_generated_type = FluxorType_Red;
			m_fluxor_generation_time = 3.f;
			m_fluxor_generation_cost = m_flux_max;
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
			m_flux_max = 30;
			m_add_speed = 100;
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
	for (int i = 0; i < 4; i++)
	{
		new_module->m_arrow[i]->setPosition(new_module->getPosition().x + cos(i * M_PI_2)*(new_module->m_size.x / 2 - new_module->m_arrow[i]->m_size.x / 2), new_module->getPosition().y + sin(i * M_PI_2)*(new_module->m_size.x / 2 - new_module->m_arrow[i]->m_size.x / 2));
	}

	(*CurrentGame).addToScene(new_module, ModuleLayer, ModuleObject);
	(*CurrentGame).addToScene(new_module->m_glow, GlowLayer, BackgroundObject);
	for (int i = 0; i < 4; i++)
	{
		(*CurrentGame).addToScene(new_module->m_arrow[i], GlowLayer, BackgroundObject);
	}

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

	for (int i = 0; i < 4; i++)
	{
		m_arrow[i]->visible = false;
		m_arrow[i]->GarbageMe = true;
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

	UpdateActivation();

	UpdateLinks();

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
	if (m_isGeneratingFluxor && m_flux == m_flux_max)//m_flux >= m_fluxor_generation_cost)
	{
		if (m_fluxor_spawn_clock.getElapsedTime().asSeconds() > m_fluxor_generation_time)
		{
			Fluxor* fluxor = new Fluxor(m_fluxor_generated_type);
			m_flux -= m_fluxor_generation_cost;

			//HACK
			int main_link = GetMainLink();
			if (main_link >= 0)
			{
				fluxor->SetSpeedVectorFromAbsoluteSpeedAndAngle(FLUXOR_GUIDED_BASE_SPEED, main_link * M_PI_2 - M_PI_2);
			}
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
		//fluxor accelerator
		if (!fluxor->m_docked)
		{
			fluxor->AddSpeed(&fluxor->m_speed, (float)m_add_speed);
		}

		if (fluxor->m_FluxorType == FluxorType_Blue && fluxor->m_life_clock.getElapsedTime().asSeconds() > 0.5f)//avoid being consummed by the very module that created it
		{
			//consumption (automatic)
			if (fluxor->m_flux > 0)
			{
				if (m_flux < m_flux_max)
				{
					m_flux++;
					fluxor->m_flux--;
					fluxor->m_docked = true;
				}
				else
				{
					fluxor->m_docked = false;
					//fluxor->GarbageMe = true;
				}
			}
			else
			{
				fluxor->GarbageMe = true;
			}
		}

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
				if (fluxor->m_flux < fluxor->m_flux_max && m_flux > 0)
				{
					if (fluxor->m_transfer_buffer == 0)
					{
						fluxor->m_transfer_buffer = m_flux;
						fluxor->m_docked = true;
						fluxor->m_flux_transfer_clock.restart();
					}

					if (fluxor->m_flux_transfer_clock.getElapsedTime().asSeconds() > m_flux_transfer_delay)
					{
						fluxor->m_flux++;
						fluxor->m_transfer_buffer--;
						fluxor->m_flux_transfer_clock.restart();
						fluxor->m_flux_waste_clock.restart();
					}

					if (fluxor->m_transfer_buffer == 0)
					{
						fluxor->m_docked = false;
					}
				}
				else
				{
					fluxor->m_docked = false;
					fluxor->m_transfer_buffer = 0;
				}
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

void Module::ResolveProductionBufferList()
{
	size_t fluxorGenerationBufferSize = m_fluxor_generation_buffer.size();
	if (fluxorGenerationBufferSize > 0)
	{
		for (size_t i = 0; i < fluxorGenerationBufferSize; i++)
		{
			int main_link = GetMainLink();
			if (main_link >= 0)
			{
				m_fluxor_generation_buffer[i]->SetSpeedVectorFromAbsoluteSpeedAndAngle(m_fluxor_generation_buffer[i]->m_absolute_speed, main_link * M_PI_2);
			}
			
			(*CurrentGame).addToScene(m_fluxor_generation_buffer[i], FluxorLayer, FluxorObject);
		}

		m_fluxor_generation_buffer.clear();
	}
}

void Module::SwitchLinkDirection()
{
	bool bool_array[4] = { m_link[3].m_exists, m_link[0].m_exists, m_link[1].m_exists, m_link[2].m_exists };
	for (int i = 0; i < 4; i++)
	{
		m_link[i].m_exists = bool_array[i];
	}
}

void Module::UpdateLinks()
{
	for (int i = 0; i < 4; i++)
	{
		m_arrow[i]->visible = m_link[i].m_exists;
		m_arrow[i]->setAnimationLine(m_link[i].m_activated);
	}
}

int Module::GetMainLink()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_link[i].m_exists)
			return i;
	}

	return -1;
}