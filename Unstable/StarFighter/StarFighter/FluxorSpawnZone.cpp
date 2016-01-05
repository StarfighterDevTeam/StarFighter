#include "FluxorSpawnZone.h"

extern Game* CurrentGame;

using namespace sf;

FluxorSpawnZone::FluxorSpawnZone()
{
	
}

FluxorSpawnZone::FluxorSpawnZone(sf::FloatRect spawn_bounds, unsigned int population)
{
	m_spawn_bounds = spawn_bounds;
	m_population = population;

	for (size_t i = 0; i < population; i++)
	{
		Fluxor* fluxor = Fluxor::CreateFluxor(FluxorType_Green, true, spawn_bounds);
		m_fluxors.push_back(fluxor);
		m_respawn_timers.insert(std::pair<size_t, float> (i, -1));
	}
}

void FluxorSpawnZone::update(sf::Time deltaTime)
{
	//respawn clock loop
	float curTime = m_respawn_clock.getElapsedTime().asSeconds();
	if (curTime  > FLUXOR_RESPAWN_MAX_TIME)
	{
		m_respawn_clock.restart();
	}

	//marking Fluxors to respawn
	for (size_t i = 0; i < m_population; i++)
	{
		if (m_fluxors[i] == NULL)
		{
			//timer not yet randomized?
			if (m_respawn_timers[i] < 0)
			{
				//randomize it
				float randomTime = RandomizeFloatBetweenValues(sf::Vector2f(FLUXOR_RESPAWN_MIN_TIME, FLUXOR_RESPAWN_MAX_TIME));
				randomTime = fmod((curTime + randomTime), FLUXOR_RESPAWN_MAX_TIME);

				//insert new timer
				m_respawn_timers[i] = randomTime;
				printf("timer added %f / cur chrono %f (id %d)\n", randomTime, curTime, i);
			}
			else if (abs(curTime - m_respawn_timers[i]) < 1)//not perfect but will do
			{
				m_fluxors[i] = Fluxor::CreateFluxor(FluxorType_Green, true, m_spawn_bounds);
				m_respawn_timers[i] = -1;
				
				printf("fluxor respawned %f (id %d)\n", curTime, i);
			}
		}
	}
}

FluxorSpawnZone::~FluxorSpawnZone()
{
	
}