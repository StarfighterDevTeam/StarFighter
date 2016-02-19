#include "Bot.h"

extern Game* CurrentGame;

Bot::Bot(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size)  : GameObject(position, speed,  textureName, size)
{
	collider_type = GameObjectType::FriendlyFire;
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
	display_name = "Bot";
}

Bot::~Bot()
{
	if (this->weapon != NULL)
	{
		this->weapon->~Weapon();
	}
}

void Bot::setTarget (GameObject* m_target)
{
	this->target = m_target;
	this->setPosition(m_target->getPosition());
}

void Bot::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = this->speed;
	float l_hyperspeedMultiplier = 1.0f;

	if (this->target != NULL)
	{
		newposition.x = target->getPosition().x;
		newposition.y = target->getPosition().y;
		disable_fire = target->disable_fire;
	}
	else
	{
		if (hyperspeedMultiplier > 1)
		{
			newspeed.x += GameObject::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier - 1) * (*CurrentGame).vspeed).x;
			newspeed.y += GameObject::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier - 1) * (*CurrentGame).vspeed).y;
		}
		else if (hyperspeedMultiplier < 1.0f)
		{
			l_hyperspeedMultiplier = hyperspeedMultiplier;
			newspeed.x *= l_hyperspeedMultiplier;
			newspeed.y *= l_hyperspeedMultiplier;
		}

		newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();
	}
	
	//call bobbyPattern
	
	offset = Pattern.GetOffset(deltaTime.asSeconds() * l_hyperspeedMultiplier, true);
	
	offset = GameObject::getSpeed_for_Direction((*CurrentGame).direction, offset);
	newposition.x += offset.x;
	newposition.y += offset.y;
	//bot spread value
	newposition.x += spread.x;
	newposition.y += spread.y;

	this->setPosition(newposition.x,newposition.y);

	//rotation
	this->rotate(this->rotation_speed*deltaTime.asSeconds() * l_hyperspeedMultiplier);

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
	if (this->weapon != NULL && this->target != NULL)
	{
		if (this->weapon->isFiringReady(deltaTime, hyperspeedMultiplier))
		{
			if (!disable_fire && (target->isCollidingWithInteractiveObject == No_Interaction) && !target->disable_fire)
			{
				if (InputGuy::isFiring() || this->automatic_fire)
				{
					//calculating the angle we want to face, if any
					float target_angle = this->getRotation();
					if (this->weapon->target_seaking != NO_SEAKING || (this->weapon->target_seaking == SEMI_SEAKING && this->weapon->rafale_index == 0))
					{
						target_angle = fmod(GameObject::getRotation_for_Direction((*CurrentGame).direction) - (*CurrentGame).GetAngleToNearestGameObject(GameObjectType::EnemyObject, this->getPosition()), 360);
					}
					float current_angle = this->getRotation();
					float delta = current_angle - target_angle;
					if (delta > 180)
						delta -= 360;
					else if (delta < -180)
						delta += 360;

					float theta = this->getRotation() / 180 * M_PI;
					if (this->weapon->target_seaking != NO_SEAKING)
					{
						theta -= delta / 180 * M_PI;
					}

					if (this->weapon->target_seaking == SEMI_SEAKING && this->weapon->rafale_index > 0 && this->weapon->rafale_index < this->weapon->rafale)
					{
						//semi-seaking and rafale not ended = no update of target or weapon position
					}
					else
					{
						this->weapon->weapon_current_offset.x = this->weapon->weaponOffset.x + this->m_size.x / 2 * sin(theta);
						this->weapon->weapon_current_offset.y = this->weapon->weaponOffset.y - this->m_size.y / 2 * cos(theta);

						//transmitting the angle to the weapon, which will pass it to the bullets
						this->weapon->shot_angle = theta;
					}

					this->weapon->setPosition(this->getPosition().x + this->weapon->weapon_current_offset.x, this->getPosition().y + this->weapon->weapon_current_offset.y);
					this->weapon->Fire(GameObjectType::FriendlyFire, deltaTime, hyperspeedMultiplier);
				}
			}
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
	bot->weapon = this->weapon->Clone();
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