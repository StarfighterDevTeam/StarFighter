#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Ship::Ship()
{
	
}

#define DASH_RADIUS					300.f
#define DASH_SPEED					2000.f


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

	m_state = Character_Idle;
	m_dash_enemy = NULL;

	m_dash_radius = DASH_RADIUS;
	m_dash_speed = DASH_SPEED;

	//debug
	m_dash_radius_feedback.setRadius(m_dash_radius);
	m_dash_radius_feedback.setOrigin(sf::Vector2f(m_dash_radius, m_dash_radius));
	m_dash_radius_feedback.setFillColor(sf::Color(0, 0, 0, 0));
	m_dash_radius_feedback.setOutlineThickness(1);
	m_dash_radius_feedback.setOutlineColor(sf::Color(255, 255, 255, 30));
	m_dash_radius_feedback.setPosition(getPosition());
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
	m_previous_speed = m_speed;

	sf::Vector2f inputs_direction = InputGuy::getDirections();

	if (!m_disable_inputs)
	{
		m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		m_movingX = inputs_direction.x != 0;
		m_movingY = inputs_direction.y != 0;
	}

	//Action input
	UpdateInputStates();
	
	//DASH
	GameObject* dash_enemy = (*CurrentGame).getDashTarget(m_dash_radius);
	if (m_state == Character_Idle)
	{
		ManageAcceleration(inputs_direction);

		if (m_inputs_states[Action_Firing] == Input_Tap)
		{
			//do some action
			(*CurrentGame).CreateSFTextPop("action", Font_Arial, 20, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y / 2 - 20);
			m_state = Character_Dash;

			float cur_angle = (getRotation() - 180) / 180 * M_PI;
			sf::Vector2f dash_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_dash_radius, cur_angle);

			if (dash_enemy)
			{
				m_dash_enemy = dash_enemy;

				dash_vector = m_dash_enemy->getPosition() - getPosition();

				m_overdash_distance = GetAbsoluteSpeed(dash_vector) * 0.5f;
				dash_vector += dash_vector * 0.5f;
			}

			//dash_vector = sf::Vector2f(dash_vector.x * (1 / ISO_FACTOR_X), dash_vector.y * (1 / ISO_FACTOR_Y));;
			m_dash_target = getPosition() + dash_vector;

			m_speed = dash_vector;
			//m_speed = sf::Vector2f(dash_vector.x * (1 / ISO_FACTOR_X), dash_vector.y * (1 / ISO_FACTOR_Y));
			//m_speed = dash_vector;
 			ScaleSpeed(&m_speed, m_dash_speed);
		}
	}

	if (m_state == Character_Dash)
	{
		sf::Vector2f dash_vector = m_dash_target - getPosition();

		//enemy position update
		if (m_dash_enemy != NULL)
		{
			dash_vector = m_dash_enemy->getPosition() - getPosition();

			sf::Vector2f overdash_vector = dash_vector;
			ScaleSpeed(&overdash_vector, m_overdash_distance);
		
			dash_vector += overdash_vector;
		
			m_dash_target = getPosition() + dash_vector;
		}

		float dist_to_target = GetDistanceBetweenPositions(getPosition(), m_dash_target);
		if (dist_to_target > DASH_SPEED * deltaTime.asSeconds())
		{
			//m_speed = dash_vector;
			//ScaleSpeed(&m_speed, DASH_SPEED);
		}
		else
		{
			m_speed = sf::Vector2f(0, 0);
			setPosition(m_dash_target);
			m_state = Character_Idle;
		}
		
		printf("Speed: %f, %f (angle: %f)\n", m_speed.x, m_speed.y, GetAngleRadBetweenPositions(getPosition(), m_dash_target));
	}

	//MaxSpeedConstraints();
	if (m_state == Character_Idle)
	{
		IdleDecelleration(deltaTime);
	}
	
	UpdateRotation();

	GameObject::update(deltaTime);

	//HUD
	m_is_asking_SFPanel = SFPanel_None;
	if (m_SFTargetPanel)
	{
		m_SFTargetPanel->Update(deltaTime);
	}

	ScreenBorderContraints();

	m_dash_radius_feedback.setPosition(getPosition());
	m_dash_radius_feedback.setRadius(m_dash_radius);
	m_dash_radius_feedback.setOrigin(sf::Vector2f(m_dash_radius, m_dash_radius));
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
		m_speed.x = 0;
	}

	if (!m_movingY)
	{
		m_speed.y = 0;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	m_speed.x += inputs_direction.x* SHIP_ACCELERATION;
	m_speed.y += inputs_direction.y *SHIP_ACCELERATION;

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
	//anti ghost-inputs, helps cleaning movements especially when the character stops
	if (m_speed != m_previous_speed)
	{
		return;
	}

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

void Ship::CollisionWithEnemy(GameObject* enemy)
{
	if (enemy == m_dash_enemy)
	{
		m_dash_enemy = NULL;
	}
}

void Ship::Draw(sf::RenderTexture& screen)
{
	GameObject::Draw(screen);

	if (m_visible)
	{
		screen.draw(m_dash_radius_feedback);
	}
}