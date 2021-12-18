#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

#define MARLIN_SIZE_X			250
#define MARLIN_SIZE_Y			80

#define HORIZONTAL_SPEED		200.0f
#define JUMP_SPEED			30000.0f
#define DIVE_SPEED			5000.0f
#define DIVE_MAX				-100.f
#define GRAVITY_SPEED		2500.0f
#define RESISTANCE_SPEED		16.0f
#define MARLIN_DENSITY		1.4f	//between 0 and 2

// ----------------SHIP ---------------
Ship::Ship()
{

}

void Ship::Init()
{
	m_layer = PlayerShipLayer;
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

	m_speed = sf::Vector2f(0, 0);
	float lane_y = (*CurrentGame).m_lane->getPosition().y;
	setPosition(sf::Vector2f(400, lane_y));

	m_debug_text = new SFText(*(*CurrentGame).m_font, 20, sf::Color::White, sf::Vector2f(200, 200), PlayerBlue);
	(*CurrentGame).addToFeedbacks(m_debug_text);

	m_rect = new SFRectangle(getPosition(), sf::Vector2f(MARLIN_SIZE_X, MARLIN_SIZE_Y), sf::Color::Transparent, 2, sf::Color::Red, PlayerBlue);
	(*CurrentGame).addToFeedbacks(m_rect);

	m_rect_mid = new SFRectangle(getPosition(), sf::Vector2f(MARLIN_SIZE_X + 20, 0), sf::Color::Transparent, 1, sf::Color::Red, PlayerBlue);
	(*CurrentGame).addToFeedbacks(m_rect_mid);

	m_gravity_rect = new SFRectangle(getPosition(), sf::Vector2f(4, 0), sf::Color::Magenta, 0, sf::Color::Transparent, PlayerBlue);
	(*CurrentGame).addToFeedbacks(m_gravity_rect);

	m_archimede_rect = new SFRectangle(getPosition(), sf::Vector2f(4, 0), sf::Color::Cyan, 0, sf::Color::Transparent, PlayerBlue);
	(*CurrentGame).addToFeedbacks(m_archimede_rect);

	m_resistance_rect = new SFRectangle(getPosition(), sf::Vector2f(4, 0), sf::Color::Black, 0, sf::Color::Transparent, PlayerBlue);
	(*CurrentGame).addToFeedbacks(m_resistance_rect);

	m_jump_rect = new SFRectangle(getPosition(), sf::Vector2f(4, 0), sf::Color::Green, 0, sf::Color::Transparent, PlayerBlue);
	(*CurrentGame).addToFeedbacks(m_jump_rect);

	m_net_rect = new SFRectangle(getPosition(), sf::Vector2f(4, 0), sf::Color::Red, 0, sf::Color::Transparent, PlayerBlue);
	(*CurrentGame).addToFeedbacks(m_net_rect);

	m_display_debug_rect = true;
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
	delete m_debug_text;
	delete m_rect;
	delete m_rect_mid;
}

void Ship::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void Ship::update(sf::Time deltaTime)
{
	//horizontal scrolling
	m_speed.x = HORIZONTAL_SPEED;
	if (getPosition().x > 1500)
		setPosition(sf::Vector2f(400, getPosition().y));

	//immersion calculation: convert to a value betweent 0 (no archimede applied) to 1 (full archimede applied)
	float altitude = -getPosition().y - (-(*CurrentGame).m_lane->getPosition().y);
	float immersion = -(altitude / (MARLIN_SIZE_Y * 0.5));
	immersion = Lerp(immersion, -1, 1, 0, 1);

	//gravity
	float gravity = GRAVITY_SPEED * MARLIN_DENSITY * deltaTime.asSeconds();

	//archimede
	float archimede = GRAVITY_SPEED * deltaTime.asSeconds() * immersion * 2;

	//Action input
	UpdateInputStates();
	float jump = 0;
	if ((*CurrentGame).m_window_has_focus == true)
	{
		//if (m_inputs_states[Action_Jumping] == Input_Tap || m_inputs_states[Action_Jumping] == Input_Hold)
		if (InputGuy::getDirections().y < 0)
		{
			jump = JUMP_SPEED * immersion * deltaTime.asSeconds();
		}
		else if (InputGuy::getDirections().y > 0 && altitude > DIVE_MAX)
		{
			jump = - DIVE_SPEED * immersion * deltaTime.asSeconds();
		}
	}
	
	//resistance
	float resistance = RESISTANCE_SPEED * immersion * m_speed.y * deltaTime.asSeconds();

	//apply forces
	m_speed.y -= jump + archimede - gravity + resistance;

	GameObject::update(deltaTime);

	//DEBUG TOOLS
	ostringstream ss;
	ss << "ASL : " + to_string((int)altitude) + "\n";
	ss << "Vz : " + to_string((int)-m_speed.y) + "\n";
	ss << "Imm : " + to_string((int)(immersion * 100)) + "%\n";
	ss << "\n";
	ss << "G : " + to_string((int)gravity) + "\n";
	ss << "Archi : " + to_string((int)archimede) + "\n";
	ss << "Jump : " + to_string((int)jump) + "\n";
	ss << "Res : " + to_string((int)resistance) + "\n";
	ss << "Net : " + to_string((int)(-gravity + archimede + jump + resistance)) + "\n";
	ss << "\n" << "F4: hide vectors";

	m_debug_text->setString(ss.str());

	m_gravity_rect->setSize(sf::Vector2f(2, gravity * deltaTime.asSeconds() * 100));
	m_archimede_rect->setSize(sf::Vector2f(2, archimede * deltaTime.asSeconds() * 100));
	m_resistance_rect->setSize(sf::Vector2f(2, resistance * deltaTime.asSeconds() * 100));
	m_jump_rect->setSize(sf::Vector2f(2, jump * deltaTime.asSeconds() * 100));
	m_net_rect->setSize(sf::Vector2f(2, (-gravity + archimede + resistance + jump)  * deltaTime.asSeconds() * 100));

	m_rect->setPosition(getPosition());
	m_rect_mid->setPosition(sf::Vector2f(getPosition().x, getPosition().y - (MARLIN_DENSITY - 1) * MARLIN_SIZE_Y * 0.5));
	

	m_gravity_rect->setPosition(getPosition());
	m_archimede_rect->setPosition(getPosition());
	m_archimede_rect->setOrigin(sf::Vector2f(0, m_archimede_rect->getSize().y));
	m_resistance_rect->setPosition(sf::Vector2f(getPosition().x - 10, getPosition().y));
	m_resistance_rect->setOrigin(sf::Vector2f(0, m_resistance_rect->getSize().y));
	m_jump_rect->setPosition(sf::Vector2f(getPosition().x + 10, getPosition().y));
	m_jump_rect->setOrigin(sf::Vector2f(0, m_jump_rect->getSize().y));
	m_net_rect->setPosition(sf::Vector2f(getPosition().x + 20, getPosition().y));
	m_net_rect->setOrigin(sf::Vector2f(0, m_net_rect->getSize().y));

	//PlayStroboscopicEffect(sf::seconds(3), sf::seconds(0.05));

	//display/hide debug displays
	if (m_inputs_states[Action_Debug] == Input_Tap)
	{
		m_display_debug_rect = !m_display_debug_rect;

		if (m_display_debug_rect == false)
		{
			m_gravity_rect->setFillColor(sf::Color::Transparent);
			m_archimede_rect->setFillColor(sf::Color::Transparent);
			m_resistance_rect->setFillColor(sf::Color::Transparent);
			m_jump_rect->setFillColor(sf::Color::Transparent);
			m_net_rect->setFillColor(sf::Color::Transparent);

			m_rect->setOutlineColor(sf::Color::Transparent);
			m_rect_mid->setOutlineColor(sf::Color::Transparent);
		}
		else
		{
			m_gravity_rect->setFillColor(sf::Color::Magenta);
			m_archimede_rect->setFillColor(sf::Color::Cyan);
			m_resistance_rect->setFillColor(sf::Color::Black);
			m_jump_rect->setFillColor(sf::Color::Green);
			m_net_rect->setFillColor(sf::Color::Red);

			m_rect->setOutlineColor(sf::Color::Red);
			m_rect_mid->setOutlineColor(sf::Color::Red);
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
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		m_stroboscopic_effect_clock.restart();
	}
}

void Ship::UpdateInputStates()
{
	if ((*CurrentGame).m_window_has_focus)
	{
		GetInputState(InputGuy::isFiring(), Action_Jumping);
		GetInputState(InputGuy::isUsingDebugCommand(), Action_Debug);
		
	}
	else
	{
		GetInputState(false, Action_Jumping);
		GetInputState(false, Action_Debug);
	}
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
	printf("Saving game in local file.\n");
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