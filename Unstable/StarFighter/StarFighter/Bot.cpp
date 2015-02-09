#include "Bot.h"

extern Game* CurrentGame;

Bot::Bot(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size)  : Independant(position, speed,  textureName, size)
{
	collider_type = IndependantType::FriendlyFire;
	visible = true;
	visible = true;
	isOnScene = true;
	DontGarbageMe = true;
	radius = 0;
	angspeed = 0;
	vspeed = 0;
	hasTarget = false;
	hasWeapon = false;
	spread = sf::Vector2f(0,0);
	damage = 0;
}

void Bot::setTarget (Independant* m_target)
{
	this->target = m_target;
	hasTarget = true;
	this->setPosition(m_target->getPosition());
}

void Bot::update(sf::Time deltaTime)
{
	static sf::Vector2f newposition, offset;

	if (hasTarget)
	{
		newposition.x = target->getPosition().x;
		newposition.y = target->getPosition().y;
	}
	else
	{
		newposition.x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();
	}
	
	//call bobbyPattern
	offset = Pattern.GetOffset(deltaTime.asSeconds(), true);
	newposition.x += offset.x;
	newposition.y += offset.y;
	//bot spread value
	newposition.x += spread.x;
	newposition.y += spread.y;

	this->setPosition(newposition.x,newposition.y);

	//automatic fire
	if (isOnScene && hasWeapon && InputGuy::isFiring())
	{
		weapon->weaponOffset = sf::Vector2f((this->m_size.y / 2) * weapon->getFireDirection_for_Direction((*CurrentGame).direction).x, 
			(this->m_size.y / 2) * weapon->getFireDirection_for_Direction((*CurrentGame).direction).y) ;

		weapon->setPosition(this->getPosition().x + weapon->weaponOffset.x, this->getPosition().y + weapon->weaponOffset.y);

		weapon->Fire(IndependantType::FriendlyFire);
	}

	AnimatedSprite::update(deltaTime);
}

Bot* Bot::Clone()
{
	Bot* bot = new Bot(this->getPosition(), this->speed, this->textureName, this->m_size);
	bot->display_name = this->display_name;
	bot->radius = this->radius;
	bot->angspeed = this->angspeed;
	bot->vspeed = this->vspeed;
	bot->Pattern = this->Pattern;
	bot->spread = this->spread;
	bot->hasWeapon = this->hasWeapon;
	if (bot->hasWeapon)
		bot->weapon = this->weapon;
	bot->hasTarget = this->hasTarget;
	bot->damage = this->damage;
	bot->armor = this->armor;
	bot->armor_max = this->armor_max;
	bot->shield= this->shield;
	bot->shield_max = this->shield_max;
	bot->shield_regen = this->shield_regen;

	return bot;
}

void Bot::setRadius(float m_radius, float clockwise)
{
	vector<float>* v = new vector<float>;
	v->push_back(m_radius);
	v->push_back(clockwise);  // clockwise (>)
	this->Pattern.SetPattern(this->Pattern.GetCurrentPatternType(),this->vspeed,v); //vitesse angulaire (degres/s)in
}