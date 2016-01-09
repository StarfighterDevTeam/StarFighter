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
		m_respawn_timers.insert(std::pair<size_t, float> (i, -1));//negative value = not to be respawned
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

	//looking for dead Fluxors
	size_t FluxorsVectorSize = m_fluxors.size();
	for (size_t i = 0; i < FluxorsVectorSize; i++)
	{
		if (m_fluxors[i])
		{
			size_t FluxorsGameVectorSize = (*CurrentGame).GetSceneGameObjectsTyped(FluxorUnguidedObject).size();
			bool element_found = false;
			for (size_t j = 0; j < FluxorsGameVectorSize; j++)
			{
				if (m_fluxors[i] == (Fluxor*)(*CurrentGame).GetSceneGameObjectsTyped(FluxorUnguidedObject)[j])
				{
					element_found = true;
					break;
				}
			}
			if (!element_found)
			{
				m_fluxors[i] = NULL;
			}
		}
	}

	//respawn dead Fluxors
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
			}
			else if (abs(curTime - m_respawn_timers[i]) < 1)//not perfect but will do with a 1-sec accuracy
			{
				m_fluxors[i] = Fluxor::CreateFluxor(FluxorType_Green, true, m_spawn_bounds);
				printf("spawn\n");
				m_respawn_timers[i] = -1;
			}
		}
	}
}

FluxorSpawnZone::~FluxorSpawnZone()
{
	
}