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

	m_speed_max = TILE_SIZE * 4 * SONG_BPM / 60;
	m_move_state = Move_Idle;
	m_next_action = NBVAL_PlayerActions;
	
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
		
	(*CurrentGame).m_editor_door->m_visible = m_editor_mode;

	if (m_editor_mode == false)
	{
		if (m_move_state != Move_Moving)
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
		else
		{
			if (m_inputs_states[Action_Left] == Input_Tap)
				m_next_action = Action_Left;
			else if (m_inputs_states[Action_Right] == Input_Tap)
				m_next_action = Action_Right;
			else if (m_inputs_states[Action_Up] == Input_Tap)
				m_next_action = Action_Up;
			else if (m_inputs_states[Action_Down] == Input_Tap)
				m_next_action = Action_Down;
		}
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
			if (m_inputs_states[Action_Erase] == Input_Tap)
			{
				Door::EraseDoor((*CurrentGame).m_editor_door->m_tileA, (*CurrentGame).m_editor_door->m_tileB);
			}
			//offset door
			else if (m_inputs_states[Action_Offset] == Input_Tap)
			{
				Door::OffsetDoor((*CurrentGame).m_editor_door->m_tileA, (*CurrentGame).m_editor_door->m_tileB);
			}
			//add new door
			else if (m_inputs_states[Action_Add1] == Input_Tap)
			{
				Door::AddDoor((*CurrentGame).m_editor_door->m_tileA, (*CurrentGame).m_editor_door->m_tileB, 0, 1, true);
			}
			else if (m_inputs_states[Action_Add2] == Input_Tap)
			{
				Door::AddDoor((*CurrentGame).m_editor_door->m_tileA, (*CurrentGame).m_editor_door->m_tileB, 4, 1, true);
			}
			else if (m_inputs_states[Action_Add3] == Input_Tap)
			{
				Door::AddDoor((*CurrentGame).m_editor_door->m_tileA, (*CurrentGame).m_editor_door->m_tileB, 8, 1, true);
			}
			else if (m_inputs_states[Action_Add4] == Input_Tap)
			{
				Door::AddDoor((*CurrentGame).m_editor_door->m_tileA, (*CurrentGame).m_editor_door->m_tileB, 12, 1, true);
			}
			else if (m_inputs_states[Action_Add5] == Input_Tap)
			{
				Door::AddDoor((*CurrentGame).m_editor_door->m_tileA, (*CurrentGame).m_editor_door->m_tileB, 16, 1, true);
			}
			else if (m_inputs_states[Action_Add6] == Input_Tap)
			{
				Door::AddDoor((*CurrentGame).m_editor_door->m_tileA, (*CurrentGame).m_editor_door->m_tileB, 24, 1, true);
			}
			else if (m_inputs_states[Action_Add7] == Input_Tap)
			{
				Door::AddDoor((*CurrentGame).m_editor_door->m_tileA, (*CurrentGame).m_editor_door->m_tileB, 64, 1, true);
			}
		}
	}

	//apply move
	if (m_move_state == Move_Moving)
	{
		sf::Vector2f destination_coord = (*CurrentGame).getTilePosition(m_tile_coord);
		sf::Vector2f move_vector = destination_coord - getPosition();

		int x = move_vector.x == 0 ? 0 : (move_vector.x > 0 ? 1 : -1);
		int y = move_vector.y == 0 ? 0 : (move_vector.y > 0 ? 1 : -1);

		if (GetVectorLengthSquared(move_vector) <= m_speed_max * m_speed_max * deltaTime.asSeconds() * deltaTime.asSeconds())
		{
			//stream (hold)
			bool stream_success = false;
			if (x == -1 && m_inputs_states[Action_Left] == Input_Hold)
				stream_success = Move(Action_Left);
			else if (x == 1 && m_inputs_states[Action_Right] == Input_Hold)
				stream_success = Move(Action_Right);
			else if (y == -1 && m_inputs_states[Action_Up] == Input_Hold)
				stream_success = Move(Action_Up);
			else if (y == 1 && m_inputs_states[Action_Down] == Input_Hold)
				stream_success = Move(Action_Down);
			
			if (stream_success == false)
			{
				setPosition(destination_coord);
				m_move_state = Move_Idle;

				if (m_next_action == Action_Right || m_next_action == Action_Left || m_next_action == Action_Up || m_next_action == Action_Down)
				{
					Move(m_next_action);
				}
			}
		}
		else
		{
			setPosition(getPosition().x + m_speed_max * deltaTime.asSeconds() * x, getPosition().y + m_speed_max * deltaTime.asSeconds() * y);
		}	
	}

	//setPosition(START_X + m_tile_coord.first * TILE_SIZE, START_Y - m_tile_coord.second * TILE_SIZE);

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

bool Ship::IsMovementPossible(pair<int, int> tileA, pair<int, int> tileB)
{
	for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[DoorObject])
	{
		Door* door = (Door*)object;

		if (door->m_visible == true && door->m_frequency == 0 && ((door->m_tileA == tileA && door->m_tileB == tileB) || (door->m_tileA == tileB && door->m_tileB == tileA)))
			return false;
	}

	return true;
}

bool Ship::Move(PlayerActions action)
{
	m_next_action = NBVAL_PlayerActions;

	switch (action)
	{
		case Action_Left:
		{
			if (m_tile_coord.first == 0)
				return false;

			if (IsMovementPossible(m_tile_coord, pair<int, int>(m_tile_coord.first - 1, m_tile_coord.second)) == false)
				return false;
			
			m_tile_coord.first--;
			break;
		}
		case Action_Right:
		{
			if (m_tile_coord.first == NB_TILES_X - 1)
				return false;

			if (IsMovementPossible(m_tile_coord, pair<int, int>(m_tile_coord.first + 1, m_tile_coord.second)) == false)
				return false;

			m_tile_coord.first++;
			break;
		}
		case Action_Up:
		{
			if (m_tile_coord.second == NB_TILES_Y - 1)
				return false;

			if (IsMovementPossible(m_tile_coord, pair<int, int>(m_tile_coord.first, m_tile_coord.second + 1)) == false)
				return false;

			m_tile_coord.second++;
			break;
		}
		case Action_Down:
		{
			if (m_tile_coord.second == 0)
				return false;

			if (IsMovementPossible(m_tile_coord, pair<int, int>(m_tile_coord.first, m_tile_coord.second - 1)) == false)
				return false;

			m_tile_coord.second--;
			break;
		}
	}

	m_move_state = Move_Moving;

	return true;
}

bool Ship::MoveEditor(PlayerActions action)
{
	switch (action)
	{
		case Action_Left:
		{
			if (MinBetweenIntValues((*CurrentGame).m_editor_door->m_tileA.first, (*CurrentGame).m_editor_door->m_tileB.first) == 0)
				return false;

			(*CurrentGame).m_editor_door->m_tileA.first--;
			(*CurrentGame).m_editor_door->m_tileB.first--;
			break;
		}
		case Action_Right:
		{
			if (MaxBetweenIntValues((*CurrentGame).m_editor_door->m_tileA.first, (*CurrentGame).m_editor_door->m_tileB.first) == NB_TILES_X - 1)
				return false;

			(*CurrentGame).m_editor_door->m_tileA.first++;
			(*CurrentGame).m_editor_door->m_tileB.first++;
			break;
		}
		case Action_Up:
		{
			if (MaxBetweenIntValues((*CurrentGame).m_editor_door->m_tileA.second, (*CurrentGame).m_editor_door->m_tileB.second) == NB_TILES_Y - 1)
				return false;

			(*CurrentGame).m_editor_door->m_tileA.second++;
			(*CurrentGame).m_editor_door->m_tileB.second++;
			break;
		}
		case Action_Down:
		{
			if (MinBetweenIntValues((*CurrentGame).m_editor_door->m_tileA.second, (*CurrentGame).m_editor_door->m_tileB.second) == 0)
				return false;

			(*CurrentGame).m_editor_door->m_tileA.second--;
			(*CurrentGame).m_editor_door->m_tileB.second--;
			break;
		}
		case Action_TurnEditor:
		{
			//horizontal connexion = vertical door
			if ((*CurrentGame).m_editor_door->m_tileA.second == (*CurrentGame).m_editor_door->m_tileB.second)
			{
				(*CurrentGame).m_editor_door->m_tileB.first = (*CurrentGame).m_editor_door->m_tileA.first;
				(*CurrentGame).m_editor_door->m_tileB.second = (*CurrentGame).m_editor_door->m_tileA.second == NB_TILES_Y - 1 ? (*CurrentGame).m_editor_door->m_tileA.second - 1 : (*CurrentGame).m_editor_door->m_tileA.second + 1;
			}
			else
			{
				(*CurrentGame).m_editor_door->m_tileB.second = (*CurrentGame).m_editor_door->m_tileA.second;
				(*CurrentGame).m_editor_door->m_tileB.first = (*CurrentGame).m_editor_door->m_tileA.first == NB_TILES_X - 1 ? (*CurrentGame).m_editor_door->m_tileA.first - 1 : (*CurrentGame).m_editor_door->m_tileA.first + 1;
			}

			break;
		}
	}

	//horizontal connexion = vertical door
	if ((*CurrentGame).m_editor_door->m_tileA.second == (*CurrentGame).m_editor_door->m_tileB.second)
	{
		(*CurrentGame).m_editor_door->setPosition(START_X + (0.5 + (MinBetweenIntValues((*CurrentGame).m_editor_door->m_tileA.first, (*CurrentGame).m_editor_door->m_tileB.first))) * TILE_SIZE, START_Y - ((*CurrentGame).m_editor_door->m_tileB.second * TILE_SIZE));
		(*CurrentGame).m_editor_door->setRotation(90);
	}
	else
	{
		(*CurrentGame).m_editor_door->setPosition(START_X + ((*CurrentGame).m_editor_door->m_tileA.first * TILE_SIZE), START_Y - (0.5 + (MinBetweenIntValues((*CurrentGame).m_editor_door->m_tileA.second, (*CurrentGame).m_editor_door->m_tileB.second))) * TILE_SIZE);
		(*CurrentGame).m_editor_door->setRotation(0);
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
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Subtract), Action_Erase);
		GetInputState(Keyboard::isKeyPressed(sf::Keyboard::Multiply), Action_Offset);
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
		GetInputState(false, Action_Erase);
		GetInputState(false, Action_Offset);
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