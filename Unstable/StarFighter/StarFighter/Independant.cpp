#include "Independant.h"

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin)  : Sprite()
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(textureName, size.x, size.y);

	this->setOrigin(origin.x, origin.y);
	this->setTexture(*texture);
	this->speed = speed;
	this->m_size.x=size.x;
	this->m_size.y=size.y;
	this->setPosition(position.x,position.y);
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Sprite()
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(textureName, size.x, size.y);

	this->setOrigin(size.x/2, size.y/2);
	this->setTexture(*texture);
	this->speed = speed;
	this->m_size.x=size.x;
	this->m_size.y=size.y;
	this->setPosition(position.x,position.y);
	this->visible = false;
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture) : Sprite()
{
	this->setTexture(*texture);
	this->speed = speed;
	this->setPosition(position.x,position.y);
	this->visible = false;
}

void Independant::update(sf::Time deltaTime)
{
	float x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
	float y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();
	this->setPosition(x,y);
	this->visible = false;
}

void Independant::setVisible(bool m_visible)
{
	if (m_visible) visible=true;
	else visible=false;
}

bool Independant::collide_with (const Independant& independant)
{
	// If not visibe, let's not even bother with the collision
	if (!visible)
        return false;
	
	// Some types of independant don't colide with each others
	// 0=bg; 1=ship ; 2=friendly fire ; 3= enemy fire ; 4=enemy;	
	else if (   (independant.collider_type==0) 
		
			|| (independant.collider_type==1 && (collider_type==0 || collider_type==2)) 
		
			|| (independant.collider_type==2 && (collider_type==0 || collider_type==1 || collider_type==3)) 

			|| (independant.collider_type==3 && (collider_type==0 || collider_type==2 || collider_type==4)) 
		
			|| (independant.collider_type==4 && (collider_type==0 || collider_type==2 || collider_type==3))	)		

		return false;

	 // Preliminary test : are the two sprites even aligned ?
    else if (independant.getPosition().x - (independant.m_size.x/2) > getPosition().x + (m_size.x/2)

		|| independant.getPosition().x + (independant.m_size.x/2) < getPosition().x - (m_size.x/2)


    || independant.getPosition().y + (independant.m_size.y/2) < getPosition().y - (m_size.y/2)
																								

    || independant.getPosition().y - (independant.m_size.y/2) > getPosition().y + (m_size.y/2))
        return false;                        

    // Second test : are the corners included in the other sprite, or vice versa ?
    else
    {
        if ( (   ( (getPosition().x - (m_size.x/2) < independant.getPosition().x - (independant.m_size.x/2)) && (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x + (m_size.x/2))   // up-left corner
                && (getPosition().y - (m_size.y/2) < independant.getPosition().y - (independant.m_size.y/2)) && (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) )

            ||  ( (getPosition().x - (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2)) && (independant.getPosition().x + (independant.m_size.x/2) < getPosition().x + (m_size.x/2))   // up-right corner
                && ( getPosition().y - (m_size.y/2) < independant.getPosition().y - (independant.m_size.y/2)) && (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) )

            ||  ( (getPosition().x - (m_size.x/2) < independant.getPosition().x - (independant.m_size.x/2)) && (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x + (m_size.x/2))   // down-left corner
                && (getPosition().y - (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) && (independant.getPosition().y + (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) )

            ||  ( (getPosition().x - (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2)) && (independant.getPosition().x + (independant.m_size.x/2) < getPosition().x + (m_size.x/2))   // down-right corner
                && (getPosition().y - (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) && (independant.getPosition().y + (independant.m_size.y/2) < getPosition().y + (m_size.y/2)))  )

         ||

              (  ((independant.getPosition().x - (independant.m_size.x/2) < getPosition().x - (m_size.x/2)) && (getPosition().x - (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2))   // up-left corner
                && (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y - (m_size.y/2)) && (getPosition().y - (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) )
                    //return true;
                || ( (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x + (m_size.x/2)) && (getPosition().x + (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2))  // up-right corner
                && (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y - (m_size.y/2)) && (getPosition().y - (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) )
                    //return true;
                || ( (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x - (m_size.x/2)) && (getPosition().x - (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2))   // down-left corner
                && (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) && (getPosition().y + (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)) )
                    //return true;
                || ( (independant.getPosition().x - (independant.m_size.x/2) < getPosition().x + (m_size.x/2)) && (getPosition().x + (m_size.x/2) < independant.getPosition().x + (independant.m_size.x/2))   // down-right corner
                && (independant.getPosition().y - (independant.m_size.y/2) < getPosition().y + (m_size.y/2)) && (getPosition().y + (m_size.y/2) < independant.getPosition().y + (independant.m_size.y/2)))  )  )

            return true;
        else
            return false;
    }
    return false;
}