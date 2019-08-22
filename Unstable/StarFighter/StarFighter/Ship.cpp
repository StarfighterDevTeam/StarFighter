#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Ship::Ship()
{
	m_star_sector_index = sf::Vector2i(0, 0);
}

void Ship::Init()
{
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

	//Star Hunter
	SetStarSectorIndex((*CurrentGame).m_current_star_sector.m_index);
	setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));
	m_heading = 0;
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

void Ship::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void Ship::Update(sf::Time deltaTime)
{
	sf::Vector2f inputs_direction = sf::Vector2f(0, 0);
	if ((*CurrentGame).m_window_has_focus)
	{
		inputs_direction = InputGuy::getDirections();

		if (InputGuy::isSpeeding() == true)
			inputs_direction.y = -1;
		else if (InputGuy::isBraking() == true)
			inputs_direction.y = 1;
		else if (sf::Joystick::isConnected(0) == true)
			inputs_direction.y = 0;
	}

	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	//m_speed.x = inputs_direction.x * SHIP_MAX_SPEED;
	//m_speed.y = inputs_direction.y * SHIP_MAX_SPEED;

	m_acceleration = 0;
	static float max_acceleration = 2000;
	if (inputs_direction.y < 0)
		m_acceleration = max_acceleration * inputs_direction.y;
	
	static float turn_speed = 100;
	m_heading += inputs_direction.x * turn_speed * deltaTime.asSeconds();

	sf::Vector2f acceleration_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_acceleration, m_heading * M_PI / 180);

	static float max_braking = 3000;
	static float idle_decelleration = 1000;
	sf::Vector2f braking_vector = sf::Vector2f(0, 0);
	float current_inertia_angle = GetAngleRadForVector(m_speed);

	if (inputs_direction.y > 0)
		braking_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(max_braking, current_inertia_angle);
	else if (inputs_direction.y == 0)
		braking_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(idle_decelleration, current_inertia_angle);

	braking_vector.x = abs(m_speed.x) > abs(braking_vector.x) ? braking_vector.x : -m_speed.x;//braking cannot exceed speed (that would make us go backward)
	braking_vector.y = abs(m_speed.y) > abs(braking_vector.y) ? braking_vector.y : -m_speed.y;

	m_speed += (acceleration_vector + braking_vector) * deltaTime.asSeconds();
	static float max_speed = 800;
	NormalizeVector(&m_speed, max_speed);

	setRotation(m_heading);
	
	////Action input
	//UpdateInputStates();
	//if (m_inputs_states[Action_Speeding] == Input_Tap)
	//{
	//	//do some action
	//	//(*CurrentGame).CreateSFTextPop("action", Font_Arial, 20, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y/2 - 20);
	//}
	//

	//UpdateRotation();

	GameObject::Update(deltaTime);

	//HUD
	//m_is_asking_SFPanel = SFPanel_None;
	//if (m_SFTargetPanel)
	//{
	//	m_SFTargetPanel->Update(deltaTime);
	//}
	//printf("Sector: %d, %d, position: %f, %f\n", m_star_sector_index.x, m_star_sector_index.y, m_position.x, m_position.y);
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
		GetInputState(InputGuy::isSpeeding(), Action_Speeding);
		GetInputState(InputGuy::isBraking(), Action_Braking);
	}
	else
	{
		GetInputState(false, Action_Speeding);
		GetInputState(false, Action_Braking);
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