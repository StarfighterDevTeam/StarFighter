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
}

Ship::Ship(Lane* lane, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(sf::Vector2f(0,0), speed, textureName, size, origin, frameNumber, animationNumber)
{
	this->Init();
	m_lane = lane;
	setPosition(sf::Vector2f(lane->getPosition().x, lane->getPosition().y));

	m_position_offset.x = 0.f;
	m_position_offset.y = 0.f;
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
	sf::Vector2f inputs_direction = InputGuy::getDirections();

	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	ManageAcceleration(inputs_direction, deltaTime);
	//Action input
	UpdateInputStates();
	//if (m_inputs_states[Action_Firing] == Input_Tap)
	//{
	//	//do some action
	//	(*CurrentGame).CreateSFTextPop("action", Font_Arial, 20, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y/2 - 20);
	//}
	
	IdleDecelleration(inputs_direction, deltaTime);
	//UpdateRotation();

	UpdatePosition(deltaTime);

	AnimatedSprite::update(deltaTime);
}

void Ship::UpdatePosition(sf::Time deltaTime)
{
	//stick to lane
	sf::Vector2f spawner_pos = sf::Vector2f(m_lane->m_spawner->getPosition().x, m_lane->m_spawner->getPosition().y);
	sf::Vector2f lane_pos;
	
	float lane_rad_angle = -(m_lane->m_lane_angle) * M_PI / 180.f;
	lane_pos.x = (m_lane->m_lane_offset.y - SWORDFISH_DEPTH_DEFAULT) * sin(lane_rad_angle);
	lane_pos.y = (m_lane->m_lane_offset.y - SWORDFISH_DEPTH_DEFAULT) * cos(lane_rad_angle);
	lane_pos.x = lane_pos.x < 0 ? ceil(lane_pos.x) : floor(lane_pos.x);
	lane_pos.y = lane_pos.y < 0 ? ceil(lane_pos.y) : floor(lane_pos.y);

	//apply current swordfish offset
	//compute triangular wave function
	
	//get input and apply lane limits
	m_position_offset.x += m_speed.x * deltaTime.asSeconds();

	float offset_y_before_moving = m_position_offset.y;

	
	m_position_offset.y += m_speed.y * deltaTime.asSeconds();

	if (offset_y_before_moving > 0 && m_position_offset.y <= 0)//return to surface
	{
		m_speed.y = 0;
		m_position_offset.y = 0;
	}
	if (offset_y_before_moving < 0 && m_position_offset.y >= 0)//jump into surface
	{
		m_speed.y = 0;
		m_position_offset.y = 0;
	}

	m_position_offset.x += m_lane->m_center_delta;

	//limits
	if (m_position_offset.x >= m_lane->m_lane_width / 2)
	{
		m_position_offset.x = m_lane->m_lane_width / 2;
	}
	if (m_position_offset.x <= -m_lane->m_lane_width / 2)
	{
		m_position_offset.x = -m_lane->m_lane_width / 2;
	}

	if (m_position_offset.y > SWORDFISH_DEPTH_MAX)
	{
		m_position_offset.y = SWORDFISH_DEPTH_MAX;
		m_speed.y = 0;
	}

	sf::Vector2f move_offset;
	move_offset.x = m_position_offset.x * cos(-lane_rad_angle) + m_position_offset.y * sin(lane_rad_angle);
	move_offset.y = m_position_offset.x * sin(-lane_rad_angle) + m_position_offset.y * cos(lane_rad_angle);

	setPosition(sf::Vector2f(spawner_pos.x + lane_pos.x + move_offset.x, spawner_pos.y + lane_pos.y + move_offset.y));
	setRotation(m_lane->m_lane_angle);
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

void Ship::ManageAcceleration(sf::Vector2f inputs_direction, sf::Time deltaTime)
{
	m_speed.x += inputs_direction.x* SWORDFISH_LATERAL_ACCELERATION;
	//if (m_position_offset.y >= 0)
	//{
	//	m_speed.y += inputs_direction.y > 0 ? inputs_direction.y*SWORDFISH_DIVE_ACCELERATION : 0.f;
	//
	//	if (abs(m_speed.y) > SWORDFISH_MAX_DIVE_SPEED)
	//	{
	//		m_speed.y = m_speed.y > 0 ? SWORDFISH_MAX_DIVE_SPEED : -SWORDFISH_MAX_DIVE_SPEED;
	//	}
	//}

	if (m_position_offset.y == 0)
	{
		m_speed.y += inputs_direction.y < 0 ? -SWORDFISH_AIRJUMP_ACCELERATION : 0.f;//jump
		m_speed.y += inputs_direction.y > 0 ? SWORDFISH_DIVE_ACCELERATION : 0.f;//dive
	}
	
	//max speed constraints
	if (abs(m_speed.x) > SWORDFISH_MAX_SPEED && m_position_offset.y == 0)
	{
		m_speed.x = m_speed.x > 0 ? SWORDFISH_MAX_SPEED : -SWORDFISH_MAX_SPEED;
	}
	if (abs(m_speed.x) > SWORDFISH_MAX_AIRJUMP_LATERAL_SPEED && m_position_offset.y < 0)
	{
		m_speed.x = m_speed.x > 0 ? SWORDFISH_MAX_AIRJUMP_LATERAL_SPEED : -SWORDFISH_MAX_AIRJUMP_LATERAL_SPEED;
	}
	if (abs(m_speed.x) > SWORDFISH_MAX_DIVE_LATERAL_SPEED && m_position_offset.y > 0)
	{
		m_speed.x = m_speed.x > 0 ? SWORDFISH_MAX_DIVE_LATERAL_SPEED : -SWORDFISH_MAX_DIVE_LATERAL_SPEED;
	}
}

void Ship::IdleDecelleration(sf::Vector2f inputs_direction, sf::Time deltaTime)
{
	//idle decceleration
	if (!m_movingX && m_position_offset.y == 0)
	{
		m_speed.x -= m_speed.x*deltaTime.asSeconds()* SWORDFISH_DECCELERATION_COEF / 100.f;

		if (abs(m_speed.x) < SWORDFISH_MIN_SPEED)
			m_speed.x = 0;
	}

	if (m_position_offset.y > 0 && (inputs_direction.y <= 0 || m_position_offset.y < SWORDFISH_DEPTH_MAX))
	{
		m_speed.y -= SWORDFISH_ARCHIMED_ACCELERATION;
	}

	if (m_position_offset.y < 0)
	{
		m_speed.y += SWORDFISH_GRAVITY_ACCELERATION;
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