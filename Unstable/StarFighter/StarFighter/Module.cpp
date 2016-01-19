#include "Module.h"

extern Game* CurrentGame;

using namespace sf;


void Module::Initialize()
{
	m_flux = 0;
	m_flux_max = 1;
	m_flux_max_under_construction = 1;
	m_flux_max_after_construction = 1;
	//m_glow = new Glow(this, m_team, MODULE_GLOW_RADIUS, 1, MODULE_GLOW_ANIMATION_DURATION, MODULE_GLOW_MIN_RADIUS);
	m_glow = new Glow(this, sf::Color::Cyan, MODULE_GLOW_RADIUS, 1, MODULE_GLOW_ANIMATION_DURATION, MODULE_GLOW_MIN_RADIUS);
	m_glow->m_visible = false;
	m_team_marker = (*CurrentGame).m_team_markers[m_team]->Clone();
	m_team_marker->setAnimationLine(m_team);
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
	m_shield_range = 0;
	m_upgrade_player_stats = false;
	m_isCondensatingFluxor = false;
	m_shield = NULL;

	//Flux display
	m_flux_text = new SFText(((*CurrentGame).font2), 20, sf::Color::Green, sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + MODULE_FLUX_DISPLAY_OFFSET_Y), m_team);
	m_flux_text->m_alliance = AllianceNeutral;

	m_flux_gauge = NULL;
	//AddFluxGauge(GaugeStyle_Green, sf::Vector2f(0, m_size.y / 2));
	//(*CurrentGame).addToFeedbacks(m_flux_gauge);

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());

	m_free_tile_feedback = NULL;
	m_is_connected_to_a_circuit = false;

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
			if (team == PlayerBlue)
				textureName = "Assets/2D/module_generator_P1.png";
			else if (team == PlayerRed)
				textureName = "Assets/2D/module_generator_P2.png";
			else
				textureName = "Assets/2D/module_generator_P1.png";
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
		//case ModuleType_Factory:
		//{
		//	textureName = "Assets/2D/module_factory.png";
		//	break;
		//}
		//case ModuleType_Factory_Up:
		//{
		//	textureName = "Assets/2D/module_factory_up.png";
		//	break;
		//}
		case ModuleType_Shield:
		{
			textureName = "Assets/2D/module_shield.png";
			break;
		}
		//case ModuleType_Turret:
		//{
		//	textureName = "Assets/2D/module_turret.png";
		//	break;
		//}
		//case ModuleType_Barrier:
		//{
		//	textureName = "Assets/2D/module_barrier.png";
		//	break;
		//}
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
			textureName = "Assets/2D/module.png";
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
			m_flux_max_under_construction = 100;
			m_flux_max_after_construction = 10;
			m_isAutogeneratingFlux = true;
			m_flux_autogeneration_time = 0.5f;
			m_isGeneratingFluxor = true;
			m_fluxor_generated_type = FluxorType_Blue;
			m_fluxor_generation_time = 5.f;// 0.5f;
			m_fluxor_generation_cost = 0;// FLUXOR_FLUX_VALUE;
			break;
		}
		case ModuleType_Armory:
		{
			m_flux_max_under_construction = 20;
			m_flux_max_after_construction = 20;
			m_upgrade_player_stats = true;
			break;
		}
		case ModuleType_Battery:
		{
			m_flux_max_under_construction = 30;
			m_flux_max_after_construction = 300;
			m_isAutogeneratingFlux = true;
			m_flux_autogeneration_time = 1.f;
			break;
		}
		case ModuleType_Relay:
		{
			m_flux_max_under_construction = 5;
			m_flux_max_after_construction = 10;
			m_isRefillingFlux = true;
			break;
		}
		//case ModuleType_Factory:
		//{
		//	m_flux_max_under_construction = 15;
		//	m_flux_max_after_construction = 10;
		//	m_isGeneratingFluxor = true;
		//	m_fluxor_generated_type = FluxorType_Red;
		//	m_fluxor_generation_time = 3.f;
		//	m_fluxor_generation_cost = m_flux_max_after_construction;
		//	break;
		//}
		//case ModuleType_Factory_Up:
		//{
		//	m_flux_max_under_construction = 30;
		//	m_flux_max_after_construction = 20;
		//	m_isGeneratingFluxor = true;
		//	m_fluxor_generated_type = FluxorType_Purple;
		//	m_fluxor_generation_time = 3.f;
		//	m_fluxor_generation_cost = m_flux_max_after_construction;
		//	break;
		//}
		case ModuleType_Shield:
		{
			m_flux_max_under_construction = 60;
			m_flux_max_after_construction = 30;
			m_shield_range = 1;
			break;
		}
		//case ModuleType_Turret:
		//{
		//	m_flux_max_under_construction = 30;
		//	m_flux_max_after_construction = 50;
		//	m_turret_range = 2;
		//	m_isGeneratingFluxor = true;
		//	m_fluxor_generated_type = FluxorType_Black;
		//	m_fluxor_generation_time = 2.f;
		//	m_fluxor_generation_cost = 5;
		//	break;
		//}
		case ModuleType_Amplifier:
		{
			m_flux_max_under_construction = 25;
			m_flux_max_after_construction = 20;
			m_add_flux = 10;
			break;
		}
		case ModuleType_Accelerator:
		{
			m_flux_max_under_construction = 15;
			m_flux_max_after_construction = 30;
			m_add_speed = 100;
			break;
		}
		case ModuleType_Condensator:
		{
			m_flux_max_under_construction = 30;
			m_flux_max_after_construction = 50;
			m_isCondensatingFluxor = true;
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
		//if (m_flux_gauge)
		//{
		//	m_flux_gauge = (*CurrentGame).m_flux_gauges[GaugeStyle_Blue]->Clone();
		//}
	}
	if (m_team_marker)
	{
		m_team_marker->m_under_construction = under_construction;
	}

	setAnimationLine(under_construction);
}

Module* Module::CreateModule(sf::Vector2u grid_index, ModuleType moduleType, PlayerTeams team, bool construction_finished, bool force_direction, int forced_link_direction, unsigned flux)
{
	//construction over own existing module?
	if ((*CurrentGame).m_module_grid[grid_index.x][grid_index.y])
	{
		if ((*CurrentGame).m_module_grid[grid_index.x][grid_index.y]->m_team != team)
		{
			//can't build on other player's modules
			return NULL;
		}
		else
		{
			//overwrite own module
			(*CurrentGame).m_module_grid[grid_index.x][grid_index.y]->m_visible = false;
			(*CurrentGame).m_module_grid[grid_index.x][grid_index.y]->m_GarbageMe = true;
		}
	}

	//updating game grid knowledge
	Module* new_module = new Module(moduleType, team);
	(*CurrentGame).m_module_grid[grid_index.x][grid_index.y] = (GameObject*)new_module;

	//complete module data
	new_module->setPosition(sf::Vector2f(grid_index.x*TILE_SIZE + TILE_SIZE / 2, grid_index.y*TILE_SIZE + TILE_SIZE / 2));
	new_module->m_curGridIndex = grid_index;

	//add to scene
	(*CurrentGame).addToScene(new_module, ModuleLayer, ModuleObject);
	if (new_module->m_glow)
	{
		new_module->m_glow->setPosition(new_module->getPosition());
		(*CurrentGame).addToScene(new_module->m_glow, GlowLayer, BackgroundObject);
	}
	for (int i = 0; i < 4; i++)
	{
		new_module->m_arrow[i]->setPosition(new_module->getPosition().x + cos(i * M_PI_2)*(new_module->m_size.x / 2 - new_module->m_arrow[i]->m_size.x / 2), new_module->getPosition().y + sin(i * M_PI_2)*(new_module->m_size.x / 2 - new_module->m_arrow[i]->m_size.x / 2));
		new_module->m_arrow[i]->m_team = new_module->m_team;
		new_module->m_arrow[i]->m_alliance = new_module->m_alliance;
		new_module->m_arrow[i]->m_under_construction = new_module->m_under_construction;
		(*CurrentGame).addToScene(new_module->m_arrow[i], GlowLayer, BackgroundObject);
	}
	if (new_module->m_team_marker)
	{
		new_module->m_team_marker->setPosition(new_module->getPosition());
		(*CurrentGame).addToScene(new_module->m_team_marker, TeamMarkerLayer, BackgroundObject);
	}
	(*CurrentGame).addToFeedbacks(new_module->m_flux_text);

	//overload parameters
	if (construction_finished)
	{
		new_module->FinishConstruction();
	}
	if (force_direction)
	{
		for (int i = 0; i < forced_link_direction; i++)
		{
			new_module->SwitchLinkDirection();
		}
	}
	//auto direction
	else
	{
		new_module->SetDirectionAutomatically();
	}
	
	new_module->m_flux = flux;
	if (flux > new_module->m_flux_max)
	{
		new_module->m_flux = new_module->m_flux_max;
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

	if (m_team_marker)
	{
		m_team_marker->m_visible = false;
		m_team_marker->m_GarbageMe = true;
	}
	if (m_flux_gauge)
	{
		m_flux_gauge->m_visible = false;
		m_flux_gauge->m_GarbageMe = true;
	}
	
	if (m_shield)
	{
		m_shield->m_visible = false;
		m_shield->m_GarbageMe = true;
	}

	if (m_free_tile_feedback)
	{
		m_free_tile_feedback->m_visible = false;
		m_free_tile_feedback->m_GarbageMe = true;
	}

	size_t SizeFreeTileCondensatorFeedbacksVector = m_free_tile_condensator_feedbacks.size();
	for (size_t i = 0; i < SizeFreeTileCondensatorFeedbacksVector; i++)
	{
		if (m_free_tile_condensator_feedbacks[i])
		{
			m_free_tile_condensator_feedbacks[i]->m_visible = false;
			m_free_tile_condensator_feedbacks[i]->m_GarbageMe = true;
			m_free_tile_condensator_feedbacks[i] = NULL;
		}
	}

	//updating grid knowledge
	if ((*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y])
	{
		if ((*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y] == (GameObject*)this)
		{
			(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y] = NULL;
		}
	}
}

void Module::FinishConstruction()
{
	//end of alpha
	SetConstructionStatus(false);

	//updating game grid knowledge
	//(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y] = (GameObject*)this;

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
	m_flux_max = m_under_construction ? m_flux_max_under_construction : m_flux_max_after_construction;

	if (m_flux > m_flux_max && m_flux_max > 0)
	{
		m_flux = m_flux_max;
	}
	if (m_glow)
	{
		m_glow->m_visible = m_flux == m_flux_max && !m_under_construction;
	}

	if (m_team_marker)
	{
		m_team_marker->setFrame(m_under_construction, true);
	}

	//shield?
	if (m_shield_range > 0 && m_flux > 0 && !m_under_construction && !m_shield)
	{
		m_shield = new GameObject(getPosition(), sf::Vector2f(0, 0), "Assets/2D/shield.png", sf::Vector2f(384, 384));
		sf::Color color = (*CurrentGame).m_team_colors[m_team];
		m_shield->setColor(Color(color.r, color.g, color.b, 70));
		m_shield->m_target = this;
		(*CurrentGame).addToScene(m_shield, ShieldLayer, ShieldObject);
	}
	else if (m_shield)
	{
		m_shield->m_visible = m_flux > 0;
	}

	GameObject::update(deltaTime);

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());

	//update links, circuit knowledge and feedbacks
	UpdateLinks();
	CheckCircuit();
	
	//module properties
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

	//if (m_flux_gauge)
	//{
	//	m_flux_gauge->update(m_flux, m_flux_max);
	//	ostringstream ss;
	//	ss << m_flux << "/" << m_flux_max;
	//	m_flux_gauge->setString(ss.str());
	//	m_flux_gauge->setPosition(getPosition());
	//}
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
				//Allied guided Fluxor passes right through the middle of the Module. Enemy Fluxors collide on the Module edge
				if (fluxor->m_alliance != m_alliance || GameObject::DistancePointToSement(getPosition().x, getPosition().y, fluxor->m_initial_position.x, fluxor->m_initial_position.y, fluxor->getPosition().x, fluxor->getPosition().y) == 0)
				//if (GameObject::DistancePointToSement(getPosition().x, getPosition().y, fluxor->m_initial_position.x, fluxor->m_initial_position.y, fluxor->getPosition().x, fluxor->getPosition().y) == 0)
				{
					//make sure a fluxor leaving the module is not colliding again
					if (fluxor->m_alliance == m_alliance && fluxor->m_initial_position == getPosition() && !fluxor->m_docked)
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
		if (!(*CurrentGame).m_fluxors[m_fluxor_generated_type]->m_needs_link_to_circulate || m_has_child_to_refill || (*CurrentGame).m_fluxors[m_fluxor_generated_type]->m_flux_attacker)//if it needs a link to circulate, we check that an activated link exists and that a linked module that need this ressource
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
					Fluxor* fluxor = new Fluxor(m_fluxor_generated_type, m_team);

					fluxor->m_team = m_team;
					fluxor->m_alliance = m_alliance;
					fluxor->m_guided = true;
					if (pTarget)
					{
						fluxor->m_target = pTarget;
					}

					m_flux -= m_fluxor_generation_cost;

					//feedback
					if (m_fluxor_generation_cost > 0)
					{
						if (USE_FEEDBACK_GENERATE_FLUXOR)
						{
							SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, sf::Color::Cyan, getPosition(), m_team);
							text_feedback->m_alliance = m_alliance;
							ostringstream ss;
							ss << "-" << m_fluxor_generation_cost;
							text_feedback->setString(ss.str());
							SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, NULL, sf::Vector2f(0, 0));
							pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y));
							delete text_feedback;
							(*CurrentGame).addToFeedbacks(pop_feedback);
						}
					}

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
		if (m_flux < m_flux_max || m_flux_max == 0)
		{
			if (m_flux_autogeneration_clock.getElapsedTime().asSeconds() > m_flux_autogeneration_time)
			{
				m_flux++;
				m_flux_autogeneration_clock.restart();

				//feedback
				if (USE_FEEDBACK_AUTOGENERATION)
				{
					SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, sf::Color::Cyan, getPosition(), m_team);
					text_feedback->m_alliance = m_alliance;
					text_feedback->setString("+1");
					SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, NULL, sf::Vector2f(0, 0));
					pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y));
					delete text_feedback;
					(*CurrentGame).addToFeedbacks(pop_feedback);
				}

				if (USE_FEEDBACK_ACTIVATION)
				{
					if (m_flux == m_flux_max)
					{
						SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, sf::Color::Cyan, getPosition(), m_team);
						text_feedback->m_alliance = m_alliance;
						
						SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_LONG_TOTAL_TIME, NULL, sf::Vector2f(0, -TEXT_POP_OFFSET_Y));
						text_feedback->setString("Activated");
						pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y - m_size.y / 2 - TEXT_POP_OFFSET_Y));
						
						delete text_feedback;
						(*CurrentGame).addToFeedbacks(pop_feedback);
					}
				}
			}
		}
		else
		{
			m_flux_autogeneration_clock.restart();
		}	
	}
}

bool Module::ConsummeFluxor(Fluxor* fluxor)
{
	if (fluxor && fluxor->m_consummable_by_modules)
	{
		if ((m_flux < m_flux_max || m_flux_max == 0) && fluxor->m_flux > 0)
		{
			if (fluxor->m_transfert_buffer_memory == 0)
			{
				fluxor->m_transfert_buffer_memory = m_flux_max - m_flux > fluxor->m_flux ? fluxor->m_flux : m_flux_max - m_flux;
			}

			if (m_flux_consumption_clock.getElapsedTime().asSeconds() > m_flux_transfer_delay)
			{
				m_flux++;
				fluxor->m_flux--;

				//feedback
				if (USE_FEEDBACK_ACTIVATION)
				{
					if (m_flux == m_flux_max)
					{
						SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, sf::Color::Cyan, getPosition(), m_team);
						text_feedback->m_alliance = m_alliance;
						text_feedback->setString("Activated");
						SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_LONG_TOTAL_TIME, NULL, sf::Vector2f(0, -TEXT_POP_OFFSET_Y));
						pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y - m_size.y / 2 - TEXT_POP_OFFSET_Y));
						delete text_feedback;
						(*CurrentGame).addToFeedbacks(pop_feedback);
					}
				}
			}
			
			//consumption finished?
			if (m_flux == m_flux_max || fluxor->m_flux == 0)
			{
				//feedback
				if (USE_FEEDBACK_CONSUMPTION)
				{
					SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 16, sf::Color::Cyan, getPosition(), m_team);
					text_feedback->m_alliance = m_alliance;
					ostringstream ss;
					ss << "-" << fluxor->m_transfert_buffer_memory;
					text_feedback->setString(ss.str());
					SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_FLUXOR_DISTANCE_NOT_FADED, TEXT_POP_FLUXOR_DISTANCE_FADE_OUT, TEXT_POP_FLUXOR_TOTAL_TIME, NULL, sf::Vector2f(0, m_size.y / 2 - TEXT_POP_FLUXOR_OFFSET_Y));
					pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y - TEXT_POP_FLUXOR_OFFSET_Y));
					delete text_feedback;
					(*CurrentGame).addToFeedbacks(pop_feedback);
				}

				fluxor->m_transfert_buffer_memory = 0;
			}
			else
			{
				fluxor->m_docked = true;
			}

			return true;
		}
	}

	return false;
}

void Module::CondensateFluxor(Fluxor* fluxor)
{
	if (fluxor)
	{
		if (m_flux == m_flux_max)
		{
			if (!fluxor->m_condensed_to_circulate)
			{
				if (m_isCondensatingFluxor && fluxor->m_needs_link_to_circulate)
				{
					fluxor->m_condensed_to_circulate = true;
					fluxor->setColor(sf::Color(255, 255, 255, 255));

					fluxor->m_wasting_flux = true;

					//feedback
					if (USE_FEEDBACK_CONDENSATION)
					{
						SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 20, sf::Color::Cyan, getPosition(), m_team);
						text_feedback->m_alliance = m_alliance;
						text_feedback->setString("Condensation");
						SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, NULL, sf::Vector2f(0, 0));
						pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y));
						delete text_feedback;
						(*CurrentGame).addToFeedbacks(pop_feedback);
					}
				}
			}
		}
	}
}

void Module::DecondensateFluxor(Fluxor* fluxor)
{
	if (fluxor && fluxor->m_condensed_to_circulate)
	{
		fluxor->m_condensed_to_circulate = false;
		fluxor->setColor(sf::Color(255, 255, 255, GHOST_ALPHA_VALUE));

		fluxor->m_wasting_flux = false;

		//feedback
		if (USE_FEEDBACK_DECONDENSATION)
		{
			SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 20, sf::Color::Red, getPosition(), m_team);
			text_feedback->m_alliance = m_alliance;
			text_feedback->setString("- Condensation");
			SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, NULL, sf::Vector2f(0, -TEXT_POP_OFFSET_Y));
			pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y - m_size.y / 2 - TEXT_POP_OFFSET_Y));
			delete text_feedback;
			(*CurrentGame).addToFeedbacks(pop_feedback);
		}
	}
}

bool Module::AmplifyFluxor(Fluxor* fluxor)
{
	if (fluxor && fluxor->m_can_be_refilled_by_modules)
	{
		if (m_flux == m_flux_max && (m_isRefillingFlux || m_add_flux > 0) && (fluxor->m_can_be_refilled_by_modules || !fluxor->m_consummable_by_modules))
		{
			if (!fluxor->m_needs_link_to_circulate || m_has_child_to_refill)//if it needs a link to circulate and no activated link exists, amplyfing is pointless. Also if no child needs to be refilled.
			{
				if (fluxor->m_transfer_buffer == 0)
				{
					//increasing potential
					if (fluxor->m_flux_max > 0 && m_add_flux > 0)
					{
						fluxor->m_flux_max += m_add_flux;
					}
					//refilling or increasing?
					fluxor->m_transfer_buffer = m_isRefillingFlux ? fluxor->m_flux_max - fluxor->m_flux : m_add_flux;
					fluxor->m_transfert_buffer_memory = fluxor->m_transfer_buffer;
					fluxor->m_flux_transfer_clock.restart();
				}

				if (fluxor->m_transfer_buffer > 0 && fluxor->m_flux_transfer_clock.getElapsedTime().asSeconds() > m_flux_transfer_delay)
				{
					fluxor->m_flux++;
					fluxor->m_transfer_buffer--;
					fluxor->m_flux_transfer_clock.restart();
					fluxor->m_flux_waste_clock.restart();

					//fedback once finished
					if ((m_isRefillingFlux && USE_FEEDBACK_REFILL) || (m_add_flux > 0 && USE_FEEDBACK_AMPLIFICATION))
					if (fluxor->m_transfer_buffer == 0)
					{
						SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, fluxor->m_color, getPosition(), m_team);
						text_feedback->m_alliance = m_alliance;
						ostringstream ss;
						if (m_isRefillingFlux)
						{
							ss << "+" << fluxor->m_transfert_buffer_memory;
							text_feedback->setCharacterSize(16);
						}
						else
						{
							ss << "+" << m_add_flux;
						}
						text_feedback->setString(ss.str());
						SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, NULL, sf::Vector2f(0, m_size.y / 2 - TEXT_POP_FLUXOR_OFFSET_Y));
						pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y - TEXT_POP_FLUXOR_OFFSET_Y));
						delete text_feedback;
						(*CurrentGame).addToFeedbacks(pop_feedback);
					}
				}

				if (fluxor->m_transfer_buffer == 0)
				{
					UndockFluxor(fluxor);
					fluxor->m_transfert_buffer_memory = 0;
				}
				else
				{
					fluxor->m_docked = true;
				}

				return true;
			}
		}
	}

	return false;
}

void Module::AttackModule(Fluxor* fluxor)
{
	if (fluxor)
	{
		if (m_flux >= 0 && fluxor->m_flux > 0)
		{
			if (fluxor->m_flux_attack_clock.getElapsedTime().asSeconds() > fluxor->m_flux_attack_delay)
			{
				//instant damage or damage over time?
				int damage = 1;
				if (fluxor->m_flux_attack_delay == 0)
				{
					damage = fluxor->m_flux;
				}

				//kill?
				if ((int)m_flux - damage < 0 )
				{
					this->m_GarbageMe = true;
					this->m_visible = false;

					fluxor->m_flux -= m_flux;
					if (fluxor->m_flux_stealer)
					{
						fluxor->m_flux_stolen += m_flux;
					}

					//feedback
					if (USE_FEEDBACK_DESTRUCTION)
					{
						SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, Color::Red, getPosition(), m_team);
						text_feedback->m_alliance = AllianceNeutral;
						SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_LONG_TOTAL_TIME, NULL, sf::Vector2f(0, -TEXT_POP_OFFSET_Y));
						pop_feedback->setString("Destroyed");
						pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y - m_size.y / 2 - TEXT_POP_OFFSET_Y));
						delete text_feedback;
						(*CurrentGame).addToFeedbacks(pop_feedback);
					}
				}
				else
				{
					m_flux -= damage;
					fluxor->m_flux -= damage;
					if (fluxor->m_flux_stealer)
					{
						fluxor->m_flux_stolen += damage;
					}
					
					//feedback fluxor
					//SFText* text_feedback2 = new SFText((*CurrentGame).m_fonts[Font_Arial], 16, fluxor->m_color, getPosition(), m_team);
					//text_feedback2->m_alliance = fluxor->m_alliance;
					//SFTextPop* pop_feedback2 = new SFTextPop(text_feedback2, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, fluxor, sf::Vector2f(0, -TEXT_POP_FLUXOR_OFFSET_Y));
					//pop_feedback2->setString("-1");
					//delete text_feedback2;
					//(*CurrentGame).addToFeedbacks(pop_feedback2);
				}

				//feedback module
				if (USE_FEEDBACK_ATTACK)
				{
					SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, fluxor->m_color, getPosition(), m_team);
					text_feedback->m_alliance = AllianceNeutral;
					ostringstream ss;
					ss << "-" << damage;
					text_feedback->setString(ss.str());
					SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, NULL, sf::Vector2f(0, 0));
					pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y));
					delete text_feedback;
					(*CurrentGame).addToFeedbacks(pop_feedback);
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
				//end of condensed effect?
				DecondensateFluxor(fluxor);

				if (!ConsummeFluxor(fluxor))
				{
					CondensateFluxor(fluxor);
					AmplifyFluxor(fluxor);
				}
			}

			if (!fluxor->m_docked)
			{
				if (fluxor->m_flux > 0)//otherwise there is no point, he's a dead fluxor anyway
				{
					UpdateFluxorDirection(fluxor);

					//accelerator
					if (m_add_speed != 0 && m_flux == m_flux_max)
					{
						fluxor->AddSpeed(&fluxor->m_speed, (float)m_add_speed);
						fluxor->NormalizeSpeed(&fluxor->m_speed, FLUXOR_GUIDED_MAX_SPEED);
						fluxor->m_absolute_speed = GetAbsoluteSpeed(fluxor->m_speed);
					}
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
				//fluxor->setPosition(this->getPosition());
				if (!fluxor->m_flux_attack_piercing)
				{
					if (fluxor->m_flux_stealer)
					{
						fluxor->BringStealerBack();
					}
					else
					{
						fluxor->m_visible = false;
						fluxor->m_GarbageMe = true;
					}
				}
			}
			else
			{
				//fluxor->setPosition(this->getPosition());
			}
		}
	}
}

bool Module::UndockFluxor(Fluxor* fluxor)
{
	if (fluxor)
	{
		//the Fluxor may go on if it is doesn't need link to circulate, unless condensated. Otherwise he needs an active link to continue his journey.
		if (!fluxor->m_needs_link_to_circulate || fluxor->m_condensed_to_circulate || IsMainLinkActivated())
		{
			fluxor->m_docked = false;
			return true;
		}
		else
		{
			fluxor->m_visible = false;
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
	int hyperlink = m_isCondensatingFluxor && !m_under_construction;
	hyperlink *= GRID_WIDTH > GRID_HEIGHT ? GRID_WIDTH : GRID_HEIGHT;

	//clear all condensator's free tile feedbacks, because we are going to create new ones every frame, so they can always be up to date (to hard to update)
	if (!m_free_tile_condensator_feedbacks.empty())
	{
		size_t SizeFreeTileCondensatorFeedbacksVector = m_free_tile_condensator_feedbacks.size();
		for (size_t i = 0; i < SizeFreeTileCondensatorFeedbacksVector; i++)
		{
			if (m_free_tile_condensator_feedbacks[i])
			{
				m_free_tile_condensator_feedbacks[i]->m_visible = false;
				m_free_tile_condensator_feedbacks[i]->m_GarbageMe = true;
				m_free_tile_condensator_feedbacks[i] = NULL;
			}
		}
		m_free_tile_condensator_feedbacks.clear();
	}

	//begin scanning cells around
	for (int i = 0; i < 4; i++)
	{
		m_arrow[i]->m_visible = m_link[i].m_exists;

		//reset of arrow status to default value
		m_link[i].m_activated = Link_Deactivated;

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
					int max_temp = GRID_CELLS_FOR_MODULE_LINK_ACTIVATION + hyperlink;
					int max = max_temp > GRID_WIDTH - m_curGridIndex.x ? GRID_WIDTH - m_curGridIndex.x : max_temp;
					for (int j = 1; j <= max; j++)
					{
						if (m_curGridIndex.x + j < GRID_WIDTH)
						{
							module = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x + j][m_curGridIndex.y];
							if (module)
							{
								if (module->m_under_construction || !module->m_visible)
								{
									module = NULL;
									continue;
								}
								else
								{
									if (module->m_team == this->m_team || module->m_alliance == this->m_alliance)
									{
										if (m_link[i].m_exists)
										{
											m_link[i].m_activated = Link_Activated;
										}
									}
									else
									{
										m_link[i].m_activated = Link_Deactivated;
									}

									break;
								}
							}
							else
							{
								//feedback for Module "Condensator"
								if (m_isCondensatingFluxor && m_link[i].m_exists)
								{
									SFRectangle* rect = new SFRectangle(Game::GridToPosition(sf::Vector2u(m_curGridIndex.x + j, m_curGridIndex.y)), sf::Vector2f(TILE_SIZE, TILE_SIZE), sf::Color(0, 255, 0, 70), 0, sf::Color(0, 255, 0, 255), m_team);
									m_free_tile_condensator_feedbacks.push_back(rect);
									rect = NULL;
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
					int max_temp = GRID_CELLS_FOR_MODULE_LINK_ACTIVATION + hyperlink;
					int max = max_temp > GRID_HEIGHT - m_curGridIndex.y ? GRID_HEIGHT - m_curGridIndex.y : max_temp;
					for (int j = 1; j <= max; j++)
					{
						if (m_curGridIndex.y + j < GRID_HEIGHT)
						{
							module = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y + j];
							if (module)
							{
								if (module->m_under_construction || !module->m_visible)
								{
									module = NULL;
									continue;
								}
								else
								{
									if (module->m_team == this->m_team || module->m_alliance == this->m_alliance)
									{
										if (m_link[i].m_exists)
										{
											m_link[i].m_activated = Link_Activated;
										}
									}
									else
									{
										m_link[i].m_activated = Link_Deactivated;
									}

									break;
								}
							}
							//feedback for Module "Condensator"
							if (m_isCondensatingFluxor && m_link[i].m_exists)
							{
								SFRectangle* rect = new SFRectangle(Game::GridToPosition(sf::Vector2u(m_curGridIndex.x, m_curGridIndex.y + j)), sf::Vector2f(TILE_SIZE, TILE_SIZE), sf::Color(0, 255, 0, 70), 0, sf::Color(0, 255, 0, 255), m_team);
								m_free_tile_condensator_feedbacks.push_back(rect);
								rect = NULL;
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
					int max_temp = GRID_CELLS_FOR_MODULE_LINK_ACTIVATION + hyperlink;
					int max = max_temp > m_curGridIndex.x ? m_curGridIndex.x: max_temp;
					for (int j = 1; j <= max; j++)
					{
						if (m_curGridIndex.x - j >= 0)
						{
							module = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x - j][m_curGridIndex.y];
							if (module)
							{
								if (module->m_under_construction || !module->m_visible)
								{
									module = NULL;
									continue;
								}
								else
								{
									if (module->m_team == this->m_team || module->m_alliance == this->m_alliance)
									{
										if (m_link[i].m_exists)
										{
											m_link[i].m_activated = Link_Activated;
										}
									}
									else
									{
										m_link[i].m_activated = Link_Deactivated;
									}

									break;
								}
							}
							//feedback for Module "Condensator"
							if (m_isCondensatingFluxor && m_link[i].m_exists)
							{
								SFRectangle* rect = new SFRectangle(Game::GridToPosition(sf::Vector2u(m_curGridIndex.x - j, m_curGridIndex.y)), sf::Vector2f(TILE_SIZE, TILE_SIZE), sf::Color(0, 255, 0, 70), 0, sf::Color(0, 255, 0, 255), m_team);
								m_free_tile_condensator_feedbacks.push_back(rect);
								rect = NULL;
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
					int max_temp = GRID_CELLS_FOR_MODULE_LINK_ACTIVATION + hyperlink;
					int max = max_temp > m_curGridIndex.y ? m_curGridIndex.y : max_temp;
					for (int j = 1; j <= max; j++)
					{
						if (m_curGridIndex.y - j >= 0)
						{
							module = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y - j];
							if (module)
							{
								if (module->m_under_construction || !module->m_visible)
								{
									module = NULL;
									continue;
								}
								else
								{
									if (module->m_team == this->m_team || module->m_alliance == this->m_alliance)
									{
										if (m_link[i].m_exists)
										{
											m_link[i].m_activated = Link_Activated;
										}
									}
									else
									{
										m_link[i].m_activated = Link_Deactivated;
									}

									break;
								}
							}
							//feedback for Module "Condensator"
							if (m_isCondensatingFluxor && m_link[i].m_exists)
							{
								SFRectangle* rect = new SFRectangle(Game::GridToPosition(sf::Vector2u(m_curGridIndex.x, m_curGridIndex.y - j)), sf::Vector2f(TILE_SIZE, TILE_SIZE), sf::Color(0, 255, 0, 70), 0, sf::Color(0, 255, 0, 255), m_team);
								m_free_tile_condensator_feedbacks.push_back(rect);
								rect = NULL;
							}
						}
					}
				}
			}

			//case of "short circuit" (links pointer each other)
			if (module)
			{
				if (module->GetMainLinkIndex() == (GetMainLinkIndex() + 2) % 4 && (module->m_team == this->m_team || module->m_alliance == this->m_alliance))
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
}

void Module::CheckCircuit()
{
	m_has_child_to_refill = false;
	Module* module_parent = this;
	vector<Module*> checked_modules;

	if (!m_under_construction && m_isGeneratingFluxor && (*CurrentGame).m_fluxors[m_fluxor_generated_type]->m_consummable_by_modules)
	{
		m_is_connected_to_a_circuit = true;
	}
	while (module_parent->GetMainLinkedModule())//check if it has an active link with another module
	{
		int main_link = module_parent->GetMainLinkIndex();
		Module* module_child = module_parent->m_linked_modules[main_link];
		if (module_parent->m_is_connected_to_a_circuit)
		{
			module_child->m_is_connected_to_a_circuit = true;
		}

		//module already checked? (= infinite loop)
		if (find(checked_modules.begin(), checked_modules.end(), module_child) != checked_modules.end())
		{
			break;
		}

		if (module_child->m_flux < module_child->m_flux_max)
		{
			m_has_child_to_refill = true;
			//break;
		}
		//else
		//{
		//child has full stocks, we shall continue the loop until we find a child thas need to be refilled
		checked_modules.push_back(module_parent);
		module_parent = module_child;
		//}
	}
}

void Module::UpdateFreeTileFeedbacks()
{
	Color free_cell_max_flux_feedback = Color(0, 255, 0, 70);
	Color free_cell_not_max_flux_feedback = Color(255, 120, 0, 70);

	if (m_is_connected_to_a_circuit && !m_isCondensatingFluxor)//is a generator of blue fluxors, or is eventually linked to one. Condensators are handled separately (below).
	{
		//look for a connected free cell
		int link_index = GetLinkIndexToFreeConnectedCell();
		if (link_index >= 0)
		{
			int next_x = (link_index == 0) - (link_index == 2);
			int next_y = (link_index == 1) - (link_index == 3);

			sf::Vector2f position = Game::GridToPosition(sf::Vector2u(m_curGridIndex.x + next_x, m_curGridIndex.y + next_y));
			//create new feedback
			if (!m_free_tile_feedback)
			{
				m_free_tile_feedback = new SFRectangle(position, sf::Vector2f(TILE_SIZE, TILE_SIZE), sf::Color(255, 255, 255, 70), 0, sf::Color(0, 255, 0, 255), m_team);
				m_free_tile_feedback->m_alliance = m_alliance;
				(*CurrentGame).addToFeedbacks(m_free_tile_feedback, GridFeedbackLayer);
			}
			//update position of existing feedback
			else
			{
				m_free_tile_feedback->setPosition(position);
			}

			//update color in any case
			if (m_flux == m_flux_max)
			{
				m_free_tile_feedback->setFillColor(free_cell_max_flux_feedback);
				m_free_tile_feedback->m_prioritary = true;
			}
			else
			{
				m_free_tile_feedback->setFillColor(free_cell_not_max_flux_feedback);
				m_free_tile_feedback->m_prioritary = false;
			}

			//check for doublons and clean them
			std::vector<SFRectangle*> existing_tile_feedbacks = (*CurrentGame).GetSceneSFRectanglesLayered(GridFeedbackLayer);
			size_t FakeGridFeedbacksVectorSize = existing_tile_feedbacks.size();
			for (size_t i = 0; i < FakeGridFeedbacksVectorSize; i++)
			{
				if (existing_tile_feedbacks[i] && existing_tile_feedbacks[i] != m_free_tile_feedback && existing_tile_feedbacks[i]->m_visible && existing_tile_feedbacks[i]->m_alliance == m_alliance && existing_tile_feedbacks[i] != m_free_tile_feedback && existing_tile_feedbacks[i]->getPosition() == m_free_tile_feedback->getPosition())
				{
					//delete doublon feedback, based on priority
					if (!existing_tile_feedbacks[i]->m_prioritary && m_free_tile_feedback->m_prioritary)
					{
						existing_tile_feedbacks[i]->m_visible = false;
						existing_tile_feedbacks[i]->m_GarbageMe = true;
						existing_tile_feedbacks[i] = NULL;
					}
					else
					{
						m_free_tile_feedback->m_visible = false;
						m_free_tile_feedback->m_GarbageMe = true;
						m_free_tile_feedback = NULL;
					}
					
					break;
				}
			}
		}
		else
		{
			//delete obsolete feedback if existing
			if (m_free_tile_feedback)
			{
				m_free_tile_feedback->m_visible = false;
				m_free_tile_feedback->m_GarbageMe = true;
				m_free_tile_feedback = NULL;
			}
		}
	}
	else
	{
		//delete obsolete feedback if existing
		if (m_free_tile_feedback)
		{
			m_free_tile_feedback->m_visible = false;
			m_free_tile_feedback->m_GarbageMe = true;
			m_free_tile_feedback = NULL;
		}
	}

	// CONDENSATOR: same story but with vectors of feedbacks now...

	//finish the job of condensator feedbacks
	if (!m_free_tile_condensator_feedbacks.empty() && m_is_connected_to_a_circuit)
	{
		size_t SizeFreeTileCondensatorFeedbacksVector = m_free_tile_condensator_feedbacks.size();
		for (size_t i = 0; i < SizeFreeTileCondensatorFeedbacksVector; i++)
		{
			assert(m_free_tile_condensator_feedbacks[i] != NULL);

			//complete data
			m_free_tile_condensator_feedbacks[i]->m_alliance = m_alliance;
			(*CurrentGame).addToFeedbacks(m_free_tile_condensator_feedbacks[i], GridFeedbackLayer);

			//update color in any case
			if (m_flux == m_flux_max)
			{
				m_free_tile_condensator_feedbacks[i]->setFillColor(free_cell_max_flux_feedback);
				m_free_tile_condensator_feedbacks[i]->m_prioritary = true;
			}
			else
			{
				m_free_tile_condensator_feedbacks[i]->setFillColor(free_cell_not_max_flux_feedback);
				m_free_tile_condensator_feedbacks[i]->m_prioritary = false;
			}

			//check for doublons and clean them
			std::vector<SFRectangle*> existing_tile_feedbacks = (*CurrentGame).GetSceneSFRectanglesLayered(GridFeedbackLayer);
			size_t FakeGridFeedbacksVectorSize = existing_tile_feedbacks.size();
			for (size_t j = 0; j < FakeGridFeedbacksVectorSize; j++)
			{
				if (existing_tile_feedbacks[j] && existing_tile_feedbacks[j] != m_free_tile_condensator_feedbacks[i] && existing_tile_feedbacks[j]->m_visible && existing_tile_feedbacks[j]->m_alliance == m_alliance && existing_tile_feedbacks[j]->getPosition() == m_free_tile_condensator_feedbacks[i]->getPosition())
				{
					//delete doublon feedback, based on priority
					if (!existing_tile_feedbacks[j]->m_prioritary && m_free_tile_condensator_feedbacks[i]->m_prioritary)
					{
						existing_tile_feedbacks[j]->m_visible = false;
						existing_tile_feedbacks[j]->m_GarbageMe = true;
						existing_tile_feedbacks[j] = NULL;
					}
					else
					{
						m_free_tile_condensator_feedbacks[i]->m_visible = false;
						m_free_tile_condensator_feedbacks[i]->m_GarbageMe = true;
						m_free_tile_condensator_feedbacks[i] = NULL;
					}
					
					break;
				}
			}
		}
	}
	//not connected to ciruit -> destroy feedbacks
	else if (!m_free_tile_condensator_feedbacks.empty())
	{
		size_t SizeFreeTileCondensatorFeedbacksVector = m_free_tile_condensator_feedbacks.size();
		for (size_t i = 0; i < SizeFreeTileCondensatorFeedbacksVector; i++)
		{
			if (m_free_tile_condensator_feedbacks[i])
			{
				m_free_tile_condensator_feedbacks[i]->m_visible = false;
				m_free_tile_condensator_feedbacks[i]->m_GarbageMe = true;
				m_free_tile_condensator_feedbacks[i] = NULL;
			}
		}
		m_free_tile_condensator_feedbacks.clear();
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
			//Fluxor allowed to continue his journey?
			if (!fluxor->m_needs_link_to_circulate || fluxor->m_condensed_to_circulate || IsMainLinkActivated())
			{
				//no child module to refill, going on is pointless. Unless it's an attacker that could go on an aggress the enemy
				if (fluxor->m_consummable_by_modules && !m_has_child_to_refill && !fluxor->m_flux_attacker)
				{
					fluxor->m_visible = false;
					fluxor->m_GarbageMe = true;
					return false;
				}
				//if we need a link to circulate and we are not condensed, then the next module should be nearby (right next to the module). Otherwise it means it's a Condensator that just consummed the Fluxor and his "active link" is not legit.
				else if (fluxor->m_needs_link_to_circulate && !fluxor->m_condensed_to_circulate
					&& ((GetMainLinkedModule()->m_curGridIndex.x > m_curGridIndex.x ? GetMainLinkedModule()->m_curGridIndex.x - m_curGridIndex.x : m_curGridIndex.x - GetMainLinkedModule()->m_curGridIndex.x) > 1 
					|| (GetMainLinkedModule()->m_curGridIndex.y > m_curGridIndex.y ? GetMainLinkedModule()->m_curGridIndex.y - m_curGridIndex.y : m_curGridIndex.y - GetMainLinkedModule()->m_curGridIndex.y) > 1))
				{
					fluxor->m_visible = false;
					fluxor->m_GarbageMe = true;
					return false;
				}
				else
				{
					fluxor->m_docked = false;
				}
			}
			else
			{
				fluxor->m_visible = false;
				fluxor->m_GarbageMe = true;
				return false;
			}

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
							fluxor->m_visible = false;
							fluxor->m_GarbageMe = true;
							return false;
						}
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
	if (m_team_marker)
	{
		m_team_marker->m_team = team;
		m_team_marker->m_alliance = alliance;
	}
	if (m_glow)
	{
		m_glow->m_team = team;
		m_glow->m_alliance = alliance;
	}
	//if (m_flux_gauge)
	//{
	//	m_flux_gauge->m_team = team;
	//	m_flux_gauge->m_alliance = alliance;
	//}
}

void Module::AddFluxGauge(GaugeStyles gauge, sf::Vector2f offset)
{
	m_flux_gauge = (*CurrentGame).m_flux_gauges[0]->Clone();
	m_flux_gauge->m_offset = offset;
}

void Module::SetDirectionAutomatically()
{
	//other cases
	int link_parent_index = -1;
	int link_child_index = -1;
	if (USE_SMART_ARROWS == true)
	{
		for (int i = 0; i < 4; i++)
		{
			//cases where module is on a border of the map
			if (i == 0 && m_curGridIndex.x == GRID_WIDTH - 1)
			{
				continue;
			}
			else if (i == 1 && m_curGridIndex.y == GRID_HEIGHT - 1)
			{
				continue;
			}
			else if (i == 2 && m_curGridIndex.x == 0)
			{
				continue;
			}
			else if (i == 3 && m_curGridIndex.y == 0)
			{
				continue;
			}

			//other cases
			int next_x = (i == 0) - (i == 2);
			int next_y = (i == 1) - (i == 3);

			if ((*CurrentGame).m_module_grid[m_curGridIndex.x + next_x][m_curGridIndex.y + next_y])
			{
				Module* pModule = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x + next_x][m_curGridIndex.y + next_y];
				if (pModule->m_team == this->m_team)
				{
					//1) is there a parent module linking to this one? (we can go on and find a better one if we already have found a parent and a child)
					if (pModule->GetMainLinkIndex() == (i + 2) % 4)
					{
						//risk of invalid link? abort smart arrow
						if (pModule->GetMainLinkIndex() == (link_parent_index + 2) % 4)
						{
							link_parent_index = -1;
							continue;
						}

						// if we alreay know a parent, can it be a better one?
						if (link_parent_index < 0 || (i == 0 && m_alliance % 2 == 0) || (i == 2 && m_alliance % 2 == 1))
						{
							link_parent_index = pModule->GetMainLinkIndex();
						}

						continue;
					}

					//2) is there a child module we should link to? (and choose the best one if several)
					if (link_child_index < 0 || (i == 0 && m_alliance % 2 == 0) || (i == 2 && m_alliance % 2 == 1))
					{
						link_child_index = i;
					}
				}
			}
		}
	}

	//Decision-making:
	int number_of_turns = 0;

	// 1) Has parent, but no child -> turn in the continuity of the parent's direction
	if (link_parent_index >= 0 && link_child_index < 0)
	{
		number_of_turns = link_parent_index;
	}
	// 2) Has parent and child -> turn to child
	else if (link_parent_index >= 0 && link_child_index >= 0)
	{
		number_of_turns = link_child_index;
	}
	// 3) has child, but no parent -> turn to child if generating Blue Fluxors
	else if (link_parent_index < 0 && link_child_index >= 0 && m_isGeneratingFluxor && (*CurrentGame).m_fluxors[m_fluxor_generated_type]->m_consummable_by_modules)
	{
		number_of_turns = link_child_index;
	}
	// 4) by default, we turn it left or right depending on alliance index, which is a pretty arrousing method
	else
	{
		number_of_turns = ((m_alliance * 2) % 4);
	}

	//Apply decision
	for (int j = 0; j < number_of_turns; j++)
	{
		SwitchLinkDirection();
	}
}

int Module::GetLinkIndexToFreeConnectedCell()
{
	//other cases
	int link = GetMainLinkIndex();

	//cases where module is on a border of the map
	if (link == 0 && m_curGridIndex.x == GRID_WIDTH - 1)
	{
		return -1;
	}
	else if (link == 1 && m_curGridIndex.y == GRID_HEIGHT - 1)
	{
		return -1;
	}
	else if (link == 2 && m_curGridIndex.x == 0)
	{
		return -1;
	}
	else if (link == 3 && m_curGridIndex.y == 0)
	{
		return -1;
	}

	int next_x = (link == 0) - (link == 2);
	int next_y = (link == 1) - (link == 3);

	//cell is free?
	if (!(*CurrentGame).m_module_grid[m_curGridIndex.x + next_x][m_curGridIndex.y + next_y])
	{
		//return link index to free cell
		return link;
	}
	else
	{
		return -1;
	}
}