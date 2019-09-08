#include "FX.h"

extern Game* CurrentGame;

using namespace sf;

FX::FX(FXType FX_type, sf::Vector2f position)
{
	string textureName;
	sf::Vector2f textureSize;
	int frameNumber;
	int animationNumber = 1;

	switch (FX_type)
	{
		case FX_Hit:
		{
			textureSize = sf::Vector2f(400, 295);
			textureName = "2D/FX_explosion_S.png";
			frameNumber = 2;
			break;
		}
		case FX_HitShield:
		{
			textureSize = sf::Vector2f(400, 295);
			textureName = "2D/FX_explosion_S_blue.png";
			frameNumber = 2;
			break;
		}
		//case FX_Hit:
		//{
		//	textureSize = sf::Vector2f(256, 256);
		//	textureName = "2D/FX_hit.png";
		//	frameNumber = 16;
		//	break;
		//}
		case FX_Death:
		{
			textureSize = sf::Vector2f(400, 295);
			textureName = "2D/FX_explosion.png";
			frameNumber = 4;
			break;
		}
	}

	Init(position, sf::Vector2f(0, 0), textureName, textureSize, frameNumber, animationNumber);
	
	//update position and rotation "manually" because they won't be updated during the frame of their creation
	setPosition(sf::Vector2f(m_position.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_position.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
}

FX::~FX()
{

}

void FX::Update(sf::Time deltaTime)
{
	int previousFrame = m_currentFrame;

	AnimatedSprite::Update(deltaTime);

	if (previousFrame == m_frameNumber - 1 && m_currentFrame == 0)
	{
		m_garbageMe = true;
		m_visible = false;
	}
}