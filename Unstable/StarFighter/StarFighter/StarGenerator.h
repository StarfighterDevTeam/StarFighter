#ifndef STARGENERATOR_H_INCLUDED
#define STARGENERATOR_H_INCLUDED

#include "Star.h"

class StarGenerator
{
public :
	StarGenerator();
	void Update(sf::Time deltaTime);
	static Star* CreateStar();
	static Star* CreateStar(sf::Vector2i sector_index);

	float m_generator_clock;
	float m_generator_threshold;
};

#endif // STARGENERATOR_H_INCLUDED
