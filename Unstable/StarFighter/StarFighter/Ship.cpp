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
	m_upgrade_level = 0;
	m_isAutogeneratingFlux = true;
	m_flux_autogeneration_time = 0.1f;

	//Flux display
	m_flux_text = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, sf::Color::Green, sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + PLAYER_FLUX_DISPLAY_OFFSET_Y), m_team);
	m_flux_text->m_alliance = m_alliance;
	
	(*CurrentGame).addToFeedbacks(m_flux_text);
	
	//AddFluxGauge(GaugeStyle_Green, sf::Vector2f(0, m_size.y / 2 + PLAYER_FLUX_DISPLAY_OFFSET_Y));
	//(*CurrentGame).addToFeedbacks(m_flux_gauge);

	//inputs
	m_SwitchKey_released = false;
	m_BuildKey_released = false;

	//team and alliance
	SetTeam(m_team, (*CurrentGame).GetTeamAlliance(m_team));
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, PlayerTeams team, int frameNumber, int animationNumber) : FluxEntity(position, textureName, size, origin, frameNumber, animationNumber)
{
	m_team = team;
	m_alliance = (*CurrentGame).GetTeamAlliance(team);
	this->Init();
}

Ship::~Ship()
{
	if (m_warning)
	{
		m_warning->m_visible = false;
		m_warning->m_GarbageMe = true;
	}
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

void Ship::update(sf::Time deltaTime)
{
	UpdatePlayerStats();
	AutogenerateFlux();

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
	if (m_flux_text)
	{
		ostringstream ss;
		ss << m_flux;
		if (m_flux_max > 0)
			ss << "/" << m_flux_max;
		m_flux_text->setString(ss.str());
		m_flux_text->setPosition(sf::Vector2f(getPosition().x - m_flux_text->getGlobalBounds().width / 2, getPosition().y + m_size.y / 2 + PLAYER_FLUX_TEXT_OFFSET_Y));

		m_flux_text->setColor(m_flux == m_flux_max ? Color::Red : Color::Green);
	}

	if (m_flux_gauge)
	{
		m_flux_gauge->update(m_flux, m_flux_max);
		ostringstream ss;
		ss << m_flux << "/" << m_flux_max;
		m_flux_gauge->setString(ss.str());
		m_flux_gauge->setPosition(getPosition());
	}
	
	//warning feedback
	UpdateWarningFeedback();

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());
	
	if (InputGuy::isSpawningModule1(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(1);
	}
	if (InputGuy::isSpawningModule2(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(2);
	}
	if (InputGuy::isSpawningModule3(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(3);
	}
	if (InputGuy::isSpawningModule4(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(4);
	}
	if (InputGuy::isSpawningModule5(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(5);
	}
	if (InputGuy::isSpawningModule6(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(6);
	}
	if (InputGuy::isSpawningModule7(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(7);
	}
	if (InputGuy::isSpawningModule8(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(8);
	}
	if (InputGuy::isSpawningModule9(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(9);
	}
	if (InputGuy::isSpawningModule0(m_controllerType) && m_BuildKey_released)
	{
		m_BuildKey_released = !TryBuildModule(10);
	}

	//DEBUG
	if (InputGuy::isSellingModule(m_controllerType))
	{
		TrySellModule();
	}
	if (InputGuy::isFinishModuleConstruction(m_controllerType))
	{
		Module::DebugFinishModule(m_curGridIndex);
		m_flux = 0;
	}
	if (InputGuy::isRefillingFlux(m_controllerType))
	{
		if (m_flux_max > 0)
		{
			//fedback
			if (m_flux != m_flux_max)
			{
				SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, sf::Color::Green, sf::Vector2f(getPosition().x, getPosition().y - m_size.y / 2), m_team);
				text_feedback->m_alliance = m_alliance;
				SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, this, sf::Vector2f(0, -TEXT_POP_OFFSET_Y));
				ostringstream ss;
				ss << "+" << m_flux_max - m_flux << " (Debug)";
				pop_feedback->setString(ss.str());
				delete text_feedback;
				(*CurrentGame).addToFeedbacks(pop_feedback);
			}

			m_flux = m_flux_max;
			if ((*CurrentGame).playerShips[1])
				(*CurrentGame).playerShips[1]->m_flux = (*CurrentGame).playerShips[1]->m_flux_max;
		}
	}
	
	//Inputs release
	m_SwitchKey_released = !InputGuy::isUsing(m_controllerType);
	m_BuildKey_released = (!InputGuy::isSpawningModule1(m_controllerType)
		&& !InputGuy::isSpawningModule2(m_controllerType)
		&& !InputGuy::isSpawningModule3(m_controllerType)
		&& !InputGuy::isSpawningModule4(m_controllerType)
		&& !InputGuy::isSpawningModule5(m_controllerType)
		&& !InputGuy::isSpawningModule6(m_controllerType)
		&& !InputGuy::isSpawningModule7(m_controllerType)
		&& !InputGuy::isSpawningModule8(m_controllerType)
		&& !InputGuy::isSpawningModule9(m_controllerType)
		&& !InputGuy::isSpawningModule0(m_controllerType));
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
		//m_speed.x -= (m_speed.x) * deltaTime.asSeconds()* SHIP_DECCELERATION_COEF / 100.f;

		//if (abs(m_speed.x) < SHIP_MIN_SPEED)
			m_speed.x = 0;
	}

	if (!movingY)
	{
		//m_speed.y -= (m_speed.y)*deltaTime.asSeconds()*SHIP_DECCELERATION_COEF / 100.f;

		//if (abs(m_speed.y) < SHIP_MIN_SPEED)
			m_speed.y = 0;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	//m_speed.x += inputs_direction.x* SHIP_ACCELERATION;
	//m_speed.y += inputs_direction.y*SHIP_ACCELERATION;
	m_speed.x = inputs_direction.x * m_speed_max;
	m_speed.y = inputs_direction.y * m_speed_max;
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

				fluxor->m_visible = false;
				fluxor->m_GarbageMe = true;

				//feedback
				if (USE_FEEDBACK_GREEN_FLUXOR)
				{
					SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, sf::Color::Green, sf::Vector2f(getPosition().x, getPosition().y - m_size.y / 2), m_team);
					text_feedback->m_alliance = m_alliance;
					SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, this, sf::Vector2f(0, -TEXT_POP_OFFSET_Y));
					ostringstream ss;
					ss << "+" << fluxor->m_flux;
					pop_feedback->setString(ss.str());
					delete text_feedback;
					(*CurrentGame).addToFeedbacks(pop_feedback);
				}
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
						
					}
					else
					{
						
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

void Ship::SetTeam(PlayerTeams team, TeamAlliances alliance)
{
	FluxEntity::SetTeam(team, alliance);
}

bool Ship::TryBuildModule(int module_key)
{
	if (module_key <= NBVAL_ModuleType)
	{
		if (m_flux >= (*CurrentGame).m_modules[(ModuleType)(module_key - 1)]->m_flux_max_under_construction)
		{
			Module* module = Module::CreateModule(m_curGridIndex, (ModuleType)(module_key - 1), m_team);
			if (module)
			{
				m_flux -= (*CurrentGame).m_modules[(ModuleType)(module_key - 1)]->m_flux_max_under_construction;

				//feedback
				if (USE_FEEDBACK_CONSTRUCTION)
				{
					SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, Color::Red, getPosition(), m_team);
					text_feedback->m_alliance = m_alliance;
					ostringstream ss;
					ss << "-" << (*CurrentGame).m_modules[(ModuleType)(module_key - 1)]->m_flux_max_under_construction << " (building)";
					text_feedback->setString(ss.str());
					SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, this, sf::Vector2f(0, -TEXT_POP_OFFSET_Y));
					delete text_feedback;
					(*CurrentGame).addToFeedbacks(pop_feedback);
				}
			}

			return module;
		}
	}

	return false;
}

bool Ship::TrySellModule()
{
	if ((*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y] && (*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y]->m_team == m_team)
	{
		Module* module = (Module*)(*CurrentGame).m_module_grid[m_curGridIndex.x][m_curGridIndex.y];
		if (module)
		{
			if (module->m_is_sold)
			{
				module = NULL;
				return false;
			}
			else
			{
				module->m_is_sold = true;
				if (!module->m_under_construction)
				{
					module->m_flux = module->m_flux_max_under_construction;
				}
				module->SetConstructionStatus(true);

				unsigned int flux_recovered = SELLING_RATIO_OF_FLUX_RECOVERY * module->m_flux_max_under_construction;

				m_flux += flux_recovered;
				if (m_flux > m_flux_max)
				{
					m_flux = m_flux_max;
				}

				//feedback
				if (USE_FEEDBACK_CONSTRUCTION)
				{
					SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, Color::Green, getPosition(), m_team);
					text_feedback->m_alliance = m_alliance;
					ostringstream ss;
					ss << "+" << flux_recovered << " (selling -" << (int)(SELLING_RATIO_OF_FLUX_RECOVERY * 100) << "%)";
					text_feedback->setString(ss.str());
					SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_LONG_TOTAL_TIME, this, sf::Vector2f(0, -TEXT_POP_OFFSET_Y));
					delete text_feedback;
					(*CurrentGame).addToFeedbacks(pop_feedback);
				}

				module = NULL;
				return true;
			}
		}
	}

	return false;
}