#include "Bot.h"

extern Game* CurrentGame;

Bot::Bot(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size)  : GameObject(position, speed,  textureName, size)
{
	m_collider_type = BackgroundObject;
	m_layer = BotLayer;
	m_visible = false;
	m_isOnScene = true;
	m_DontGarbageMe = true;
	m_radius = 0;
	m_angspeed = 0;
	m_vspeed = 0;
	m_spread = sf::Vector2f(0,0);
	m_damage = 0;
	m_key_repeat = false;
	m_display_name = "Bot";
	m_target = NULL;
}

Bot::~Bot()
{
	delete m_weapon;
}

void Bot::setTarget (GameObject* target)
{
	m_target = target;

	if (target)
		setPosition(target->getPosition());
}

void Bot::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	//if ((*CurrentGame).m_waiting_for_dialog_validation == true)
	//{
	//	AnimatedSprite::update(deltaTime);
	//	return;
	//}

	if (m_visible == true)
	{
		static sf::Vector2f newposition, offset, newspeed;
		newspeed = m_speed;
		float l_hyperspeedMultiplier = 1.0f;

		if (m_target != NULL)
		{
			newposition.x = m_target->getPosition().x;
			newposition.y = m_target->getPosition().y;
			m_disable_fire = m_target->m_disable_fire;
		}
		else
		{
			if (hyperspeedMultiplier > 1)
			{
				newspeed.x += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).x;
				newspeed.y += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).y;
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

		//pattern
		if (hyperspeedMultiplier < 1.0f)
			offset = m_pattern.getOffset(deltaTime.asSeconds() * hyperspeedMultiplier, true);
		else
			offset = m_pattern.getOffset(deltaTime.asSeconds(), true);

		offset = GameObject::getSpeed_for_Direction((*CurrentGame).m_direction, offset);
		newposition.x += offset.x;
		newposition.y += offset.y;

		//bot spread value
		sf::Vector2f spread = GameObject::getSpeed_for_Direction((*CurrentGame).m_direction, m_spread);
		newposition.x += spread.x;
		newposition.y += spread.y;

		setPosition(newposition.x, newposition.y);
		rotate(m_rotation_speed*deltaTime.asSeconds() * l_hyperspeedMultiplier);

		AnimatedSprite::update(deltaTime);
	}
}

void Bot::setGhost(bool ghost)
{
	GameObject::setGhost(ghost || m_damage == 0);
}

void Bot::Fire(sf::Time deltaTime, float hyperspeedMultiplier, bool firing)
{
	//automatic fire
	if (m_weapon != NULL)
	{
		//UPDATE WEAPON POSITION
		//calculating the angle we want to face, if any
		float target_angle = getRotation();
		if (m_weapon->m_target_homing != NO_HOMING || (m_weapon->m_target_homing == SEMI_HOMING && m_weapon->m_rafale_index == 0))
		{
			target_angle = fmod(GameObject::getRotation_for_Direction((*CurrentGame).m_direction) - (*CurrentGame).GetAngleToNearestGameObject(EnemyObject, getPosition()), 360);
		}
		float current_angle = getRotation();
		float delta = current_angle - target_angle;
		if (delta > 180)
			delta -= 360;
		else if (delta < -180)
			delta += 360;

		float theta = getRotation() / 180 * M_PI;
		if (m_weapon->m_target_homing != NO_HOMING)
		{
			theta -= delta / 180 * M_PI;
		}

		if (m_weapon->m_target_homing == SEMI_HOMING && m_weapon->m_rafale_index > 0 && m_weapon->m_rafale_index < m_weapon->m_rafale)
		{
			//semi-HOMING and rafale not ended = no update of target or weapon position
		}
		else
		{
			m_weapon->m_weapon_current_offset.x = m_weapon->m_weaponOffset.x + m_size.y / 2 * sin(theta);
			m_weapon->m_weapon_current_offset.y = m_weapon->m_weaponOffset.x - m_size.y / 2 * cos(theta);

			//transmitting the angle to the weapon, which will pass it to the bullets
			m_weapon->m_shot_angle = theta;
		}

		m_weapon->setPosition(getPosition().x + m_weapon->m_weapon_current_offset.x, getPosition().y + m_weapon->m_weapon_current_offset.y);

		//UPDATE FIRING COOLDOWN AND FIRE
		if (m_weapon->isFiringReady(deltaTime, hyperspeedMultiplier))
		{
			if (firing == true)
			{
				m_weapon->Fire(FriendlyFire, deltaTime);

				(*CurrentGame).PlaySFX(SFX_Fire);
			}
		}

		//UPDATE BEAMS
		for (Ammo* beam : m_weapon->m_beams)
		{
			//update beam positions
			float beam_offset_x = beam->m_offset_x * cos(m_weapon->m_shot_angle) + beam->m_size.y / 2 * sin(m_weapon->m_shot_angle);
			float beam_offset_y = beam->m_offset_x * sin(m_weapon->m_shot_angle) - beam->m_size.y / 2 * cos(m_weapon->m_shot_angle);

			beam->setPosition(m_weapon->getPosition().x + beam_offset_x, m_weapon->getPosition().y + beam_offset_y);
		}

		if (m_weapon->m_beams.empty() == false)//end of beam because no valid fire input
		{
			if (firing == false)
			{
				for (Ammo* beam : m_weapon->m_beams)
					beam->Death();

				m_weapon->m_beams.clear();
				m_weapon->m_readyFireTimer = 0;
			}
		}
	}
}

Bot* Bot::Clone()
{
	Bot* bot = new Bot(this->getPosition(), this->m_speed, this->m_textureName, this->m_size);
	bot->m_display_name = this->m_display_name;
	bot->m_radius = this->m_radius;
	bot->m_angspeed = this->m_angspeed;
	bot->m_vspeed = this->m_vspeed;
	bot->m_pattern = this->m_pattern;
	bot->m_spread = this->m_spread;
	bot->m_weapon = this->m_weapon->Clone();
	bot->m_damage = this->m_damage;
	bot->m_armor = this->m_armor;
	bot->m_armor_max = this->m_armor_max;
	bot->m_shield= this->m_shield;
	bot->m_shield_max = this->m_shield_max;
	bot->m_shield_regen = this->m_shield_regen;
	bot->m_rotation_speed = this->m_rotation_speed;

	return bot;
}

void Bot::setRadius(float m_radius, float clockwise)
{
	vector<float> patternParams;
	patternParams.push_back(m_radius);
	patternParams.push_back(clockwise);  // clockwise (>)
	m_pattern.setPattern(m_pattern.m_pattern_type, m_vspeed, patternParams); //vitesse angulaire (degres/s)in
}