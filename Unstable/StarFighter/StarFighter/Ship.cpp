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

	//Flux display
	m_flux_text.setFont(*(*CurrentGame).font2);
	m_flux_text.setCharacterSize(20);
	m_flux_text.setColor(sf::Color::White);
	m_flux_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + PLAYER_FLUX_DISPLAY_OFFSET_Y));
	(*CurrentGame).addToFeedbacks(&m_flux_text);

	//inputs
	m_CtrlKey_released = false;
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	this->Init();
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	this->Init();
}

Ship::~Ship()
{
	(*CurrentGame).removeFromFeedbacks(&m_flux_text);
}

void Ship::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void Ship::update(sf::Time deltaTime)
{
	m_CtrlKey_released = !InputGuy::isUsing();

	if (m_flux > m_flux_max  && m_flux_max > 0)
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
	ostringstream ss;
	ss << m_flux << "/" << m_flux_max;
	m_flux_text.setString(ss.str());
	m_flux_text.setPosition(sf::Vector2f(getPosition().x - m_flux_text.getGlobalBounds().width / 2, getPosition().y + m_size.y / 2 + PLAYER_FLUX_DISPLAY_OFFSET_Y));

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());
	
	//DEBUG
	if (InputGuy::isSpawningModule1())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 1 - 1), m_team);
	}
	if (InputGuy::isSpawningModule2())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 2 - 1), m_team);
	}
	if (InputGuy::isSpawningModule3())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 3 - 1), m_team);
	}
	if (InputGuy::isSpawningModule4())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 4 - 1), m_team);
	}
	if (InputGuy::isSpawningModule5())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 5 - 1), m_team);
	}
	if (InputGuy::isSpawningModule6())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 6 - 1), m_team);
	}
	if (InputGuy::isSpawningModule7())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 7 - 1), m_team);
	}
	if (InputGuy::isSpawningModule8())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 8 - 1), m_team);
	}
	if (InputGuy::isSpawningModule9())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 9 - 1), m_team);
	}
	if (InputGuy::isSpawningModule0())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 10 - 1), m_team);
	}
	if (InputGuy::isErasingModule())
	{
		Module::EraseModule(m_curGridIndex);
	}
	if (InputGuy::isFinishModuleConstruction())
	{
		Module::DebugFinishModule(m_curGridIndex);
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

	//max speed constraints
	if (abs(m_speed.x) > SHIP_MAX_SPEED)
	{
		m_speed.x = m_speed.x > 0 ? SHIP_MAX_SPEED : -SHIP_MAX_SPEED;
	}
	if (abs(m_speed.y) > SHIP_MAX_SPEED)
	{
		m_speed.y = m_speed.y > 0 ? SHIP_MAX_SPEED : -SHIP_MAX_SPEED;
	}
}

void Ship::MaxSpeedConstraints()
{
	float ship_max_speed = SHIP_MAX_SPEED;

	//max speed constraints
	NormalizeSpeed(&m_speed, ship_max_speed);
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

				if (fluxor->m_guided)
				{
					fluxor->GarbageMe = true;
				}
				else
				{
					fluxor->Death();
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
					if (InputGuy::isFiring())
					{
						unsigned int flux_max = module->m_under_construction ? module->m_flux_max_under_construction : module->m_flux_max;
						if (module->m_flux < flux_max && m_flux > 0 && m_flux_transfer_limiter_clock.getElapsedTime().asSeconds() > FLUX_TRANSFER_LIMITER_TIME)
						{
							m_flux--;
							module->m_flux++;
							m_flux_transfer_limiter_clock.restart();
						}
					}
				}

				if (InputGuy::isUsing() && m_CtrlKey_released)
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