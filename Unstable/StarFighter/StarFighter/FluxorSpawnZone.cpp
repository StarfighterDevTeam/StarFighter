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
		Fluxor::CreateFluxor(FluxorType_Green, true, spawn_bounds);
	}
}

FluxorSpawnZone::~FluxorSpawnZone()
{
	
}