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
	m_is_asking_SFPanel = SFPanel_Specific;

//#define DEBUG_FAST_SPAWN
	int nb_predator = 2;
	int nb_boid = 50;
#ifdef DEBUG_FAST_SPAWN
	{
		for (int i = 0; i < nb_predator; i++)
		{
			sf::Vector2f predator_pos = sf::Vector2f(RandomizeFloatBetweenValues(sf::Vector2f(200, 1800)), RandomizeFloatBetweenValues(sf::Vector2f(200, 800)));
			Predator* predator = new Predator(predator_pos, "2D/boid.png", sf::Vector2f(32, 32), sf::Vector2f(16, 16));
			(*CurrentGame).addToScene(predator, PredatorLayer, PredatorObject);
		}

		for (int i = 0; i < nb_boid; i++)
		{
			sf::Vector2f boid_pos = sf::Vector2f(RandomizeFloatBetweenValues(sf::Vector2f(200, 1800)), RandomizeFloatBetweenValues(sf::Vector2f(200, 800)));
			Boid* boid = new Boid(boid_pos, "2D/boid.png", sf::Vector2f(32, 32), sf::Vector2f(16, 16));
			(*CurrentGame).addToScene(boid, BoidLayer, BoidObject);
		}
	}
#endif
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
	sf::Vector2f inputs_direction = InputGuy::getDirections();

	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	ManageAcceleration(inputs_direction);
	
	//Action input
	UpdateInputStates();
	if (m_inputs_states[Action_Firing] == Input_Tap)
	{
		//do some action
		//(*CurrentGame).CreateSFTextPop("action", Font_Arial, 20, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y/2 - 20);
	}


	//Spawning living things
	{
		sf::Vector2i mousepos_i = sf::Mouse::getPosition(*(*CurrentGame).getMainWindow());
		sf::Vector2f mousepos = (*CurrentGame).getMainWindow()->mapPixelToCoords(mousepos_i, (*CurrentGame).m_view);
		if (mousepos.x > 0 && mousepos.x < REF_WINDOW_RESOLUTION_X && mousepos.y > 0 && mousepos.y < REF_WINDOW_RESOLUTION_Y)
		{
			LivingThing* living = NULL;

			if (m_inputs_states[Action_SpawnBoid] == Input_Tap)
				living = Ship::CreateLivingThing(Living_Boid, mousepos);

			if (m_inputs_states[Action_SpawnPredator] == Input_Tap)
				living = Ship::CreateLivingThing(Living_Predator, mousepos);

			if (m_inputs_states[Action_SpawnPlancton] == Input_Tap)
				living = Ship::CreateLivingThing(Living_Plancton, mousepos);

			if (living != NULL)
				(*CurrentGame).addToScene(living, living->m_layer, living->m_collider_type);

			if (m_inputs_states[Action_Flee] == Input_Tap)
			{
				(*CurrentGame).AddVirtualThreat(mousepos);

				//FX
				FX* fx = new FX(mousepos, sf::Vector2f(0, 0), "2D/FX_VirtualThreat.png", sf::Vector2f(147.f, 147.f), 30, 1);
				(*CurrentGame).addToScene(fx, ExplosionLayer, Neutral);
			}
		}	
	}

	MaxSpeedConstraints();
	IdleDecelleration(deltaTime);
	UpdateRotation();

	GameObject::update(deltaTime);

	//HUD
	//m_is_asking_SFPanel = SFPanel_None;
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
	NormalizeSpeed(&m_speed, ship_max_speed);
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
	GetInputState(InputGuy::isSpawningBoid(), Action_SpawnBoid);
	GetInputState(InputGuy::isSpawningPredator(), Action_SpawnPredator);
	GetInputState(InputGuy::isSpawningPlancton(), Action_SpawnPlancton);
	GetInputState(InputGuy::isFleeing(), Action_Flee);
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

LivingThing* Ship::CreateLivingThing(LivingThingType type, sf::Vector2f position)
{
	LivingThing* living = NULL;

	switch (type)
	{
		case Living_Boid:
		{
			if ((*CurrentGame).m_living_count[type] < BOID_MAX_POPULATION)
				living = new Boid(position);
			break;
		}
		case Living_Predator:
		{
			if ((*CurrentGame).m_living_count[type] < PREDATOR_MAX_POPULATION)
				living = new Predator(position);
			break;
		}
		case Living_Plancton:
		{
			if ((*CurrentGame).m_living_count[type] < PLANCTON_MAX_POPULATION)
				living = new Plancton(position);
			break;
		}
	}

	return living;
}