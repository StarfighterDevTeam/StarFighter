#include "Module.h"

extern Game* CurrentGame;

using namespace sf;


void Module::Initialize()
{
	m_flux = 0;
	m_flux_max = 1;
	m_flux_max_under_construction = 1;
	m_flux_max_after_construction = 1;
	m_glow = new Glow(this, m_team, MODULE_GLOW_RADIUS, 1, MODULE_GLOW_ANIMATION_DURATION, MODULE_GLOW_MIN_RADIUS);
	m_glow->m_visible = false;
	SetConstructionStatus(true);

	m_isGeneratingFluxor = false;
	m_has_child_to_refill = false;
	m_isRefillingFlux = false;
	m_flux_transfer_delay = MODULE_TRANSFER_DELAY;
	m_fluxor_generation_cost = 0;
	m_add_speed = 0;
	m_add_flux = 0;
	m_isAutogeneratingFlux = false;
	m_turret_range = 0;
	m_upgrade_player_stats = false;
	m_isCondensatingFluxor = false;

	//Flux display
	m_flux_text = new SFText(((*CurrentGame).font2), 20, sf::Color::Green, sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + MODULE_FLUX_DISPLAY_OFFSET_Y), m_team);
	m_flux_text->m_alliance = (TeamAlliances)(*CurrentGame).GetTeamAlliance(m_team);

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());

	//links
	GameObject arrow = GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "Assets/2D/arrow.png", sf::Vector2f(12, 18), sf::Vector2f(6, 9), 1, 3);
	for (int i = 0; i < 4; i++)
	{
		m_link[i].m_exists = i == 0;
		m_link[i].m_activated = Link_Deactivated;

		m_arrow[i] = arrow.Clone();
		m_arrow[i]->m_visible = m_link[i].m_exists;
		float angle = i * 90;
		m_arrow[i]->rotate(i * 90);

		m_linked_modules[i] = NULL;
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

Module::Module(ModuleType moduleType) : Module(moduleType, PlayerNeutral)
{

}

Module::Module(ModuleType moduleType, PlayerTeams team)
{
	std::string textureName;
	m_moduleType = moduleType;
	m_team = team;

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
		case ModuleType_Factory_Up:
		{
			textureName = "Assets/2D/module_factory_up.png";
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
		case ModuleType_Barrier:
		{
			textureName = "Assets/2D/module_barrier.png";
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
		case ModuleType_Condensator:
		{
			textureName = "Assets/2D/module_condensator.png";
			break;
		}
			
		default:
		{
			textureName = "Assets/2D/module_generator.png";
			break;
		}
	}
	
	const unsigned int W = TILE_SIZE;
	const unsigned int H = TILE_SIZE;

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(W, H), 1, 2);
	setOrigin(sf::Vector2f(W / 2, H / 2));

	Initialize();

	//need to have created flux text before using this -> Initialize()
	SetTeam(team, (*CurrentGame).GetTeamAlliance(team));

	//type specific parameters
	switch (moduleType)
	{
		case ModuleType_Generator:
		{	 
			m_flux_max_after_construction = 10;
			m_flux_max_under_construction = 500;
			m_isAutogeneratingFlux = true;
			m_flux_autogeneration_time = 0.5f;
			m_isGeneratingFluxor = true;
			m_fluxor_generated_type = FluxorType_Blue;
			m_fluxor_generation_time = 0.5f;
			m_fluxor_generation_cost = FLUXOR_FLUX_VALUE;
			break;
		}
		case ModuleType_Armory:
		{
			m_flux_max_after_construction = 20;
			m_flux_max_under_construction = 200;
			m_upgrade_player_stats = true;
			break;
		}
		case ModuleType_Battery:
		{
			m_flux_max_after_construction = 1500;
			m_flux_max_under_construction = 200;
			break;
		}
		case ModuleType_Relay:
		{
			m_flux_max_after_construction = 10;
			m_flux_max_under_construction = 50;
			m_isRefillingFlux = true;
			break;
		}
		case ModuleType_Factory:
		{
			m_flux_max_after_construction = 10;
			m_flux_max_under_construction = 150;
			m_isGeneratingFluxor = true;
			m_fluxor_generated_type = FluxorType_Red;
			m_fluxor_generation_time = 3.f;
			m_fluxor_generation_cost = m_flux_max_after_construction;
			break;
		}
		case ModuleType_Factory_Up:
		{
			m_flux_max_after_construction = 10;
			m_flux_max_under_construction = 300;
			m_isGeneratingFluxor = true;
			m_fluxor_generated_type = FluxorType_Purple;
			m_fluxor_generation_time = 3.f;
			m_fluxor_generation_cost = m_flux_max_after_construction;
			break;
		}
		case ModuleType_Shield:
		{
			m_flux_max_after_construction = 100;
			m_flux_max_under_construction = 200;
			break;
		}
		case ModuleType_Turret:
		{
			m_flux_max_after_construction = 50;
			m_flux_max_under_construction = 300;
			m_turret_range = 2;
			m_isGeneratingFluxor = true;
			m_fluxor_generated_type = FluxorType_Black;
			m_fluxor_generation_time = 2.f;
			m_fluxor_generation_cost = 5;
			break;
		}
		case ModuleType_Barrier:
		{
			m_flux_max_after_construction = 50;
			m_flux_max_under_construction = 100;
			m_isAutogeneratingFlux = true;
			m_flux_autogeneration_time = 1.f;
		}
		case ModuleType_Amplifier:
		{
			m_flux_max_after_construction = 20;
			m_flux_max_under_construction = 250;
			m_add_flux = 10;
			break;
		}
		case ModuleType_Accelerator:
		{
			m_flux_max_after_construction = 30;
			m_flux_max_under_construction = 150;
			m_add_speed = 100;
			break;
		}
		case ModuleType_Condensator:
		{
			m_flux_max_after_construction = 50;
			m_flux_max_under_construction = 300;
			m_isCondensatingFluxor = true;
			break;
		}
	}

	if (USE_UNGUIDED_FLUXORS_TO_BUILD == true)
	{
		m_flux_max_under_construction *= USE_UNGUIDED_FLUXORS_COST_MULTIPLIER;
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

void Module::SetConstructionStatus(bool under_construction)
{
	m_under_construction = under_construction;
	if (under_construction)
	{
		setColor(Color(255, 255, 255, GHOST_ALPHA_VALUE));
	}
	else
	{
		setColor(Color(255, 255, 255, 255));
	}

	setAnimationLine(under_construction);
}

Module* Module::CreateModule(sf::Vector2u grid_index, ModuleType moduleType, PlayerTeams team, bool construction_finished, int link_activation, unsigned int flux)
{
	Module* new_module = new Module(moduleType, team);

	//construction over own existing module?
	bool construction_allowed = true;
	if ((*CurrentGame).m_module_grid[grid_index.x][grid_index.y])
	{
		if ((*CurrentGame).m_module_grid[grid_index.x][grid_index.y]->m_team != team)
		{
			//can't build on other player's modules
			construction_allowed = false;
		}
		else
		{
			//overwrite own module
			(*CurrentGame).m_module_grid[grid_index.x][grid_index.y]->m_visible = false;
			(*CurrentGame).m_module_grid[grid_index.x][grid_index.y]->m_GarbageMe = true;
		}
	}
	else if ((*CurrentGame).m_alliance_module_grid[new_module->m_alliance][grid_index.x][grid_index.y])
	{
		if ((*CurrentGame).m_alliance_module_grid[new_module->m_alliance][grid_index.x][grid_index.y]->m_team != team)
		{
			//can't build on other player's modules
			construction_allowed = false;
		}
		else
		{
			//overwrite own module
			(*CurrentGame).m_alliance_module_grid[new_module->m_alliance][grid_index.x][grid_index.y]->m_visible = false;
			(*CurrentGame).m_alliance_module_grid[new_module->m_alliance][grid_index.x][grid_index.y]->m_GarbageMe = true;
		}
	}

	//construction
	if (construction_allowed)
	{
		//module under construction goes to a temporary grid only shared with allied players, until the module is built
		(*CurrentGame).m_alliance_module_grid[new_module->m_alliance][grid_index.x][grid_index.y] = (GameObject*)new_module;

		//complete module data
		new_module->setPosition(sf::Vector2f(grid_index.x*TILE_SIZE + TILE_SIZE / 2, grid_index.y*TILE_SIZE + TILE_SIZE / 2));
		new_module->m_curGridIndex = grid_index;
		if (new_module->m_glow)
		{
			new_module->m_glow->setPosition(new_module->getPosition());
		}
		for (int i = 0; i < 4; i++)
		{
			new_module->m_arrow[i]->setPosition(new_module->getPosition().x + cos(i * M_PI_2)*(new_module->m_size.x / 2 - new_module->m_arrow[i]->m_size.x / 2), new_module->getPosition().y + sin(i * M_PI_2)*(new_module->m_size.x / 2 - new_module->m_arrow[i]->m_size.x / 2));
			new_module->m_arrow[i]->m_team = new_module->m_team;
			new_module->m_arrow[i]->m_alliance = new_module->m_alliance;
			new_module->m_arrow[i]->m_under_construction = new_module->m_under_construction;
		}

		(*CurrentGame).addToScene(new_module, ModuleLayer, ModuleObject);
		(*CurrentGame).addToScene(new_module->m_glow, GlowLayer, BackgroundObject);
		for (int i = 0; i < 4; i++)
		{
			(*CurrentGame).addToScene(new_module->m_arrow[i], GlowLayer, BackgroundObject);
		}
		(*CurrentGame).addToFeedbacks(new_module->m_flux_text);

		//overload parameters
		if (construction_finished)
		{
			new_module->FinishConstruction();
		}
		for (int i = 0; i < link_activation; i++)
		{
			new_module->SwitchLinkDirection();
		}
		new_module->m_flux = flux;
		if (flux > new_module->m_flux_max)
		{
			new_module->m_flux = new_module->m_flux_max;
		}
	}
	//no construction
	else
	{
		delete new_module;
		return NULL;
	}

	return new_module;
}

void Module::EraseModule(sf::Vector2u grid_index)
{
	//update game grid knownledge
	if ((*CurrentGame).m_module_grid[grid_index.x][grid_index.y])
	{
		(*CurrentGame).m_module_grid[grid_index.x][grid_index.y]->m_GarbageMe = true;
		(*CurrentGame).m_module_grid[grid_index.x][grid_index.y] = NULL;
	}
}

void Module::DebugFinishModule(sf::Vector2u grid_index)
{
	if ((*CurrentGame).m_module_grid[grid_index.x][grid_index.y])
	{
		Module* module = (Module*)(*CurrentGame).m_module_grid[grid_index.x][grid_index.y];
		if (module->m_under_construction)
		{
			module->FinishConstruction();
		}
	}
}

void Module::DebugRefillingModuleFlux(sf::Vector2u grid_index)
{
	if ((*CurrentGame).m_module_grid[grid_index.x][grid_index.y])
	{
		Module* module = (Module*)(*CurrentGame).m_module_grid[grid_index.x][grid_index.y];
		if (!module->m_under_construction)
		{
			module->m_flux = module->m_flux_max;
		}
	}
}

Module::~Module()
{
	if (m_flux_text)
		m_flux_text->m_GarbageMe = true;

	if (m_glow)
	{
		m_glow->m_visible = false;
		m_glow->m_GarbageMe = true;
	}

	for (int i = 0; i < 4; i++)
	{
		m_arrow[i]->m_visible = false;
		m_arrow[i]->m_GarbageMe = true;
	}

	//updating grid knowledge
	if ((*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y])
	{
		if ((*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y] == (GameObject*)this)
		{
			(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y] = NULL;
		}
		if ((*CurrentGame).m_alliance_module_grid[m_alliance][m_curGridIndex.x][m_curGridIndex.y] == (GameObject*)this)
		{
			(*CurrentGame).m_alliance_module_grid[m_alliance][m_curGridIndex.x][m_curGridIndex.y] = NULL;
		}
	}
}

void Module::FinishConstruction()
{
	//end of alpha
	SetConstructionStatus(false);

	//updating game grid knowledge
	(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y] = (GameObject*)this;

	if (m_flux_text)
	{
		m_flux_text->setColor(sf::Color::Cyan);
		m_flux_text->m_team = PlayerNeutral;
		m_flux_text->m_alliance = AllianceNeutral;
	}
		
	m_under_construction = false;
	m_flux = 0;
	m_flux_max = m_flux_max_after_construction;
	m_flux_autogeneration_clock.restart();
	m_fluxor_spawn_clock.restart();
	m_flux_consumption_clock.restart();

	//wipe out unguided Fluxors that are found on the cell being built on
	size_t FluxorVectorSize = (*CurrentGame).GetSceneGameObjectsTyped(FluxorUnguidedObject).size();
	for (size_t i = 0; i < FluxorVectorSize; i++)
	{
		if ((*CurrentGame).GetSceneGameObjectsTyped(FluxorUnguidedObject)[i] == NULL)
			continue;

		Fluxor* fluxor = (Fluxor*)(*CurrentGame).GetSceneGameObjectsTyped(FluxorUnguidedObject)[i];
		if (fluxor->m_visible && !fluxor->m_guided)
		{
			//quick collision test
			sf::IntRect boundsA(SimpleCollision::FToIRect(fluxor->getGlobalBounds()));
			sf::IntRect boundsB(SimpleCollision::FToIRect(this->getGlobalBounds()));
			if (boundsA.intersects(boundsB))
			{
				fluxor->m_GarbageMe = true;
			}
		}
	}
}

void Module::update(sf::Time deltaTime)
{
	//construction under progress
	if (m_under_construction)
	{
		//too late, a module has been built on this cell already
		if ((*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y] && (*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y]->m_visible && !(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y]->m_GarbageMe)
		{
			m_visible = false;
			m_GarbageMe = true;
		}
		else
		{
			//construction finished
			if (m_flux == m_flux_max_under_construction)
			{
				FinishConstruction();
			}
			//still under construction
			else if (m_construction_clock.getElapsedTime().asSeconds() > (1.f / MODULE_FLUX_CONSTRUCTION_PER_SECOND))
			{
				m_flux++;
				m_construction_clock.restart();
			}
		}
	}
	m_flux_max = m_under_construction ? m_flux_max_under_construction : m_flux_max_after_construction;

	if (m_flux > m_flux_max && m_flux_max > 0)
	{
		m_flux = m_flux_max;
	}
	if (m_glow)
	{
		m_glow->m_visible = m_flux == m_flux_max && !m_under_construction;
	}

	GameObject::update(deltaTime);

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());

	UpdateLinks();

	if (!m_under_construction)
	{
		AutogenerateFlux();

		GenerateFluxor();
	}
	
	//hud
	if (m_flux_text && m_flux_text->m_visible)
	{
		ostringstream ss;
		ss << m_flux;
		if (m_flux_max > 0)
		{
			ss << "/" << m_flux_max;
		}
		m_flux_text->setString(ss.str());

		ss << m_flux << "/" << m_flux_max;
		m_flux_text->setPosition(sf::Vector2f(getPosition().x - m_flux_text->getGlobalBounds().width / 2, getPosition().y + m_size.y / 2 + MODULE_FLUX_DISPLAY_OFFSET_Y));
	}
}

void Module::GetFluxor(GameObject* object)
{
	if (object)
	{
		Fluxor* fluxor = (Fluxor*)object;

		if (!m_under_construction)
		{
			if (!fluxor->m_guided)
			{
				float angle = GetAngleRadBetweenPositions(fluxor->getPosition(), fluxor->m_initial_position);
				fluxor->SetSpeedVectorFromAbsoluteSpeedAndAngle(fluxor->m_absolute_speed, angle);
				fluxor->setPosition(fluxor->m_initial_position);
				fluxor->m_turn_delay = Fluxor::RandomizeTurnDelay();
				fluxor->m_turn_clock.restart();
			}
			else
			{
				//check if the guided Fluxor passes right through the middle of the Module
				if (GameObject::DistancePointToSement(getPosition().x, getPosition().y, fluxor->m_initial_position.x, fluxor->m_initial_position.y, fluxor->getPosition().x, fluxor->getPosition().y) == 0)
				{
					//make sure a fluxor leaving the module is not colliding again
					if (fluxor->m_initial_position == getPosition() && !fluxor->m_docked)
					{
						//do nothing
					}
					else
					{
						ApplyModuleEffect(fluxor);
					}
				}
			}
		}
	}
}

bool Module::GenerateFluxor()
{
	if (m_isGeneratingFluxor && m_flux == m_flux_max)
	{
		Fluxor* pFluxor = (*CurrentGame).m_fluxors[m_fluxor_generated_type];

		if (!pFluxor->m_needs_link_to_circulate || m_has_child_to_refill)//if it needs a link to circulate, we check that an activated link exists and that a linked module that need this ressource
		{
			//turret?
			Fluxor* pTarget = NULL;
			if (m_turret_range > 0)
			{
				pTarget = SearchNearbyAttackers(m_team, (m_turret_range + 1) * TILE_SIZE);//+1 because this is what we actually mean by "range N": it avoids an enemy to reach a module located N tiles away
			}

			if (m_turret_range == 0 || pTarget)//turrets only fire when a target is in sight within range
			{
				if (m_fluxor_spawn_clock.getElapsedTime().asSeconds() > m_fluxor_generation_time)
				{
					Fluxor* fluxor = new Fluxor(m_fluxor_generated_type);

					fluxor->m_team = m_team;
					fluxor->m_alliance = m_alliance;
					fluxor->m_guided = true;
					if (pTarget)
					{
						fluxor->m_target = pTarget;
					}

					m_flux -= m_fluxor_generation_cost;

					//turret?
					if (m_turret_range == 0)
					{
						fluxor->m_absolute_speed = FLUXOR_GUIDED_BASE_SPEED;
					}
					else
					{
						fluxor->m_absolute_speed = FLUXOR_BULLET_SPEED;
					}
					
					UpdateFluxorDirection(fluxor);

					fluxor->setPosition(getPosition());
					fluxor->m_initial_position = getPosition();

					(*CurrentGame).addToScene(fluxor, FluxorLayer, FluxorGuidedObject);
					//flux display
					if (fluxor->m_displaying_flux)
					{
						(*CurrentGame).addToFeedbacks(fluxor->m_flux_text);
					}

					m_fluxor_spawn_clock.restart();

					fluxor->m_modules_visited.push_back(this);//must come after UpdateFluxorDirection(fluxor)

					return true;
				}
			}
		}

		return false;
	}
	else
	{
		m_fluxor_spawn_clock.restart();
		return false;
	}
}

void Module::AutogenerateFlux()
{
	if (m_isAutogeneratingFlux)
	{
		if (m_flux < m_flux_max)
		{
			if (m_flux_autogeneration_clock.getElapsedTime().asSeconds() > m_flux_autogeneration_time)
			{
				m_flux++;
				m_flux_autogeneration_clock.restart();
			}
		}
		else
		{
			m_flux_autogeneration_clock.restart();
		}	
	}
}

void Module::ConsummeFluxor(Fluxor* fluxor)
{
	if (fluxor)
	{
		if (m_flux < m_flux_max && fluxor->m_flux > 0)
		{
			if (m_flux_consumption_clock.getElapsedTime().asSeconds() > m_flux_transfer_delay)
			m_flux++;
			fluxor->m_flux--;

			//consumption finished?
			if (m_flux == m_flux_max || fluxor->m_flux == 0)
			{
				UpdateLinks();
				UndockFluxor(fluxor);
			}
			else
			{
				fluxor->m_docked = true;
			}
		}
	}
}

void Module::CondensateFluxor(Fluxor* fluxor)
{
	if (fluxor)
	{
		if (m_flux == m_flux_max)
		{
			if (fluxor->m_condensed_to_circulate)
			{
				//end of condensed effect
				fluxor->m_condensed_to_circulate = false;
				fluxor->setColor(sf::Color(255, 255, 255, GHOST_ALPHA_VALUE));
			}
			else
			{
				if (m_isCondensatingFluxor && fluxor->m_needs_link_to_circulate)
				{
					fluxor->m_condensed_to_circulate = true;
					fluxor->setColor(sf::Color(255, 255, 255, 255));
				}
			}
		}
	}
}

void Module::AmplifyFluxor(Fluxor* fluxor)
{
	if (fluxor)
	{
		if (fluxor->m_flux < fluxor->m_flux_max || (fluxor->m_flux_max == 0 && m_add_flux > 0))
		{
			if (fluxor->m_transfer_buffer == 0)
			{
				//increasing potential
				if (m_add_flux > 0 && fluxor->m_flux_max > 0)
				{
					fluxor->m_flux_max += m_add_flux;
				}
				//refilling or increasing?
				fluxor->m_transfer_buffer = m_isRefillingFlux ? fluxor->m_flux_max - fluxor->m_flux : m_add_flux;
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
				UndockFluxor(fluxor);
			}
			else
			{
				fluxor->m_docked = true;
			}
		}
		else
		{
			UndockFluxor(fluxor);
			fluxor->m_transfer_buffer = 0;
		}
	}
}

void Module::AttackModule(Fluxor* fluxor)
{
	if (fluxor)
	{
		if (m_flux >= 0 && fluxor->m_flux > 0)
		{
			if (fluxor->m_flux_attack_clock.getElapsedTime().asSeconds() > fluxor->m_flux_attack_delay)
			{
				//kill?
				if (m_flux == 0)
				{
					this->m_GarbageMe = true;
				}
				else
				{
					m_flux--;
					fluxor->m_flux--;
					if (fluxor->m_flux_stealer)
					{
						fluxor->m_flux_stolen++;
					}
				}

				fluxor->m_flux_attack_clock.restart();
			}
			
			//consumption finished?
			if (this->m_GarbageMe || fluxor->m_flux == 0)
			{
				fluxor->m_docked = false;
			}
			else
			{
				fluxor->m_docked = true;
			}
		}
	}
}

void Module::ApplyModuleEffect(Fluxor* fluxor)
{
	if (fluxor)
	{
		fluxor->m_modules_visited.push_back(this);

		//by default, we assume at each frame that the Fluxor will be released unless told otherwise.
		//therefore each method must explicitly lock it again if needed, every frame with "fluxor->m_docked = true"
		if (fluxor->m_team == this->m_team || fluxor->m_alliance == this->m_alliance)
		{
			fluxor->m_docked = false;

			if (!m_under_construction)
			{
				//module "condensation"
				CondensateFluxor(fluxor);

				//module "refill/amplify fluxor"
				if (fluxor->m_can_be_refilled_by_modules)
				{
					//if (m_flux == m_flux_max && ((m_isRefillingFlux && !fluxor->m_consummable_by_modules) || (fluxor->m_consummable_by_modules && m_add_flux > 0)))
					if (m_flux == m_flux_max && (m_isRefillingFlux || m_add_flux > 0) && (fluxor->m_can_be_refilled_by_modules || !fluxor->m_consummable_by_modules))
					{
						if (!fluxor->m_needs_link_to_circulate || m_has_child_to_refill)//if it needs a link to circulate and no activated link exists, amplyfing is pointless. Also if no child needs to be refilled.
						{
							AmplifyFluxor(fluxor);
						}
					}
				}

				//consumption (automatic) - must be done last
				if (fluxor->m_consummable_by_modules)
				{
					ConsummeFluxor(fluxor);
				}
			}

			////module "factory"
			//if (m_isGeneratingFluxor)
			//{
			//	Fluxor* new_fluxor = new Fluxor(m_fluxor_generated_type);
			//	new_fluxor->m_speed = (sf::Vector2f(0, 100));
			//	m_fluxor_generation_buffer.push_back(new_fluxor);
			//}

			if (!fluxor->m_docked)
			{
				UndockFluxor(fluxor);
				UpdateFluxorDirection(fluxor);

				//accelerator
				if (m_add_speed != 0 && m_flux == m_flux_max)
				{
					fluxor->AddSpeed(&fluxor->m_speed, (float)m_add_speed);
					fluxor->NormalizeSpeed(&fluxor->m_speed, FLUXOR_GUIDED_MAX_SPEED);
					fluxor->m_absolute_speed = GetAbsoluteSpeed(fluxor->m_speed);
				}
			}
			else
			{
				fluxor->setPosition(getPosition());
			}
		}
		//interaction with enemy modules
		else
		{
			fluxor->m_docked = false;

			if (fluxor->m_flux_attacker)
			{
				AttackModule(fluxor);
			}

			if (!fluxor->m_docked)
			{
				fluxor->setPosition(this->getPosition());
				if (!fluxor->m_flux_attack_piercing)
				{
					if (fluxor->m_flux_stealer)
					{
						fluxor->BringStealerBack();
					}
					else
					{
						fluxor->m_GarbageMe = true;
					}
				}
			}
			else
			{
				fluxor->setPosition(this->getPosition());
			}
		}
	}
}

bool Module::UndockFluxor(Fluxor* fluxor)
{
	if (fluxor)
	{
		if (!fluxor->m_consummable_by_modules || fluxor->m_condensed_to_circulate || IsMainLinkActivated())//the Fluxor may go on if it is not consummed by modules, unless condensated, OR has an active link to continue its journey
		{
			fluxor->m_docked = false;
			return true;
		}
		else
		{
			fluxor->m_GarbageMe = true;
			return false;
		}
	}
	else
	{
		printf("<!> Trying to Module::UndockFluxor(Fluxor* fluxor) a NULL pointer.\n");
		return false;
	}
}

bool Module::IsMainLinkActivated()
{
	int main_link = GetMainLinkIndex();

	return main_link >= 0 && m_link[main_link].m_activated == Link_Activated;
}

Module* Module::GetMainLinkedModule()
{
	int main_link = GetMainLinkIndex();

	if (main_link >= 0 && m_link[main_link].m_activated == Link_Activated)
	{
		return m_linked_modules[main_link];
	}
	else
	{
		return NULL;
	}
}

void Module::ResolveProductionBufferList()
{
	size_t fluxorGenerationBufferSize = m_fluxor_generation_buffer.size();
	if (fluxorGenerationBufferSize > 0)
	{
		for (size_t i = 0; i < fluxorGenerationBufferSize; i++)
		{
			//int main_link = GetMainLinkIndex();
			//if (main_link >= 0)
			//{
			//	m_fluxor_generation_buffer[i]->SetSpeedVectorFromAbsoluteSpeedAndAngle(m_fluxor_generation_buffer[i]->m_absolute_speed, main_link * M_PI_2);
			//}
			
			GameObjectType type = m_fluxor_generation_buffer[i]->m_guided ? FluxorGuidedObject : FluxorUnguidedObject;

			(*CurrentGame).addToScene(m_fluxor_generation_buffer[i], FluxorLayer, type);
			if (m_fluxor_generation_buffer[i]->m_displaying_flux)
			{
				(*CurrentGame).addToFeedbacks(m_fluxor_generation_buffer[i]->m_flux_text);
			}
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
	//condensation?
	int hyperlink = m_isCondensatingFluxor;
	hyperlink *= GRID_WIDTH > GRID_HEIGHT ? GRID_WIDTH : GRID_HEIGHT;

	for (int i = 0; i < 4; i++)
	{
		m_arrow[i]->m_visible = m_link[i].m_exists;

		Module* module = NULL;
		if (m_under_construction)
		{
			m_link[i].m_activated = Link_Deactivated;
		}
		else
		{
			if (i == 0)
			{
				if (m_curGridIndex.x == GRID_WIDTH - 1)
				{
					m_link[i].m_activated = Link_Deactivated;
				}
				else
				{
					int max_temp = GRID_CELLS_FOR_MODULE_LINK_ACTIVATION + 1 + hyperlink;
					int max = max_temp > GRID_WIDTH - m_curGridIndex.x - 1 ? GRID_WIDTH - m_curGridIndex.x - 1 : max_temp;
					for (int j = 1; j < max; j++)
					{
						if (m_curGridIndex.x + j < GRID_WIDTH)
						{
							module = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x + j][m_curGridIndex.y];
							if (module)
							{
								if (module->m_under_construction)
								{
									module = NULL;
									continue;
								}
								else
								{
									if (module->m_team == this->m_team || module->m_alliance == this->m_alliance)
									{
										m_link[i].m_activated = Link_Activated;
										break;
									}
									else
									{
										m_link[i].m_activated = Link_Deactivated;
										break;
									}
								}
							}
						}
					}
				}
			}
			else if (i == 1)
			{
				if (m_curGridIndex.y == GRID_HEIGHT - 1)
				{
					m_link[i].m_activated = Link_Deactivated;
				}
				else
				{
					int max_temp = GRID_CELLS_FOR_MODULE_LINK_ACTIVATION + 1 + hyperlink;
					int max = max_temp > GRID_HEIGHT - m_curGridIndex.y - 1 ? GRID_HEIGHT - m_curGridIndex.y - 1 : max_temp;
					for (int j = 1; j < max; j++)
					{
						if (m_curGridIndex.y + j < GRID_HEIGHT)
						{
							module = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y + j];
							if (module)
							{
								if (module->m_under_construction)
								{
									module = NULL;
									continue;
								}
								else
								{
									if (module->m_team == this->m_team || module->m_alliance == this->m_alliance)
									{
										m_link[i].m_activated = Link_Activated;
										break;
									}
									else
									{
										m_link[i].m_activated = Link_Deactivated;
										break;
									}
								}
							}
						}
					}
				}
			}
			else if (i == 2)
			{
				if (m_curGridIndex.x == 0)
				{
					m_link[i].m_activated = Link_Deactivated;
				}
				else
				{
					int max_temp = GRID_CELLS_FOR_MODULE_LINK_ACTIVATION + 1 + hyperlink;
					int max = max_temp > m_curGridIndex.x ? m_curGridIndex.x : max_temp;
					for (int j = 1; j < max; j++)
					{
						if (m_curGridIndex.x - j >= 0)
						{
							module = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x - j][m_curGridIndex.y];
							if (module)
							{
								if (module->m_under_construction)
								{
									module = NULL;
									continue;
								}
								else
								{
									if (module->m_team == this->m_team || module->m_alliance == this->m_alliance)
									{
										m_link[i].m_activated = Link_Activated;
										break;
									}
									else
									{
										m_link[i].m_activated = Link_Deactivated;
										break;
									}
								}
							}
						}
					}
				}
			}
			else// if (i == 3)
			{
				if (m_curGridIndex.y == 0)
				{
					m_link[i].m_activated = Link_Deactivated;
				}
				else
				{
					int max_temp = GRID_CELLS_FOR_MODULE_LINK_ACTIVATION + 1 + hyperlink;
					int max = max_temp > m_curGridIndex.y ? m_curGridIndex.y : max_temp;
					for (int j = 1; j < max; j++)
					{
						if (m_curGridIndex.y - j >= 0)
						{
							module = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y - j];
							if (module)
							{
								if (module->m_under_construction)
								{
									module = NULL;
									continue;
								}
								else
								{
									if (module->m_team == this->m_team || module->m_alliance == this->m_alliance)
									{
										m_link[i].m_activated = Link_Activated;
										break;
									}
									else
									{
										m_link[i].m_activated = Link_Deactivated;
										break;
									}
								}
							}
						}
					}
				}
			}

			//case of "short circuit" (links pointer each other)
			if (module)
			{
				if (module->m_link[(i + 2) % 4].m_exists && module->m_link[(i + 2) % 4].m_activated == Link_Activated && (module->m_team == this->m_team || module->m_alliance == this->m_alliance))
				{
					m_link[i].m_activated = Link_Invalid;
				}
			}
		}

		//update knownledge of linked modules
		m_linked_modules[i] = module;
		
		//update arrow visual
		m_arrow[i]->setAnimationLine(m_link[i].m_activated);
	}

	//update circuit knowledge
	m_has_child_to_refill = false;
	Module* module_parent = this;
	vector<Module*> checked_modules;

	while (module_parent->GetMainLinkedModule())//check if it has an active link with another module
	{
		int main_link = module_parent->GetMainLinkIndex();
		Module* module_child = module_parent->m_linked_modules[main_link];

		//module already checked? (= infinite loop)
		if (find(checked_modules.begin(), checked_modules.end(), module_child) != checked_modules.end())
		{
			break;
		}
		
		if (module_child->m_flux < module_child->m_flux_max)
		{
			m_has_child_to_refill = true;
			break;
		}
		else
		{
			//child has full stocks, we shall continue the loop until we find a child thas need to be refilled
			checked_modules.push_back(module_parent);
			module_parent = module_child;
		}
	}
}

int Module::GetMainLinkIndex()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_link[i].m_exists)
			return i;
	}

	return -1;
}

bool Module::UpdateFluxorDirection(Fluxor* fluxor)
{
	if (fluxor)
	{
		if (fluxor->m_guided)
		{
			//fluxor has a new target?
			if (fluxor->m_target && !fluxor->m_target_memory)
			{
				//go to target
				fluxor->SetSpeedVectorFromAbsoluteSpeedAndAngle(fluxor->m_absolute_speed, GetAngleRadBetweenPositions(this->getPosition(), fluxor->m_target->getPosition()));
				fluxor->setPosition(sf::Vector2f(getPosition().x, getPosition().y));
				fluxor->m_target_memory = true;

				return true;
			}
			else if (!fluxor->m_target)
			{
				int main_link = GetMainLinkIndex();
				if (main_link >= 0)
				{
					//linked module is not a loop circuit?
					if (m_linked_modules[main_link])
					{
						vector<Module*>::iterator it = find(fluxor->m_modules_visited.begin(), fluxor->m_modules_visited.end(), m_linked_modules[main_link]);

						//element found?
						if (it != fluxor->m_modules_visited.end() && ((*it)->m_team == fluxor->m_team || (*it)->m_alliance == fluxor->m_alliance))
						{
							//This is a looping circuit (allied module visited twice). This is forbidden, and the Fluxor shall therefore die now.
							fluxor->m_GarbageMe = true;
							return false;
						}
					}

					if (fluxor->m_FluxorType == FluxorType_Blue && !m_has_child_to_refill)
					{
						//no child module to refill, going on is pointless
						fluxor->m_GarbageMe = true;
						return false;
					}

					//linked module is legit, we shall proceed to direction change
					if (fluxor->m_target)
					{
						//go to target
						fluxor->SetSpeedVectorFromAbsoluteSpeedAndAngle(fluxor->m_absolute_speed, GetAngleRadBetweenPositions(this->getPosition(), fluxor->m_target->getPosition()));
						fluxor->setPosition(sf::Vector2f(getPosition().x, getPosition().y));
					}
					else
					{
						fluxor->SetSpeedVectorFromAbsoluteSpeedAndAngle(fluxor->m_absolute_speed, main_link * M_PI_2 - M_PI_2);
						if (main_link % 2 == 0)
							fluxor->setPosition(sf::Vector2f(fluxor->getPosition().x, getPosition().y));
						else
							fluxor->setPosition(sf::Vector2f(getPosition().x, fluxor->getPosition().y));
					}

					return true;
				}
				else
				{
					printf("<!> Trying to generate fluxors with a module that doesn't have link, in Module::UpdateFluxorDirection(Fluxor* fluxor)\n.");
					return false;
				}
			}
			else
			{
				//it already has a target and is going for it, nothing to do
				return true;
			}
		}
		else
		{
			printf("<!> Trying to use Module::UpdateFluxorDirection(Fluxor* fluxor) on a fluxor that is not guided.\n.");
			return false;
		}
	}

	printf("<!> Trying to update a NULL pointer with Module::UpdateFluxorDirection(Fluxor* fluxor)\n.");
	return false;
}

Fluxor* Module::SearchNearbyAttackers(PlayerTeams team_not_to_target, float range)
{
	Fluxor* fluxor = NULL;
	float shortest_distance = -1;

	size_t FluxorsVectorSize = (*CurrentGame).GetSceneGameObjectsTyped(FluxorGuidedObject).size();
	for (size_t i = 0; i < FluxorsVectorSize; i++)
	{
		if ((*CurrentGame).GetSceneGameObjectsTyped(FluxorGuidedObject)[i] == NULL)
			continue;

		Fluxor* pCandidate = (Fluxor*)(*CurrentGame).GetSceneGameObjectsTyped(FluxorGuidedObject)[i];

		if ((pCandidate->m_flux_attacker && pCandidate->m_team != team_not_to_target && pCandidate->m_alliance != (*CurrentGame).GetTeamAlliance(team_not_to_target)))
		{
			if (pCandidate->m_isOnScene && !pCandidate->m_ghost && pCandidate->m_visible)
			{
				const float a = this->getPosition().x - pCandidate->getPosition().x;
				const float b = this->getPosition().y - pCandidate->getPosition().y;

				float distance_to_ref = (a * a) + (b * b);
				//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
				if (distance_to_ref < shortest_distance || shortest_distance < 0)
				{
					if (distance_to_ref < range * range)
					{
						shortest_distance = distance_to_ref;
						fluxor = pCandidate;
					}
				}
			}
		}
	}

	return fluxor;
}

void Module::SetTeam(PlayerTeams team, TeamAlliances alliance)
{
	m_team = team;
	m_alliance = alliance;
	if (m_flux_text)
	{
		m_flux_text->m_team = team;
		m_flux_text->m_alliance = alliance;
	}
}