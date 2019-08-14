#include "Wing.h"

extern Game* CurrentGame;

using namespace sf;

Wing::Wing(sf::Vector2f position, AllianceType alliance, float heading) : L16Entity(position, alliance, 8, Circle_L16Entity_Air)
{
	m_type = L16Entity_Wing;
	
	m_speed_min = 70;
	m_speed_max = 100;
	m_acceleration = 500;
	m_roll_rate_min = 100;
	m_roll_rate_max = 500;
	m_wingspan = (getRadius() * 2) + (getRadius() * 2);
	m_chord_length = getRadius() / 2;
	m_manoeuvrability = 0.06 * (getRadius() * 2 / m_wingspan);//0.03;

	m_heading = heading;
	m_roll = 0;
	m_speed = GetVectorFromLengthAndAngle(m_speed_min, heading * M_PI / 180);
	m_autopilot = false;
	m_wings = new LineObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), m_alliance);

	m_radar_activated = true;
	m_radar_frequency = 0.5;
	m_radar_range = 500;
	m_radar_wavespeed = 200;
	m_radar_heading = m_heading;
	m_radar_coverage = 30;

	m_weapons.push_back(new Weapon(this, Weapon_AAM, Ballistic_AAM));
}

Wing::~Wing()
{
	delete m_wings;
}

void Wing::Draw(RenderTarget& screen)
{
	CircleObject::Draw(screen);
	screen.draw(m_wings->m_quad, 4, sf::Quads);
}

void Wing::update(sf::Time deltaTime)
{
	sf::Vector2f inputs_direction = sf::Vector2f(0, 0);
	m_autopilot = !m_selected;
	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		(*it)->update(deltaTime);
	}

	//manual pilot
	if (m_autopilot == false)
	{
		if ((*CurrentGame).m_window_has_focus == true)
		{
			inputs_direction = InputGuy::getDirections();

			if ((*CurrentGame).m_inputs_states[Action_Firing] == Input_Tap)
			{
				Fire();
			}
		}
	}
	else
	{
		//auto pilot
		if (m_mission == L16Mission_Idle)
		{
			//circle patrol
			inputs_direction.x = 1;
		}
	}

	//apply speed & direction
	float previous_speed = GetVectorLength(m_speed);
	//float roll_speed = Lerp(previous_speed, m_speed_min, m_speed_max, m_roll_rate_max, m_roll_rate_min);
	float roll_speed = CosInterpolation(previous_speed, m_speed_min, m_speed_max, m_roll_rate_max, m_roll_rate_min);
	if (inputs_direction.x < 0)
	{
		m_roll += roll_speed * deltaTime.asSeconds();
	}
	else if (inputs_direction.x > 0)
	{
		m_roll -= roll_speed * deltaTime.asSeconds();
	}
	else
	{
		if (m_roll > roll_speed * deltaTime.asSeconds())
		{
			m_roll -= roll_speed * deltaTime.asSeconds();
		}
		else if (m_roll < -roll_speed * deltaTime.asSeconds())
		{
			m_roll += roll_speed * deltaTime.asSeconds();
		}
		else
		{
			m_roll = 0;
		}
	}

	Bound(m_roll, -90, 90);
	if (inputs_direction != sf::Vector2f(0, 0))
	{
		m_heading += m_roll * m_manoeuvrability;
	}
	
	BoundAngle(m_heading, 360);

	sf::Vector2f acceleration_vector = GetVectorFromLengthAndAngle(m_acceleration * deltaTime.asSeconds(), m_heading * M_PI / 180);
	m_speed += acceleration_vector;

	//float speed_max = Lerp(-abs(m_roll), -90, 0, m_speed_min, m_speed_max);
	float speed_max = CosInterpolation(abs(m_roll), 0, 90, m_speed_max, m_speed_min);
	NormalizeVector(&m_speed, speed_max);

	//printf("roll: %f, roll speed: %f, heading: %f, speed: %f\n", m_roll, roll_speed, m_heading, GetVectorLength(m_speed));

	m_radar_heading = m_heading;

	L16Entity::update(deltaTime);

	UpdateWingsPositionToNewHeading();
}

void Wing::UpdateWingsPositionToNewHeading()
{
	float perpAngle = (m_heading + 90) * M_PI / 180;

	float wingspan = Lerp(-abs(m_roll), -90, 0, (getRadius() * 2) + (m_chord_length * 2), m_wingspan);
	float xL = getPosition().x + cos(perpAngle) * wingspan / 2;
	float yL = getPosition().y - sin(perpAngle) * wingspan / 2;

	float xR = getPosition().x - cos(perpAngle) * wingspan / 2;
	float yR = getPosition().y + sin(perpAngle) * wingspan / 2;

	m_wings->m_points[0].position = sf::Vector2f(xL, yL);
	m_wings->m_points[1].position = sf::Vector2f(xR, yR);

	m_wings->UpdateQuadPointsPosition();
}

void Wing::Fire()
{
	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		if ((*it)->IsReadyToFire() == true)
		{
			(*it)->Fire();
		}
	}
}