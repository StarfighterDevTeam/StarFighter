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
	sf::Vector2f inputs_direction = InputGuy::getDirections();

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
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 1 - 1));
	}
	if (InputGuy::isSpawningModule2())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 2 - 1));
	}
	if (InputGuy::isSpawningModule3())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 3 - 1));
	}
	if (InputGuy::isSpawningModule4())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 4 - 1));
	}
	if (InputGuy::isSpawningModule5())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 5 - 1));
	}
	if (InputGuy::isSpawningModule6())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 6 - 1));
	}
	if (InputGuy::isSpawningModule7())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 7 - 1));
	}
	if (InputGuy::isSpawningModule8())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 8 - 1));
	}
	if (InputGuy::isSpawningModule9())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 9 - 1));
	}
	if (InputGuy::isSpawningModule0())
	{
		Module::CreateModule(m_curGridIndex, (ModuleType)(ModuleType_Generator + 0 - 1));
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
			if (fluxor->m_FluxorType == FluxorType_Blue)
			{
				m_flux += fluxor->m_flux;
				if (m_flux > m_flux_max)
				{
					m_flux = m_flux_max;
				}

				fluxor->Death();
			}
		}
	}
}

void Ship::GetModule(GameObject* object)
{
	if (object)
	{
		Module* module = (Module*)object;

		if (m_curGridIndex.x == module->m_curGridIndex.x && m_curGridIndex.y == module->m_curGridIndex.y)
		{
			if (InputGuy::isFiring())
			{
				if (module->m_flux < module->m_flux_max && m_flux > 0 && m_flux_transfer_limiter_clock.getElapsedTime().asSeconds() > FLUX_TRANSFER_LIMITER_TIME)
				{
					m_flux--;
					module->m_flux++;
					m_flux_transfer_limiter_clock.restart();
				}
			}

			if (module->m_activated)
			{
				if (InputGuy::isUsing())
				{
					////HACK PROTO
					if (module->m_moduleType != ModuleType_C)
					{
						Module* new_module = new Module();

						if (module->m_moduleType == ModuleType_A)
						{
							new_module->m_curGridIndex = sf::Vector2u(7, 5);
							new_module->m_moduleType = ModuleType_B;
						}
						else if (module->m_moduleType == ModuleType_B)
						{
							new_module->m_curGridIndex = sf::Vector2u(9, 5);
							new_module->m_moduleType = ModuleType_O;
						}
						else if (module->m_moduleType == ModuleType_O)
						{
							new_module->m_curGridIndex = sf::Vector2u(9, 5);
							new_module->m_moduleType = ModuleType_C;
						}

						new_module->m_parents.push_back(module);
						m_construction_buffer.push_back(new_module);
					}
				}
			}
		}
	}
}

void Ship::ResolveConstructionBufferList()
{
	size_t constructionBufferSize = m_construction_buffer.size();
	for (size_t i = 0; i < constructionBufferSize; i++)
	{
		if ((*CurrentGame).isCellFree(m_construction_buffer[i]->m_curGridIndex))
		{
			//Module* new_module = Module::CreateModule(m_construction_buffer_list[i].first, m_construction_buffer_list[i].second);

			Module* new_module = Module::CreateModule(m_construction_buffer[i]->m_curGridIndex, m_construction_buffer[i]->m_moduleType);
			new_module->m_parents.push_back(m_construction_buffer[i]->m_parents.front());
			m_construction_buffer[i]->m_parents.front()->m_children.push_back(new_module);

			//HACK PROTO
			m_construction_buffer[i]->m_parents.front()->m_flux -= new_module->m_flux_max;
			
		}
		else
		{
			//HACK PROTO
			if (m_construction_buffer[i]->m_parents.front()->m_moduleType == ModuleType_O)
			{
				m_construction_buffer[i]->m_parents.front()->GarbageMe = true;

				Module* new_module = Module::CreateModule(m_construction_buffer[i]->m_curGridIndex, m_construction_buffer[i]->m_moduleType);
				//hack: skipping the "dead" module
				new_module->m_parents.push_back(m_construction_buffer[i]->m_parents.front()->m_parents.front());
				m_construction_buffer[i]->m_parents.front()->m_parents.front()->m_children.push_back(new_module);
				//m_construction_buffer[i]->m_parents.front()->m_flux -= new_module->m_flux_max;
			}
		}
	}

	m_construction_buffer.clear();
}