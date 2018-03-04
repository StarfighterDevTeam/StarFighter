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

	m_state = PlayerJump_Idle;
	m_size_hitbox = sf::Vector2f(22, 88);
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
	if (m_inputs_states[Action_Respawn] == Input_Tap)
	{
		Respawn();
	}

	sf::Vector2f inputs_direction = InputGuy::getDirections();

	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	//Action input
	UpdateInputStates();
	if (m_inputs_states[Action_EditorMode] == Input_Tap)
	{
		(*CurrentGame).m_editorMode = !(*CurrentGame).m_editorMode;
		(*CurrentGame).m_editor_cursor->m_visible = (*CurrentGame).m_editorMode;
		printf("Editor mode = %d\n", (int)(*CurrentGame).m_editorMode);
	}

	if (!(*CurrentGame).m_editorMode)
	{
		ManageAcceleration(inputs_direction);
		ManageJump();

		

		//MaxSpeedConstraints();
		IdleDecelleration(deltaTime);
		//UpdateRotation();
		ChooseAnimation();

		GameObject::update(deltaTime);

		//HUD
		m_is_asking_SFPanel = SFPanel_None;
		if (m_SFTargetPanel)
		{
			m_SFTargetPanel->Update(deltaTime);
		}

		ScreenBorderContraints();

		//if (m_speed.y != 0)
		//printf("speed = %f\n", m_speed.y);
	}
	
	//Editor mode only
	else
	{
		if (inputs_direction.x > 0 && (*CurrentGame).m_editor_cursor->getPosition().x < (*CurrentGame).m_map_size.x)
		{
			m_inputs_states[Action_EditorFast] != Input_Hold ? (*CurrentGame).m_editor_cursor->move(sf::Vector2f(1, 0)) : (*CurrentGame).m_editor_cursor->move(sf::Vector2f(10, 0));
		}
		if (inputs_direction.x < 0 && (*CurrentGame).m_editor_cursor->getPosition().x > 0)
		{
			m_inputs_states[Action_EditorFast] != Input_Hold ? (*CurrentGame).m_editor_cursor->move(sf::Vector2f(-1, 0)) : (*CurrentGame).m_editor_cursor->move(sf::Vector2f(-10, 0));
		}
		if (inputs_direction.y > 0 && (*CurrentGame).m_editor_cursor->getPosition().y > 0)
		{
			m_inputs_states[Action_EditorFast] != Input_Hold ? (*CurrentGame).m_editor_cursor->move(sf::Vector2f(0, 1)) : (*CurrentGame).m_editor_cursor->move(sf::Vector2f(0, 10));
		}
		if (inputs_direction.y < 0 && (*CurrentGame).m_editor_cursor->getPosition().y < (*CurrentGame).m_map_size.y)
		{
			m_inputs_states[Action_EditorFast] != Input_Hold ? (*CurrentGame).m_editor_cursor->move(sf::Vector2f(0, -1)) : (*CurrentGame).m_editor_cursor->move(sf::Vector2f(0, -10));
		}

		//if (m_inputs_states[Action_EditorNextType] == Input_Tap)
		//{
		//	size_t current_tile = Game::GetTileIndex((*CurrentGame).m_editor_cursor->getPosition().x / tile_size + 1, (*CurrentGame).m_editor_cursor->getPosition().y / tile_size + 1);
		//	(*CurrentGame).m_tile_types[current_tile] = (*CurrentGame).m_tile_types[current_tile] >= NBVAL_TileTtype - 1 ? (TileType)0 : (TileType)((*CurrentGame).m_tile_types[current_tile] + 1);
		//	SaveShip(this);
		//}
		//
		//if (m_inputs_states[Action_EditorPreviousType] == Input_Tap)
		//{
		//	size_t current_tile = Game::GetTileIndex((*CurrentGame).m_editor_cursor->getPosition().x / tile_size + 1, (*CurrentGame).m_editor_cursor->getPosition().y / tile_size + 1);
		//	(*CurrentGame).m_tile_types[current_tile] = (*CurrentGame).m_tile_types[current_tile] == (TileType)0 ? (TileType)(NBVAL_TileTtype - 1) : (TileType)((*CurrentGame).m_tile_types[current_tile] - 1);
		//	SaveShip(this);
		//}
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

	//if (!m_movingY)
	//{
	//	m_speed.y -= m_speed.y*deltaTime.asSeconds()*SHIP_DECCELERATION_COEF / 100.f;
	//
	//	if (abs(m_speed.y) < SHIP_MIN_SPEED)
	//		m_speed.y = 0;
	//}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	m_speed.x += inputs_direction.x* SHIP_ACCELERATION;

	//m_speed.y += inputs_direction.y* SHIP_JUMP_ACCELERATION;

	//max speed constraints
	if (abs(m_speed.x) > SHIP_MAX_SPEED_HORIZONTAL)
	{
		m_speed.x = m_speed.x > 0 ? SHIP_MAX_SPEED_HORIZONTAL : -SHIP_MAX_SPEED_HORIZONTAL;
	}
	if (abs(m_speed.y) > SHIP_MAX_SPEED_VERTICAL)
	{
		m_speed.y = m_speed.y > 0 ? SHIP_MAX_SPEED_VERTICAL : -SHIP_MAX_SPEED_VERTICAL;
	}
}

void Ship::MaxSpeedConstraints()
{
	//float ship_max_speed = SHIP_MAX_SPEED;
	//
	////max speed constraints
	//NormalizeSpeed(&m_speed, ship_max_speed);
}

void Ship::ManageJump()
{	//Jump
	if ((m_state == PlayerJump_Idle || m_state == PlayerJump_Hanging) && m_inputs_states[Action_Jumping] == Input_Tap)
	{
		m_speed.y -= SHIP_JUMP_ACCELERATION;
		m_state = PlayerJump_Jumping;    
	}

	//Gravity
	if (m_state == PlayerJump_Jumping || m_state == PlayerJump_Falling)
	{
		m_speed.y += SHIP_GRAVITY;
	}

	//Hanging to an edge prevents movement
	if (m_state == PlayerJump_Hanging)
	{
		m_speed = sf::Vector2f(0, 0);
	}
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

int Ship::ChooseAnimation()
{
	if (m_speed.x > 0)
	{
		setAnimationLine(Animation_WalkRight, false);
	}
	else if (m_speed.x < 0)
	{
		setAnimationLine(Animation_WalkLeft, false);
	}
	else
	{
		setAnimationLine(m_currentAnimationIndex, false);
	}

	return m_currentAnimationIndex;
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
	GetInputState(InputGuy::isJumping(), Action_Jumping);
	GetInputState(InputGuy::isEditorMode(), Action_EditorMode);
	GetInputState(InputGuy::isEditorFast(), Action_EditorFast);
	GetInputState(InputGuy::isRestartingScript(), Action_Respawn);
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

bool Ship::Land(float coordinate)
{
	bool current_state = m_state;
	m_state = PlayerJump_Idle;

	m_speed.y = 0;
	setPosition(sf::Vector2f(getPosition().x, coordinate - (m_size.y / 2) - 1)); // 1 pixel above ground

	return (current_state != PlayerJump_Idle);
}

bool Ship::Fall()
{
	bool current_state = m_state;
	
	if (m_state != PlayerJump_Hanging)
	{
		m_state = PlayerJump_Falling;
	}
	
	return (current_state != PlayerJump_Falling);
}

bool Ship::HitWallFromLeft(float coordinate)
{
	m_speed.x = 0;
	setPosition(sf::Vector2f(coordinate + m_size_hitbox.x / 2, getPosition().y));

	return false;
}

bool Ship::HitWallFromRight(float coordinate)
{
	m_speed.x = 0;
	setPosition(sf::Vector2f(coordinate - m_size_hitbox.x / 2, getPosition().y));

	return false;
}

bool Ship::HitCeiling(float coordinate)
{
	m_speed.y = 0;
	setPosition(sf::Vector2f(getPosition().x, coordinate + m_size_hitbox.y / 2));

	return false;
}

bool Ship::HangToWallFromLeft(float coordinate_x, float edge_height)
{
	//m_speed.x = 0;
	//m_speed.y = 0;
	//m_state = PlayerJump_Hanging;
	//setPosition(sf::Vector2f(coordinate_x - m_size_hitbox.x / 2, getPosition().y));
	bool current_state = m_state;

	m_speed.x = 0;
	m_speed.y = 0;
	setPosition(sf::Vector2f(coordinate_x - m_size_hitbox.x / 2, edge_height));
	//
	m_state = PlayerJump_Hanging;

	return (current_state != PlayerJump_Hanging);
}

bool Ship::HangToWallFromRight(float coordinate_x, float edge_height)
{
	//m_speed.x = 0;
	//m_speed.y = 0;
	//m_state = PlayerJump_Hanging;
	//setPosition(sf::Vector2f(coordinate_x + m_size_hitbox.x / 2, getPosition().y));

	bool current_state = m_state;

	m_speed.x = 0;
	m_speed.y = 0;
	setPosition(sf::Vector2f(coordinate_x + m_size_hitbox.x / 2, edge_height));

	m_state = PlayerJump_Hanging;

	return (current_state != PlayerJump_Hanging);
}

void Ship::Respawn()
{
	setPosition(sf::Vector2f(1000, 500));
	m_state = PlayerJump_Idle;
}
