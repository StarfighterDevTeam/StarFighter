#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Ship::Ship()
{
	
}

void Ship::Init()
{
	m_collider_type = GameObjectType::PlayerShip;
	moving = false;
	movingX = movingY = false;
	m_disable_inputs = false;
	m_controllerType = AllControlDevices;
	m_flux = 0;
	m_flux_max = SHIP_MAX_FLUX;
	m_speed_max = SHIP_MAX_SPEED;
	m_flux_transfer_time = 0.1f;
	m_flux_autogeneration_time = 1.f;
	m_flux_autogeneration = 0;
	m_upgrade_level = 0;

	//Flux display
	m_flux_text.setFont(*(*CurrentGame).font2);
	m_flux_text.setCharacterSize(20);
	m_flux_text.setColor(sf::Color::Green);
	m_flux_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + PLAYER_FLUX_DISPLAY_OFFSET_Y));
	if (USE_UNGUIDED_FLUXORS_TO_BUILD == true)
	{
		(*CurrentGame).addToFeedbacks(&m_flux_text);
	}

	//Build feedback
	m_build_text.setFont(*(*CurrentGame).font2);
	m_build_text.setCharacterSize(20);
	m_build_text.setColor(sf::Color::Green);
	m_build_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y - m_size.y / 2));
	(*CurrentGame).addToFeedbacks(&m_build_text);
	m_build_text_status = Player_NotOverConstruction;

	//inputs
	m_SwitchKey_released = false;
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, PlayerTeams team, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_team = team;
	m_alliance = (TeamAlliances)(*CurrentGame).GetTeamAlliance(team);
	this->Init();
}

Ship::~Ship()
{
	if (USE_UNGUIDED_FLUXORS_TO_BUILD == true)
	{
		(*CurrentGame).removeFromFeedbacks(&m_flux_text);
	}
	(*CurrentGame).removeFromFeedbacks(&m_build_text);
}

void Ship::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void Ship::UpdatePlayerStats()
{
	m_upgrade_level = 0;
	for (size_t i = 0; i < GRID_WIDTH; i++)
	{
		for (size_t j = 0; j < GRID_HEIGHT; j++)
		{
			if ((*CurrentGame).m_module_grid[i][j])
			{
				Module* module = (Module*)(*CurrentGame).m_module_grid[i][j];
				if (module->m_team == this->m_team && !module->m_under_construction && module->m_flux == module->m_flux_max && module->m_upgrade_player_stats)
				{
					m_upgrade_level++;
					if (m_upgrade_level == SHIP_MAX_UPGRADE_LEVEL)
					{
						goto exit_loop;
					}
				}
			}
		}
		if (m_upgrade_level == SHIP_MAX_UPGRADE_LEVEL)
		{
			break;
		}
	}

	exit_loop:

	//anti-cheat
	if (m_upgrade_level > SHIP_MAX_UPGRADE_LEVEL)
	{
		m_upgrade_level == SHIP_MAX_UPGRADE_LEVEL;
	}

	m_flux_max = SHIP_MAX_FLUX + (SHIP_MAX_FLUX_BONUS_PER_LEVEL * m_upgrade_level);
	m_speed_max = SHIP_MAX_SPEED + (SHIP_MAX_SPEED_BONUS_PER_LEVEL * m_upgrade_level);
}

void Ship::FluxAutogeneration()
{
	if (m_flux_autogeneration > 0)
	{
		if (m_flux_autogeneration_clock.getElapsedTime().asSeconds() > m_flux_autogeneration_time)
		{
			m_flux += m_flux_autogeneration;
			m_flux_autogeneration_clock.restart();
		}
	}
}

void Ship::update(sf::Time deltaTime)
{
	m_SwitchKey_released = !InputGuy::isUsing(m_controllerType);

	UpdatePlayerStats();

	FluxAutogeneration();
	if (m_flux > m_flux_max && m_flux_max > 0)
	{
		m_flux = m_flux_max;
	}

	sf::Vector2f inputs_direction = InputGuy::getDirections(m_controllerType);

	if (!m_disable_inputs)
	{
		moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		movingX = inputs_direction.x != 0;
		movingY = inputs_direction.y != 0;
	}

	ManageAcceleration(inputs_direction);
	MaxSpeedConstraints();
	IdleDecelleration(deltaTime);
	UpdateRotation();

	GameObject::update(deltaTime);

	ScreenBorderContraints();	

	//hud
	if (USE_UNGUIDED_FLUXORS_TO_BUILD == true)
	{
		ostringstream ss;
		ss << m_flux << "/" << m_flux_max;
		m_flux_text.setString(ss.str());
		m_flux_text.setPosition(sf::Vector2f(getPosition().x - m_flux_text.getGlobalBounds().width / 2, getPosition().y + m_size.y / 2 + PLAYER_FLUX_DISPLAY_OFFSET_Y));
	}
	
	m_build_text.setPosition(sf::Vector2f(getPosition().x - m_build_text.getGlobalBounds().width / 2, getPosition().y - m_size.y / 2 - m_build_text.getGlobalBounds().height*1.5));
	if (m_build_text_status == Player_OverConstruction)
	{
		m_build_text.setString("Hold 'Build'");
	}
	else if (m_build_text_status == Player_ConstructionInProgress)
	{
		m_build_text.setString("Building...");
	}
	else if (m_build_text_status == Player_NoRessourcesToBuild)
	{
		m_build_text.setString("No ressources");
	}
	
	if (m_build_text_status != Player_NotOverConstruction)
	{
		m_build_text.setColor(sf::Color::Green);
	}
	else
	{
		m_build_text.setColor(sf::Color::Transparent);
	}
	m_build_text_status = Player_NotOverConstruction;

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());
	
	if (InputGuy::isSpawningModule1(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 1 - 1), m_team);
	}
	if (InputGuy::isSpawningModule2(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 2 - 1), m_team);
	}
	if (InputGuy::isSpawningModule3(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 3 - 1), m_team);
	}
	if (InputGuy::isSpawningModule4(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 4 - 1), m_team);
	}
	if (InputGuy::isSpawningModule5(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 5 - 1), m_team);
	}
	if (InputGuy::isSpawningModule6(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 6 - 1), m_team);
	}
	if (InputGuy::isSpawningModule7(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 7 - 1), m_team);
	}
	if (InputGuy::isSpawningModule8(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 8 - 1), m_team);
	}
	if (InputGuy::isSpawningModule9(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 9 - 1), m_team);
	}
	if (InputGuy::isSpawningModule0(m_controllerType))
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 10 - 1), m_team);
	}

	//DEBUG
	if (InputGuy::isErasingModule(m_controllerType))
	{
		Module::EraseModule(m_curGridIndex);
	}
	if (InputGuy::isFinishModuleConstruction(m_controllerType))
	{
		Module::DebugFinishModule(m_curGridIndex);

		if (USE_UNGUIDED_FLUXORS_TO_BUILD == true)
		{
			m_flux = 0;
		}
	}
	if (InputGuy::isRefillingFlux(m_controllerType))
	{
		Module::DebugRefillingModuleFlux(m_curGridIndex);
	}
	
}

void Ship::ScreenBorderContraints()
{
	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		m_speed.x = 0;
	}

	if (this->getPosition().x > (*CurrentGame).map_size.x - (m_size.x / 2))
	{
		this->setPosition((*CurrentGame).map_size.x - (m_size.x / 2), this->getPosition().y);
		m_speed.x = 0;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		m_speed.y = 0;
	}

	if (this->getPosition().y > (*CurrentGame).map_size.y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, (*CurrentGame).map_size.y - (m_size.y / 2));
		m_speed.y = 0;
	}
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	//idle decceleration
	if (!movingX)
	{
		m_speed.x -= (m_speed.x) * deltaTime.asSeconds()* SHIP_DECCELERATION_COEF / 100.f;

		if (abs(m_speed.x) < SHIP_MIN_SPEED)
			m_speed.x = 0;
	}

	if (!movingY)
	{
		m_speed.y -= (m_speed.y)*deltaTime.asSeconds()*SHIP_DECCELERATION_COEF / 100.f;

		if (abs(m_speed.y) < SHIP_MIN_SPEED)
			m_speed.y = 0;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	m_speed.x += inputs_direction.x* SHIP_ACCELERATION;
	m_speed.y += inputs_direction.y*SHIP_ACCELERATION;
}

void Ship::MaxSpeedConstraints()
{
	//max speed constraints
	NormalizeSpeed(&m_speed, m_speed_max);
}

void Ship::UpdateRotation()
{
	//turning toward targeted position
	if (m_speed.x == 0 && m_speed.y == 0)
	{
		//do nothing
	}
	else if (m_speed.x == 0 && m_speed.y > 0)
	{
		setRotation(180);
	}
	else if (m_speed.x == 0 && m_speed.y < 0)
	{
		setRotation(0);
	}
	else if (m_speed.y == 0 && m_speed.x > 0)
	{
		setRotation(90);
	}
	else if (m_speed.y == 0 && m_speed.x < 0)
	{
		setRotation(270);
	}
	else
	{
		setRotation((GetAngleRadForSpeed(m_speed) * 180 / (float)M_PI));
	}
}

void Ship::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		stroboscopic_effect_clock.restart();
	}
}

//FLUX SPECIFIC
void Ship::GetFluxor(GameObject* object)
{
	if (object)
	{
		if (m_flux < m_flux_max)
		{
			Fluxor* fluxor = (Fluxor*)object;
			if (fluxor->m_consummable_by_players)
			{
				m_flux += fluxor->m_flux;
				if (m_flux > m_flux_max)
				{
					m_flux = m_flux_max;
				}

				fluxor->m_GarbageMe = true;
			}
		}
	}
}

void Ship::GetModule(GameObject* object)
{
	if (object)
	{
		if (object->m_team == this->m_team)
		{
			Module* module = (Module*)object;

			if (m_curGridIndex.x == module->m_curGridIndex.x && m_curGridIndex.y == module->m_curGridIndex.y)
			{
				if (module->m_under_construction)
				{
					if (InputGuy::isFiring(m_controllerType))
					{
						m_build_text_status = (USE_UNGUIDED_FLUXORS_TO_BUILD == true && m_flux == 0) ? Player_NoRessourcesToBuild : Player_ConstructionInProgress;

						unsigned int flux_max = module->m_under_construction ? module->m_flux_max_under_construction : module->m_flux_max;
						if (module->m_flux < flux_max && m_flux_transfer_limiter_clock.getElapsedTime().asSeconds() > m_flux_transfer_time && (USE_UNGUIDED_FLUXORS_TO_BUILD == false || m_flux > 0))
						{
							if (USE_UNGUIDED_FLUXORS_TO_BUILD == true)
							{
								m_flux--;
							}
							module->m_flux++;
							m_flux_transfer_limiter_clock.restart();
						}
					}
					else
					{
						m_build_text_status = Player_OverConstruction;
						m_flux_transfer_limiter_clock.restart();
					}
				}

				if (InputGuy::isUsing(m_controllerType) && m_SwitchKey_released)
				{
					module->SwitchLinkDirection();
				}
			}
		}
	}
}

//void Ship::ResolveProductionBufferList()
//{
//	size_t constructionBufferSize = m_construction_buffer.size();
//	for (size_t i = 0; i < constructionBufferSize; i++)
//	{
//		if ((*CurrentGame).m_module_grid[m_construction_buffer[i]->m_curGridIndex.x][m_construction_buffer[i]->m_curGridIndex.y])
//		{
//			Module* new_module = Module::CreateModule(m_construction_buffer[i]->m_curGridIndex, m_construction_buffer[i]->m_moduleType);
//			new_module->m_parents.push_back(m_construction_buffer[i]->m_parents.front());
//			m_construction_buffer[i]->m_parents.front()->m_children.push_back(new_module);
//
//			//HACK PROTO
//			m_construction_buffer[i]->m_parents.front()->m_flux -= new_module->m_flux_max;
//		}
//	}
//
//	m_construction_buffer.clear();
//}