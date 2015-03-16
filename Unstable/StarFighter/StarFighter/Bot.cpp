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
	spread = sf::Vector2f(0,0);
	damage = 0;
	key_repeat = false;
	target = NULL;
}

void Bot::setTarget (Independant* m_target)
{
	this->target = m_target;
	this->setPosition(m_target->getPosition());
}

void Bot::update(sf::Time deltaTime)
{
	static sf::Vector2f newposition, offset;

	if (this->target != NULL)
	{
		newposition.x = target->getPosition().x;
		newposition.y = target->getPosition().y;
		disable_fire = target->disable_fire;
	}
	else
	{
		newposition.x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();
	}
	
	//call bobbyPattern
	offset = Pattern.GetOffset(deltaTime.asSeconds(), true);
	offset = Independant::getSpeed_for_Direction((*CurrentGame).direction, offset);
	newposition.x += offset.x;
	newposition.y += offset.y;
	//bot spread value
	newposition.x += spread.x;
	newposition.y += spread.y;

	this->setPosition(newposition.x,newposition.y);

	//rotation
	this->rotate(this->rotation_speed*deltaTime.asSeconds());

	//auto fire option (F key)
	if (InputGuy::setAutomaticFire())
	{
		if (!this->key_repeat)
		{
			this->automatic_fire = !this->automatic_fire;
			this->key_repeat = true;
		}
	}
	else
	{
		this->key_repeat = false;
	}

	//automatic fire
	if (this->weapon && !disable_fire)
	{
		if (InputGuy::isFiring() || this->automatic_fire)
		{
			float theta = this->getRotation() / 180 * M_PI;
			float x_weapon_offset = this->m_size.y / 2 * sin(theta);
			float y_weapon_offset = - this->m_size.y / 2 * cos(theta);

			this->weapon->setPosition(this->getPosition().x + x_weapon_offset, this->getPosition().y + y_weapon_offset);
			this->weapon->shot_angle = theta;
			this->weapon->Fire(IndependantType::FriendlyFire);
		}
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
	bot->weapon = this->weapon;
	bot->target = this->target;
	bot->damage = this->damage;
	bot->armor = this->armor;
	bot->armor_max = this->armor_max;
	bot->shield= this->shield;
	bot->shield_max = this->shield_max;
	bot->shield_regen = this->shield_regen;
	bot->rotation_speed = this->rotation_speed;

	return bot;
}

void Bot::setRadius(float m_radius, float clockwise)
{
	vector<float>* v = new vector<float>;
	v->push_back(m_radius);
	v->push_back(clockwise);  // clockwise (>)
	this->Pattern.SetPattern(this->Pattern.currentPattern,this->vspeed,v); //vitesse angulaire (degres/s)in
}