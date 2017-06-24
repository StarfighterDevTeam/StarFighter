#include "Lane.h"

extern Game* CurrentGame;

using namespace sf;

Lane::Lane(string csv_file, float bpm) : GameObject(sf::Vector2f(990, 400), sf::Vector2f(0, 0), "2D/lane.png", sf::Vector2f(400, 1200))
{
	m_csv_file = csv_file;
	m_bpm = bpm;
	m_initial_position = getPosition();
	m_first_period = true;

	LOGGER_WRITE(Logger::DEBUG, "Loading lane scripts");

	vector<vector<string> > laneConfig = *(FileLoaderUtils::FileLoader(csv_file));
	size_t laneConfigVectorSize = laneConfig.size();
	for (size_t i = 0; i < laneConfigVectorSize; i++)
	{
		LaneData new_data;
		new_data.m_width = stof(laneConfig[i][LaneData_Width]);
		new_data.m_offset = stof(laneConfig[i][LaneData_OffsetX]);
		for (size_t j = 0; j < 4; j++)
		{
			new_data.m_sardine[j] = stoi(laneConfig[i][LaneData_Sardine_1_4 + j]);
		}
		
		//new_data.m_counter = stoi(laneConfig[i][LaneData_Counter]);
		m_lane_data.push_back(new_data);
	}
	laneConfig.clear();

	m_lane_width = m_size.x;
	m_lane_offset = 0.f;
	m_offset_moved = 0.f;

	m_offset_delta = 0.f;
	m_width_delta = 0.f;

	for (size_t i = 0; i < 4; i++)
	{
		m_sardine[i] = 0;
	}

	m_period_clock.restart();
	m_change_clock.restart();

	CreateNextLanePeriod();
}

bool Lane::CreateNextLanePeriod()
{
	if (!m_lane_data.empty())
	{
		if (m_first_period)
		{
			for (size_t i = 0; i < 4; i++)
			{
				m_sardine[i] = m_lane_data.front().m_sardine[i];//always load sardines one lane period in advance, so they can be spawned upfront
			}
			m_first_period = false;

			return true;
		}
		else
		{
			float new_width = m_lane_data.front().m_width;
			float new_offset = m_lane_data.front().m_offset;

			if (m_lane_data.size() > 1)
			{
				for (size_t i = 0; i < 4; i++)
				{
					m_sardine[i] = m_lane_data[1].m_sardine[i];//always load sardines one lane period in advance, so they can be spawned upfront
				}
			}

			m_lane_data.erase(m_lane_data.begin());

			m_width_delta = new_width - m_lane_width;
			m_offset_delta = new_offset - m_lane_offset;

			m_change_clock.restart();

			printf("Build new lane period: width:%f, offsetX: %f\n", new_width, new_offset);

			return true;
		}
		
	}
	else
	{
		return false;
	}
}

void Lane::CreateSardine(SardineType type)
{
	Sardine* new_sardine = new Sardine(type, this);

	//float next_offset_delta = m_lane_data.front().m_offset - m_lane_offset;
	//new_sardine->setPosition(sf::Vector2f(getPosition().x + next_offset_delta, SWORDFISH_INITIAL_OFFSET_Y - 4.f * 4.f * m_bpm));//spawning in the future X position of the lane
	
	new_sardine->setPosition(sf::Vector2f(getPosition().x, SWORDFISH_INITIAL_OFFSET_Y - 2.f * 4.f * m_bpm));//spawning in the future X position of the lane
	new_sardine->m_speed.y = 4.f*m_bpm;

	(*CurrentGame).addToScene(new_sardine, SardineLayer, SardineObject);
}

void Lane::update(sf::Time deltaTime)
{
	if (m_period_clock.getElapsedTime().asSeconds() > 4.0f * 60.f / m_bpm) // LANE_PERIOD_IN_SECONDS)
	{
		if (CreateNextLanePeriod())
		{
			m_period_clock.restart();
		}
	}
	
	//Spawn sardines
	for (size_t i = 0; i < 4; i++)
	{
		if (m_period_clock.getElapsedTime().asSeconds() > 1.0f * i * 60.f / m_bpm && m_sardine[i] > 0)
		{
			CreateSardine(SardineType_Tap);
			m_sardine[i] = 0;
			printf("Create Sardine %d\n", i+1);
		}
	}

	//Interpolation to new values of offset and width
	if (m_change_clock.getElapsedTime().asSeconds() < 4.0f * 60.f / m_bpm) //LANE_CHANGE_IN_SECONDS)
	{
		ScaleLane(m_lane_width + m_width_delta*deltaTime.asSeconds() / 1.0f * 60.f / m_bpm); //LANE_CHANGE_IN_SECONDS);
		m_lane_offset += m_offset_delta*deltaTime.asSeconds() / 1.0f * 60.f / m_bpm; //LANE_CHANGE_IN_SECONDS;
		//printf("clock: %f\n", m_change_clock.getElapsedTime().asSeconds());
	}
	else
	{
		m_width_delta = 0.f;
		m_offset_delta = 0.f;
	}

	//float offset;
	//offset = offset < 0 ? ceil(offset) : floor(offset);

	float old_position_x = getPosition().x;
	setPosition(sf::Vector2f(m_initial_position.x + m_lane_offset, m_initial_position.y));
	m_offset_moved = old_position_x - getPosition().x;

	AnimatedSprite::update(deltaTime);
}

void Lane::ScaleLane(float width)
{
	if (width == m_lane_width)
		return;

	sf::Vector2f factor = sf::Vector2f(width / m_lane_width, 1);
	scale(factor);
	m_lane_width = width;
}