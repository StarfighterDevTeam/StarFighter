#include "Enemy.h"

extern Game* CurrentGame;

//DEBUG
void Enemy::Draw(sf::RenderTexture& screen)
{
	/*DEBUG*/
	/*Text text;
	text.setFont(*m_font);
	text.setCharacterSize(12);
	text.setColor(sf::Color::White);
	text.setPosition(sf::Vector2f(getPosition().x - 100, getPosition().y - 130));
	ostringstream ss;

	//hack
	//if (m_pattern.m_patternParams.empty() == false)
	//	m_pattern.m_patternParams[0] = 0;//param 0 = clockwise inutile car on a déjà le signe de speed.  Remplacer par un booléen "randomize speed sign".
	//m_pattern.m_patternSpeed = -100;

	//display
	//ss << "type: " << to_string(int(m_pattern.m_pattern_type)) << " / w: " << to_string(int(m_pattern.m_width)) << " / h: " << to_string(int(m_pattern.m_height));
	ss << "\n " << m_currentPhase->m_display_name.c_str();// << " / offy: " << to_string(m_pattern.m_offset.y) << " / spd: " << to_string(int(m_pattern.m_speed));
	ss << "\nclock: " << to_string((int)m_phaseTimer.asSeconds());
	
	//for (float f : m_pattern.m_patternParams)
	//	ss << to_string(int(f)) << " / ";

	text.setString(ss.str());
	screen.draw(text);*/

	for (Weapon* weapon : m_weapons_list)
		weapon->Draw(screen);
	
	GameObject::Draw(screen);
}

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* FX_death, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x/2, size.y/2), frameNumber, animationNumber)
{
	m_collider_type = EnemyObject;
	m_layer = EnemyObjectLayer;

	m_visible = true;
	m_angspeed = 0;
	m_radius = 0;
	m_FX_death = FX_death;
	m_rotation_speed = 0;
	m_face_target = false;
	m_reset_facing = false;
	m_shoot_when_aligned = false;
	m_bouncing = NoBouncing;
	m_enemyTimer = 0;
	m_level = 1;
	m_input_blocker = false;
	m_currentPhase = NULL;

	//life bars
	m_health_feedbackTimer = 0;

	m_armorBar_offsetY = - (m_size.y / 2) - (ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2) - ENEMY_HP_BAR_OFFSET_Y;
	m_shieldBar_offsetY = - (m_size.y / 2) - (1.5 * ENEMY_HP_BAR_CONTAINER_SIZE_Y) - ENEMY_HP_BAR_OFFSET_Y - ENEMY_SHIELD_BAR_OFFSET_Y;
	//m_offsetBetweenHealthBars = armorBar_offsetY - shieldBar_offsetY;

	m_armorBarContainer.setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	m_armorBarContainer.setFillColor(sf::Color(0, 0, 0, 128));
	m_armorBarContainer.setOutlineThickness(0);
	//m_armorBarContainer->setOutlineColor(sf::Color(0, 255, 255, 128));
	m_armorBarContainer.setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);
	
	m_armorBar.setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	m_armorBar.setFillColor(sf::Color(COLOR_GREEN_R_VALUE, COLOR_GREEN_G_VALUE, COLOR_GREEN_B_VALUE, COLOR_GREEN_A_VALUE));//green
	m_armorBar.setOutlineThickness(0);
	m_armorBar.setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);

	m_shieldBarContainer.setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	m_shieldBarContainer.setFillColor(sf::Color(0, 0, 0, 128));
	m_shieldBarContainer.setOutlineThickness(0);
	//m_shieldBarContainer->setOutlineColor(sf::Color(0, 255, 255, 128));
	m_shieldBarContainer.setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);
	m_shieldBarContainer.setPosition(getPosition().x, getPosition().y - m_size.y / 2 - 1.5 * ENEMY_HP_BAR_CONTAINER_SIZE_Y - ENEMY_HP_BAR_OFFSET_Y - ENEMY_SHIELD_BAR_OFFSET_Y);
	
	m_shieldBar.setSize(sf::Vector2f(ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	m_shieldBar.setFillColor(sf::Color(COLOR_BLUE_R_VALUE, COLOR_BLUE_G_VALUE, COLOR_BLUE_B_VALUE, COLOR_BLUE_A_VALUE));//blue
	m_shieldBar.setOutlineThickness(0);
	m_shieldBar.setOrigin(ENEMY_HP_BAR_CONTAINER_SIZE_X / 2, ENEMY_HP_BAR_CONTAINER_SIZE_Y / 2);
	m_shieldBar.setPosition(getPosition().x, getPosition().y - m_size.y / 2 - 1.5 * ENEMY_HP_BAR_CONTAINER_SIZE_Y - ENEMY_HP_BAR_OFFSET_Y - ENEMY_SHIELD_BAR_OFFSET_Y);
	
	try
	{
		m_font = new sf::Font();
		if (!m_font->loadFromFile(makePath("Fonts/terminator_real_nfi.ttf")))
		{
			// error
			//TODO: font loader
		}
		m_enemyLevel.setFont(*m_font);
		m_enemyLevel.setCharacterSize(12);
		m_enemyLevel.setColor(sf::Color::White);
		m_enemyLevel.setPosition(getPosition());
	}

	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::LERROR, ex.what());
	}
}

void Enemy::UpdateHealthBars()
{
	//health bars feedbacks
	float angle_rad = 0;// GameObject::getRotation_for_Direction((*CurrentGame).m_direction) / 180 * M_PI;
		
	m_armorBar.setPosition(getPosition().x - (m_armorBar_offsetY * sin(angle_rad)), getPosition().y + (m_armorBar_offsetY * cos(angle_rad)));
	m_armorBarContainer.setPosition(getPosition().x - (m_armorBar_offsetY * sin(angle_rad)), getPosition().y + (m_armorBar_offsetY * cos(angle_rad)));

	//TODO: screen borders constraints
	//armorBar->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, armorBar->getPosition(), armorBar->getSize()));
	//armorBarContainer->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, armorBarContainer->getPosition(), armorBarContainer->getSize()));

	//update size (damage)
	m_armorBar.setSize(sf::Vector2f(1.0f * m_armor / m_armor_max * ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));
	if (m_health_feedbackTimer <= 0)
	{
		m_armorBar.m_visible = false;
		m_armorBarContainer.m_visible = false;
	}
		
	m_shieldBar.setPosition(getPosition().x - (m_shieldBar_offsetY * sin(angle_rad)), getPosition().y + (m_shieldBar_offsetY * cos(angle_rad)));
	m_shieldBarContainer.setPosition(getPosition().x - (m_shieldBar_offsetY * sin(angle_rad)), getPosition().y + (m_shieldBar_offsetY * cos(angle_rad)));

	//TODO: screen borders constraints
	//shieldBar->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, shieldBar->getPosition(), shieldBar->getSize()));
	//shieldBarContainer->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, shieldBarContainer->getPosition(), shieldBar->getSize()));
	//if shield bar touches screen, we need to move both bars
	//armorBar->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, armorBar->getPosition(), sf::Vector2f(armorBar->getSize().x, armorBar->getSize().y + (2 * offsetBetweenHealthBars) + (2 * shieldBarContainer->getSize().y))));
	//armorBarContainer->setPosition(GameObject::ApplyScreenBordersConstraints((*CurrentGame).m_direction, armorBarContainer->getPosition(), sf::Vector2f(armorBar->getSize().x, armorBar->getSize().y + (2 * offsetBetweenHealthBars) + (2 * shieldBarContainer->getSize().y))));

	m_shieldBar.setSize(sf::Vector2f(1.0f * m_shield / m_shield_max * ENEMY_HP_BAR_CONTAINER_SIZE_X, ENEMY_HP_BAR_CONTAINER_SIZE_Y));

	if (m_health_feedbackTimer <= 0)
	{
		m_shieldBar.m_visible = false;
		m_shieldBarContainer.m_visible = false;
	}

	//update enemy level display
	m_enemyLevel.setPosition(sf::Vector2f(m_armorBarContainer.getPosition().x - m_armorBarContainer.getGlobalBounds().width / 2 - m_enemyLevel.getGlobalBounds().width / 2 - ENEMY_LEVEL_DISPLAY_OFFSET_X, m_armorBarContainer.getPosition().y - m_enemyLevel.getGlobalBounds().height / 2 - ENEMY_LEVEL_DISPLAY_OFFSET_Y));

	if (m_health_feedbackTimer <= 0)
		m_enemyLevel.m_visible = false;
}

void Enemy::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	GameObject* playership = (GameObject*)(*CurrentGame).m_playership;

	//update timers
	if (m_health_feedbackTimer > 0)
	{
		m_health_feedbackTimer -= deltaTime.asSeconds();
		UpdateHealthBars();
	}

	if (m_damage_feedbackTimer > 0)
	{
		m_damage_feedbackTimer -= deltaTime.asSeconds();
		if (m_damage_feedbackTimer <= 0)
			setColor(Color(255, 255, 255, 255), true);
	}

	float l_hyperspeedMultiplier = hyperspeedMultiplier < 1 ? hyperspeedMultiplier : 1;

	if (m_collision_timer > 0)
		m_collision_timer -= deltaTime.asSeconds() * l_hyperspeedMultiplier;

	//if a dialog awaits validation or has just ended, only update animations then stop the update
	if ((*CurrentGame).m_waiting_for_dialog_validation == true || (*CurrentGame).m_end_dialog_clock.getElapsedTime().asSeconds() < END_OF_DIALOGS_DELAY)
	{
		AnimatedSprite::update(deltaTime);
		return;
	}

	//movement
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;

	//slowmotion
	newspeed.y += (l_hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed;

	bool l_ghost = false;
	if (m_currentPhase != NULL)
		for (Modifier modifier : m_currentPhase->m_modifiers)
			l_ghost = l_ghost || modifier == GhostModifier;

	setGhost(l_ghost || hyperspeedMultiplier > 1.0f);

	m_disable_fire = hyperspeedMultiplier > 1.0f;

	newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	//update pattern
	offset = m_pattern.getOffset_v2(deltaTime.asSeconds() * l_hyperspeedMultiplier);

	newposition.x += offset.x;
	newposition.y += offset.y;

	setPosition(newposition.x, newposition.y);

	//bouncing
	sf::Vector2f size = sf::Vector2f(getGlobalBounds().width, getGlobalBounds().height);
	if (m_bouncing != NoBouncing)
	{
		if (m_bouncing != BouncingVertical)
		{
			if (newposition.x < size.x / 2)
			{
				m_pattern.m_speed *= -1;
				setPosition(size.x / 2, newposition.y);
			}
			else if (newposition.x > SCENE_SIZE_X - size.x / 2)
			{
				m_pattern.m_speed *= -1;
				setPosition(SCENE_SIZE_X - size.x / 2, newposition.y);
			}
		}

		if (m_bouncing != BouncingHorizontal)
		{
			if (newposition.y < size.y / 2)
			{
				m_speed.y *= -1;
				setPosition(newposition.x, size.y / 2);
			}

			else if (newposition.y > SCENE_SIZE_Y - size.y / 2)
			{
				m_speed.y *= -1;
				setPosition(newposition.x, SCENE_SIZE_Y - size.y / 2);
			}
		}
	}

	if (m_isOnScene == true)
	{
		//TURNING TOWARDS DESIRED ANGLE
		bool hasLockingSalvoInProgress = false;
		for (Weapon* weapon : m_weapons_list)
			if (weapon->hasLockingSalvoInProgress() == true)
			{
				hasLockingSalvoInProgress = true;
				break;
			}

		if (hasLockingSalvoInProgress == false)//"locking rafale" modifier prevents for rotating during a rafale salvo in progress
		{
			if (m_face_target == false && m_reset_facing == false)//no facing instruction: just apply the rotation speed
			{
				rotate(m_rotation_speed*deltaTime.asSeconds() * l_hyperspeedMultiplier);
			}	
			else//facing instructions: compute target angle and apply rotation towards that angle
			{
				//desired angle?
				float target_angle = getRotation();

				if (m_reset_facing == true)
					target_angle = 0;
				else if (m_face_target == true)
					target_angle = fmod(getRotation() + 180 - GameObject::GetAngleDegToTargetPosition(getPosition(), getRotation(), playership->getPosition()), 360);

				//rotate towards target angle
				TurnToTargetAngle(target_angle, m_rotation_speed, deltaTime, l_hyperspeedMultiplier);
			}
		}
		
		//transition blocking firing and phase transitioning? or player in immunity delay after hit taken?
		if ((*CurrentGame).m_waiting_for_scene_transition == true)
		{
			AnimatedSprite::update(deltaTime);
			return;
		}

		//UPDATE WEAPONS
		for (Weapon* weapon : m_weapons_list)
		{
			if (m_phaseTimer < weapon->m_delay)//reset rate of fire timer while waiting for the weapon "delay"
				weapon->m_readyFireTimer = 0;
			
			//calcule weapon angle offset
			float theta = getRotation() + weapon->m_angle_offset;
				
			//angle offset to target
			float delta = GameObject::GetDeltaAngleToTargetAngle(180 - GameObject::GetAngleDegToTargetPosition(getPosition(), getRotation(), playership->getPosition()), 0);

			if (hasLockingSalvoInProgress == false)
			{
				//we turn the weapon towards the target if :
				//- we have some kind of Homing (!=NO_HOMING)
				//AND we must not be a laserbeam, because that is to strong on the enemy
				if (weapon->m_target_homing != NO_HOMING && weapon->m_rafale >= 0)
					theta += delta;

				//calcule weapon offset
				theta *= M_PI / 180;//switching to radians
				weapon->m_weapon_current_offset.x = weapon->m_weaponOffset.x * cos(theta) + m_size.y / 2 * sin(theta) * (-weapon->m_fire_direction);
				weapon->m_weapon_current_offset.y = weapon->m_weaponOffset.x * sin(theta) - m_size.y / 2 * cos(theta) * (-weapon->m_fire_direction);

				//transmitting the angle to the weapon, which will pass it to the bullets
				weapon->m_shot_angle = theta;
			}

			weapon->setPosition(getPosition().x + weapon->m_weapon_current_offset.x, getPosition().y + weapon->m_weapon_current_offset.y);

			//FIRE: Weapon can fire if:
			//- not required to face the enemy
			//- has Homing ability (not necessarily aligned with target)
			//- has Semi-homing ability and started salvo
			//- is aligned with target
			bool hadBeamsActive = weapon->m_beams.empty() == false;
			float angle_tolerance_for_alignment = weapon->m_rafale < 0 ? weapon->GetAngleToleranceForBeam(playership) : ANGLE_TOLERANCE_FOR_FACE_TARGET_ALIGNMENT;
			if (weapon->isFiringReady(deltaTime, hyperspeedMultiplier) == true && m_disable_fire == false && playership->m_collision_timer <= 0)
			{
				if (m_shoot_when_aligned == false || weapon->m_target_homing == HOMING || hasLockingSalvoInProgress == true || abs(delta) < angle_tolerance_for_alignment)
				{
					if (m_isOnScene == true)//forbid enemies to shoot from out of screen
					{
						if (weapon->m_rafale >= 0)//for enemy ships only (counting the number of shots fired)
							m_shots_fired++;

						//Fire!
						weapon->Fire(EnemyFire, deltaTime);
					}
				}
			}
			else if (weapon->m_rafale < 0 && hadBeamsActive == true && weapon->m_beams.empty() == true)//for enemy ships only (counting the number of laser beam shots fired)
			{
				m_shots_fired++;
			}

			//UPDATE BEAMS
			weapon->UpdateBeams(m_disable_fire == false);
		}
	}

	//update timers
	if (playership->m_collision_timer <= 0)
	{
		m_phaseTimer += deltaTime.asSeconds() * l_hyperspeedMultiplier;
		m_enemyTimer += deltaTime.asSeconds() * l_hyperspeedMultiplier;
	}
	
	//shield regen if not maximum
	ShieldRegen(deltaTime, hyperspeedMultiplier);

	//animate
	AnimatedSprite::update(deltaTime);

	//phases
	if (m_phases.empty() == false)
		if (m_currentPhase->m_transitions_list.empty() == false)
			CheckCondition();
}
void Enemy::RotateFeedbacks(float angle)
{
	m_armorBar.setRotation(angle);
	m_armorBarContainer.setRotation(angle);
	m_shieldBar.setRotation(angle);
	m_shieldBarContainer.setRotation(angle);
}

void Enemy::GetDamageFrom(GameObject& object)
{
	Ammo& ammo = (Ammo&)object;

	if (object.m_collider_type == FriendlyFire)
	{
		//display impact FX regardless of damage	
		if (ammo.m_isBeam == false)
			ammo.Death();

		//position of FX impact
		if (ammo.m_isBeam == false || ammo.m_collision_timer <= 0)
		{
			FX* impactFX = ammo.m_explosion->Clone();
			float angle_impact = ammo.getRotation() * M_PI / 180;
			float impact_offset_x = - m_size.y / 2 * sin(angle_impact);
			float impact_offset_y = m_size.y / 2 * cos(angle_impact);
			impactFX->setPosition(getPosition().x + impact_offset_x, getPosition().y + impact_offset_y);

			(*CurrentGame).addToScene(impactFX, true);
		}
	}

	if (object.m_collision_timer <= 0)
	{
		if (ammo.m_area_of_effect == 0)
			GetDamage(object.m_damage);
		else
			(*CurrentGame).DamageObjectsInRange(EnemyObjectLayer, object.getPosition(), ammo.m_area_of_effect, ammo.m_damage);

		if (object.m_collider_type == FriendlyFire)//bullet versus ship
		{
			if (ammo.m_isBeam == false)
				object.m_collision_timer = TIME_BETWEEN_BULLET_DAMAGE_TICK;
			else
				object.m_collision_timer = TIME_BETWEEN_BEAM_DAMAGE_TICK;
		}
		else//collision ship versus ship
			object.m_collision_timer = TIME_BETWEEN_COLLISION_DAMAGE_TICK;
	}
}

void Enemy::GetDamage(int damage)
{
	if (m_immune == true || (*CurrentGame).m_waiting_for_dialog_validation == true || (*CurrentGame).m_waiting_for_scene_transition == true)
		return;

	if (damage == 0)
		return;
	
	m_armorBarContainer.m_visible = true;
	m_armorBar.m_visible = true;
	if (m_shield_max > 0)
	{
		m_shieldBarContainer.m_visible = true;
		m_shieldBar.m_visible = true;
	}
	m_enemyLevel.m_visible = true;
	m_health_feedbackTimer = ENEMY_HEALTH_FEEDBACK_TIME;

	setColor(Color(255, 0, 0, 255), true);
	m_damage_feedbackTimer = m_damage_feedbackTimer <= 0 ? DAMAGE_FEEDBACK_TIME : m_damage_feedbackTimer;

	if (damage > m_shield)
	{
		m_armor -= (damage - m_shield);
		m_shield = 0;
	}
	else
		m_shield -= damage;

	if (m_armor <= 0)
	{
		m_armor == 0;
		Death();
	}
		
}

Enemy* Enemy::Clone()
{
	Enemy* enemy = new Enemy(getPosition(), m_speed, m_textureName, m_size, m_FX_death, m_frameNumber, m_animationNumber);

	((GameObject*)enemy)->m_armor = m_armor;
	((GameObject*)enemy)->m_armor_max = m_armor_max;
	((GameObject*)enemy)->m_shield = m_shield;
	((GameObject*)enemy)->m_shield_max = m_shield_max;
	((GameObject*)enemy)->m_shield_regen = m_shield_regen;
	((GameObject*)enemy)->m_damage = m_damage;

	for (Weapon* weapon : m_weapons_list)
		enemy->m_weapons_list.push_back(weapon->Clone());
	
	((GameObject*)enemy)->addMoney(m_money);
	enemy->m_equipment_loot = getEquipmentLoot();
	enemy->m_weapon_loot = getWeaponLoot();
	enemy->m_display_name = m_display_name;
	enemy->m_enemy_class = m_enemy_class;
	enemy->setAnimationLine(m_currentAnimationIndex);

	enemy->m_pattern = m_pattern;
	enemy->m_angspeed = m_angspeed;
	enemy->m_radius = m_radius;
	enemy->m_input_blocker = m_input_blocker;

	enemy->m_rotation_speed = m_rotation_speed;
	enemy->setRotation(getRotation());

	for (Phase* phase : m_phases)
		enemy->m_phases.push_back(phase);

	//if (m_currentPhase != NULL)
	//	enemy->m_currentPhase = enemy->m_phases.front();

	enemy->m_level = m_level;
	enemy->m_enemyLevel.setString(to_string(m_level));

	return enemy;
}

Phase* Enemy::getPhase(string phaseName)
{
	for (Phase* phase : m_phases)
		if (phase->m_name.compare(phaseName) == 0)
			return phase;

	return NULL;
}

bool Enemy::CheckCondition()
{
	GameObject* playership = (GameObject*)(*CurrentGame).m_playership;

	for (ConditionTransition* cond : m_currentPhase->m_transitions_list)
	{
		FloatCompare result = ERROR_COMPARE;

		//compute results
		switch (cond->m_condition)
		{
			case VerticalPosition:
			{
				result = compare_posY_withTarget_for_Direction(DIRECTION_UP, sf::Vector2f(cond->m_value / SCENE_SIZE_Y * SCENE_SIZE_X, cond->m_value));
				break;
			}
			case HorizontalPosition:
			{
				result = compare_posX_withTarget_for_Direction(DIRECTION_UP, sf::Vector2f(cond->m_value, cond->m_value / SCENE_SIZE_X * SCENE_SIZE_Y));
				break;
			}
			case PlayerVerticalPosition:
			{
				result = playership->compare_posY_withTarget_for_Direction(DIRECTION_UP, sf::Vector2f(cond->m_value / SCENE_SIZE_Y * SCENE_SIZE_X, cond->m_value));
				break;
			}
			case PlayerHorizontalPosition:
			{
				result = playership->compare_posX_withTarget_for_Direction(DIRECTION_UP, sf::Vector2f(cond->m_value, cond->m_value / SCENE_SIZE_X * SCENE_SIZE_Y));
				break;
			}
			case phaseClock:
			{
				if (m_phaseTimer == cond->m_value)
					result = EQUAL_TO;
				else
					result = m_phaseTimer > cond->m_value ? GREATER_THAN : LESSER_THAN;
				break;
			}
			case enemyClock:
			{
				if (m_enemyTimer == cond->m_value)
					result = EQUAL_TO;
				else
					result = m_enemyTimer > cond->m_value ? GREATER_THAN : LESSER_THAN;
				break;
			}
			case LifePourcentage:
			{
				if (cond->m_value == m_armor)//case of "death" condition handled in method Death(), when the enemy dies precisely
					result = EQUAL_TO;
				else
					result = 100.0f * m_armor / m_armor_max >= cond->m_value ? GREATER_THAN : LESSER_THAN;
				break;
			}
			case ShieldPourcentage:
			{
				if (m_shield_max == 0)//careful of the case where shield_max == 0, we don't want to be dividing by 0
					result = cond->m_value == 0 ? EQUAL_TO : LESSER_THAN;
				else
					result = 100.0f * m_shield / m_shield_max >= cond->m_value ? GREATER_THAN : LESSER_THAN;
				break;
			}
			case wakeUp:
			{
				if (m_wake_up == true)
					result = EQUAL_TO;
				break;
			}
			case EnemyProximity:
			{
				GameObject* object_found = (*CurrentGame).GetNearestGameObject(PlayerShip, getPosition(), cond->m_value);
				if (object_found != NULL)
					result = EQUAL_TO;
				else
					result = GREATER_THAN;
				break;
			}
			case ShotsFired:
			{
				if (m_shots_fired == cond->m_value)
					result = EQUAL_TO;
				else
					result = m_shots_fired > cond->m_value ? GREATER_THAN : LESSER_THAN;
				break;
			}
			case Rotation:
			{
				if (getRotation() == cond->m_value)
					result = EQUAL_TO;
				else
					result = getRotation() > cond->m_value ? GREATER_THAN : LESSER_THAN;
				break;
			}
		}

		//check results
		if (result == EQUAL_TO || (cond->m_op == GREATER_THAN && result == GREATER_THAN) || (cond->m_op == LESSER_THAN && result == LESSER_THAN))
		{
			setPhase(getPhase(cond->m_nextPhase_name));
			return true;
		}
	}

	return false;
}

void Enemy::setPhase(Phase* phase)
{
	GameObject* playership = (GameObject*)(*CurrentGame).m_playership;

	m_shots_fired = 0;

	m_speed = sf::Vector2f(0, phase->m_vspeed);

	//reset old stats
	m_immune = false;
	m_face_target = false;
	m_reset_facing = false;
	m_shoot_when_aligned = false;
	m_bouncing = NoBouncing;

	setGhost(false);

	//load new stats
	for (int i = 0; i < phase->m_modifiers.size(); i++)
	{
		switch (phase->m_modifiers[i])
		{
			case NoModifier:
			{
				//do nothing
				break;
			}
			case Immune:
			{
				m_immune = true;
				break;
			}
			case GhostModifier:
			{
				setGhost(true);
				break;
			}
			case DeathModifier:
			{
				Death();
				break;
			}
			case FaceTarget:
			{
				m_face_target = true;
				break;
			}
			case ResetFacing:
			{
				m_reset_facing = true;
				break;
			}
			case ShootWhenAligned:
			{
				m_shoot_when_aligned = true;
				break;
			}
			case Bouncing:
			{
				m_bouncing = BouncingEverywhere;
				break;
			}
			case BouncingH:
			{
				m_bouncing = BouncingHorizontal;
				break;
			}
			case BouncingV:
			{
				m_bouncing = BouncingVertical;
				break;
			}
			case FreezePlayer:
			{
				phase->m_freeze_player = true;
				break;
			}
			case KillBullets:
			{
				(*CurrentGame).killGameObjectType(EnemyFire);
				break;
			}
			default:
				break;
		}
	}

	m_rotation_speed = phase->m_rotation_speed;

	//WEAPONS
	//loading phase weapons, keeping existing weapons when they are the same and delay is 0 (which avoids resetting their readyFireTimer)
	vector<Weapon*> new_weapons_list;
	for (Weapon* weapon_phase : phase->m_weapons_list)
	{
		bool kept = false;
		for (Weapon* weapon : m_weapons_list)
		{
			bool found = false;
			if (weapon_phase->m_display_name.compare(weapon->m_display_name) == 0 && weapon_phase->m_delay == 0)//same kind of weapon?
			{
				for (Weapon* weapon_kept : new_weapons_list)//not already saved?
				{
					if (weapon_kept == weapon)
					{
						found = true;
						break;
					}
				}

				if (found == false)
				{
					new_weapons_list.push_back(weapon);//save it
					weapon->m_delay = weapon_phase->m_delay;//copy delay (useful when new delay is 0 so there is a continuity with previous weapons)
					kept = true;
					break;
				}
			}
		}
		
		if (kept == false)
			new_weapons_list.push_back(weapon_phase->Clone());
	}

	//clearing old weapons not kept
	for (Weapon* weapon : m_weapons_list)
	{
		bool found = false;
		for (Weapon* new_weapon : new_weapons_list)
		{
			if (weapon == new_weapon)
			{
				found = true;
				break;
			}
		}

		if (found == false)
			delete weapon;
	}

	//clear list and loading new list
	m_weapons_list.clear();
	for (Weapon* weapon : new_weapons_list)
		m_weapons_list.push_back(weapon);
		
	//MOVEMENT PATTERN
	m_pattern.setPattern_v2(phase->m_pattern); //vitesse angulaire (degres/s)

	//welcome shot: shot once at the beginning of the phase (actually used as a post-mortem "good-bye"shoot)
	if (phase->m_welcomeWeapon != NULL)
	{
		float theta = this->getRotation() / 180 * M_PI;
		float weapon_offset_x = phase->m_welcomeWeapon->m_weaponOffset.x - m_size.y / 2 * sin(theta);
		float weapon_offset_y = phase->m_welcomeWeapon->m_weaponOffset.y + m_size.y / 2 * cos(theta);

		phase->m_welcomeWeapon->setPosition(this->getPosition().x + weapon_offset_x, this->getPosition().y + weapon_offset_y);
		phase->m_welcomeWeapon->m_shot_angle = theta;

		phase->m_welcomeWeapon->Fire(EnemyFire);
	}

	//setting up wake_up condition
	m_wake_up = false;

	//waking up enemies
	if (phase->m_wake_up_name.empty() == false)
		(*CurrentGame).WakeUpEnemiesWithName(phase->m_wake_up_name);

	//dialogs
	if (!phase->m_dialogs.empty())
	{
		if (!(*CurrentGame).m_waiting_for_dialog_validation)
		{
			playership->SetAskingPanel(SFPanel_Dialog);
			size_t dialogsVectorSize = phase->m_dialogs.size();
			for (size_t i = 0; i < dialogsVectorSize; i++)
				playership->AddDialog(phase->m_dialogs[i]->Clone());
		}
	}

	//freeze player?
	if (phase->m_freeze_player)
		playership->SetInputBlocker(this);//lock player
	else
		if (playership->GetInputBlocker() == this)//unlock player
			playership->SetInputBlocker(NULL);

	m_phaseTimer = 0;

	m_currentPhase = phase;
}

Dialog* Enemy::LoadDialog(string name)
{
	vector<vector<string> > dialogConfig = *(FileLoaderUtils::FileLoader(DIALOGS_FILE));

	for (std::vector<vector<string> >::iterator it = (dialogConfig).begin(); it != (dialogConfig).end(); it++)
	{
		if ((*it)[DIALOG_NAME].compare(name) == 0)
		{
			Dialog* dialog = new Dialog();

			dialog->m_name = name;
			dialog->m_fade_in = (bool)(stoi((*it)[DIALOG_FADE_IN]));
			dialog->m_fade_out = (bool)(stoi((*it)[DIALOG_FADE_OUT]));
			dialog->m_enemy_speaking = (*it)[DIALOG_ENEMY_SPEAKING].compare("0") != 0;
			dialog->m_duration = atof((*it)[DIALOG_DURATION].c_str());
			dialog->m_title = (*it)[DIALOG_TITLE];
			dialog->m_title = ReplaceAll(dialog->m_title, "_", " ");
			dialog->m_title = ReplaceAll(dialog->m_title, "[COMA]", ",");
			dialog->m_body = (*it)[DIALOG_BODY];
			dialog->m_body = ReplaceAll(dialog->m_body, "_", " ");
			dialog->m_body = ReplaceAll(dialog->m_body, "[COMA]", ",");
			dialog->m_picture_name = (*it)[DIALOG_PICTURE];
			dialog->m_next_dialog_name = (*it)[DIALOG_NEXT];

			TextureLoader *loader = TextureLoader::getInstance();
			loader->loadTexture(dialog->m_picture_name, DIALOG_PANEL_PORTRAIT_SIZE_X, DIALOG_PANEL_PORTRAIT_SIZE_X);

			return dialog;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Dialog '%s'. Please check the config file", (char*)name.c_str()));
}

Phase* Enemy::LoadPhase(string name)
{
	vector<vector<string> > phaseConfig = *(FileLoaderUtils::FileLoader(PHASES_FILE));

	for (std::vector<vector<string> >::iterator it = (phaseConfig).begin(); it != (phaseConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Phase* phase = new Phase();

			phase->m_name = name;
			phase->m_display_name = (*it)[PHASE_NAME];
			phase->m_vspeed = stoi((*it)[PHASE_VSPEED]);

			//loading weapons and ammos
			for (int i = 0; i < 6; i++)
			{
				if ((*it)[PHASE_WEAPON + (i * 3)].compare("0") != 0)
				{
					Weapon* m_weapon = Enemy::LoadWeapon((*it)[PHASE_WEAPON + (i * 3)], 1);
					m_weapon->m_weaponOffset.x = atof((*it)[PHASE_WEAPON_OFFSET + (i * 3)].c_str());
					m_weapon->m_delay = atof((*it)[PHASE_WEAPON_DELAY + (i * 3)].c_str());
					phase->m_weapons_list.push_back(m_weapon);
				}
			}

			//loading phases
			GeometryPattern* pattern = GeometryPattern::LoadPattern((*it), PHASE_PATTERN);
			phase->m_pattern = pattern;

			//loading rotation speed
			phase->m_rotation_speed = stoi((*it)[PHASE_ROTATION_SPEED]);

			//loading modifier (immune to damage, etc.)
			for (int i = 0; i < 3; i++)
			{
				Modifier l_new_modifier = NoModifier;
				if ((*it)[PHASE_MODIFIER + i].compare("0") != 0)
				{
					if ((*it)[PHASE_MODIFIER + i].compare("immune") == 0)
						l_new_modifier = Immune;
					else if ((*it)[PHASE_MODIFIER + i].compare("ghost") == 0)
						l_new_modifier = GhostModifier;
					else if ((*it)[PHASE_MODIFIER + i].compare("death") == 0)
						l_new_modifier = DeathModifier;
					else if ((*it)[PHASE_MODIFIER + i].compare("face_target") == 0)
						l_new_modifier = FaceTarget;
					else if ((*it)[PHASE_MODIFIER + i].compare("reset_facing") == 0)
						l_new_modifier = ResetFacing;
					else if ((*it)[PHASE_MODIFIER + i].compare("shoot_when_aligned") == 0)
						l_new_modifier = ShootWhenAligned;
					else if ((*it)[PHASE_MODIFIER + i].compare("reset_facing") == 0)
						l_new_modifier = ResetFacing;
					else if ((*it)[PHASE_MODIFIER + i].compare("bouncing") == 0)
						l_new_modifier = Bouncing;
					else if ((*it)[PHASE_MODIFIER + i].compare("bouncingH") == 0)
						l_new_modifier = BouncingH;
					else if ((*it)[PHASE_MODIFIER + i].compare("bouncingV") == 0)
						l_new_modifier = BouncingV;
					else if ((*it)[PHASE_MODIFIER + i].compare("freeze") == 0)
						l_new_modifier = FreezePlayer;
					else if ((*it)[PHASE_MODIFIER + i].compare("kill_bullets") == 0)
						l_new_modifier = KillBullets;
				}

				phase->m_modifiers.push_back(l_new_modifier);
			}

			//loading welcome shot
			if ((*it)[PHASE_WELCOME_WEAPON].compare("0") != 0)
				phase->m_welcomeWeapon = Enemy::LoadWeapon((*it)[PHASE_WELCOME_WEAPON], 1);
			
			//load enemies (by name) to wake up
			if ((*it)[PHASE_WAKEUP].compare("0") != 0)
				phase->m_wake_up_name = (*it)[PHASE_WAKEUP];

			//loading transition to next phase
			if ((*it)[PHASE_CONDITION].compare("0") != 0)
				phase->m_transitions_list.push_back(Phase::ConditionLoader((*it), PHASE_CONDITION));

			if ((*it)[PHASE_CONDITION_2].compare("0") != 0)
				phase->m_transitions_list.push_back(Phase::ConditionLoader((*it), PHASE_CONDITION_2));

			//loading dialogs
			if ((*it)[PHASE_DIALOG_NAME].compare("0") != 0)
			{
				Dialog* dialog = Enemy::LoadDialog((*it)[PHASE_DIALOG_NAME]);
				phase->m_dialogs.push_back(dialog);
				while (!dialog->m_next_dialog_name.empty() && dialog->m_next_dialog_name.compare("0") != 0)
				{
					dialog = Enemy::LoadDialog(dialog->m_next_dialog_name);
					phase->m_dialogs.push_back(dialog);
				}
			}

			return phase;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find EnemyPhase '%s'. Please check the config file", (char*)name.c_str()));
}

void Enemy::Death()
{
	GameObject* playership = (GameObject*)(*CurrentGame).m_playership;

	FX* myFX = m_FX_death->Clone();
	myFX->setPosition(getPosition().x, getPosition().y);
	(*CurrentGame).addToScene(myFX, true);

	//Combo
	//playership->AddComboCount(m_enemy_class * 100);

	//Score
	//(*CurrentGame).m_hazard += m_money;
	//if (!m_isOnScene)
	//{
	//	(*CurrentGame).m_hazardSpawned -= m_money;//can only happen when using debugs or scripts to wipe all enemies even offscreen
	//}

	//Player XP
	//(*CurrentGame).m_playership->gain_xp(XPTable_PerEnemyClass[this->enemy_class]);

	//Loot
	//if (CreateRandomLootv2(m_enemy_class, (*CurrentGame).m_BeastScoreBonus))
	//{
	//	GenerateLoot();
	//}

	playership->addMoney(m_money);

	GameObject::Death();

	if (m_enemy_class < ENEMYPOOL_DELTA)
	{
		(*CurrentGame).PlaySFX(SFX_Kill);
	}
	else
	{
		(*CurrentGame).PlaySFX(SFX_BigKill);
	}

	//unlock player if blocked
	if (playership->GetInputBlocker() == this)
	{
		playership->SetInputBlocker(NULL);
	}

	//phase transition "Death" (post-mortem phase of 1 frame)
	if (m_currentPhase != NULL)
		for (ConditionTransition* transition : m_currentPhase->m_transitions_list)
			if (transition->m_condition == LifePourcentage && transition->m_value == 0 && (transition->m_op == LESSER_THAN || transition->m_op == EQUAL_TO))
			{
				setPhase(getPhase(transition->m_nextPhase_name));
				break;
			}
}

Enemy::~Enemy()
{
	m_armorBar.GarbageMe();
	m_armorBarContainer.GarbageMe();
	m_shieldBar.GarbageMe();
	m_shieldBarContainer.GarbageMe();
	m_enemyLevel.GarbageMe();

	for (Weapon* weapon : m_weapons_list)
		delete weapon;

	//Destroyed in ~EnemyBase after Scene~; for optimization
	//for (Phase* phase : m_phases)
	//	delete phase;
	//
	//delete m_FX_death;
}

void Enemy::GenerateLoot()
{
	sf::Vector2f speed = sf::Vector2f(0, LOOT_SPEED_Y);

	if (m_weapon_loot != NULL)
	{
		Loot* new_loot = new Loot(getPosition(), speed, m_weapon_loot->m_textureName, m_weapon_loot->m_size, m_weapon_loot->m_display_name);
		new_loot->get_weapon_from(*this);
		(*CurrentGame).addToScene((GameObject*)new_loot, true);
	}

	else if (m_equipment_loot != NULL)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, m_equipment_loot->m_textureName, m_equipment_loot->m_size, m_equipment_loot->m_display_name);
		new_loot->get_equipment_from(*this);
		(*CurrentGame).addToScene((GameObject*)new_loot, true);
	}

	else if (m_money > 0)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH), "Money");
		new_loot->setMoney(m_money);
		(*CurrentGame).addToScene((GameObject*)new_loot, true);
	}
	else
	{
		//que dalle
	}
}

bool Enemy::CreateRandomLootv2(EnemyClass loot_class, float BeastScaleBonus, bool force_BeastScale, float BeastScale_min, float BeastScale_max)
{

	////Debug commands
	//#ifndef NDEBUG
	//	Enemy::AssignRandomEquipment(Engine, 30, this, 6.f);
	//	return true;
	//#endif

	setMoney(0);
	double random_number = (double)rand() / (RAND_MAX);

	if (random_number > LootTable_DroppingSomething[loot_class])
	{
		//EMPTY DROP
		return false;
	}
	else
	{
		//setting min and max beast score values
		sf::Vector2f LootTable_BeastScale[NBVAL_EnemyClass];
		for (int b = 0; b < NBVAL_EnemyClass; b++)
		{
			if (b != ENEMYPOOL_VOID && b != ENEMYPOOL_ZETA)
			{
				LootTable_BeastScale[b].x = LootTable_BeastScale_Base[b].x + BeastScaleBonus;
				LootTable_BeastScale[b].y = LootTable_BeastScale_Base[b].y + BeastScaleBonus;
			}
			else
			{
				LootTable_BeastScale[b].x = 0;
				LootTable_BeastScale[b].y = 0;
			}
		}

		//ITEM DROP
		if (random_number < LootTable_DropIsEquipment[loot_class] * LootTable_DroppingSomething[loot_class])
		{
			//Beast Scale Score randomized here within the min and max
			float BeastScaleScore = RandomizeFloatBetweenValues(LootTable_BeastScale[loot_class].x, LootTable_BeastScale[loot_class].y);

			//Calculation of the bonus "credits" for the loot
			int loot_credits_ = ceil(BeastScaleScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(m_level + 1));

			//Equipment type
			int equipment_type_roll = rand() % ((int)NBVAL_Equipment + 1);//+1 is for the weapon type

			//"Spending credits" on item stats and assigning the equipment/weapon as a loot
			AssignRandomEquipment((EquipmentType)equipment_type_roll, m_level + 1, this, BeastScaleScore);
		}

		else
		{
			//MONEY DROP
			int money = RandomizeIntBetweenRatios(1.0f * XPTable_PerEnemyClass[loot_class] * (*CurrentGame).GetEnemiesStatsMultiplierForLevel(m_level) / 100, LootTable_BeastScale[loot_class]);
			this->m_money = money;//looting money
		}
	}

	return true;
}

bool Enemy::AssignRandomEquipment(EquipmentType equipment_type, int level, GameObject* object, float beastScore)
{
	switch (equipment_type)
	{
		case (int)Engine:
			return object->setEquipmentLoot(Enemy::CreateRandomEngine(level, beastScore));

		case (int)Armor:
			return object->setEquipmentLoot(Enemy::CreateRandomArmor(level, beastScore));

		case (int)Shield:
			return object->setEquipmentLoot(Enemy::CreateRandomShield(level, beastScore));

		case (int)Module:
			return object->setEquipmentLoot(Enemy::CreateRandomModule(level, beastScore));

		case (int)NBVAL_Equipment://WEAPON DROP
			return object->setWeaponLoot(Enemy::CreateRandomWeapon(level, false, beastScore));
	}

	return false;
}

int Enemy::GetChosenProperty(vector<int> *properties_roll_table, int properties_to_choose_from, int p)
{
	int index = rand() % (properties_to_choose_from - p);
	int chosen_property = (*properties_roll_table)[index];

	int a = properties_roll_table->back();
	(*properties_roll_table)[properties_roll_table->size() - 1] = chosen_property;
	(*properties_roll_table)[index] = a;
	return chosen_property;
}

Weapon* Enemy::LoadWeapon(string name, int fire_direction)
{
	vector<vector<string> > weaponConfig = *(FileLoaderUtils::FileLoader(WEAPON_FILE));

	for (std::vector<vector<string> >::iterator it = weaponConfig.begin(); it != weaponConfig.end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Weapon* weapon = new Weapon(Enemy::LoadAmmo((*it)[WEAPON_AMMO]));
			weapon->m_display_name = (*it)[WEAPON_DISPLAY_NAME];
			weapon->m_fire_direction = fire_direction;
			weapon->m_rate_of_fire = atof((*it)[WEAPON_RATE_OF_FIRE].c_str());
			weapon->m_shot_mode = NoShotMode;

			weapon->m_ammunition->m_damage = stoi((*it)[WEAPON_DAMAGE]);
			weapon->m_ammunition->m_ref_speed = (float)stoi((*it)[WEAPON_SPEED]);
			weapon->m_ammunition->m_speed = sf::Vector2f(0, weapon->m_ammunition->m_ref_speed);
			weapon->m_ammunition->m_range = (float)stoi((*it)[WEAPON_RANGE]);

			GeometryPattern* pattern = GeometryPattern::LoadPattern((*it), WEAPON_PATTERN);
			weapon->m_ammunition->m_pattern.setPattern_v2(pattern);
			delete pattern;

			weapon->m_multishot = stoi((*it)[WEAPON_MULTISHOT]);
			if (weapon->m_multishot > 1)
			{
				weapon->m_xspread = stoi((*it)[WEAPON_XSPREAD]);
				weapon->m_dispersion = stoi((*it)[WEAPON_DISPERSION]);
				if ((*it)[WEAPON_ALTERNATE].compare("0") != 0)
				{
					if ((*it)[WEAPON_ALTERNATE].compare("alternate") == 0)
						weapon->m_shot_mode = AlternateShotMode;
					if ((*it)[WEAPON_ALTERNATE].compare("ascending") == 0)
						weapon->m_shot_mode = AscendingShotMode;
					if ((*it)[WEAPON_ALTERNATE].compare("descending") == 0)
						weapon->m_shot_mode = DescendingShotMode;
					if ((*it)[WEAPON_ALTERNATE].compare("ascending2") == 0)
						weapon->m_shot_mode = Ascending2ShotMode;
					if ((*it)[WEAPON_ALTERNATE].compare("descending2") == 0)
						weapon->m_shot_mode = Descending2ShotMode;
				}
			}
			
			weapon->m_rafale = stoi((*it)[WEAPON_RAFALE]);
			if (weapon->m_rafale != 0)
			{
				weapon->m_rafale_cooldown = atof((*it)[WEAPON_RAFALE_COOLDOWN].c_str());
				weapon->m_rafale_locking = (bool)(stoi((*it)[WEAPON_RAFALE_LOCKING]));

				if (weapon->m_rafale < 0)
					weapon->m_ammunition->m_isBeam = true;
			}
				
			weapon->m_textureName = (*it)[WEAPON_IMAGE_NAME];
			weapon->m_size = sf::Vector2f((float)stoi((*it)[WEAPON_WIDTH]), (float)stoi((*it)[WEAPON_HEIGHT]));
			weapon->m_frameNumber = stoi((*it)[WEAPON_FRAMES]);
			weapon->m_angle_offset = (float)stoi((*it)[WEAPON_ANGLE_OFFSET]);

			if ((*it)[WEAPON_TARGET_HOMING].compare("0") != 0)
			{
				if ((*it)[WEAPON_TARGET_HOMING].compare("semi_homing") == 0)
					weapon->m_target_homing = SEMI_HOMING;
				else if ((*it)[WEAPON_TARGET_HOMING].compare("homing") == 0)
					weapon->m_target_homing = HOMING;
			}

			return weapon;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Weapon '%s'. Please check the config file", (char*)name.c_str()));
}

Ammo* Enemy::LoadAmmo(string name)
{
	vector<vector<string> > ammoConfig = *(FileLoaderUtils::FileLoader(AMMO_FILE));

	for (std::vector<vector<string> >::iterator it = (ammoConfig).begin(); it != (ammoConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Ammo* new_ammo = new Ammo(Vector2f(0, 0), Vector2f(0, 0), (*it)[AMMO_IMAGE_NAME],
				Vector2f((float)stoi((*it)[AMMO_WIDTH]), (float)stoi((*it)[AMMO_HEIGHT])), stoi((*it)[AMMO_FRAMES]), stoi((*it)[AMMO_NB_SKINS]), 0, LoadFX((*it)[AMMO_FX]), (bool)stoi((*it)[AMMO_MISSILE_MODEL]));

			new_ammo->setAnimationLine(stoi((*it)[AMMO_SKIN]) - 1);
			new_ammo->m_display_name = (*it)[AMMO_NAME];
			new_ammo->m_sound_name = (*it)[AMMO_SOUND];

			if (!(*it)[AMMO_FX].empty())
				new_ammo->m_explosion->m_display_name = (*it)[AMMO_FX];

			new_ammo->m_rotation_speed = (float)stoi((*it)[AMMO_ROTATION_SPEED]);
			new_ammo->m_area_of_effect = (float)stoi((*it)[AMMO_AREA_OF_EFFECT]);
			return new_ammo;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ammo '%s'. Please check the config file", (char*)name.c_str()));
}

FX* Enemy::LoadFX(string name)
{
	vector<vector<string> >FXConfig = *(FileLoaderUtils::FileLoader(FX_FILE));

	for (std::vector<vector<string> >::iterator it = FXConfig.begin(); it != FXConfig.end(); it++)
	{
		if ((*it)[FX_TYPE].compare("explosion") == 0)
		{
			if ((*it)[FX_NAME].compare(name) == 0)
			{
				float duration = atof(((*it)[FX_DURATION]).c_str());
				FX* myFX = new FX(Vector2f(0, 0), Vector2f(0, 0), (*it)[FX_FILENAME], Vector2f((float)stoi((*it)[FX_WIDTH]), (float)stoi((*it)[FX_HEIGHT])), stoi((*it)[FX_FRAMES]), sf::seconds(duration));
				myFX->m_display_name = (*it)[FX_NAME];

				return myFX;
			}
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find FX '%s'. Please check the config file", (char*)name.c_str()));
}

Bot* Enemy::LoadBot(string name)
{
	vector<vector<string> >botConfig = *(FileLoaderUtils::FileLoader(BOT_FILE));

	for (std::vector<vector<string> >::iterator it = (botConfig).begin(); it != (botConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			Bot* bot = new Bot(Vector2f(0, 0), Vector2f(0, 0), (*it)[BOT_IMAGE_NAME], sf::Vector2f((float)stoi((*it)[BOT_WIDTH]), (float)stoi((*it)[BOT_HEIGHT])));

			((GameObject*)bot)->m_display_name = (*it)[BOT_NAME];
			((GameObject*)bot)->m_armor = stoi((*it)[BOT_ARMOR]);
			((GameObject*)bot)->m_armor_max = stoi((*it)[BOT_ARMOR]);
			((GameObject*)bot)->m_shield = stoi((*it)[BOT_SHIELD]);
			((GameObject*)bot)->m_shield_max = stoi((*it)[BOT_SHIELD]);
			((GameObject*)bot)->m_shield_regen = stoi((*it)[BOT_SHIELD_REGEN]);
			((GameObject*)bot)->m_damage = stoi((*it)[BOT_DAMAGE]);
			bot->m_spread = Vector2f(stoi((*it)[BOT_XSPREAD]), stoi((*it)[BOT_YSPREAD]));

			GeometryPattern* pattern = GeometryPattern::LoadPattern((*it), BOT_PATTERN);
			bot->m_pattern.setPattern_v2(pattern);
			delete pattern;

			bot->m_rotation_speed = stoi((*it)[BOT_ROTATION_SPEED]);

			if ((*it)[BOT_WEAPON].compare("0") != 0)
				bot->m_weapon = Enemy::LoadWeapon((*it)[BOT_WEAPON], -1);

			return bot;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Bot '%s'. Please check the config file", (char*)name.c_str()));
}

Equipment* Enemy::LoadEquipment(string name)
{
	vector<vector<string> >equipmentConfig = *(FileLoaderUtils::FileLoader(EQUIPMENT_FILE));

	for (std::vector<vector<string> >::iterator it = (equipmentConfig).begin(); it != (equipmentConfig).end(); it++)
	{
		if ((*it)[EQUIPMENT_NAME].compare(name) == 0)
		{
			Equipment* i = new Equipment();

			i->Init(NBVAL_Equipment, stoi((*it)[EQUIPMENT_MAXSPEED]), stoi((*it)[EQUIPMENT_ACCELERATION]), stoi((*it)[EQUIPMENT_DECCELERATION]),
				stoi((*it)[EQUIPMENT_HYPERSPEED]), stoi((*it)[EQUIPMENT_HYPERSPEED_FUEL]), stoi((*it)[EQUIPMENT_ARMOR]), stoi((*it)[EQUIPMENT_SHIELD]), stoi((*it)[EQUIPMENT_SHIELD_REGEN]),
				atof((*it)[EQUIPMENT_SHIELD_RECOVERY].c_str()), stoi((*it)[EQUIPMENT_DAMAGE]),
				(*it)[EQUIPMENT_IMAGE_NAME], Vector2f(stoi((*it)[EQUIPMENT_WIDTH]), stoi((*it)[EQUIPMENT_HEIGHT])),
				stoi((*it)[EQUIPMENT_FRAMES]), (*it)[EQUIPMENT_NAME]);

			if ((*it)[EQUIPMENT_BOT].compare("0") != 0)
				i->m_bots.push_back(LoadBot((*it)[EQUIPMENT_BOT]));

			if (!(*it)[EQUIPMENT_FAKE_TEXTURE].compare("0") == 0 && !(*it)[EQUIPMENT_FAKE_WIDTH].compare("0") == 0
				&& !(*it)[EQUIPMENT_FAKE_HEIGHT].compare("0") == 0 && !(*it)[EQUIPMENT_FAKE_FRAMES].compare("0") == 0)
			{
				i->m_fake_textureName = (*it)[EQUIPMENT_FAKE_TEXTURE];
				i->m_fake_size = sf::Vector2f(stoi((*it)[EQUIPMENT_FAKE_WIDTH]), stoi((*it)[EQUIPMENT_FAKE_HEIGHT]));
				i->m_fake_frameNumber = stoi((*it)[EQUIPMENT_FAKE_FRAMES]);
			}

			//if((*it)[EQUIPMENT_COMPARE].compare("airbrake") == 0)
			//	i->equipmentType = Airbrake;
			if ((*it)[EQUIPMENT_COMPARE].compare("engine") == 0)
				i->m_equipmentType = Engine;
			else if ((*it)[EQUIPMENT_COMPARE].compare("armor") == 0)
				i->m_equipmentType = Armor;
			else if ((*it)[EQUIPMENT_COMPARE].compare("shield") == 0)
				i->m_equipmentType = Shield;
			else if ((*it)[EQUIPMENT_COMPARE].compare("module") == 0)
				i->m_equipmentType = Module;
			else
				LOGGER_WRITE(Logger::DEBUG, ("Equipment config file error: cannot find a valid equipment type for: '%s'. Please check the config file", (char*)name.c_str()));

			i->m_display_name = (*it)[EQUIPMENT_DISPLAY_NAME];

			return i;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Equipment '%s'. Please check the config file", (char*)name.c_str()));
}

void Enemy::ApplyLevelModifiers()
{
	float multiplier_ = ceil(1.0f * (*CurrentGame).GetEnemiesStatsMultiplierForLevel(m_level) / 100);

	m_armor_max = floor(m_armor_max * multiplier_);
	m_armor = m_armor_max;
	m_shield_max = floor(m_shield_max * multiplier_);
	m_shield = m_shield_max;
	m_shield_regen = floor(m_shield_regen * multiplier_);
	m_damage = floor(m_damage * multiplier_);
	for (std::vector<Weapon*>::iterator it = m_weapons_list.begin(); it != m_weapons_list.end(); it++)
	{
		(*it)->m_ammunition->m_damage = ceil((*it)->m_ammunition->m_damage * multiplier_);
	}
	this->setMoney(m_money *= 1.0f * (*CurrentGame).GetEnemiesStatsMultiplierForLevel(m_level) / 100);

	m_enemyLevel.setString(to_string(m_level));
}

Weapon* Enemy::CreateRandomWeapon(int level, bool is_bot, float beastScore)
{
	//beam weapon?
	bool beam_weapon = RandomizeFloatBetweenValues(0, 1) < WEAPON_CHANCE_OF_BEAM;

	//Creating the item with base stats
	Weapon* weapon = beam_weapon == false ? Enemy::LoadWeapon("laser", -1) : Enemy::LoadWeapon("laserbeam", -1);

	//Computing credits available for upgrades
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));

	int cost_per_multishot = 0;
	//Spending credits on the possible bonuses
	int bonus_multishot = 0;
	int bonus_damage = 0;
	int bonus_rate_of_fire = 0;
	int dispersion = 0;//flag

	int loot_credits_remaining = credits_;
	loot_credits_remaining *= is_bot ? BOT_STATS_MULTIPLIER : 1;
	while (loot_credits_remaining > 0)
	{
		int random_type_of_bonus = -1;

		//checking bonus limitations
		cost_per_multishot = floor(CREDITS_COST_PER_ONE_MULTISHOT * pow((1 + COST_PER_ONE_MULTISHOT_MULTIPLIER_PER_LEVEL), bonus_multishot));
		bool can_buy_multishot = loot_credits_remaining > cost_per_multishot;
		bool can_buy_rate_of_fire = bonus_rate_of_fire < MAX_RATE_OF_FIRE_BONUS;
		bool can_buy_damage = floor(weapon->m_ammunition->m_damage * (1 + (1.0f * loot_credits_remaining / 100))) != weapon->m_ammunition->m_damage;

		//and chosing among the authorized ones
		if (!can_buy_damage && can_buy_rate_of_fire)
			random_type_of_bonus = 2;
		else if (can_buy_multishot && can_buy_rate_of_fire)
			random_type_of_bonus = RandomizeIntBetweenValues(0, 2);
		else if (!can_buy_multishot && can_buy_rate_of_fire)
			random_type_of_bonus = RandomizeIntBetweenValues(1, 2);
		else if (can_buy_multishot && !can_buy_rate_of_fire)
			random_type_of_bonus = RandomizeIntBetweenValues(0, 1);
		else
			random_type_of_bonus = 1;


		//spending the credits in the chosen bonus
		switch (random_type_of_bonus)
		{
		case 0://multishot
		{
			loot_credits_remaining -= cost_per_multishot;
			bonus_multishot++;
			break;
		}
		case 1://flat bonus damage
		{
			loot_credits_remaining--;
			bonus_damage++;
			break;
		}
		case 2://rate of fire
		{
			loot_credits_remaining--;
			bonus_rate_of_fire++;
			break;
		}
		default:
			break;
		}

		//dispersion weapon?
		int min_multishots_for_dispersion = is_bot ? MIN_MULTISHOTS_FOR_DISPERSION_FOR_BOT - MIN_VALUE_OF_MULTISHOT : MIN_MULTISHOTS_FOR_DISPERSION - MIN_VALUE_OF_MULTISHOT;
		if (bonus_multishot == min_multishots_for_dispersion && dispersion == 0)
			dispersion = RandomizeFloatBetweenValues(0, 1) < WEAPON_CHANCE_OF_DISPERSION ? 1 : -1;
	}

	//allocating bonuses to the weapon
	weapon->m_multishot += bonus_multishot;
	weapon->m_ammunition->m_damage += ceil((bonus_damage + cost_per_multishot * bonus_multishot) * weapon->m_ammunition->m_damage / weapon->m_multishot * 0.01);
	weapon->m_rate_of_fire = weapon->m_rate_of_fire / (1 + bonus_rate_of_fire * 0.01);

	//spread of multishot weapons
	if (weapon->m_multishot > 1)
	{
		//int sprite_size = is_bot ? ASSUMED_BOT_SIZE : ASSUMED_SHIP_SIZE;
		//weapon->m_xspread = RandomizeIntBetweenValues(MIN_WEAPON_XSPREAD, sprite_size * 2 / weapon->m_multishot);
		weapon->m_xspread = MIN_WEAPON_XSPREAD;

		if (dispersion > 0)
		{
			weapon->m_dispersion = RandomizeFloatBetweenValues(WEAPON_MIN_DISPERSION, is_bot ? WEAPON_MAX_DISPERSION_FOR_BOT : WEAPON_MAX_DISPERSION);
			weapon->m_ammunition->m_damage *= (1 + WEAPON_DISPERSION_DAMAGE_MALUS);

			weapon->m_display_name = to_string(weapon->m_multishot) + "-shots_wide_laser";
		}
		else
			weapon->m_display_name = to_string(weapon->m_multishot) + "-shots_laser";
	}

	//beam weapons
	if (beam_weapon == true)
	{
		weapon->m_rafale = -1;
		weapon->m_rafale_cooldown = 0;
		weapon->m_rate_of_fire = 0;
		weapon->m_ammunition->m_isBeam = true;
		weapon->m_ammunition->m_damage *= weapon->m_multishot;
		weapon->m_multishot = 1;
		weapon->m_display_name = "Laserbeam";
	}

	//saving level and credits used
	weapon->m_level = level;
	weapon->m_credits = credits_;
	weapon->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	//epic stats
	if (Game::GetItemQualityClass(weapon->m_quality) == ItemQuality_Epic)
	{
		int random_epic_ability = RandomizeIntBetweenValues(0, 1);

		switch (random_epic_ability)
		{
			case 0:
			{
				weapon->m_target_homing = HOMING;
				weapon->m_ammunition->m_damage * (1 - WEAPON_HOMING_DAMAGE_MALUS);
				break;
			}
			case 1:
			{
				weapon->m_ammunition->m_pattern.setPattern_v2(Circle_, 1200, 0, 30, 0, 0);
				weapon->m_ammunition->m_speed.y *= 0.8;
				break;
			}
		}
	}

#ifndef NDEBUG
	//if (!is_bot)
		//printf("\nNew weapon created: level %d, quality %f, xp: %d, bonus_multishot: %d, bonus_damage: %d, bonus_rof: %d\n\n", level, weapon->m_quality, credits_, bonus_multishot, bonus_damage, bonus_rate_of_fire);
#endif

	return weapon;
}


Equipment* Enemy::CreateRandomArmor(int level, float beastScore)
{
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));

	//Spending credits on the possible bonuses
	int bonus_armor = 0;
	int bonus_damage = 0;

	int loot_credits_remaining = credits_;
	while (loot_credits_remaining > 0)
	{
		int random_type_of_bonus = RandomizeIntBetweenValues(0, 5);

		//spending the credits in the chosen bonus
		switch (random_type_of_bonus)
		{
		case 0://damage
		{
			loot_credits_remaining--;
			bonus_damage++;
			break;
		}

		default://armor
		{
			loot_credits_remaining--;
			bonus_armor++;
			break;
		}
		}
	}

	//Creating the item with base stats
	Equipment* equipment = Enemy::LoadEquipment("armor_default");

	//allocating bonuses to the weapon
	equipment->m_armor += bonus_armor * equipment->m_armor * 0.01;
	//equipment->m_damage += bonus_damage * FIRST_LEVEL_COLLISION_DAMAGE * 0.01;

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_;
	equipment->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	//bombs
	equipment->m_bombs = BOMB_DEFAULT_NUMBER;

	return equipment;
}

Equipment* Enemy::CreateRandomShield(int level, float beastScore)
{
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));

	//Spending credits on the possible bonuses
	int bonus_shield = 0;
	int bonus_shield_regen = 0;
	int bonus_shield_recovery = 0;

	int loot_credits_remaining = credits_;

	bool can_buy_shield_regen = loot_credits_remaining > 20;

	while (loot_credits_remaining > 0)
	{
		int random_type_of_bonus = can_buy_shield_regen ? RandomizeIntBetweenValues(0, 3) : RandomizeIntBetweenValues(1, 3);

		//spending the credits in the chosen bonus
		switch (random_type_of_bonus)
		{
			case 0://shield regen
			{
				loot_credits_remaining--;
				bonus_shield_regen++;
				break;
			}
			case 1://shield regen
			{
				loot_credits_remaining--;
				bonus_shield_recovery++;
				break;
			}
			default://shield
			{
				loot_credits_remaining--;
				bonus_shield++;
				break;
			}
		}
	}

	//Creating the item with base stats
	Equipment* equipment = Enemy::LoadEquipment("shield_default");

	//allocating bonuses to the weapon
	equipment->m_shield += bonus_shield * equipment->m_shield * 0.01;
	equipment->m_shield_regen += ceil(bonus_shield_regen * equipment->m_shield_regen * 0.01);
	equipment->m_shield_recovery_time -= equipment->m_shield_recovery_time - equipment->m_shield_recovery_time / (1 + bonus_shield_recovery * 0.01);

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_;
	equipment->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	return equipment;
}

Equipment* Enemy::CreateRandomEngine(int level, float beastScore)
{
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));

	//Spending credits on the possible bonuses
	int loot_credits_remaining = credits_;

	int bonus_hyperspeed = 0;
	int bonus_fuel = 0;

	while (loot_credits_remaining > 0)
	{
		int random_type_of_bonus = RandomizeIntBetweenValues(0, 2);

		//spending the credits in the chosen bonus
		switch (random_type_of_bonus)
		{
		case 0://hyperspeed
		{
			loot_credits_remaining--;
			bonus_hyperspeed++;
			break;
		}
		default://fuel
		{
			loot_credits_remaining--;
			bonus_fuel++;
			break;
		}
		}
	}

	//Creating the item with base stats
	Equipment* equipment = Enemy::LoadEquipment("engine_default");

	//allocating bonuses to the weapon
	equipment->m_hyperspeed += bonus_hyperspeed * equipment->m_hyperspeed * 0.01;
	equipment->m_hyperspeed_fuel += bonus_fuel * equipment->m_hyperspeed_fuel * 0.01;

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_;
	equipment->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	//epic stats
	if (Game::GetItemQualityClass(equipment->m_quality) >= ItemQuality_Medium)
	{
		int random_ability = RandomizeIntBetweenValues(0, 1);

		switch (random_ability)
		{
			case 0://cruising
			{
				equipment->m_can_hyperspeed = true;
				break;
			}
			case 1://jumping
			{
				equipment->m_can_jump = true;
				break;
			}
		}
	}

#ifndef NDEBUG
	//printf("\nNew engine created: level %d, quality %f, xp: %d, bonus_hyperspeed: %d, bonus_fuel: %d\n\n", level, equipment->m_quality, credits_, bonus_hyperspeed, bonus_fuel);
#endif

	return equipment;
}

Equipment* Enemy::CreateRandomModule(int level, float beastScore)
{
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));

	int cost_per_bot = CREDITS_COST_PER_ONE_ADDITIONAL_BOT;

	//buying additional bots?
	int loot_credits_remaining = credits_;
	int number_of_bots = 0;
	bool can_buy_additional_bot = true;
	while (can_buy_additional_bot)
	{
		number_of_bots++;
		loot_credits_remaining -= number_of_bots == 0 ? 0 : cost_per_bot;
		cost_per_bot = floor(cost_per_bot * pow((1 + COST_PER_ONE_BOT_MULTIPLIER_PER_LEVEL), number_of_bots));
		can_buy_additional_bot = loot_credits_remaining > cost_per_bot && number_of_bots < MAX_NUMBER_OF_BOTS_PER_ITEM;
	}

	loot_credits_remaining = credits_ / number_of_bots;

	//Spending credits on the possible bonuses
	//Weapon* weapon = Enemy::CreateRandomWeapon(level, true, beastScore);

	//Creating the item with base stats
	Equipment* equipment = Enemy::LoadEquipment("module_default");

	//BOTS
	if (equipment->m_bots.empty() == false)
	{
		Bot* bot = equipment->m_bots.front();

		//Randomize pattern
		int clockwise = ((rand() % 2) * 2) - 1;
		bot->m_pattern.m_clockwise = clockwise;

		if (RandomizeIntBetweenValues(0, 1) == 0)//chance of "oscillator" pattern
		{
			bot->m_pattern.m_speed *= 2;
			bot->m_pattern.m_height = 0;
		}

		bot->m_pattern.resetPattern();

		//Randomize weapon
		Weapon* weapon = Enemy::CreateRandomWeapon(level, true, beastScore);
		delete bot->m_weapon;
		bot->m_weapon = weapon;

		//Multiple bots
		for (int i = 1; i < number_of_bots; i++)
		{
			Bot* new_bot = bot->Clone();

			//bots auto spreading based on number of bots
			int s = i % 2 == 0 ? 1 : -1;
			int x = i / 2;
			new_bot->m_spread.x *= s * (1 + x);

			//movement pattern spreading
			//if (bot->m_pattern.m_pattern_type == Circle_)
			//{
			//	if (number_of_bots % 2 + 1 == 2)
			//		new_bot->m_pattern.m_starting_point = 6 + bot->m_pattern.m_starting_point;
			//	else if (number_of_bots % 2 + 1 == 3)
			//		new_bot->m_pattern.m_starting_point = 4 * (i % 2 + 1) + bot->m_pattern.m_starting_point;
			//	else if (number_of_bots % 2 + 1 == 4)
			//		new_bot->m_pattern.m_starting_point = 3 * (i % 2 + 1) + bot->m_pattern.m_starting_point;
			//		
			//	new_bot->m_pattern.resetPattern();
			//}
			
			equipment->m_bots.push_back(new_bot);
		}
	}

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_;
	equipment->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	return equipment;
}

//#define EQUIPMENT_DECCELERATION_MULTIPLIER		10
//#define EQUIPMENT_ACCELERATION_MULTIPLIER		10
//#define EQUIPMENT_MAXSPEED_MULTIPLIER			10
//#define EQUIPMENT_ARMOR_MULTIPLIER				10
//#define EQUIPMENT_SHIELD_MULTIPLIER				10
//#define EQUIPMENT_SHIELD_REGEN_MULTIPLIER		10
//#define EQUIPMENT_DAMAGE_MULTIPLIER				10

// ENEMY BASE
EnemyBase::~EnemyBase()
{
	for (Phase* phase : m_enemy->m_phases)
		delete phase;

	delete m_enemy->m_FX_death;

	delete m_enemy;
}

void Enemy::SaveEquipmentData(ofstream& data, Equipment* equipment, bool skip_type)
{
	if (equipment)
	{
		if (!skip_type)
		{
			switch (equipment->m_equipmentType)
			{
				case Engine:
				{
					data << "Engine ";
					break;
				}
				case Armor:
				{
					data << "Armor ";
					break;
				}
				case Shield:
				{
					data << "Shield ";
					break;
				}
				case Module:
				{
					data << "Module ";
					break;
				}
				default:
				{
					data << "Unknown ";
					break;
				}
			}
		}

		data << equipment->m_display_name << " ";
		data << equipment->m_level << " ";
		data << equipment->m_credits << " ";
		data << equipment->m_quality << " ";
		data << equipment->m_textureName << " ";
		data << equipment->m_size.x << " ";
		data << equipment->m_size.y << " ";
		data << equipment->m_frameNumber << " ";
		data << equipment->m_max_speed << " ";
		data << equipment->m_deceleration << " ";
		data << equipment->m_acceleration << " ";
		data << equipment->m_hyperspeed << " ";
		data << equipment->m_hyperspeed_fuel << " ";
		data << equipment->m_armor << " ";
		data << equipment->m_shield << " ";
		data << equipment->m_shield_regen << " ";
		data << equipment->m_shield_recovery_time << " ";
		data << equipment->m_damage << " ";
		data << equipment->m_bombs << " ";
		data << equipment->m_can_hyperspeed << " ";
		data << equipment->m_can_jump << " ";

		if (!equipment->m_bots.empty())
		{
			data << equipment->m_bots.front()->m_display_name << " ";
			data << equipment->m_bots.size() << " ";
			data << equipment->m_bots.front()->m_textureName << " ";
			data << equipment->m_bots.front()->m_size.x << " ";
			data << equipment->m_bots.front()->m_size.y << " ";
			data << equipment->m_bots.front()->m_frameNumber << " ";
			data << equipment->m_bots.front()->m_spread.x << " ";
			data << equipment->m_bots.front()->m_spread.y << " ";
			data << equipment->m_bots.front()->m_rotation_speed << " ";
			data << equipment->m_bots.front()->m_pattern.m_pattern_type << " ";

			if (equipment->m_bots.front()->m_pattern.m_pattern_type != NoMovePattern)
			{
				data << equipment->m_bots.front()->m_pattern.m_speed << " ";
				data << equipment->m_bots.front()->m_pattern.m_clockwise << " ";

				if (equipment->m_bots.front()->m_pattern.m_pattern_type == Circle_ || equipment->m_bots.front()->m_pattern.m_pattern_type == Rectangle_)
				{
					data << equipment->m_bots.front()->m_pattern.m_width << " ";
					data << equipment->m_bots.front()->m_pattern.m_height << " ";
					data << equipment->m_bots.front()->m_pattern.m_starting_point << " ";
				}
			}

			if (equipment->m_bots.front()->m_weapon)
			{
				Enemy::SaveWeaponData(data, equipment->m_bots.front()->m_weapon, true, true);
				return;
			}
			else
				data << "0";
		}
		else
			data << "0";
	}
	else
		data << "0";

	data << endl;
}

void Enemy::SaveWeaponData(ofstream& data, Weapon* weapon, bool skip_type, bool skip_level)
{
	if (skip_type == false)
		data << "Weapon ";

	if (weapon != NULL)
	{
		data << weapon->m_display_name << " ";
		if (skip_level == false)
		{
			data << weapon->m_level << " ";
			data << weapon->m_credits << " ";
			data << weapon->m_quality << " ";
		}
		data << weapon->m_textureName << " ";
		data << weapon->m_size.x << " ";
		data << weapon->m_size.y << " ";
		data << weapon->m_frameNumber << " ";
		data << weapon->m_rate_of_fire << " ";
		data << weapon->m_rafale << " ";
		data << weapon->m_rafale_cooldown << " ";

		data << weapon->m_multishot << " ";
		data << weapon->m_xspread << " ";
		data << weapon->m_dispersion << " ";
		data << (int)weapon->m_shot_mode << " ";
		data << (int)weapon->m_target_homing << " ";
		data << weapon->m_angle_offset << " ";

		data << weapon->m_ammunition->m_display_name << " ";
		data << weapon->m_ammunition->m_damage << " ";
		data << weapon->m_ammunition->m_speed.y << " ";
		data << weapon->m_ammunition->m_range << " ";
		data << weapon->m_ammunition->m_currentAnimationIndex << " ";
		data << (int)weapon->m_ammunition->m_pattern.m_pattern_type << " ";

		if (weapon->m_ammunition->m_pattern.m_pattern_type != NoMovePattern)
		{
			data << weapon->m_ammunition->m_pattern.m_speed << " ";
			data << weapon->m_ammunition->m_pattern.m_clockwise << " ";

			if (weapon->m_ammunition->m_pattern.m_pattern_type == Circle_ || weapon->m_ammunition->m_pattern.m_pattern_type == Rectangle_)
			{
				data << weapon->m_ammunition->m_pattern.m_width << " ";
				data << weapon->m_ammunition->m_pattern.m_height << " ";
				data << weapon->m_ammunition->m_pattern.m_starting_point << " ";
			}
		}

		data << weapon->m_ammunition->m_is_missile_model << " ";
	}
	else
		data << "0";

	data << endl;
}

Equipment* Enemy::LoadSavedEquipmentFromLine(string line)
{
	string equipment_type;
	string display_name;

	std::istringstream ss(line);
	ss >> equipment_type >> display_name;
	//ss.str(line.substr(ss.tellg()));

	Equipment* equipment = new Equipment();

	string texture_name;
	int level;
	int credits;
	float quality;
	int width;
	int height;
	int frames;
	float max_speed;
	float deceleration;
	float acceleration;
	float hyperspeed;
	int hyperspeed_fuel;
	int armor;
	int shield;
	int shield_regen;
	float shield_recovery;
	int damage;
	int bombs;
	bool can_hyperspeed;
	bool can_jump;

	string bot_name;
	int bot_number;
	string bot_texture_name;
	int bot_width;
	int bot_height;
	int bot_frames;
	float bot_spread_x;
	float bot_spread_y;
	float bot_rotation_speed;
	int bot_pattern_type;
	float bot_pattern_speed;
	int bot_pattern_clockwise;
	float bot_pattern_width;
	float bot_pattern_height;
	int bot_pattern_starting_point;

	string bot_weapon_name;
	string bot_weapon_texture_name;
	int bot_weapon_width;
	int bot_weapon_height;
	int bot_weapon_frames;
	float bot_weapon_rate_of_fire;
	int bot_weapon_rafale;
	float bot_rafale_cooldown;
	int bot_weapon_multishot;
	float bot_weapon_xspread;
	float bot_weapon_dispersion;
	int bot_weapon_shot_mode;
	int bot_weapon_target_homing;
	float bot_weapon_angle_offset;

	string bot_ammo_name;
	int bot_ammo_damage;
	float bot_ammo_speed;
	float bot_ammo_range;
	int bot_ammo_skin;
	int bot_ammo_pattern_type;
	float bot_ammo_pattern_speed;
	int bot_ammo_pattern_clockwise;
	float bot_ammo_pattern_width;
	float bot_ammo_pattern_height;
	int bot_ammo_pattern_starting_point;

	if (display_name.compare("0") == 0)
	{
		//do nothing
	}
	else
	{
		ss >> level >> credits >> quality >> texture_name >> width >> height >> frames >> max_speed >> deceleration >> acceleration >> hyperspeed >> hyperspeed_fuel
			>> armor >> shield >> shield_regen >> shield_recovery >> damage >> bombs >> can_hyperspeed >> can_jump >> bot_name;

		if (bot_name.compare("0") != 0)
		{
			ss >> bot_number >> bot_texture_name >> bot_width >> bot_height >> bot_frames >> bot_spread_x >> bot_spread_y >> bot_rotation_speed >> bot_pattern_type;
			if (bot_pattern_type == Line_)
				ss >> bot_pattern_speed >> bot_pattern_clockwise;
			else if (bot_pattern_type == Circle_ || bot_pattern_type == Rectangle_)
				ss >> bot_pattern_speed >> bot_pattern_clockwise >> bot_pattern_width >> bot_pattern_height >> bot_pattern_starting_point;

			ss >> bot_weapon_name;
			if (bot_weapon_name.compare("0") != 0)
			{
				ss >> bot_weapon_texture_name >> bot_weapon_width >> bot_weapon_height >> bot_weapon_frames >> bot_weapon_rate_of_fire >>
					bot_weapon_rafale >> bot_rafale_cooldown >> bot_weapon_multishot >> bot_weapon_xspread >> bot_weapon_dispersion >> bot_weapon_shot_mode
					>> bot_weapon_target_homing >> bot_weapon_angle_offset;

				ss >> bot_ammo_name >> bot_ammo_damage >> bot_ammo_speed >> bot_ammo_range >> bot_ammo_skin >> bot_ammo_pattern_type;

				if (bot_ammo_pattern_type == Line_)
					ss >> bot_ammo_pattern_speed >> bot_ammo_pattern_clockwise;
				else if (bot_ammo_pattern_type == Circle_ || bot_ammo_pattern_type == Rectangle_)
					ss >> bot_ammo_pattern_speed >> bot_ammo_pattern_clockwise >> bot_ammo_pattern_width >> bot_ammo_pattern_height >> bot_ammo_pattern_starting_point;
			}
		}
	}

	EquipmentType type = NBVAL_Equipment;
	if (equipment_type.compare("Shield") == 0)
		type = Shield;
	else if (equipment_type.compare("Armor") == 0)
		type = Armor;
	else if (equipment_type.compare("Engine") == 0)
		type = Engine;
	else if (equipment_type.compare("Module") == 0)
		type = Module;

	equipment->Init(type, max_speed, acceleration, deceleration, hyperspeed, hyperspeed_fuel, armor, shield, shield_regen, shield_recovery, damage, texture_name, sf::Vector2f(width, height), frames, display_name);
	equipment->m_level = level;
	equipment->m_credits = credits;
	equipment->m_quality = quality;
	equipment->m_bombs = bombs;
	equipment->m_can_hyperspeed = can_hyperspeed;
	equipment->m_can_jump = can_jump;

	if (bot_name.compare("0") != 0)
	{
		Bot* bot = new Bot(Vector2f(0, 0), Vector2f(0, 0), bot_texture_name, sf::Vector2f(bot_width, bot_height));
		bot->m_display_name = bot_name;
		bot->m_spread = sf::Vector2f(bot_spread_x, bot_spread_y);
		bot->m_rotation_speed = bot_rotation_speed;

		GeometryPattern* bot_pattern = new GeometryPattern();
		bot_pattern->m_pattern_type = (PatternType)bot_pattern_type;

		if (bot_pattern->m_pattern_type != NoMovePattern)
		{
			bot_pattern->m_speed = bot_pattern_speed;
			bot_pattern->m_clockwise = bot_pattern_clockwise;

			if (bot_pattern->m_pattern_type == Circle_ || bot_pattern->m_pattern_type == Rectangle_)
			{
				bot_pattern->m_width = bot_pattern_width;
				bot_pattern->m_height = bot_pattern_height;
				bot_pattern->m_starting_point = bot_pattern_starting_point;
			}
		}
		bot->m_pattern.setPattern_v2(bot_pattern);
		delete bot_pattern;

		if (bot_weapon_name.compare("0") == 0)
		{
			//do nothing
		}
		else
		{
			Ammo* ammo = Enemy::LoadAmmo(bot_ammo_name);
			ammo->m_display_name = bot_ammo_name;
			ammo->m_speed.y = bot_ammo_speed;
			ammo->m_damage = bot_ammo_damage;
			ammo->m_range = bot_ammo_range;
			ammo->setAnimationLine(bot_ammo_skin);
			ammo->m_isBeam = bot_weapon_rafale < 0;

			GeometryPattern* ammo_pattern = new GeometryPattern();
			ammo_pattern->m_pattern_type = (PatternType)bot_ammo_pattern_type;

			if (ammo_pattern->m_pattern_type != NoMovePattern)
			{
				ammo_pattern->m_speed = bot_ammo_pattern_speed;
				ammo_pattern->m_clockwise = bot_ammo_pattern_clockwise;

				if (ammo_pattern->m_pattern_type == Circle_ || ammo_pattern->m_pattern_type == Rectangle_)
				{
					ammo_pattern->m_width = bot_ammo_pattern_width;
					ammo_pattern->m_height = bot_ammo_pattern_height;
					ammo_pattern->m_starting_point = bot_ammo_pattern_starting_point;
				}
			}
			ammo->m_pattern.setPattern_v2(ammo_pattern);
			delete ammo_pattern;

			Weapon* weapon = new Weapon(ammo);
			weapon->m_display_name = bot_weapon_name;
			weapon->m_fire_direction = -1;
			weapon->m_rate_of_fire = bot_weapon_rate_of_fire;
			weapon->m_shot_mode = (ShotMode)bot_weapon_shot_mode;

			weapon->m_multishot = bot_weapon_multishot;
			weapon->m_xspread = bot_weapon_xspread;
			weapon->m_dispersion = bot_weapon_dispersion;
			weapon->m_rafale = bot_weapon_rafale;
			weapon->m_rafale_cooldown = bot_rafale_cooldown;

			weapon->m_textureName = bot_weapon_texture_name;
			weapon->m_size = sf::Vector2f(bot_weapon_width, bot_weapon_height);
			weapon->m_frameNumber = bot_weapon_frames;
			weapon->m_target_homing = (TargetHoming)bot_weapon_target_homing;
			weapon->m_angle_offset = bot_weapon_angle_offset;

			bot->m_weapon = weapon;
		}

		for (int i = 0; i < bot_number; i++)
		{
			if (i == 0)
				equipment->m_bots.push_back(bot);
			else
			{
				Bot* new_bot = bot->Clone();

				//bots auto spreading based on number of bots
				int s = i % 2 == 0 ? 1 : -1;
				int x = i / 2;
				new_bot->m_spread.x *= s * (1 + x);

				equipment->m_bots.push_back(new_bot);
			}
		}
	}

	return equipment;
}

Weapon* Enemy::LoadSavedWeaponFromLine(string line)
{
	string equipment_type;
	string display_name;

	//std::istringstream(line) >> equipment_type >> display_name;

	std::istringstream ss(line);
	ss >> equipment_type >> display_name;

	Equipment* equipment = new Equipment();

	string weapon_texture_name;
	int weapon_level;
	int weapon_credits;
	float weapon_quality;
	int weapon_width;
	int weapon_height;
	int weapon_frames;
	float weapon_rate_of_fire;
	int weapon_rafale;
	float rafale_cooldown;
	int weapon_multishot;
	float weapon_xspread;
	float weapon_dispersion;
	int weapon_shot_mode;
	int weapon_target_homing;
	float weapon_angle_offset;

	string ammo_name;
	int ammo_damage;
	float ammo_speed;
	float ammo_range;
	int ammo_skin;
	int ammo_pattern_type;
	float ammo_pattern_speed;
	int ammo_pattern_clockwise;
	float ammo_pattern_width;
	float ammo_pattern_height;
	int ammo_pattern_starting_point;

	ss >> weapon_level >> weapon_credits >> weapon_quality >> weapon_texture_name >> weapon_width >> weapon_height >> weapon_frames >> weapon_rate_of_fire >>
		weapon_rafale >> rafale_cooldown >> weapon_multishot >> weapon_xspread >> weapon_dispersion >> weapon_shot_mode
		>> weapon_target_homing >> weapon_angle_offset >> ammo_name >> ammo_damage >> ammo_speed >> ammo_range >> ammo_skin >> ammo_pattern_type;

	if (ammo_pattern_type != NoMovePattern)
		ss >> ammo_pattern_speed >> ammo_pattern_clockwise;

	if (ammo_pattern_type == Circle_ || ammo_pattern_type == Rectangle_)
		ss >> ammo_pattern_width >> ammo_pattern_height >> ammo_pattern_starting_point;

	EquipmentType type = NBVAL_Equipment;

	Ammo* ammo = Enemy::LoadAmmo(ammo_name);
	ammo->m_display_name = ammo_name;
	ammo->m_speed.y = ammo_speed;
	ammo->m_damage = ammo_damage;
	ammo->m_range = ammo_range;
	ammo->setAnimationLine(ammo_skin);
	ammo->m_isBeam = weapon_rafale < 0;

	GeometryPattern* ammo_pattern = new GeometryPattern();

	ammo_pattern->m_pattern_type = (PatternType)ammo_pattern_type;
	if (ammo_pattern->m_pattern_type != NoMovePattern)
	{
		ammo_pattern->m_speed = ammo_pattern_speed;
		ammo_pattern->m_clockwise = ammo_pattern_clockwise;

		if (ammo_pattern->m_pattern_type == Circle_ || ammo_pattern->m_pattern_type == Rectangle_)
		{
			ammo_pattern->m_width = ammo_pattern_width;
			ammo_pattern->m_height = ammo_pattern_height;
			ammo_pattern->m_starting_point = ammo_pattern_starting_point;
		}
	}
	ammo->m_pattern.setPattern_v2(ammo_pattern);
	delete ammo_pattern;

	Weapon* weapon = new Weapon(ammo);
	weapon->m_display_name = display_name;
	weapon->m_level = weapon_level;
	weapon->m_credits = weapon_credits;
	weapon->m_quality = weapon_quality;
	weapon->m_fire_direction = -1;
	weapon->m_rate_of_fire = weapon_rate_of_fire;
	weapon->m_shot_mode = (ShotMode)weapon_shot_mode;

	weapon->m_multishot = weapon_multishot;
	weapon->m_xspread = weapon_xspread;
	weapon->m_dispersion = weapon_dispersion;
	weapon->m_rafale = weapon_rafale;
	weapon->m_rafale_cooldown = rafale_cooldown;

	weapon->m_textureName = weapon_texture_name;
	weapon->m_size = sf::Vector2f(weapon_width, weapon_height);
	weapon->m_frameNumber = weapon_frames;
	weapon->m_target_homing = (TargetHoming)weapon_target_homing;
	weapon->m_angle_offset = weapon_angle_offset;

	return weapon;
}

GameObject* Enemy::CloneEquipmentIntoGameObject(Equipment* new_equipment)
{
	assert(new_equipment != NULL);

	GameObject* capsule = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), new_equipment->m_textureName, new_equipment->m_size, sf::Vector2f(new_equipment->m_size.x / 2, new_equipment->m_size.y / 2), new_equipment->m_frameNumber);
	capsule->setEquipmentLoot(new_equipment->Clone());
	capsule->m_display_name = new_equipment->m_display_name;

	return capsule;
}

GameObject* Enemy::CloneWeaponIntoGameObject(Weapon* new_weapon)
{
	assert(new_weapon != NULL);

	GameObject* capsule = new GameObject(new_weapon->getPosition(), sf::Vector2f(0, 0), new_weapon->m_textureName, new_weapon->m_size, sf::Vector2f(new_weapon->m_size.x / 2, new_weapon->m_size.y / 2), new_weapon->m_frameNumber);
	capsule->setWeaponLoot(new_weapon->Clone());
	capsule->m_display_name = new_weapon->m_display_name;

	return capsule;
}