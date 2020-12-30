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
	//ss << "\n " << m_currentPhase->m_display_name.c_str();// << " / offy: " << to_string(m_pattern.m_offset.y) << " / spd: " << to_string(int(m_pattern.m_speed));
	ss << "\nclock: " << to_string((int)m_pattern.m_clockwise);
	
	//for (float f : m_pattern.m_patternParams)
	//	ss << to_string(int(f)) << " / ";

	text.setString(ss.str());
	screen.draw(text);
	*/

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
				m_pattern.m_clockwise *= -1;
				setPosition(size.x / 2, newposition.y);
			}
			else if (newposition.x > SCENE_SIZE_X - size.x / 2)
			{
				m_pattern.m_clockwise *= -1;
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
				{
					if (playership->m_ghost == false)
						target_angle = fmod(getRotation() + 180 - GameObject::GetAngleDegToTargetPosition(getPosition(), getRotation(), playership->getPosition()), 360);
				}
					
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
				{
					theta += delta;
					if (playership->m_ghost == false)
						weapon->m_shot_angle = theta * M_PI / 180;//switching to radians
				}
				else
					weapon->m_shot_angle = theta * M_PI / 180;//switching to radians

				//calcule weapon offset
				weapon->m_weapon_current_offset.x = weapon->m_weaponOffset.x * cos(weapon->m_shot_angle) + m_size.y / 2 * sin(weapon->m_shot_angle) * (-weapon->m_fire_direction);
				weapon->m_weapon_current_offset.y = weapon->m_weaponOffset.x * sin(weapon->m_shot_angle) - m_size.y / 2 * cos(weapon->m_shot_angle) * (-weapon->m_fire_direction);
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
	m_phaseTimer += deltaTime.asSeconds() * l_hyperspeedMultiplier;
	m_enemyTimer += deltaTime.asSeconds() * l_hyperspeedMultiplier;
	
	//shield regen if not maximum
	ShieldRegen(deltaTime, hyperspeedMultiplier);

	//animate
	AnimatedSprite::update(deltaTime);

	//phases
	if (m_phases.empty() == false)
		if (m_currentPhase->m_transitions_list.empty() == false)
			CheckCondition();
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
		Death(true);
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
		enemy->m_phases.push_back(phase->Clone());

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
			case PhaseClock:
			{
				if (m_phaseTimer == cond->m_value)
					result = EQUAL_TO;
				else
					result = m_phaseTimer > cond->m_value ? GREATER_THAN : LESSER_THAN;
				break;
			}
			case EnemyClock:
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
			case WakeUp:
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
				Death(true);
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
				(*CurrentGame).killGameObjectType(EnemyFire, false);
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
	if (phase->m_pattern->m_clockwise == 0 && m_pattern.m_clockwise == 0)//if random clockwise, choose depending on getPosition().x
	{
		if (getPosition().x < SCENE_SIZE_X * 0.40)
			phase->m_pattern->m_clockwise = 1;
		else if (getPosition().x > SCENE_SIZE_X * 0.50)
			phase->m_pattern->m_clockwise = -1;
	}

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
	Dialog* dialog = new Dialog();

	dialog->m_name = name;
	dialog->m_fade_in = (bool)(stoi((*CurrentGame).m_dialogsConfig[name][DIALOG_FADE_IN]));
	dialog->m_fade_out = (bool)(stoi((*CurrentGame).m_dialogsConfig[name][DIALOG_FADE_OUT]));
	dialog->m_enemy_speaking = (*CurrentGame).m_dialogsConfig[name][DIALOG_ENEMY_SPEAKING].compare("0") != 0;
	dialog->m_duration = atof((*CurrentGame).m_dialogsConfig[name][DIALOG_DURATION].c_str());
	dialog->m_title = (*CurrentGame).m_dialogsConfig[name][DIALOG_TITLE];
	dialog->m_title = SFReplaceTexts(dialog->m_title, DIALOG_MAX_LENGTH);
	dialog->m_body = (*CurrentGame).m_dialogsConfig[name][DIALOG_BODY];
	dialog->m_body = SFReplaceTexts(dialog->m_body, DIALOG_MAX_LENGTH);
	dialog->m_picture_name = (*CurrentGame).m_dialogsConfig[name][DIALOG_PICTURE];
	dialog->m_next_dialog_name = (*CurrentGame).m_dialogsConfig[name][DIALOG_NEXT];

	TextureLoader *loader = TextureLoader::getInstance();
	loader->loadTexture(dialog->m_picture_name, DIALOG_PANEL_PORTRAIT_SIZE_X, DIALOG_PANEL_PORTRAIT_SIZE_X);

	return dialog;
}

Phase* Enemy::LoadPhase(string name)
{
	Phase* phase = new Phase();

	phase->m_name = name;
	phase->m_display_name = (*CurrentGame).m_phasesConfig[name][PHASE_NAME];
	phase->m_vspeed = stoi((*CurrentGame).m_phasesConfig[name][PHASE_VSPEED]);

	//loading weapons and ammos
	for (int i = 0; i < 6; i++)
	{
		if ((*CurrentGame).m_phasesConfig[name][PHASE_WEAPON + (i * 3)].compare("0") != 0)
		{
			Weapon* m_weapon = Enemy::LoadWeapon((*CurrentGame).m_phasesConfig[name][PHASE_WEAPON + (i * 3)], 1);
			m_weapon->m_weaponOffset.x = atof((*CurrentGame).m_phasesConfig[name][PHASE_WEAPON_OFFSET + (i * 3)].c_str());
			m_weapon->m_delay = atof((*CurrentGame).m_phasesConfig[name][PHASE_WEAPON_DELAY + (i * 3)].c_str());
			phase->m_weapons_list.push_back(m_weapon);
		}
	}

	//loading phases
	GeometryPattern* pattern = GeometryPattern::LoadPattern((*CurrentGame).m_phasesConfig[name], PHASE_PATTERN);
	phase->m_pattern = pattern;

	//loading rotation speed
	phase->m_rotation_speed = stoi((*CurrentGame).m_phasesConfig[name][PHASE_ROTATION_SPEED]);

	//loading modifier (immune to damage, etc.)
	for (int i = 0; i < 3; i++)
	{
		Modifier l_new_modifier = NoModifier;
		if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("0") != 0)
		{
			if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("immune") == 0)
				l_new_modifier = Immune;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("ghost") == 0)
				l_new_modifier = GhostModifier;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("death") == 0)
				l_new_modifier = DeathModifier;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("face_target") == 0)
				l_new_modifier = FaceTarget;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("reset_facing") == 0)
				l_new_modifier = ResetFacing;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("shoot_when_aligned") == 0)
				l_new_modifier = ShootWhenAligned;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("reset_facing") == 0)
				l_new_modifier = ResetFacing;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("bouncing") == 0)
				l_new_modifier = Bouncing;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("bouncingH") == 0)
				l_new_modifier = BouncingH;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("bouncingV") == 0)
				l_new_modifier = BouncingV;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("freeze") == 0)
				l_new_modifier = FreezePlayer;
			else if ((*CurrentGame).m_phasesConfig[name][PHASE_MODIFIER + i].compare("kill_bullets") == 0)
				l_new_modifier = KillBullets;
		}

		phase->m_modifiers.push_back(l_new_modifier);
	}

	//loading welcome shot
	if ((*CurrentGame).m_phasesConfig[name][PHASE_WELCOME_WEAPON].compare("0") != 0)
		phase->m_welcomeWeapon = Enemy::LoadWeapon((*CurrentGame).m_phasesConfig[name][PHASE_WELCOME_WEAPON], 1);
			
	//load enemies (by name) to wake up
	if ((*CurrentGame).m_phasesConfig[name][PHASE_WAKEUP].compare("0") != 0)
		phase->m_wake_up_name = (*CurrentGame).m_phasesConfig[name][PHASE_WAKEUP];

	//loading transition to next phase
	if ((*CurrentGame).m_phasesConfig[name][PHASE_CONDITION].compare("0") != 0)
		phase->m_transitions_list.push_back(Phase::ConditionLoader((*CurrentGame).m_phasesConfig[name], PHASE_CONDITION));

	if ((*CurrentGame).m_phasesConfig[name][PHASE_CONDITION_2].compare("0") != 0)
		phase->m_transitions_list.push_back(Phase::ConditionLoader((*CurrentGame).m_phasesConfig[name], PHASE_CONDITION_2));

	//loading dialogs
	if ((*CurrentGame).m_phasesConfig[name][PHASE_DIALOG_NAME].compare("0") != 0)
	{
		Dialog* dialog = Enemy::LoadDialog((*CurrentGame).m_phasesConfig[name][PHASE_DIALOG_NAME]);
		phase->m_dialogs.push_back(dialog);
		while (!dialog->m_next_dialog_name.empty() && dialog->m_next_dialog_name.compare("0") != 0)
		{
			dialog = Enemy::LoadDialog(dialog->m_next_dialog_name);
			phase->m_dialogs.push_back(dialog);
		}
	}

	return phase;
}

void Enemy::Death(bool give_money)
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

	if (give_money == true)
		playership->addMoney(m_money);

	GameObject::Death();

	if (m_enemy_class < ENEMYPOOL_DELTA)
		(*CurrentGame).PlaySFX(SFX_Kill);
	else
		(*CurrentGame).PlaySFX(SFX_BigKill);

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

	for (Phase* phase : m_phases)
		delete phase;
	//delete m_FX_death;
}

void Enemy::GenerateLoot()
{
	sf::Vector2f speed = sf::Vector2f(0, LOOT_SPEED_Y);

	if (m_money > 0)
	{
		Loot* new_loot = new Loot(this->getPosition(), speed, LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH), "Money");
		new_loot->setMoney(m_money);
		(*CurrentGame).addToScene((GameObject*)new_loot, true);
	}
}

bool Enemy::IsBoss()
{
	return m_enemy_class == ENEMYPOOL_BOSS_MAIN || m_enemy_class == ENEMYPOOL_BOSS_SECONDARY;
}

Weapon* Enemy::LoadWeapon(string name, int fire_direction)
{
	Weapon* weapon = new Weapon(Enemy::LoadAmmo((*CurrentGame).m_weaponsConfig[name][WEAPON_AMMO]));
	weapon->m_display_name = (*CurrentGame).m_weaponsConfig[name][WEAPON_DISPLAY_NAME];
	weapon->m_fire_direction = fire_direction;
	weapon->m_rate_of_fire = atof((*CurrentGame).m_weaponsConfig[name][WEAPON_RATE_OF_FIRE].c_str());
	weapon->m_shot_mode = NoShotMode;

	weapon->m_ammunition->m_damage = stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_DAMAGE]);
	weapon->m_ammunition->m_ref_speed = (float)stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_SPEED]);
	weapon->m_ammunition->m_speed = sf::Vector2f(0, weapon->m_ammunition->m_ref_speed);
	weapon->m_ammunition->m_range = (float)stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_RANGE]);

	GeometryPattern* pattern = GeometryPattern::LoadPattern((*CurrentGame).m_weaponsConfig[name], WEAPON_PATTERN);
	weapon->m_ammunition->m_pattern.setPattern_v2(pattern);
	delete pattern;

	weapon->m_multishot = stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_MULTISHOT]);
	if (weapon->m_multishot > 1)
	{
		weapon->m_xspread = stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_XSPREAD]);
		weapon->m_dispersion = (float)stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_DISPERSION]);
		if ((*CurrentGame).m_weaponsConfig[name][WEAPON_ALTERNATE].compare("0") != 0)
		{
			if ((*CurrentGame).m_weaponsConfig[name][WEAPON_ALTERNATE].compare("alternate") == 0)
				weapon->m_shot_mode = AlternateShotMode;
			else if ((*CurrentGame).m_weaponsConfig[name][WEAPON_ALTERNATE].compare("ascending") == 0)
				weapon->m_shot_mode = AscendingShotMode;
			else if ((*CurrentGame).m_weaponsConfig[name][WEAPON_ALTERNATE].compare("descending") == 0)
				weapon->m_shot_mode = DescendingShotMode;
			else if ((*CurrentGame).m_weaponsConfig[name][WEAPON_ALTERNATE].compare("ascending2") == 0)
				weapon->m_shot_mode = Ascending2ShotMode;
			else if ((*CurrentGame).m_weaponsConfig[name][WEAPON_ALTERNATE].compare("descending2") == 0)
				weapon->m_shot_mode = Descending2ShotMode;
		}
	}
			
	weapon->m_rafale = stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_RAFALE]);
	if (weapon->m_rafale != 0)
	{
		weapon->m_rafale_cooldown = atof((*CurrentGame).m_weaponsConfig[name][WEAPON_RAFALE_COOLDOWN].c_str());
		weapon->m_rafale_locking = (bool)(stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_RAFALE_LOCKING]));

		if (weapon->m_rafale < 0)
			weapon->m_ammunition->m_isBeam = true;
	}
				
	weapon->m_textureName = (*CurrentGame).m_weaponsConfig[name][WEAPON_IMAGE_NAME];
	weapon->m_size = sf::Vector2f((float)stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_WIDTH]), (float)stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_HEIGHT]));
	weapon->m_frameNumber = stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_FRAMES]);
	weapon->m_angle_offset = (float)stoi((*CurrentGame).m_weaponsConfig[name][WEAPON_ANGLE_OFFSET]);

	if ((*CurrentGame).m_weaponsConfig[name][WEAPON_TARGET_HOMING].compare("0") != 0)
	{
		if ((*CurrentGame).m_weaponsConfig[name][WEAPON_TARGET_HOMING].compare("semi_homing") == 0)
			weapon->m_target_homing = SEMI_HOMING;
		else if ((*CurrentGame).m_weaponsConfig[name][WEAPON_TARGET_HOMING].compare("homing") == 0)
			weapon->m_target_homing = HOMING;
	}

	return weapon;
}

Ammo* Enemy::LoadAmmo(string name)
{
	Ammo* new_ammo = new Ammo(Vector2f(0, 0), Vector2f(0, 0), (*CurrentGame).m_ammoConfig[name][AMMO_IMAGE_NAME],
		Vector2f((float)stoi((*CurrentGame).m_ammoConfig[name][AMMO_WIDTH]), (float)stoi((*CurrentGame).m_ammoConfig[name][AMMO_HEIGHT])), stoi((*CurrentGame).m_ammoConfig[name][AMMO_FRAMES]), stoi((*CurrentGame).m_ammoConfig[name][AMMO_NB_SKINS]), 0, LoadFX((*CurrentGame).m_ammoConfig[name][AMMO_FX]), (bool)stoi((*CurrentGame).m_ammoConfig[name][AMMO_MISSILE_MODEL]));

	new_ammo->setAnimationLine(stoi((*CurrentGame).m_ammoConfig[name][AMMO_SKIN]) - 1);
	new_ammo->m_display_name = (*CurrentGame).m_ammoConfig[name][AMMO_NAME];
	new_ammo->m_sound_name = (*CurrentGame).m_ammoConfig[name][AMMO_SOUND];

	if ((*CurrentGame).m_ammoConfig[name][AMMO_FX].empty() == false)
		new_ammo->m_explosion->m_display_name = (*CurrentGame).m_ammoConfig[name][AMMO_FX];

	new_ammo->m_rotation_speed = (float)stoi((*CurrentGame).m_ammoConfig[name][AMMO_ROTATION_SPEED]);
	new_ammo->m_area_of_effect = (float)stoi((*CurrentGame).m_ammoConfig[name][AMMO_AREA_OF_EFFECT]);

	return new_ammo;
}

FX* Enemy::LoadFX(string name)
{
	FX* myFX = new FX(Vector2f(0, 0), Vector2f(0, 0), (*CurrentGame).m_FXConfig[name][FX_FILENAME], Vector2f((float)stoi((*CurrentGame).m_FXConfig[name][FX_WIDTH]), (float)stoi((*CurrentGame).m_FXConfig[name][FX_HEIGHT])), (bool)stoi((*CurrentGame).m_FXConfig[name][FX_IS_PERMANENT]), stoi((*CurrentGame).m_FXConfig[name][FX_FRAMES]));
	myFX->m_display_name = (*CurrentGame).m_FXConfig[name][FX_NAME];

	return myFX;
}

Bot* Enemy::LoadBot(string name)
{
	Bot* bot = new Bot(Vector2f(0, 0), Vector2f(0, 0), (*CurrentGame).m_botsConfig[name][BOT_IMAGE_NAME], sf::Vector2f((float)stoi((*CurrentGame).m_botsConfig[name][BOT_WIDTH]), (float)stoi((*CurrentGame).m_botsConfig[name][BOT_HEIGHT])));

	((GameObject*)bot)->m_display_name = (*CurrentGame).m_botsConfig[name][BOT_NAME];
	((GameObject*)bot)->m_armor = stoi((*CurrentGame).m_botsConfig[name][BOT_ARMOR]);
	((GameObject*)bot)->m_armor_max = stoi((*CurrentGame).m_botsConfig[name][BOT_ARMOR]);
	((GameObject*)bot)->m_shield = stoi((*CurrentGame).m_botsConfig[name][BOT_SHIELD]);
	((GameObject*)bot)->m_shield_max = stoi((*CurrentGame).m_botsConfig[name][BOT_SHIELD]);
	((GameObject*)bot)->m_shield_regen = stoi((*CurrentGame).m_botsConfig[name][BOT_SHIELD_REGEN]);
	((GameObject*)bot)->m_damage = stoi((*CurrentGame).m_botsConfig[name][BOT_DAMAGE]);
	bot->m_spread = Vector2f((float)stoi((*CurrentGame).m_botsConfig[name][BOT_XSPREAD]), (float)stoi((*CurrentGame).m_botsConfig[name][BOT_YSPREAD]));

	GeometryPattern* pattern = GeometryPattern::LoadPattern((*CurrentGame).m_botsConfig[name], BOT_PATTERN);
	bot->m_pattern.setPattern_v2(pattern);
	delete pattern;

	bot->m_rotation_speed = stoi((*CurrentGame).m_botsConfig[name][BOT_ROTATION_SPEED]);

	if ((*CurrentGame).m_botsConfig[name][BOT_WEAPON].compare("0") != 0)
		bot->m_weapon = Enemy::LoadWeapon((*CurrentGame).m_botsConfig[name][BOT_WEAPON], -1);

	return bot;
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