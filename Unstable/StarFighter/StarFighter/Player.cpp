#include "Player.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Player::Player(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Ship(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_disable_inputs = false;
	m_controllerType = AllControlDevices;

	for (size_t i = 0; i < NBVAL_PlayerActions; i++)
	{
		m_actions_states[i] = false;
	}

	//Star Hunter
	SetStarSectorIndex((*CurrentGame).m_current_star_sector.m_index);
	setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));

	m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserGreen, PlayerFire, PlayerFireLayer, sf::Vector2f(0, m_size.y * 0.5)));

	//Flight model
	m_speed_max = 800;
	m_max_acceleration = 2000;
	m_turn_speed = 160;
	m_max_braking = 3000;
	m_idle_decelleration = 1000;
}

Player::~Player()
{

}

void Player::Update(sf::Time deltaTime)
{
	sf::Vector2f inputs_direction = sf::Vector2f(0, 0);
	if ((*CurrentGame).m_window_has_focus)
	{
		inputs_direction = InputGuy::getDirections();

		if (InputGuy::isSpeeding() == true)
			inputs_direction.y = -1;
		if (InputGuy::isBraking() == true)
			inputs_direction.y = 1;
		else if (sf::Joystick::isConnected(0) == true && InputGuy::isSpeeding() == false)
			inputs_direction.y = 0;
	}

	ApplyFlightModel(deltaTime, inputs_direction);

	for (Weapon* weapon : m_weapons)
	{
		weapon->Update(deltaTime);

		if (InputGuy::isFiring() == true)
			if (weapon->IsReadyToFire() == true)
				weapon->Fire();
	}

	for (Ship* marked_ship : m_marked_ships)
		marked_ship->m_marker->Update(deltaTime);

	Ship::Update(deltaTime);
}

void Player::Draw(RenderTarget& screen)
{
	GameObject::Draw(screen);

	for (Ship* marked_ships : m_marked_ships)
		if (marked_ships->m_marker->m_visible == true)
			marked_ships->m_marker->Draw(screen);
}

void Player::UpdateInputStates()
{
	if ((*CurrentGame).m_window_has_focus)
	{
		GetInputState(InputGuy::isSpeeding(), Action_Speeding);
		GetInputState(InputGuy::isBraking(), Action_Braking);
		GetInputState(InputGuy::isBraking(), Action_Firing);
	}
	else
	{
		GetInputState(false, Action_Speeding);
		GetInputState(false, Action_Braking);
		GetInputState(false, Action_Firing);
	}
}

bool Player::UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition)
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

void Player::GetInputState(bool input_guy_boolean, PlayerActions action)
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
int Player::SaveShip(Player* ship)
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

bool Player::LoadShip(Player* ship)
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

void Player::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}