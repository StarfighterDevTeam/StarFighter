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
		m_actions_states[i] = false;

	m_SFTargetPanel = NULL;
	m_is_asking_SFPanel = SFPanel_None;

	//MICRO BOATS
	m_acceleration = 0.f;
	m_reverse_move = false;

	m_debug_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_debug_text.setCharacterSize(20);
	m_debug_text.setColor(sf::Color::Black);
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Init();
}

Ship::Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	Init();
}

Ship::~Ship()
{

}

void Ship::Draw(sf::RenderTexture* screen)
{
	screen->draw(*this);
	screen->draw(m_debug_text);
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


	//MICRO BOATS

	//Get acceleration
	m_acceleration = 0;
	float movement_angle = GetAngleRadForSpeed(m_speed);
	float delta_angle = movement_angle - getRotation() * M_PI / 180;
	delta_angle = fmod((delta_angle + 180), 360) - 180;
	m_reverse_move = abs(delta_angle) > M_PI_2;
	float current_speed = GetAbsoluteSpeed() * (m_reverse_move == false ? 1 : -1);
	printf("delta angle : %d\n", (int)m_reverse_move);

	//Acceleration
	if (inputs_direction.y < 0)//------------------------------------------> up
	{
		m_acceleration = SHIP_ACCELERATION_FORWARD;
		current_speed += m_acceleration * deltaTime.asSeconds();
	}
	else if (inputs_direction.y > 0)//-------------------------------------> down
	{
		m_acceleration = current_speed > 0 ? - SHIP_BRAKE_SPEED : - SHIP_MAX_SPEED_BACKWARD;
		current_speed += m_acceleration * deltaTime.asSeconds();
	}
	else//-----------------------------------------------------------------> idle = deceleration
	{
		if (abs(current_speed) < SHIP_MIN_SPEED)
			current_speed = 0;
		else
		{
			m_acceleration = m_reverse_move == false ? -SHIP_DECELERATION : SHIP_DECELERATION;
			current_speed += m_acceleration * deltaTime.asSeconds();
		}
	}

	//Turn
	if (inputs_direction.x > 0)//------------------------------------------> right
	{
		setRotation(getRotation() + SHIP_TURN_RATE * deltaTime.asSeconds());
	}
	else if (inputs_direction.x < 0)//------------------------------------------> left
	{
		setRotation(getRotation() - SHIP_TURN_RATE * deltaTime.asSeconds());
	}

	
	//Thrust and orientation converted into speed
	if (current_speed > SHIP_MAX_SPEED_FORWARD)
		current_speed = SHIP_MAX_SPEED_FORWARD;

	if (current_speed < -SHIP_MAX_SPEED_BACKWARD)
		current_speed = -SHIP_MAX_SPEED_BACKWARD;

	SetSpeedVectorFromAbsoluteSpeedAndAngle(current_speed, (getRotation() + 180) * M_PI / 180);

	//Min speed
	//if (GetAbsoluteSpeed() < SHIP_MIN_SPEED)
	//{
	//	m_speed = sf::Vector2f(0, 0);
	//}

	//Feedback
	string ss = "speed: " + to_string((int)current_speed) + "\nrotation: " + to_string((int)getRotation());
	m_debug_text.setString(ss);
	m_debug_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + 50));
	
	//Action input
	//UpdateInputStates();
	//if (m_inputs_states[Action_Firing] == Input_Tap)
	//{
	//	//do some action
	//	(*CurrentGame).CreateSFTextPop("action", Font_Arial, 20, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y/2 - 20);
	//}

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