#include "Fluxor.h"

extern Game* CurrentGame;

using namespace sf;

void Fluxor::Initialize()
{
	m_guided = false;
	m_docked = false;
	m_wasting_flux = false;
	m_flux = 0;
	m_flux_max = 0;
	m_displaying_flux = false;
	m_transfer_buffer = 0;
	m_team = PlayerNeutral;
	m_target = NULL;
	m_target_memory = false;

	m_consummable_by_players = false;
	m_consummable_by_modules = false;
	m_flux_attacker = false;
	m_flux_attack_piercing = false;
	m_flux_stealer = false;
	m_flux_stolen = 0;
	m_needs_link_to_circulate = false;
	m_fluxovore = false;
	m_can_be_refilled_by_modules = false;

	m_has_spawn_bounds = false;

	//Flux display
	m_flux_text.setFont(*(*CurrentGame).font2);
	m_flux_text.setCharacterSize(20);
	m_flux_text.setColor(sf::Color::White);
	m_flux_text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + m_size.y / 2 + FLUXOR_FLUX_DISPLAY_OFFSET_Y));
}

Fluxor::Fluxor()
{
	Initialize();
}

Fluxor::Fluxor(FluxorType FluxorType)
{
	//texture
	std::string textureName;
	switch (FluxorType)
	{
		case FluxorType_Blue:
		{
			textureName = "Assets/2D/fluxor_blue.png";
			break;
		}
		case FluxorType_Green:
		{
			textureName = "Assets/2D/fluxor_green.png";
			break;
		}
		case FluxorType_Red:
		{
			textureName = "Assets/2D/fluxor_red.png";
			break;
		}
		case FluxorType_Purple:
		{
			textureName = "Assets/2D/fluxor_purple.png";
			break;
		}
		case FluxorType_Black:
		{
			textureName = "Assets/2D/fluxor_black.png";
			break;
		}
		default:
		{
			textureName = "Assets/2D/fluxor_blue.png";
			break;
		}
	}
		
	const unsigned int W = FLUXOR_WIDTH;
	const unsigned int H = FLUXOR_HEIGHT;
		
	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(W, H), 1, 1);
	setOrigin(sf::Vector2f(W / 2, H / 2));

	Initialize();

	m_FluxorType = FluxorType;

	//Type specific parameters
	switch (FluxorType)
	{
		case FluxorType_Green:
		{
			m_displaying_flux = true;
			m_wasting_flux = false;
			m_flux = GREEN_FLUXOR_VALUE;
			m_consummable_by_players = true;
			m_consummable_by_modules = true;
			break;
		}
		case FluxorType_Blue:
		{
			m_displaying_flux = true;
			m_wasting_flux = false;
			m_flux = FLUXOR_FLUX_VALUE;
			m_consummable_by_modules = true;
			m_can_be_refilled_by_modules = true;
			m_needs_link_to_circulate = true;
			setColor(sf::Color(255, 255, 255, GHOST_ALPHA_VALUE));
			break;
		}
		case FluxorType_Red:
		{
			m_displaying_flux = true;
			m_wasting_flux = true;
			m_flux_waste = FLUXOR_WASTE_VALUE;
			m_flux_waste_delay = FLUXOR_WASTE_DELAY;
			m_flux_attack_delay = FLUXOR_ATTACK_DELAY;
			m_flux = 10;
			m_flux_max = 10;
			m_can_be_refilled_by_modules = true;
			m_flux_attacker = true;
			m_flux_attack_piercing = true;
			break;
		}
		case FluxorType_Purple:
		{
			m_displaying_flux = true;
			m_wasting_flux = true;
			m_flux_waste = FLUXOR_WASTE_VALUE;
			m_flux_waste_delay = FLUXOR_WASTE_DELAY;
			m_flux_attack_delay = FLUXOR_ATTACK_DELAY;
			m_flux = 20;
			m_flux_max = 20;
			m_can_be_refilled_by_modules = true;
			m_flux_attacker = true;
			m_flux_stealer = true;
			break;
		}
		case FluxorType_Black:
		{
			m_displaying_flux = true;
			m_flux_attack_delay = FLUXOR_ATTACK_DELAY;
			m_flux = 20;
			m_flux_max = 0;
			m_fluxovore = true;
			break;
		}
	}

	//Generic parameters
	if (m_consummable_by_players)
	{
		m_flux_text.setColor(sf::Color::Green);
	}
	else if (m_consummable_by_modules)
	{
		m_flux_text.setColor(sf::Color::Cyan);
	}
	else if (m_flux_attacker)
	{
		m_flux_text.setColor(sf::Color::Red);
	}
	else if (m_fluxovore)
	{
		m_flux_text.setColor(sf::Color::Black);
	}
}

Fluxor::Fluxor(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Initialize();
}

Fluxor* Fluxor::CreateFluxor(FluxorType FluxorType, bool within_bounds, sf::FloatRect bounds)
{
	Fluxor* new_fluxor = new Fluxor(FluxorType);
	
	new_fluxor->m_has_spawn_bounds = within_bounds;
	if (within_bounds)
	{
		new_fluxor->m_spawn_bounds = bounds;
	}
	new_fluxor->setPosition(RandomizePosition(within_bounds, bounds));
	
	new_fluxor->m_initial_position = new_fluxor->getPosition();
	new_fluxor->m_speed = RandomizeSpeed();
	new_fluxor->m_absolute_speed = GetAbsoluteSpeed(new_fluxor->m_speed);
	new_fluxor->m_turn_delay = RandomizeTurnDelay();

	GameObjectType type = new_fluxor->m_guided ? FluxorGuidedObject : FluxorUnguidedObject;

	(*CurrentGame).addToScene(new_fluxor, FluxorLayer, type);
	if (new_fluxor->m_displaying_flux)
	{
		(*CurrentGame).addToFeedbacks(&new_fluxor->m_flux_text);
	}

	return new_fluxor;
}

Fluxor::~Fluxor()
{
	if (m_displaying_flux)
		(*CurrentGame).removeFromFeedbacks(&m_flux_text);
}

Fluxor* Fluxor::Clone()
{
	Fluxor* clone = new Fluxor(this->getPosition(), this->m_speed, this->m_textureName, this->m_size, sf::Vector2f(this->m_size.x / 2, this->m_size.y / 2), this->m_frameNumber, this->m_animationNumber);
	clone->m_collider_type = this->m_collider_type;
	clone->m_layer = this->m_layer;
	clone->m_FluxorType = this->m_FluxorType;
	clone->m_displaying_flux = this->m_displaying_flux;
	clone->m_flux = this->m_flux;
	clone->m_flux_max = this->m_flux_max;
	clone->m_wasting_flux = this->m_wasting_flux;
	clone->m_flux_waste = this->m_flux_waste;
	clone->m_flux_waste_delay = this->m_flux_waste_delay;

	return clone;
}

void Fluxor::update(sf::Time deltaTime)
{
	if (m_flux > m_flux_max && m_flux_max > 0)
	{
		m_flux = m_flux_max;
	}

	if (!m_docked)
	{
		//flux waste
		WastingFlux();

		//chaos turns
		if (!m_guided)
		{
			if (m_turn_clock.getElapsedTime().asSeconds() > m_turn_delay)
			{
				ChaosTurns();
			}

			if (ScreenBorderContraints())
			{
				m_turn_delay = RandomizeTurnDelay();
				m_turn_clock.restart();
			}
		}

		UpdateRotation();

		GameObject::update(deltaTime);
	}
	else
	{
		m_flux_waste_clock.restart();

		AnimatedSprite::update(deltaTime);

		//reset docked every frame. The collision will be in charge of re-docking the fluxor every frame if necessary.
		m_docked = false;
	}

	//death by flux consumption
	if (m_flux == 0)
	{
		GarbageMe = true;
	}

	//hud
	if (m_displaying_flux)
	{
		ostringstream ss;
		ss << m_flux;
		if (m_flux_max > 0)
			ss << "/" << m_flux_max;
		m_flux_text.setString(ss.str());
		if (m_flux_attacker || m_fluxovore)
		{
			m_flux_text.setPosition(sf::Vector2f(getPosition().x - m_flux_text.getGlobalBounds().width / 2, getPosition().y - m_size.y / 2 - m_flux_text.getGlobalBounds().height - FLUXOR_FLUX_DISPLAY_OFFSET_Y));
		}
		else
		{
			m_flux_text.setPosition(sf::Vector2f(getPosition().x - m_flux_text.getGlobalBounds().width / 2, getPosition().y + m_size.y / 2 + FLUXOR_FLUX_DISPLAY_OFFSET_Y));
		}
	}
}

float Fluxor::RandomizeTurnDelay()
{
	return RandomizeFloatBetweenValues(sf::Vector2f(FLUXOR_TURN_MIN_DELAY, FLUXOR_TURN_MAX_DELAY));
}

sf::Vector2f Fluxor::RandomizePosition(bool within_bounds, sf::FloatRect bounds)
{
	const unsigned int W = FLUXOR_WIDTH;
	const unsigned int H = FLUXOR_HEIGHT;

	sf::Vector2f position;
	bool position_is_valid = false;

	unsigned int number_of_tries = 0;
	while (!position_is_valid && number_of_tries < 100)
	{
		number_of_tries++;
		if (number_of_tries == 100)
		{
			printf("<!> Can't find a valid position in Fluxor::RandomizePosition(bool within_bounds, sf::FloatRect bounds) after %d tries. Spawning Fluxor in a bad spot.\n", number_of_tries);
		}

		position_is_valid = true;
		
		if (within_bounds)
		{
			position.x = RandomizeFloatBetweenValues(sf::Vector2f(bounds.left + W / 2, bounds.left + bounds.width - W / 2));
			position.y = RandomizeFloatBetweenValues(sf::Vector2f(bounds.top + H / 2, bounds.top + bounds.height - H / 2));
		}
		else
		{
			position.x = RandomizeFloatBetweenValues(sf::Vector2f(W / 2, (*CurrentGame).map_size.x - W / 2));
			position.y = RandomizeFloatBetweenValues(sf::Vector2f(H / 2, (*CurrentGame).map_size.y - H / 2));
		}

		//checking if position is valid
		if ((*CurrentGame).GetClosestObject(position, PlayerShip))
		{
			float distance_to_player = GameObject::GetDistanceBetweenPositions(position, (*CurrentGame).GetClosestObject(position, PlayerShip)->getPosition());
			if (distance_to_player < TILE_SIZE)
			{
				position_is_valid = false;
				continue;
			}
		}

		if (!(*CurrentGame).isCellFree(position))
		{
			position_is_valid = false;
			continue;
		}
	}

	return position;
}

sf::Vector2f Fluxor::RandomizeSpeed()
{
	float absolute_speed = RandomizeFloatBetweenValues(sf::Vector2f(FLUXOR_SPEED_MIN, FLUXOR_SPEED_MAX));
	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	sf::Vector2f speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(absolute_speed, angle);

	return speed;
}

void Fluxor::ChaosTurns()
{
	m_turn_delay = RandomizeTurnDelay();
	m_turn_clock.restart();
	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	m_speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_absolute_speed, angle);
}

bool Fluxor::ScreenBorderContraints()
{
	bool touching_screen_border = false;
	if (this->getPosition().x < this->m_size.x / 2)
	{
		this->setPosition(m_size.x / 2, this->getPosition().y);
		m_speed.x *= -1;
		touching_screen_border = true;
	}

	if (this->getPosition().x >(*CurrentGame).map_size.x - (m_size.x / 2))
	{
		this->setPosition((*CurrentGame).map_size.x - (m_size.x / 2), this->getPosition().y);
		m_speed.x *= -1;
		touching_screen_border = true;
	}

	if (this->getPosition().y < m_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_size.y / 2);
		m_speed.y *= -1;
		touching_screen_border = true;
	}

	if (this->getPosition().y >(*CurrentGame).map_size.y - (m_size.y / 2))
	{
		this->setPosition(this->getPosition().x, (*CurrentGame).map_size.y - (m_size.y / 2));
		m_speed.y *= -1;
		touching_screen_border = true;
	}

	return touching_screen_border;
}

void Fluxor::UpdateRotation()
{
	//turning toward targeted position
	if (m_speed.x == 0 && m_speed.y == 0)
	{
		//do nothing
	}
	else if (m_speed.x == 0 && m_speed.y > 0)
	{
		setRotation(90);
	}
	else if (m_speed.x == 0 && m_speed.y < 0)
	{
		setRotation(90);
	}
	else if (m_speed.y == 0 && m_speed.x > 0)
	{
		setRotation(0);
	}
	else if (m_speed.y == 0 && m_speed.x < 0)
	{
		setRotation(270);
	}
	else
	{
		setRotation((GetAngleRadForSpeed(m_speed) * 180 / (float)M_PI) - 90);
	}
}

void Fluxor::WastingFlux()
{
	if (m_wasting_flux && m_flux > 0)
	{
		if (m_flux_waste_clock.getElapsedTime().asSeconds() > m_flux_waste_delay)
		{
			if (m_flux_waste <= m_flux)
			{
				m_flux -= m_flux_waste;
			}
			else
			{
				m_flux = 0;
			}

			m_flux_waste_clock.restart();
		}
	}
}

void Fluxor::GetFluxor(GameObject* object)
{
	if (object)
	{
		Fluxor* fluxor = (Fluxor*)object;

		if (fluxor->m_guided && this->m_guided)
		{
			if (fluxor->m_team != this->m_team && fluxor->m_alliance != this->m_alliance)
			{
				if (m_fluxovore && fluxor->m_flux_attacker)
				{
					AttackFluxor(fluxor);
				}
			}
		}
	}
}

void Fluxor::AttackFluxor(Fluxor* fluxor)
{
	if (fluxor)
	{
		//steal flux?
		if (m_flux_stealer)
		{
			m_flux_stolen = fluxor->m_flux;
		}
		//deal damage
		if (m_flux < fluxor->m_flux)
		{
			fluxor->m_flux -= m_flux;
		}
		else
		{
			fluxor->m_flux = 0;
		}

		if (m_flux_stealer)
		{
			BringStealerBack();
		}
		else
		{
			GarbageMe = true;
		}
	}
}

void Fluxor::BringStealerBack()
{
	m_speed = (sf::Vector2f(m_speed.x *= -1, m_speed.y *= -1));
	m_flux = m_flux_stolen;
	m_flux_stolen = 0;
	m_flux_max = 0;
	m_flux_attacker = false;
	m_wasting_flux = false;
	m_displaying_flux = true;
	m_consummable_by_modules = true;
	m_can_be_refilled_by_modules = false;
	m_flux_text.setColor(sf::Color::Cyan);
}