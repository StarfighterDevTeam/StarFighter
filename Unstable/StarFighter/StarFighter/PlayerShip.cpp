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
	m_DontGarbageMe = true;

	//PIRATE specific
	m_curForwardSpeed = 30.f;
	m_targetForwardSpeed = m_curForwardSpeed;
	m_turnSpeed = 0.8f;
	m_targetHeadingDegrees = 135.f;
	m_maxSailRange = 400.f;
	m_maxVisionRange = 500.f;

	setPosition(MapTile::MapTileCoordToPosition(sf::Vector2u(10, 10)));

	//range feedbacks
	m_sailRangeFeedback.setFillColor(sf::Color(0, 0, 0, 0));
	m_sailRangeFeedback.setOutlineColor(sf::Color::Cyan);
	m_sailRangeFeedback.setOutlineThickness(2.f);
	(*CurrentGame).addToFeedbacks(&m_sailRangeFeedback);

	m_visionRangeFeedback.setFillColor(sf::Color(0, 0, 0, 0));
	m_visionRangeFeedback.setOutlineColor(sf::Color::Magenta);
	m_visionRangeFeedback.setOutlineThickness(2.f);
	(*CurrentGame).addToFeedbacks(&m_visionRangeFeedback);

	//debug
	SFText* tileText = new SFText((*CurrentGame).m_font[Font_Arial], 24, sf::Color(Color::White), getPosition(), PlayerTeams::PlayerBlue);
	(*CurrentGame).addToFeedbacks(tileText);
	m_textCurTileDebug = tileText;
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
	if ((*CurrentGame).m_windowHasFocus)
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

	//Apply speed and rotation to move the object to its new position
	GameObject::update(DTIME);

	//Map borders constraints
	if (getPosition().x < 0)
		setPosition(sf::Vector2f(0, getPosition().y));
	if (getPosition().x >= (*CurrentGame).m_mapSize.x)
		setPosition(sf::Vector2f((*CurrentGame).m_mapSize.x - 1, getPosition().y));
	if (getPosition().y < 0)
		setPosition(sf::Vector2f(getPosition().x, 0));
	if (getPosition().y >= (*CurrentGame).m_mapSize.y)
		setPosition(sf::Vector2f(getPosition().x, (*CurrentGame).m_mapSize.y - 1));

	//Current tile
	if (m_curTile)
		m_curTile->setColor(sf::Color::White);//reset highlight of the previous tile
	m_curTile = MapTile::PositionToMapTile(getPosition());//compute current tile from position
	m_curTile->setColor(sf::Color::Green);//highlight the new current tile

	if (m_textCurTileDebug)
	{
		m_textCurTileDebug->setString(to_string(m_curTile->m_coord_x) + " ; " + to_string(m_curTile->m_coord_y));
		m_textCurTileDebug->setPosition(sf::Vector2f(getPosition().x - 18.f, getPosition().y - 1.f * MAP_TILE_SIZE));
	}

	//Mouse tile
	sf::Vector2f clampedMousePos = (*CurrentGame).m_mousePos;//clamp mouse position to windows borders
	clampedMousePos.x = Bound((*CurrentGame).m_mousePos.x, getPosition().x - REF_WINDOW_RESOLUTION_X * 0.5f + 1, getPosition().x + REF_WINDOW_RESOLUTION_X * 0.5f - 1);
	clampedMousePos.y = Bound((*CurrentGame).m_mousePos.y, getPosition().y - REF_WINDOW_RESOLUTION_Y * 0.5f + 1, getPosition().y + REF_WINDOW_RESOLUTION_Y * 0.5f - 1);
	if (m_mouseTile)
		m_mouseTile->setColor(sf::Color::White);
	m_mouseTile = MapTile::PositionToMapTile(clampedMousePos);
	m_mouseTile->setColor(sf::Color::Red);

	//Range feedbacks
	m_sailRangeFeedback.setRadius(m_maxSailRange);
	m_sailRangeFeedback.setOrigin(sf::Vector2f(m_sailRangeFeedback.getRadius(), m_sailRangeFeedback.getRadius()));
	m_sailRangeFeedback.setPosition(getPosition());

	m_visionRangeFeedback.setRadius(m_maxVisionRange);
	m_visionRangeFeedback.setOrigin(sf::Vector2f(m_visionRangeFeedback.getRadius(), m_visionRangeFeedback.getRadius()));
	m_visionRangeFeedback.setPosition(getPosition());

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
	if ((*CurrentGame).m_windowHasFocus)
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