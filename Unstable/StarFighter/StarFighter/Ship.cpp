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
	m_moving = false;
	m_movingX = m_movingY = false;
	m_disable_inputs = false;
	m_controllerType = AllControlDevices;
	for (size_t i = 0; i < NBVAL_PlayerActions; i++)
	{
		m_actions_states[i] = false;
	}

	m_SFUnitInfoPanel = NULL;
	m_is_asking_SFUnitPanel = false;
	m_SFContextInfoPanel = NULL;
	m_is_asking_SFContextPanel_string = "";
	m_hovered_object = NULL;
	m_selected_object = NULL;
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
	
}

void Ship::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void Ship::update(sf::Time deltaTime)
{
	//cleaning selection and highlight from elements that are going to be garbaged next frame
	if (m_hovered_object && m_hovered_object->m_GarbageMe)
	{
		m_hovered_object = NULL;
	}
	if (m_selected_object && m_selected_object->m_GarbageMe)
	{
		m_selected_object = NULL;
	}

	//get mouse coordinates
	sf::Vector2i mousepos2i = sf::Mouse::getPosition(*(*CurrentGame).getMainWindow());
	m_mouse_pos = (*CurrentGame).getMainWindow()->mapPixelToCoords(mousepos2i, (*CurrentGame).m_view);

	ManageHud(deltaTime);

	sf::Vector2f inputs_direction = InputGuy::getDirections();

	//mouse scroll on screen borders
	if (inputs_direction == sf::Vector2f(0, 0))
	{
		const float x = (*CurrentGame).m_view.getSize().x / 2;
		const float y = (*CurrentGame).m_view.getSize().y / 2;
		const float a = (*CurrentGame).m_view.getCenter().x;
		const float b = (*CurrentGame).m_view.getCenter().y;

		if (m_mouse_pos.x > a + x - MOUSE_SCROLL_MARGIN && m_mouse_pos.x < a + x && m_mouse_pos.y < b + y && m_mouse_pos.y > b - y)
		{
			inputs_direction.x = 1.0f;
		}
		if (m_mouse_pos.x < a - x + MOUSE_SCROLL_MARGIN && m_mouse_pos.x > a - x && m_mouse_pos.y < b + y && m_mouse_pos.y > b - y)
		{
			inputs_direction.x = -1.0f;
		}
		if (m_mouse_pos.y < b - y + MOUSE_SCROLL_MARGIN && m_mouse_pos.y > b - y && m_mouse_pos.x > a - x && m_mouse_pos.x < a + x)
		{
			inputs_direction.y = -1.0f;
		}
		if (m_mouse_pos.y > b + y - MOUSE_SCROLL_MARGIN && m_mouse_pos.y < b + y && m_mouse_pos.x > a - x && m_mouse_pos.x < a + x)
		{
			inputs_direction.y = 1.0f;
		}
	}

	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	//cursor sticks to mouse at all time
	setPosition(m_mouse_pos);

	//keyboard moves camera
	ManageAcceleration(inputs_direction);
	
	//Get action inputs
	UpdateInputStates();
	
	//Selection (left click)
	if (m_inputs_states[Action_Select] == Input_Tap)
	{
		SelectObject(m_hovered_object);//this function resists null ptr
	}

	//Action (right click)
	if (m_selected_object)
	{
		if (m_selected_object->m_collider_type == StarshipObject)
		{
			Starship* starship = (Starship*)m_selected_object;

			if (starship->m_mission == StarshipMission_Idle)
			{
				if (m_hovered_object && m_hovered_object->m_collider_type == LocationObject)
				{
					Location* location = (Location*)m_hovered_object;

					//unload fuel?
					if (location == starship->m_current_location)
					{
						m_is_asking_SFContextPanel_string = "Unload fuel tank";
						//Action (right click)
						if (m_inputs_states[Action_Assigning] == Input_Tap)
						{
							starship->UnloadFuelTank(location);
						}
					}
					//scan mission?
					else if (starship->m_scout_range > 0 && !location->m_identified)
					{
						m_is_asking_SFContextPanel_string = "Scan";
						//Action (right click)
						if (m_inputs_states[Action_Assigning] == Input_Tap)
						{
							starship->AssignMission(StarshipMission_Scan, sf::Vector2f(0, 0), location, NULL);
						}
					}
					//mining mission?
					else if (starship->m_nb_drills > 0 && location->m_identified)
					{
						m_is_asking_SFContextPanel_string = "Drill";
						//Action (right click)
						if (m_inputs_states[Action_Assigning] == Input_Tap)
						{
							starship->AssignMission(StarshipMission_Drill, sf::Vector2f(0, 0), location, starship->m_current_location);
						}
					}
				}
				//scout mission ?
				else if (starship->m_scout_range > 0 && !m_hovered_object)
				{
					m_is_asking_SFContextPanel_string = "Scout";
					//Action (right click)
					if (m_inputs_states[Action_Assigning] == Input_Tap)
					{
						starship->AssignMission(StarshipMission_Scout, getPosition());
					}
				}
			}
		}
		else if (m_selected_object->m_collider_type == LocationObject)
		{
			Location* location = (Location*)m_selected_object;

			if (m_hovered_object && m_hovered_object->m_collider_type == StarshipObject)
			{
				Starship* starship = (Starship*)m_hovered_object;

				if (starship->m_current_location == location)
				{
					m_is_asking_SFContextPanel_string = "Supply fuel tank";
					//Action (right click)
					if (m_inputs_states[Action_Assigning] == Input_Tap)
					{
						location->SupplyFuelTank(starship->m_fuel_tank, *starship);
					}
				}
			}

			if (m_inputs_states[Action_Produce_1] == Input_Tap)
			{
				location->Produce("miner", false);
			}

			if (m_inputs_states[Action_Produce_2] == Input_Tap)
			{
				location->Produce("probe", false);
			}

			if (m_inputs_states[Action_Produce_3] == Input_Tap)
			{
				location->Produce("cargo", false);
			}
		}
	}
	

	MaxSpeedConstraints();
	IdleDecelleration(deltaTime);
	UpdateRotation();

	GameObject::update(deltaTime);

	ScreenBorderContraints();	
}

void Ship::ManageHud(sf::Time deltaTime)
{
	//Hud
	m_is_asking_SFUnitPanel = false;
	m_is_asking_SFContextPanel_string = "";

	if (m_hovered_object)
	{
		if (m_SFUnitInfoPanel && m_SFUnitInfoPanel->GetUnit() && m_SFUnitInfoPanel->GetUnit() != m_hovered_object)
		{
			//don't display it yet, first we need to destroy the previous panel
		}
		else
		{
			m_is_asking_SFUnitPanel = true;
		}
	}
	else if (m_selected_object)
	{
		if (m_SFUnitInfoPanel && m_SFUnitInfoPanel->GetUnit() && m_SFUnitInfoPanel->GetUnit() != m_selected_object)
		{
			//don't display it yet, first we need to destroy the previous panel
		}
		else
		{
			m_is_asking_SFUnitPanel = true;
		}
	}

	if (m_SFUnitInfoPanel)
	{
		sf::Vector2f position = sf::Vector2f(m_SFUnitInfoPanel->getSize().x / 2 + m_SFUnitInfoPanel->getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().x / 2 + getPosition().x, m_SFUnitInfoPanel->getSize().y / 2 + m_SFUnitInfoPanel->getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + getPosition().y);
		m_SFUnitInfoPanel->setPosition(position);
		m_SFUnitInfoPanel->Update(deltaTime);
	}

	if (m_SFContextInfoPanel)
	{
		sf::Vector2f position = sf::Vector2f(m_SFContextInfoPanel->getSize().x / 2 + m_SFContextInfoPanel->getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().x / 2 + getPosition().x, m_SFContextInfoPanel->getSize().y / 2 + m_SFContextInfoPanel->getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + getPosition().y);
		m_SFContextInfoPanel->setPosition(position);
		m_SFContextInfoPanel->Update(deltaTime);
	}
}

bool Ship::ScreenBorderContraints()
{
	bool touched_screen_border = false;

	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		m_speed.x = 0;
		touched_screen_border = true;
	}

	if (this->getPosition().x > (*CurrentGame).m_map_size.x - (m_size.x / 2))
	{
		this->setPosition((*CurrentGame).m_map_size.x - (m_size.x / 2), this->getPosition().y);
		m_speed.x = 0;
		touched_screen_border = true;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		m_speed.y = 0;
		touched_screen_border = true;
	}

	if (this->getPosition().y > (*CurrentGame).m_map_size.y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, (*CurrentGame).m_map_size.y - (m_size.y / 2));
		m_speed.y = 0;
		touched_screen_border = true;
	}

	return touched_screen_border;
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	//idle decceleration
	if (!m_movingX)
	{
		m_speed.x = 0;
	}

	if (!m_movingY)
	{
		m_speed.y = 0;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	m_speed.x += inputs_direction.x * SHIP_ACCELERATION;
	m_speed.y += inputs_direction.y * SHIP_ACCELERATION;

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
	if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		m_stroboscopic_effect_clock.restart();
	}
}

void Ship::UpdateInputStates()
{
	GetInputState(InputGuy::isSelecting(), Action_Select);
	GetInputState(InputGuy::isAssigning(), Action_Assigning);
	GetInputState(InputGuy::isProducing1(), Action_Produce_1);
	GetInputState(InputGuy::isProducing2(), Action_Produce_2);
	GetInputState(InputGuy::isProducing3(), Action_Produce_3);
}

bool Ship::UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition)
{
	if (state_required == Input_Tap && condition && m_inputs_states[action] == Input_Tap)
	{
		m_actions_states[action] = !m_actions_states[action];
		return true;
	}
	else if (state_required == Input_Hold && condition)
	{
		m_actions_states[action] = m_inputs_states[action] == Input_Hold;
		return true;
	}
	else if (!condition)
	{
		m_actions_states[action] = false;
	}
	return false;
}

void Ship::GetInputState(bool input_guy_boolean, PlayerActions action)
{
	if (input_guy_boolean)
	{
		m_inputs_states[action] = m_inputs_states[action] == Input_Release ? Input_Tap : Input_Hold;
	}
	else
	{
		m_inputs_states[action] = Input_Release;
	}
}

//SAVE AND LOAD LOCAL FILE
int Ship::SaveShip(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Saving game in local file.\n");
	assert(ship != NULL);

	ofstream data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		data << "Save ";// << ship->m_speed.x << endl;

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No existing save file founded. A new file is going to be created.\n" << endl;
	}

	return 0;
}

bool Ship::LoadShip(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading ship from local file.\n");
	assert(ship != NULL);

	std::ifstream  data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		while (std::getline(data, line))
		{
			std::istringstream ss(line);

			string saved_content;
			ss >> saved_content;
			//ship->content = saved_content;
		}

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found. A new file is going to be created.\n" << endl;
		return false;
	}
}

void Ship::HoverObject(GameObject* object)
{
	if (object != m_hovered_object && m_hovered_object)
	{
		StockEntity* entity = (StockEntity*)m_hovered_object;
		entity->m_hovered = false;
	}

	m_hovered_object = object;

	if (object)
	{
		StockEntity* entity = (StockEntity*)object;
		entity->m_hovered = true;
	}
}

void Ship::SelectObject(GameObject* object)
{
	if (object != m_selected_object && m_selected_object)
	{
		StockEntity* entity = (StockEntity*)m_selected_object;
		entity->m_selected = false;
	}

	m_selected_object = object;

	if (object)
	{
		StockEntity* entity = (StockEntity*)object;
		entity->m_selected = true;
	}
}