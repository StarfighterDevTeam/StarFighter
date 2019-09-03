#ifndef PLANET_H_INCLUDED
#define PLANET_H_INCLUDED

#include "Marker.h"

class Mission;

#define NB_PLANET_TYPES		18

class Planet : public SpatialObject
{
public :
	Planet(sf::Vector2i sector_index, Hostility hostility, int nb_missions);
	virtual ~Planet();
	void SetHostility(Hostility hostility) override;
	void Draw(RenderTarget& screen) override;
	void SetPosition(sf::Vector2f position) override;

	float m_gravity_range;
	float m_gravity_period;
	CircleShape m_orbit_circle;

	int m_nb_missions;
	vector<Mission*> m_missions;
	int m_planet_id;
};

#endif // PLANET_H_INCLUDED
