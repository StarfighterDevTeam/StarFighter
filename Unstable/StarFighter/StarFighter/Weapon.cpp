#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(Ammo* ammunition)
{
	m_speed.x = 0;
	m_speed.y = 0;
	m_multishot = 1;
	m_xspread = 0;
	m_rate_of_fire = 0.4f;
	m_shot_index = 0;
	m_dispersion = 0.f;
	m_firing_ready = true;
	m_rafale_cooldown = 0.8f;
	m_rafale = 0;
	m_rafale_index = 0;
	m_target_homing = NO_HOMING;
	m_shot_mode = NoShotMode;
	m_angle_offset = 0;
	m_delay = 0.f;
	m_weaponOffset = sf::Vector2f(0, 0);
	m_face_target = false;
	m_fire_pattern_return = false;
	m_display_name = "Laser";
	m_level = 1;
	m_credits = 0;
	m_quality = 0;
	m_readyFireTimer = 0;
	m_beam_timer = 0;
	m_shot_angle = 0;

	m_ammunition = ammunition;
}

Weapon::~Weapon()
{
	if (m_ammunition != NULL)
		delete m_ammunition;

	for (Ammo* beam : m_beams)
		beam->Death();
}

void Weapon::Draw(sf::RenderTexture& screen)
{
	if (m_rafale < 0)
	{
		float laser_warning_delay = m_ammunition->m_size.x > 30 ? ENEMY_BIG_LASERBEAM_POINTER_DELAY : ENEMY_SMALL_LASERBEAM_POINTER_DELAY;

		if (m_readyFireTimer > m_rate_of_fire - laser_warning_delay)//laserbeam aiming feedback for enemies
		{
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(m_ammunition->m_size.x * 0.1, m_ammunition->m_size.y));
			rect.setOrigin(sf::Vector2f(0.5 * m_ammunition->m_size.x * 0.1, 0.5 * m_ammunition->m_size.y));
			float alpha = ProrataBetweenThreshold(m_readyFireTimer, sf::Vector2f(m_rate_of_fire - laser_warning_delay, m_rate_of_fire)) * 120;
			rect.setFillColor(sf::Color(255, 0, 0, alpha));

			float beam_offset_x = m_ammunition->m_offset_x * cos(m_shot_angle) + m_ammunition->m_size.y / 2 * sin(m_shot_angle) * (-m_fire_direction);
			float beam_offset_y = m_ammunition->m_offset_x * sin(m_shot_angle) - m_ammunition->m_size.y / 2 * cos(m_shot_angle) * (-m_fire_direction);

			rect.setPosition(getPosition().x + beam_offset_x, getPosition().y + beam_offset_y);
			rect.setRotation(m_shot_angle * 180 / M_PI);

			screen.draw(rect);
		}
	}
}

void Weapon::CreateBullet(GameObjectType collider_type, float offsetX, float dispersion)
{
	Ammo* bullet = m_ammunition->Clone();

	float l_dispersion = collider_type == EnemyFire ? -dispersion : dispersion;

	//transmitting the value to the bullet
	bullet->m_shot_angle = m_shot_angle;
	bullet->m_offset_x = offsetX;

	//calculation of bullet offset respect to the weapon position
	float bullet_offset_x = bullet->m_offset_x * cos(m_shot_angle) - (bullet->m_size.y / 2 - (bullet->m_size.y * (int)bullet->m_isBeam)) * sin(m_shot_angle);
	float bullet_offset_y = bullet->m_offset_x * sin(m_shot_angle) + (bullet->m_size.y / 2 - (bullet->m_size.y * (int)bullet->m_isBeam)) * cos(m_shot_angle);

	bullet->setPosition(getPosition().x + bullet_offset_x, getPosition().y + bullet_offset_y);

	bullet->m_speed.x = bullet->m_ref_speed * sin(m_shot_angle + (l_dispersion *  M_PI / 180)) * (-m_fire_direction);
	bullet->m_speed.y = bullet->m_ref_speed * cos(m_shot_angle + (l_dispersion *  M_PI / 180)) * m_fire_direction;

	bullet->setRotation((m_shot_angle * 180.0f / M_PI) + l_dispersion);

	bullet->m_visible = true;
	bullet->m_collider_type = collider_type;
	bullet->m_isOnScene = true;
	
	//beam layer
	if (bullet->m_isBeam == false)
		bullet->m_layer = collider_type == EnemyFire ? EnemyFireLayer : FriendlyFireLayer;
	else
		bullet->m_layer = collider_type == EnemyFire ? EnemyBeamLayer : FriendlyBeamLayer;

	//missile default target position
	if (bullet->m_is_missile_model == true)
	{
		if ((*CurrentGame).m_direction == DIRECTION_UP || (*CurrentGame).m_direction == NO_DIRECTION)
			bullet->m_missile_target_position = sf::Vector2f(getPosition().x, ((m_fire_direction + 1) / 2) * SCENE_SIZE_Y);//0 if player, SCENE_SIZE_Y if enemy
		else if ((*CurrentGame).m_direction == DIRECTION_DOWN)
			bullet->m_missile_target_position = sf::Vector2f(getPosition().x, ((- m_fire_direction + 1) / 2) * SCENE_SIZE_Y);//SCENE_SIZE_Y if player, 0 if enemy
		else if ((*CurrentGame).m_direction == DIRECTION_RIGHT)
			bullet->m_missile_target_position = sf::Vector2f(((- m_fire_direction + 1) / 2) * SCENE_SIZE_X, getPosition().y);//SCENE_SIZE_X if player, 0 if enemy
		else if ((*CurrentGame).m_direction == DIRECTION_LEFT)
			bullet->m_missile_target_position = sf::Vector2f(((m_fire_direction + 1) / 2) * SCENE_SIZE_X, getPosition().y);//0 if player, SCENE_SIZE_X if enemy

		//missile trail fx
		if (collider_type == EnemyFire && bullet->m_trail != NULL)
			bullet->m_trail->m_offset.y = -bullet->m_trail->m_offset.y;
	}

	(*CurrentGame).addToScene(bullet, true);

	if (m_rafale < 0)//continuous beam created, we need to keep track of it
		m_beams.push_back(bullet);
}

bool Weapon::isFiringReady(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if (hyperspeedMultiplier < 1.0f)
	{
		m_readyFireTimer += deltaTime.asSeconds() * hyperspeedMultiplier;
		m_beam_timer += deltaTime.asSeconds() * hyperspeedMultiplier;
	}
	else
	{
		m_readyFireTimer += deltaTime.asSeconds();
		m_beam_timer += deltaTime.asSeconds();
	}

	//enf of beam timer
	if (m_beams.empty() == false && m_rafale_cooldown > 0 && m_beam_timer > m_rafale_cooldown)
	{
		for (Ammo* beam : m_beams)
			beam->Death();

		m_beams.clear();
		m_readyFireTimer = 0;
	}

	//rafale
	bool rafale_ready = (m_rafale == 0);
	if (m_rafale > 0 && m_readyFireTimer > m_rafale_cooldown)
	{
		m_rafale_index = 0;
		m_shot_index = 0;
		rafale_ready = true;
	}

	if (m_readyFireTimer > m_rate_of_fire && rafale_ready == true)
		m_firing_ready = true;
	else if (m_rafale > 0)
	{
		if (m_readyFireTimer > m_rate_of_fire && m_rafale_index <= m_rafale - 1)
			m_firing_ready = true;
	}
	else if (m_rafale < 0)
		m_firing_ready = m_beams.empty() == true && m_readyFireTimer > m_rate_of_fire;

	return m_firing_ready;
}

void Weapon::Fire(GameObjectType collider_type, sf::Time deltaTime)
{
	if (m_multishot > 1)
	{
		if (m_dispersion == 360)//treating this particular case to avoid a double bullet on the 360th degree
			m_dispersion *= (1 - (1 / m_multishot));//ex: 10 shots at 360°, we make it shots at 324° instead (360 - 360/10 to avoid the double bullet)

		if (m_shot_mode == NoShotMode)
			FireMultiShot(collider_type);
		else if (m_shot_mode == AlternateShotMode)
			FireAlternateShot(collider_type);
		else if (m_shot_mode == AscendingShotMode)
			FireAscendingShot(collider_type);
		else if (m_shot_mode == DescendingShotMode)
			FireDescendingShot(collider_type);
		else if (m_shot_mode == Ascending2ShotMode)
		{
			if (!m_fire_pattern_return)
				FireAscendingShot(collider_type);
			else
				FireDescendingShot(collider_type);

			if (m_shot_index == 0)
				m_fire_pattern_return = !m_fire_pattern_return;
		}
		else if (m_shot_mode == Descending2ShotMode)
		{
			if (!m_fire_pattern_return)
				FireDescendingShot(collider_type);
			else
				FireAscendingShot(collider_type);

			if (m_shot_index == 0)
				m_fire_pattern_return = !m_fire_pattern_return;
		}
	}
	else//single shot
		CreateBullet(collider_type);

	m_readyFireTimer = 0;
	m_firing_ready = false;

	if (m_rafale > 0 && m_shot_index == 0)
		m_rafale_index++;
	else if (m_rafale < 0)//continuous beam started
		m_beam_timer = 0;
}

void Weapon::FireMultiShot(GameObjectType collider_type)
{
	if (m_multishot % 2 != 0) //case of an odd number of bullets
	{
		//central bullet
		CreateBullet(collider_type, 0);

		// the rest of the bullets are spread evenly on the left and right of the central bullet
		if (m_multishot > 1)
		{
			for (int i = 1; i < (((m_multishot - 1) / 2) + 1); i++)
			{
				int s = 1;//used for symetry
				for (int j = 0; j < 2; j++)//2 loops: j=1 and then j=-1
				{
					CreateBullet(collider_type, i*s*m_xspread, i*s*m_dispersion / 2 / ((m_multishot - 1) / 2));
					s = -s;
				}
			}
		}
	}

	if (m_multishot % 2 == 0 && m_multishot != 0) //case of an even number of bullets
	{
		for (int i = 1; i < (((m_multishot - 1) / 2) + 2); i++)
		{
			int s = 1;//used for symetry
			for (int j = 0; j < 2; j++)//2 loops: j=1 and then j=-1
			{
				CreateBullet(collider_type, (i*s*m_xspread) - (s*m_xspread / 2), i*s*m_dispersion / (m_multishot - 1) - s*(m_dispersion / (m_multishot - 1) / 2));
				s = -s;
			}
		}
	}
}

void Weapon::FireAlternateShot(GameObjectType m_collider_type)
{
	if (m_multishot % 2 != 0) //case of an odd number of bullets
	{
		if (m_shot_index % 2 != 0)
			CreateBullet(m_collider_type, -(((m_shot_index - 1) / 2) + 1)*m_xspread, -(((m_shot_index - 1) / 2) + 1)*m_dispersion / 2 / ((m_multishot - 1) / 2));
		else
			CreateBullet(m_collider_type, (m_shot_index / 2)*m_xspread, (m_shot_index / 2)*m_dispersion / 2 / ((m_multishot - 1) / 2));
	}

	if (m_multishot % 2 == 0) //case of an even number of bullets
	{
		if (m_shot_index % 2 != 0)
			CreateBullet(m_collider_type, -(((m_shot_index / 2) + 1)*m_xspread) + (m_xspread / 2), -((m_shot_index / 2) + 1)*m_dispersion / (m_multishot - 1) + m_dispersion / (m_multishot - 1) / 2);
		else
			CreateBullet(m_collider_type, (((m_shot_index / 2) + 1)*m_xspread) - (m_xspread / 2), ((m_shot_index / 2) + 1)*m_dispersion / (m_multishot - 1) - (m_dispersion / (m_multishot - 1) / 2));
	}

	if (m_shot_index < m_multishot - 1)
		m_shot_index++;
	else
		m_shot_index = 0;
}

//left to right order
void Weapon::FireAscendingShot(GameObjectType collider_type)
{
	if (m_multishot % 2 != 0) //case of an even number of bullets
	{
		//left and center bullets (from left to right)
		if (m_shot_index < (((m_multishot - 1) / 2) + 1))
			CreateBullet(collider_type, (-((m_multishot - 1) / 2) + m_shot_index)*m_xspread, (-((m_multishot - 1) / 2) + m_shot_index)*(m_dispersion / (m_multishot - 1)));
		else//right
			CreateBullet(collider_type, (m_shot_index - ((m_multishot - 1) / 2))*m_xspread, (m_shot_index - ((m_multishot - 1) / 2))*(m_dispersion / (m_multishot - 1)));
	}

	if (m_multishot % 2 == 0) //case of an odd number of bullets
	{
		//left and center bullets (from left to right)
		if (m_shot_index < (m_multishot / 2))
			CreateBullet(collider_type, (-((m_multishot / 2) + m_shot_index)*m_xspread) + (m_xspread / 2), -((m_multishot / 2) - m_shot_index + (m_xspread / 2))*(m_dispersion / (m_multishot - 1)));
		else//right
			CreateBullet(collider_type, (m_shot_index - (m_multishot / 2))*m_xspread + (m_xspread / 2), (m_shot_index - (m_multishot / 2) + (m_xspread / 2))*(m_dispersion / (m_multishot - 1)));
	}

	if (m_shot_index < m_multishot - 1)
		m_shot_index++;
	else
		m_shot_index = 0;
}

//right to left order
void Weapon::FireDescendingShot(GameObjectType collider_type)
{
	if (m_multishot % 2 != 0) //case of an even number of bullets
	{
		//right and center bullets (from right to left)
		if (m_shot_index < (((m_multishot - 1) / 2) + 1))
			CreateBullet(collider_type, (((m_multishot - 1) / 2) - m_shot_index)*m_xspread, (((m_multishot - 1) / 2) - m_shot_index)*(m_dispersion / (m_multishot - 1)));
		else//left
			CreateBullet(collider_type, (-m_shot_index + ((m_multishot - 1) / 2))*m_xspread, (-m_shot_index + ((m_multishot - 1) / 2))*(m_dispersion / (m_multishot - 1)));
	}

	if (m_multishot % 2 == 0) //case of an odd number of bullets
	{
		//right and center bullets (from right to left)
		if (m_shot_index < (m_multishot / 2))
			CreateBullet(collider_type, (((m_multishot / 2) - m_shot_index)*m_xspread) + (m_xspread / 2), ((m_multishot / 2) - m_shot_index - (m_xspread / 2))*(m_dispersion / (m_multishot - 1)));
		else//left
			CreateBullet(collider_type, (-m_shot_index + (m_multishot / 2))*m_xspread - (m_xspread / 2), (-m_shot_index + (m_multishot / 2) - (m_xspread / 2))*(m_dispersion / (m_multishot - 1)));
	}

	if (m_shot_index < m_multishot - 1)
		m_shot_index++;
	else
		m_shot_index = 0;
}

void Weapon::UpdateBeams(bool firing)
{
	//update beam rotation
	for (Ammo* beam : m_beams)
	{
		float beam_offset_x = beam->m_offset_x * cos(m_shot_angle) + beam->m_size.y / 2 * sin(m_shot_angle) * (- m_fire_direction);
		float beam_offset_y = beam->m_offset_x * sin(m_shot_angle) - beam->m_size.y / 2 * cos(m_shot_angle) * (- m_fire_direction);

		beam->setPosition(getPosition().x + beam_offset_x, getPosition().y + beam_offset_y);

		//if (m_target_homing == HOMING)
			beam->setRotation(m_shot_angle * 180 / M_PI);
	}

	//end of beam because no valid fire input
	if (m_beams.empty() == false)
	{
		if (firing == false)
		{
			for (Ammo* beam : m_beams)
				beam->Death();

			m_beams.clear();
			m_readyFireTimer = 0;
		}
	}
}

bool Weapon::HasSemiHomingSalvoInProgress()
{
	if (m_target_homing != SEMI_HOMING)
		return false;

	if (m_rafale > 0)
	{
		if (m_rafale_index > 0 && m_rafale_index < m_rafale)
			return true;

		if (m_multishot > 1 && m_shot_index > 0)
			return true;
	}

	if (m_rafale < 0 && m_beams.empty() == false)
		return true;

	return false;
}

float Weapon::GetAngleToleranceForBeam(GameObject* target)
{
	float Z = PIXELS_TOLERANCE_FOR_FACE_TARGET_ALIGNMENT_BEAM;
	float X = m_ammunition->m_size.x * 0.5;
	float D = GameObject::GetDistanceBetweenPositions(getPosition(), target->getPosition());

	float ratio = D == 0 ? 0 : (Z + X) / D;

	return asin(ratio) * 180 / M_PI;
}

Weapon* Weapon::Clone()
{
	Weapon* weapon = new Weapon(this->m_ammunition->Clone());
	weapon->m_display_name = this->m_display_name;
	weapon->m_fire_direction = this->m_fire_direction;
	weapon->m_rate_of_fire = this->m_rate_of_fire;
	weapon->m_multishot = this->m_multishot;
	weapon->m_xspread = this->m_xspread;
	weapon->m_shot_mode = this->m_shot_mode;
	weapon->m_dispersion = this->m_dispersion;
	weapon->m_rafale = this->m_rafale;
	weapon->m_rafale_cooldown = this->m_rafale_cooldown;
	weapon->m_target_homing = this->m_target_homing;
	weapon->m_angle_offset = this->m_angle_offset;
	weapon->m_delay = this->m_delay;
	weapon->m_weaponOffset = this->m_weaponOffset;

	weapon->m_frameNumber = this->m_frameNumber;
	weapon->m_textureName = this->m_textureName;
	weapon->m_size = this->m_size;

	weapon->m_level = this->m_level;
	weapon->m_credits = this->m_credits;
	weapon->m_quality = this->m_quality;

	return weapon;
}