#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"

#define SHIP_FILENAME				"Assets/2D/player_sprite.png"
#define SHIP_START_X                0
#define SHIP_START_Y                0
#define SHIP_ACCELERATION_X         10.0f
#define SHIP_ACCELERATION_Y         10.0f
#define SHIP_DECCELERATION_COEF		200.0f
#define SHIP_MAX_SPEED_X            1000.0f
#define SHIP_MAX_SPEED_Y            1000.0f
#define SHIP_MIN_SPEED_X            50.0f
#define SHIP_MIN_SPEED_Y            50.0f
#define SHIP_SPRITE_RATE_SEC        0.2f

class Ship : public sf::Drawable
{
public :
	typedef AnimatedSprite super;
	Ship();
	void Init(int x, int y);
	void Update(sf::Time deltaTime);

private:
	sf::Vector2f speed;
	bool moving;
	Animation* currentAnimation;
	Animation animation;
	AnimatedSprite animatedSprite;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(animatedSprite, states);
	}
};


#endif // SHIP_H_INCLUDED
