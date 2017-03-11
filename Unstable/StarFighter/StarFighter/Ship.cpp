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

	m_SFTargetPanel = NULL;
	m_is_asking_SFPanel = SFPanel_None;

	//PICKPOCKETS SPECIFIC
	m_current_collision = NULL;
	m_awerenessTick = 0;
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
	m_collisions.clear();
	m_current_collision = NULL;
	
	size_t itemsVectorSize = m_items.size();
	for (size_t i = 0; i < itemsVectorSize; i++)
	{
		delete m_items[i];
	}
}

void Ship::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

bool Ship::GetCurrentCollision(sf::Time deltaTime)
{
	if (m_collisions.empty())
	{
		m_current_collision = NULL;
	}
	else
	{
		if (std::find(m_collisions.begin(), m_collisions.end(), m_current_collision) != m_collisions.end())
		{
			//keep current collision
		}
		else
		{
			//new collision
			m_current_collision = m_collisions.front();
			//printf("new collision: %s \n", m_current_collision->m_textureName.c_str());
		}

		m_collisions.clear();
	}

	return m_current_collision;
}

void Ship::update(sf::Time deltaTime)
{
	sf::Vector2f inputs_direction = InputGuy::getDirections();

	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	ManageAcceleration(inputs_direction);
	
	//PICKPOCKETS COLLISIONS
	GetCurrentCollision(deltaTime);
	
	//Action input
	UpdateInputStates();
	if (m_inputs_states[Action_EditorMode] == Input_Tap)
	{
		(*CurrentGame).m_editorMode = !(*CurrentGame).m_editorMode;
		(*CurrentGame).m_editor_cursor->m_visible = (*CurrentGame).m_editorMode;
		printf("Editor mode = %d\n", (int)(*CurrentGame).m_editorMode);
	}

	if (m_inputs_states[Action_Firing] == Input_Tap)
	{
		//do some action
		(*CurrentGame).CreateSFTextPop("action", Font_Arial, 20, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y / 2 - 20);
	}

	//interaction with agents
	Agent* agent = (Agent*)m_current_collision;
	for (std::map<Agent*, float>::iterator it = m_awareness_map.begin(); it != m_awareness_map.end(); it++)
	{
		if (it->first == NULL)
			continue;

		if (it->first != agent)
		{
			//vision test
			float current_distance = GameObject::GetDistanceBetweenPositions(getPosition(), it->first->getPosition());
			float current_angle = GameObject::GetAngleRadBetweenPositions(getPosition(), it->first->getPosition()) * 180 / M_PI - it->first->getRotation();
			//printf("distance: %f, angle: %f ", current_distance, current_angle);

			if (current_distance < it->first->m_vision_range && current_angle < it->first->m_vision_angle / 2 && current_angle > -it->first->m_vision_angle / 2)
			{
				//printf("vision\n");
				if (it->second < WALKER_AWARENESS_MAX_ON_SIGHT)
				{
					it->second += deltaTime.asSeconds() * WALKER_AWARENESS_RAISE_ON_SIGHT;
				}
			}
			else
			{
				//printf("pas vision\n");
				it->second -= deltaTime.asSeconds() * WALKER_AWARENESS_REDUCTION_ON_IDLE;
			}
		}
		else
		{
			//collision malus
			it->second += deltaTime.asSeconds() * WALKER_AWARENESS_RAISE_ON_COLLISION;
		}
	}

	if (m_SFTargetPanel && m_SFTargetPanel->m_panel_type == SFPanel_Stratagem)
	{
		//check inputs
		Item* item_stolen = NULL;
		if (m_inputs_states[Action_Coding1] == Input_Tap)
		{
			item_stolen = m_SFTargetPanel->CheckCodeInput(1);
		}
		else if (m_inputs_states[Action_Coding2] == Input_Tap)
		{
			item_stolen = m_SFTargetPanel->CheckCodeInput(2);
		}
		else if (m_inputs_states[Action_Coding3] == Input_Tap)
		{
			item_stolen = m_SFTargetPanel->CheckCodeInput(3);
		}
		else if (m_inputs_states[Action_Coding4] == Input_Tap)
		{
			item_stolen = m_SFTargetPanel->CheckCodeInput(4);
		}

		//stealing item from the agent
		if (item_stolen)
		{
			printf("Item stolen!\n");
			m_items.push_back(item_stolen);
		}

		if (m_awareness_map[agent] > 1)
		{
			m_awareness_map[agent] = 1;
		}
		if (m_awareness_map[agent] < 0)
		{
			m_awareness_map[agent] = 0;
		}
	}

	MaxSpeedConstraints();
	IdleDecelleration(deltaTime);
	UpdateRotation();

	GameObject::update(deltaTime);

	//HUD
	m_is_asking_SFPanel = SFPanel_None;
	if (m_SFTargetPanel)
	{
		m_SFTargetPanel->Update(deltaTime);
	}
	if (m_current_collision)
	{
		m_is_asking_SFPanel = SFPanel_Stratagem;
	}

	ScreenBorderContraints();

	//Editor mode only
	const int tile_size = TILE_SIZE;
	const int nb_tile_rows = (int)(*CurrentGame).m_map_size.x / tile_size;
	const int nb_tile_lines = (int)(*CurrentGame).m_map_size.y / tile_size;

	if ((*CurrentGame).m_editorMode)
	{
		if (m_inputs_states[Action_EditorUp] == Input_Tap)
		{
			if ((*CurrentGame).m_editor_cursor->getPosition().y > tile_size / 2)
			{
				(*CurrentGame).m_editor_cursor->setPosition((*CurrentGame).m_editor_cursor->getPosition().x, (float)((*CurrentGame).m_editor_cursor->getPosition().y - tile_size));
			}
		}

		if (m_inputs_states[Action_EditorRight] == Input_Tap)
		{
			if ((*CurrentGame).m_editor_cursor->getPosition().x < tile_size * (nb_tile_rows - 0.5))
			{
				(*CurrentGame).m_editor_cursor->setPosition((float)((*CurrentGame).m_editor_cursor->getPosition().x + tile_size), (*CurrentGame).m_editor_cursor->getPosition().y);
			}
		}

		if (m_inputs_states[Action_EditorDown] == Input_Tap)
		{
			if ((*CurrentGame).m_editor_cursor->getPosition().y < tile_size * (nb_tile_lines - 0.5))
			{
				(*CurrentGame).m_editor_cursor->setPosition((*CurrentGame).m_editor_cursor->getPosition().x, (float)((*CurrentGame).m_editor_cursor->getPosition().y + tile_size));
			}
		}

		if (m_inputs_states[Action_EditorLeft] == Input_Tap)
		{
			if ((*CurrentGame).m_editor_cursor->getPosition().x > tile_size / 2)
			{
				(*CurrentGame).m_editor_cursor->setPosition((float)((*CurrentGame).m_editor_cursor->getPosition().x - tile_size), (*CurrentGame).m_editor_cursor->getPosition().y);
			}
		}

		if (m_inputs_states[Action_EditorNextType] == Input_Tap)
		{
			size_t current_tile = Game::GetTileIndex((*CurrentGame).m_editor_cursor->getPosition().x / tile_size + 1, (*CurrentGame).m_editor_cursor->getPosition().y / tile_size + 1);
			(*CurrentGame).m_tile_types[current_tile] = (*CurrentGame).m_tile_types[current_tile] >= NBVAL_TileTtype - 1 ? (TileType)0 : (TileType)((*CurrentGame).m_tile_types[current_tile] + 1);
		}

		if (m_inputs_states[Action_EditorPreviousType] == Input_Tap)
		{
			size_t current_tile = Game::GetTileIndex((*CurrentGame).m_editor_cursor->getPosition().x / tile_size + 1, (*CurrentGame).m_editor_cursor->getPosition().y / tile_size + 1);
			(*CurrentGame).m_tile_types[current_tile] = (*CurrentGame).m_tile_types[current_tile] == (TileType)0 ? (TileType)(NBVAL_TileTtype - 1) : (TileType)((*CurrentGame).m_tile_types[current_tile] - 1);
		}
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
		m_speed.x -= m_speed.x*deltaTime.asSeconds()* SHIP_DECCELERATION_COEF / 100.f;

		if (abs(m_speed.x) < SHIP_MIN_SPEED)
			m_speed.x = 0;
	}

	if (!m_movingY)
	{
		m_speed.y -= m_speed.y*deltaTime.asSeconds()*SHIP_DECCELERATION_COEF / 100.f;

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
	if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		m_stroboscopic_effect_clock.restart();
	}
}

void Ship::UpdateInputStates()
{
	GetInputState(InputGuy::isFiring(), Action_Firing);

	GetInputState(InputGuy::isCoding1(), Action_Coding1);
	GetInputState(InputGuy::isCoding2(), Action_Coding2);
	GetInputState(InputGuy::isCoding3(), Action_Coding3);
	GetInputState(InputGuy::isCoding4(), Action_Coding4);

	GetInputState(InputGuy::isEditorMode(), Action_EditorMode);
	GetInputState(InputGuy::isEditorUp(), Action_EditorUp);
	GetInputState(InputGuy::isEditorRight(), Action_EditorRight);
	GetInputState(InputGuy::isEditorDown(), Action_EditorDown);
	GetInputState(InputGuy::isEditorLeft(), Action_EditorLeft);
	GetInputState(InputGuy::isEditorNextTile(), Action_EditorNextType);
	GetInputState(InputGuy::isEditorPreviousTile(), Action_EditorPreviousType);
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
		m_actions_states[action] = m_inputs_states[action];
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

//PICKPOCKETS SPECIFIC

void Ship::Collide(GameObject* target)
{
	if (!target)
		return;

	m_collisions.push_back(target);
}

void Ship::RemoveFromAwarenessMap(GameObject* agent)
{
	Agent* agent_ = (Agent*)agent;
	m_awareness_map.erase(agent_);
}

float Ship::GetCurrentAwareness(GameObject* agent)
{
	Agent* agent_ = (Agent*)agent;
	return m_awareness_map[agent_];
}