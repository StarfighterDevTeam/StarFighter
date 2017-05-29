#include "Lane.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Lane::Lane(GameObject* spawner, string csv_file) : GameObject(sf::Vector2f(990, 740), sf::Vector2f(0, 0), "2D/lane.png", sf::Vector2f(400, 32))
								//GameObject(sf::Vector2f(990, 740), sf::Vector2f(0, 0), sf::Color::Blue, sf::Vector2f(400, 32))
{
	m_spawner = spawner;
	m_lane_angle = 0.f;
	m_lane_width = m_size.x;
	m_position_delta = 0.f;
	m_angle_delta = 0.f;

	m_csv_file = csv_file;
	LOGGER_WRITE(Logger::DEBUG, "Loading lane scripts");

	vector<vector<string> > laneConfig = *(FileLoaderUtils::FileLoader(csv_file));
	size_t laneConfigVectorSize = laneConfig.size();
	for (size_t i = 0; i < laneConfigVectorSize; i++)
	{
		LaneData new_data;
		new_data.m_width = stof(laneConfig[i][LaneData_Width]);
		new_data.m_angle = stof(laneConfig[i][LaneData_Angle]);
		new_data.m_counter = stoi(laneConfig[i][LaneData_Counter]);
		m_lane_data.push_back(new_data);
	}
	laneConfig.clear();

	m_period_clock.restart();
	BuildNextLanePeriod();
	
}

bool Lane::BuildNextLanePeriod()
{
	if (!m_lane_data.empty())
	{
		float new_angle = m_lane_data.front().m_angle;
		float new_width = m_lane_data.front().m_width;
		m_period_counter = m_lane_data.front().m_counter;
		
		m_lane_data.erase(m_lane_data.begin());

		RotateLane(new_angle - m_lane_angle);
		ScaleLane(new_width);

		return true;
	}
	else
	{
		return false;
	}
}

void Lane::update(sf::Time deltaTime)
{
	m_angle_delta = 0.f;
	m_position_delta = 0.f;

	if (m_period_clock.getElapsedTime().asSeconds() > LANE_PERIOD_IN_SECONDS)
	{
		m_period_counter--;
		if (m_period_counter == 0)
		{
			if (BuildNextLanePeriod())
			{
				m_period_clock.restart();
			}
		}
		else
		{
			m_period_clock.restart();
		}
	}

	//RotateLane(10*deltaTime.asSeconds());
	//ScaleLane(600);

	//Apply new width and angle
	sf::Vector2f default_pos = sf::Vector2f(m_spawner->getPosition().x, m_spawner->getPosition().y );
	sf::Vector2f offset;
	float rad_angle = -m_lane_angle * M_PI / 180.f;
	offset.x = LANE_OFFSET_Z * sin(rad_angle);
	offset.y = LANE_OFFSET_Z * cos(rad_angle);
	offset.x = offset.x < 0 ? ceil(offset.x) : floor(offset.x);
	offset.y = offset.y < 0 ? ceil(offset.y) : floor(offset.y);

	sf::Vector2f old_position = getPosition();

	setPosition(sf::Vector2f(default_pos.x + offset.x, default_pos.y + offset.y));
	

	//Stock delta of position and angle in memory for the swordfish to know about them during its update
	float distance_moved = sqrt((old_position.x - getPosition().x)*(old_position.x - getPosition().x) + (old_position.y - getPosition().y)*(old_position.y - getPosition().y));
	if (m_angle_delta > 0)
	{
		m_position_delta = distance_moved;
	}
	else if (m_angle_delta < 0)
	{
		m_position_delta = -distance_moved;
	}

	AnimatedSprite::update(deltaTime);
}

void Lane::RotateLane(float deg_angle)
{
	m_lane_angle += deg_angle;
	m_angle_delta = deg_angle;
	setRotation(m_lane_angle);

	printf("rotate : %f, new angle : %f\n", deg_angle, m_lane_angle);
}

void Lane::ScaleLane(float width)
{
	if (width == m_lane_width)
		return;

	sf::Vector2f factor = sf::Vector2f(width / m_lane_width, 1);
	scale(factor);
	m_lane_width = width;
}