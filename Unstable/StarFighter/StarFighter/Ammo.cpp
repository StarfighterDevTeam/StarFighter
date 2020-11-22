#include "Ammo.h"

extern Game* CurrentGame;

Ammo::Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int damage, FX* explosion) : GameObject(position, speed,  textureName, size)
{
	m_damage = damage;
	m_armor = 1;
	m_armor_max = 1;
	m_shield = 0;
	m_shield_max = 0;
	m_shield_regen = 0;
	m_startPattern = false;
	m_ref_speed = sqrt((speed.x*speed.x) + (speed.y*speed.y));
	m_explosion = explosion;
	m_radius = 0;
	m_angspeed = 0;
	m_offset_x = 0;
	m_isBeam = false;
	m_range = 0;
	m_current_range = 0;
	m_shot_angle = 0;
	m_display_name = "Ammo";
}

Ammo* Ammo::Clone()
{
	Ammo* ammo = new Ammo(this->getPosition(),this->m_speed,this->m_textureName,this->m_size,this->m_damage, this->m_explosion);
	ammo->m_display_name = this->m_display_name;

	ammo->m_Pattern = this->m_Pattern;
	ammo->m_radius = this->m_radius;
	ammo->m_angspeed = this->m_angspeed;
	ammo->m_range = this->m_range;
	ammo->m_rotation_speed = this->m_rotation_speed;
	ammo->m_offset_x = this->m_offset_x;
	ammo->m_isBeam = this->m_isBeam;
	ammo->m_DontGarbageMe = this->m_isBeam;

	return ammo;
}

void Ammo::Death()
{
	//FX* myFX = m_explosion->Clone();
	//myFX->setPosition(this->getPosition().x, this->getPosition().y);
    //(*CurrentGame).addToScene(myFX, true);

	GameObject::Death();
}

void Ammo::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	sf::Vector2f newspeed = m_speed;
	float new_ref_speed = m_ref_speed;
	if (hyperspeedMultiplier > 1)
	{
		newspeed.x += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier -1) * (*CurrentGame).m_vspeed).x;
		newspeed.y += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).y;
		new_ref_speed *= hyperspeedMultiplier;
	}
	else if (hyperspeedMultiplier < 1)//slow motion
	{
		newspeed.x = m_speed.x * hyperspeedMultiplier;
		newspeed.y = m_speed.y * hyperspeedMultiplier;
		new_ref_speed *= hyperspeedMultiplier;
	}

	if (hyperspeedMultiplier < 1.0f)
		m_collision_timer -= deltaTime.asSeconds() * hyperspeedMultiplier;
	else
		m_collision_timer -= deltaTime.asSeconds();

	this->setGhost(hyperspeedMultiplier > 1.0f);

	//range before bullet extinction (optional. put "0" not to use)
	if (m_range > 0)
	{
		m_current_range += (new_ref_speed*deltaTime.asSeconds());
		if (m_current_range > m_range)
		{
			m_visible = false;
			m_garbageMe = true;
		}
	}

	//if not disappeared, move it
	if (m_garbageMe == false)
	{
		static sf::Vector2f newposition, offset, pattern_offset;
		
		newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

		//call bobbyPattern
		pattern_offset = m_Pattern.GetOffset(deltaTime.asSeconds());
		offset.x = pattern_offset.x * cos(m_shot_angle) + pattern_offset.y * sin(m_shot_angle);
		offset.y = pattern_offset.x * sin(m_shot_angle) + pattern_offset.y * cos(m_shot_angle);
		//offset = GameObject::getSpeed_for_Direction((*CurrentGame).m_direction, offset);
		newposition.x += offset.x;
		newposition.y += offset.y;

		setPosition(newposition.x, newposition.y);

		//rotation
		rotate(m_rotation_speed*deltaTime.asSeconds());

		AnimatedSprite::update(deltaTime);
	}
}