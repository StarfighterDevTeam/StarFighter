#ifndef FX_H_INCLUDED
#define FX_H_INCLUDED

#include "Game.h"

enum FXType
{
	FX_Hit,
	FX_Death,
};

class FX : public GameObject
{
public:
	FX(FXType FX_type, sf::Vector2f position);
	~FX();

	void Update(sf::Time deltaTime) override;
};

#endif // FX_H_INCLUDED
