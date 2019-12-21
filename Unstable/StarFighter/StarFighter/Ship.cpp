#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Ship::Ship()
{
	
}

void Ship::Init()
{
	m_editor_mode = false;
	m_editor_door = new Door(pair<int, int>(0, 0), pair<int, int>(0, 1), -1, 1);
	(*CurrentGame).addToScene(m_editor_door, EditorDoorLayer, BackgroundObject, false);
	
	m_layer = PlayerLayer;
	m_collider_type = PlayerShip;
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

	//Music Doors
	m_tile_coord = { 0, 0 };
	setPosition(START_X, START_Y);
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Init();
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	Init();
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, sf::Color color, float radius, float stroke_size) : GameObject(position, speed, color, radius, stroke_size)
{
	Init();
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
	//sf::Vector2f inputs_direction = sf::Vector2f(0, 0);
	//if ((*CurrentGame).m_window_has_focus)
	//{
	//	inputs_direction = InputGuy::getDirections();
	//}
	//
	//if (!m_disable_inputs)
	//{
	//	m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
	//	m_movingX = inputs_direction.x != 0;
	//	m_movingY = inputs_direction.y != 0;
	//}

	//ManageAcceleration(inputs_direction);
	
	//Action input
	UpdateInputStates();
	//if (m_inputs_states[Action_Firing] == Input_Tap)
	//{
	//	//do some action
	//	(*CurrentGame).CreateSFTextPop("action", Font_Arial, 20, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y/2 - 20);
	//}

	if (m_inputs_states[Action_Editor] == Input_Tap)
		m_editor_mode = !m_editor_mode;
		
	m_editor_door->m_visible = m_editor_mode;

	if (m_editor_mode == false)
	{
		if (m_inputs_states[Action_Left] == Input_Tap)
			Move(Action_Left);
		else if (m_inputs_states[Action_Right] == Input_Tap)
			Move(Action_Right);
		else if (m_inputs_states[Action_Up] == Input_Tap)
			Move(Action_Up);
		else if (m_inputs_states[Action_Down] == Input_Tap)
			Move(Action_Down);
	}
	else//EDITOR MODE
	{
		if (m_inputs_states[Action_Left] == Input_Tap)
			MoveEditor(Action_Left);
		else if (m_inputs_states[Action_Right] == Input_Tap)
			MoveEditor(Action_Right);
		else if (m_inputs_states[Action_Up] == Input_Tap)
			MoveEditor(Action_Up);
		else if (m_inputs_states[Action_Down] == Input_Tap)
			MoveEditor(Action_Down);
		else if (m_inputs_states[Action_TurnEditor] == Input_Tap)
			MoveEditor(Action_TurnEditor);
		else
		{
			//remove door
			if (m_inputs_states[Action_Remove] == Input_Tap)
			{
				Door::EraseDoor(m_editor_door->m_tileA, m_editor_door->m_tileB);
				SaveShip(this);
			}
			//add new door
			else if (m_inputs_states[Action_Add1] == Input_Tap)
			{
				Door::AddDoor(m_editor_door->m_tileA, m_editor_door->m_tileB, 0, 1, true);
				SaveShip(this);
			}
			else if (m_inputs_states[Action_Add2] == Input_Tap)
			{
				Door::AddDoor(m_editor_door->m_tileA, m_editor_door->m_tileB, 4, 1, true);
				SaveShip(this);
			}
			else if (m_inputs_states[Action_Add3] == Input_Tap)
			{
				Door::AddDoor(m_editor_door->m_tileA, m_editor_door->m_tileB, 8, 1, true);
				SaveShip(this);
			}
			else if (m_inputs_states[Action_Add4] == Input_Tap)
			{
				Door::AddDoor(m_editor_door->m_tileA, m_editor_door->m_tileB, 12, 1, true);
				SaveShip(this);
			}
			else if (m_inputs_states[Action_Add5] == Input_Tap)
			{
				Door::AddDoor(m_editor_door->m_tileA, m_editor_door->m_tileB, 16, 1, true);
				SaveShip(this);
			}
			else if (m_inputs_states[Action_Add6] == Input_Tap)
			{
				Door::AddDoor(m_editor_door->m_tileA, m_editor_door->m_tileB, 24, 1, true);
				SaveShip(this);
			}
		}
			
	}

	setPosition(START_X + m_tile_coord.first * TILE_SIZE, START_Y - m_tile_coord.second * TILE_SIZE);

	//MaxSpeedConstraints();
	//IdleDecelleration(deltaTime);
	//UpdateRotation();

	GameObject::update(deltaTime);

	//HUD
	m_is_asking_SFPanel = SFPanel_None;
	if (m_SFTargetPanel)
	{
		m_SFTargetPanel->Update(deltaTime);
	}

	ScreenBorderContraints();	
}

bool Ship::Move(PlayerActions action)
{
	switch (action)
	{
		case Action_Left:
		{
			if (m_tile_coord.first == 0)
				return false;

			m_tile_coord.first--;
			break;
		}
		case Action_Right:
		{
			if (m_tile_coord.first == NB_TILES_X - 1)
				return false;

			m_tile_coord.first++;
			break;
		}
		case Action_Up:
		{
			if (m_tile_coord.second == NB_TILES_Y - 1)
				return false;

			m_tile_coord.second++;
			break;
		}
		case Action_Down:
		{
			if (m_tile_coord.second == 0)
				return false;

			m_tile_coord.second--;
			break;
		}
	}

	return true;
}

bool Ship::MoveEditor(PlayerActions action)
{
	switch (action)
	{
		case Action_Left:
		{
			if (MinBetweenIntValues(m_editor_door->m_tileA.first, m_editor_door->m_tileB.first) == 0)
				return false;

			m_editor_door->m_tileA.first--;
			m_editor_door->m_tileB.first--;
			break;
		}
		case Action_Right:
		{
			if (MaxBetweenIntValues(m_editor_door->m_tileA.first, m_editor_door->m_tileB.first) == NB_TILES_X - 1)
				return false;

			m_editor_door->m_tileA.first++;
			m_editor_door->m_tileB.first++;
			break;
		}
		case Action_Up:
		{
			if (MaxBetweenIntValues(m_editor_door->m_tileA.second, m_editor_door->m_tileB.second) == NB_TILES_Y - 1)
				return false;

			m_editor_door->m_tileA.second++;
			m_editor_door->m_tileB.second++;
			break;
		}
		case Action_Down:
		{
			if (MinBetweenIntValues(m_editor_door->m_tileA.second, m_editor_door->m_tileB.second) == 0)
				return false;

			m_editor_door->m_tileA.second--;
			m_editor_door->m_tileB.second--;
			break;
		}
		case Action_TurnEditor:
		{
			//horizontal connexion = vertical door
			if (m_editor_door->m_tileA.second == m_editor_door->m_tileB.second)
			{
				m_editor_door->m_tileB.first = m_editor_door->m_tileA.first;
				m_editor_door->m_tileB.second = m_editor_door->m_tileA.second == NB_TILES_Y - 1 ? m_editor_door->m_tileA.second - 1 : m_editor_door->m_tileA.second + 1;
			}
			else
			{
				m_editor_door->m_tileB.second = m_editor_door->m_tileA.second;
				m_editor_door->m_tileB.first = m_editor_door->m_tileA.first == NB_TILES_X - 1 ? m_editor_door->m_tileA.first - 1 : m_editor_door->m_tileA.first + 1;
			}

			break;
		}
	}

	//horizontal connexion = vertical door
	if (m_editor_door->m_tileA.second == m_editor_door->m_tileB.second)
	{
		m_editor_door->setPosition(START_X + (0.5 + (MinBetweenIntValues(m_editor_door->m_tileA.first, m_editor_door->m_tileB.first))) * TILE_SIZE, START_Y - (m_editor_door->m_tileB.second * TILE_SIZE));
		m_editor_door->setRotation(90);
	}
	else
	{
		m_editor_door->setPosition(START_X + (m_editor_door->m_tileA.first * TILE_SIZE), START_Y - (0.5 + (MinBetweenIntValues(m_editor_door->m_tileA.second, m_editor_door->m_tileB.second))) * TILE_SIZE);
		m_editor_door->setRotation(0);
	}
		
	return true;
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
	NormalizeVector(&m_speed, ship_max_speed);
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
		setRotation((GetAngleRadForVector(m_speed) * 180 / (float)M_PI));
	}
}

void Ship::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject, true);

		m_stroboscopic_effect_clock.restart();
	}
}

void Ship::UpdateInputStates()
{
	if ((*CurrentGame).m_window_has_focus)
	{
		GetInputState(InputGuy::isFiring(), Action_Firing);

		GetInputState(InputGuy::getDirections().x < 0 && InputGuy::getDirections().y == 0, Action_Left);
		GetInputState(InputGuy::getDirections().x > 0 && InputGuy::getDirections().y == 0, Action_Right);
		GetInputState(InputGuy::getDirections().x == 0 && InputGuy::getDirections().y < 0, Action_Up);
		GetInputState(InputGuy::getDirections().x == 0 && InputGuy::getDirections().y > 0, Action_Down);

		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::F4), Action_Editor);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Add), Action_TurnEditor);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Num1), Action_Add1);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Num2), Action_Add2);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Num3), Action_Add3);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Num4), Action_Add4);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Num5), Action_Add5);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Num6), Action_Add6);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Num7), Action_Add7);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Num8), Action_Add8);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Subtract), Action_Remove);
	}
	else
	{
		GetInputState(false, Action_Firing);

		GetInputState(false, Action_Left);
		GetInputState(false, Action_Right);
		GetInputState(false, Action_Up);
		GetInputState(false, Action_Down);

		GetInputState(false, Action_Editor);
		GetInputState(false, Action_TurnEditor);
		GetInputState(false, Action_Add1);
		GetInputState(false, Action_Add2);
		GetInputState(false, Action_Add3);
		GetInputState(false, Action_Add4);
		GetInputState(false, Action_Add5);
		GetInputState(false, Action_Add6);
		GetInputState(false, Action_Add7);
		GetInputState(false, Action_Add8);
		GetInputState(false, Action_Remove);
	}
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
	printf("Save\n");

	printf("Saving game in local file.\n");
	assert(ship != NULL);

	ofstream data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		vector<Door*> save_list;
		for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[DoorObject])
		{
			Door* door = (Door*)object;

			if (door->m_frequency >= 0 && door->m_visible == true)
				save_list.push_back(door);
		}
		for (GameObject* object : (*CurrentGame).m_tmp_sceneGameObjects)
		{
			if (object->m_collider_type == DoorObject && object->m_visible == true)
			{
				Door* door = (Door*)object;

				if (door->m_frequency >= 0)
					save_list.push_back(door);
			}
		}

		for (Door* door : save_list)
			data << door->m_tileA.first << " " << door->m_tileA.second << " " << door->m_tileB.first << " " << door->m_tileB.second << " " << door->m_frequency << " " << door->m_offset << endl;

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
	printf("Loading ship from local file.\n");
	assert(ship != NULL);

	std::ifstream data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in);

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