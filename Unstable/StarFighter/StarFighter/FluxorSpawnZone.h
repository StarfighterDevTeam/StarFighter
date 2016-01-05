#ifndef FLUXORSPAWNZONE_H_INCLUDED
#define FLUXORSPAWNZONE_H_INCLUDED

#include "Fluxor.h"

class FluxorSpawnZone
{
public :
	FluxorSpawnZone();
	FluxorSpawnZone(sf::FloatRect spawn_bounds, unsigned int population);
	virtual ~FluxorSpawnZone();
	void update(sf::Time deltaTime);

	vector<Fluxor*> m_fluxors;
	sf::FloatRect m_spawn_bounds;
	unsigned int m_population;
	map<size_t, float> m_respawn_timers;
	sf::Clock m_respawn_clock;

};

#endif // FLUXORSPAWNZONE_H_INCLUDED
