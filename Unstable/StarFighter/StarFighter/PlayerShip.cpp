#include "PlayerShip.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------PLAYER SHIP ---------------
PlayerShip::PlayerShip()
{
	
}

void PlayerShip::Init()
{
	m_layer = PlayerShipLayer;
	m_collider_type = PlayerShipObject;
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

	//PIRATE specific
	m_curForwardSpeed = 10.f;
	m_targetForwardSpeed = m_curForwardSpeed;
	m_turnSpeed = 0.1f;
	m_targetHeadingDegrees = 180.f;
}

PlayerShip::PlayerShip(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2u size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	this->Init();
}

PlayerShip::PlayerShip(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2u size) : GameObject(position, speed, textureName, size)
{
	this->Init();
}

PlayerShip::~PlayerShip()
{
	
}

void PlayerShip::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void PlayerShip::update(const float DTIME)
{
	sf::Vector2f inputs_direction = sf::Vector2f(0, 0);
	if ((*CurrentGame).m_window_has_focus)
	{
		inputs_direction = InputGuy::getDirections();
	}

	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	//Ship heading
	float deltaHeading = ComputeDeltaAngleInDegrees(m_curHeadingDegrees, m_targetHeadingDegrees);
	float deltaHeadingClamped = Bound(deltaHeading, -DTIME * m_turnSpeed * GAME_SPEED, DTIME * m_turnSpeed * GAME_SPEED);
	if (m_turnSpeed != 0)
		m_curHeadingDegrees += deltaHeadingClamped;
	BoundAngle(m_curHeadingDegrees, 360.f);
	setRotation(m_curHeadingDegrees);

	//Ship speed
	m_speed.x = DTIME * sin(m_curHeadingDegrees * M_PI_FLT / 180.f) * m_curForwardSpeed * GAME_SPEED;
	m_speed.y = DTIME * -cos(m_curHeadingDegrees * M_PI_FLT / 180.f) * m_curForwardSpeed * GAME_SPEED;
	
	//Action input
	UpdateInputStates();
	if (m_inputs_states[Action_Firing] == Input_Tap)
	{
		//do some action
		(*CurrentGame).CreateSFTextPop("action", Font_Arial, 20, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y/2 - 20);
	}

	GameObject::update(DTIME);

	//HUD
	m_is_asking_SFPanel = SFPanel_None;
	if (m_SFTargetPanel)
	{
		m_SFTargetPanel->Update(DTIME);
	}
}

void PlayerShip::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		m_stroboscopic_effect_clock.restart();
	}
}

void PlayerShip::UpdateInputStates()
{
	if ((*CurrentGame).m_window_has_focus)
	{
		GetInputState(InputGuy::isFiring(), Action_Firing);
	}
	else
	{
		GetInputState(false, Action_Firing);
	}
}

bool PlayerShip::UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition)
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

void PlayerShip::GetInputState(bool input_guy_boolean, PlayerActions action)
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
int PlayerShip::SaveShip(Ship* ship)
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

bool PlayerShip::LoadShip(Ship* ship)
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