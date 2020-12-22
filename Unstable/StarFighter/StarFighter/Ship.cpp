#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

Ship::Ship(ShipModel* ship_model) : GameObject(Vector2f(0, 0), Vector2f(0, 0), ship_model->m_textureName, ship_model->m_size, Vector2f((ship_model->m_size.x / 2), (ship_model->m_size.y / 2)), ship_model->m_frameNumber)
{
	if (!ship_model)
		return;

	m_collider_type = PlayerShip;
	m_layer = PlayerShipLayer;

	m_ship_model = ship_model;

	m_automatic_fire = false;

	for (int i = 0; i < NBVAL_Equipment; i++)
		m_equipment[i] = NULL;

	m_display_name = "Wisteria";
	m_weapon = NULL;
	m_FX_death = NULL;
	m_moving = false;
	m_movingX = m_movingY = false;
	m_visible = true;
	m_DontGarbageMe = true;
	m_damage = 0;
	m_hyperspeed = 1.0f;
	m_armor = 1;
	m_shield = 0;
	m_shield_regen = 0;
	m_disable_inputs = false;
	m_disable_fire = false;
	m_disableHyperspeed = false;
	m_disableSlowmotion = false;
	m_disableRecall = false;
	m_disableJump = false;
	m_graze_count = 0;
	m_graze_level = 0;
	m_last_hazard_level_played = 0;
	m_disable_bots = true;
	m_is_asking_scene_transition = false;
	m_currentScene_hazard = 0;
	m_input_blocker = NULL;
	m_is_jumping = false;
	m_hits_taken = 0;
	m_release_to_fire = false;
	m_release_to_throw = false;

	m_level = 1;
	m_level_max = FIRST_LEVEL_MAX;
	m_xp = 0;
	m_xp_max = XP_MAX_FIRST_LEVEL;

	m_combo_count = 0;
	m_combo_count_max = COMBO_COUNT_FIRST_LEVEL;
	m_combo_level = 0;

	m_fake_ship = NULL;
	if (!ship_model->m_fake_textureName.empty())
		m_fake_ship = new FakeShip(this, ship_model->m_fake_textureName, ship_model->m_fake_size, ship_model->m_fake_frameNumber, NB_ShipAnimations);
		//(*CurrentGame).addToScene(m_fake_ship, false);

	m_combo_aura = new Aura(this, "2D/FX/Aura_Graze.png", sf::Vector2f(50, 50), 3, NB_GRAZE_LEVELS);
	//(*CurrentGame).addToScene(m_combo_aura, AuraLayer, Neutral);

	m_trail = new Aura(this, "2D/FX/Aura_HyperspeedTrail.png", sf::Vector2f(20, 34), 3, 1);
	m_trail->m_DontGarbageMe = true;
	sf::Vector2f real_size = m_fake_ship ? m_fake_ship->m_size : m_size;
	m_trail->m_offset = sf::Vector2f(0, (real_size.y / 2) + (m_trail->m_size.y / 2));
	//(*CurrentGame).addToScene(m_trail, false);

	for (int i = 0; i < GRAZING_FEEDBACK_CIRCLE_POINTS; i++)
	{
		float angle = 360.f * i / (GRAZING_FEEDBACK_CIRCLE_POINTS - 1);
		m_graze_percent_points[i * 2].color = sf::Color(0, 0, 0, 0);
		m_graze_percent_points[i * 2 + 1].color = m_graze_percent_points[i * 2].color;
	}

	m_targetPortal = NULL;
	m_targetShop = NULL;
	m_equipment_loot = NULL;
	m_weapon_loot = NULL;

	m_SFTargetPanel = NULL;
	m_is_asking_SFPanel = SFPanel_None;
	m_SFHudPanel = NULL;

	m_previouslyCollidingWithInteractiveObject = No_Interaction;
	m_isCollidingWithInteractiveObject = No_Interaction;
	m_HUD_state = HUD_Idle;

	for (size_t i = 0; i < NBVAL_PlayerActions; i++)
	{
		m_inputs_states[i] = Input_Release;
		m_actions_states[i] = false;
	}

	//feedback
	sf::Color _white = sf::Color::Color(255, 255, 255, 255);//white
	SFText* text_feedback = new SFText((*CurrentGame).m_font[Font_Terminator], 14, _white, getPosition());
	sf::Vector2f size = m_fake_ship ? m_fake_ship->m_size : m_size;
	m_recall_text = new SFTextPop(text_feedback, 0, -1, 0, this, 0, sf::Vector2f(0, -size.y / 2));
	m_recall_text->setPosition(sf::Vector2f(m_recall_text->getPosition().x - m_recall_text->getGlobalBounds().width / 2, m_recall_text->getPosition().y));
	m_recall_text->m_DontGarbageMe = true;
	m_recall_text->m_visible = false;
	delete text_feedback;
	(*CurrentGame).addToTextPops(m_recall_text);

	Init();
}

Ship::~Ship()
{
	for (int i = 0; i < NBVAL_Equipment; i++)
		delete m_equipment[i];

	delete m_ship_model;
	delete m_weapon;
	delete m_FX_death;

	for (Bot* bot : m_bot_list)
		delete bot;

	//game objects
	delete m_combo_aura;
	delete m_fake_ship;;
	delete m_trail;

	if (m_recall_text != NULL)
		m_recall_text->GarbageMe();
}

void Ship::Init()
{
	UpdateShipLevel();

	m_max_speed = getFighterFloatStatValue(Fighter_MaxSpeed);
	m_deceleration = getFighterFloatStatValue(Fighter_Deceleration);
	m_acceleration = getFighterFloatStatValue(Fighter_Acceleration);
	m_hyperspeed = getFighterFloatStatValue(Fighter_Hyperspeed);
	m_hyperspeed_fuel_max = getFighterIntStatValue(Fighter_HyperspeedFuel);
	if (m_hyperspeed_fuel > m_hyperspeed_fuel_max)
	{
		m_hyperspeed_fuel = m_hyperspeed_fuel_max;
	}
	m_armor_max = getFighterIntStatValue(Fighter_ArmorMax);
	if (m_armor > m_armor_max)
	{
		m_armor = m_armor_max;
	}
	m_shield_max = getFighterIntStatValue(Fighter_ShieldMax);
	if (m_shield > m_shield_max)
	{
		m_shield = m_shield_max;
	}
	m_shield_regen = getFighterIntStatValue(Fighter_ShieldRegen);
	m_shield_recovery_time = getFighterFloatStatValue(Fighter_ShieldRecovery);
	m_damage = getFighterIntStatValue(Fighter_ContactDamage);
	m_size = !m_ship_model->m_fake_textureName.empty() ? m_ship_model->m_fake_size : m_ship_model->m_size;
	m_textureName = m_ship_model->m_textureName;
	m_frameNumber = m_ship_model->m_frameNumber;

	//fake texture
	m_transparent = !m_ship_model->m_fake_textureName.empty();

	//Loading bots
	m_bot_list.clear();
	if (m_ship_model->m_bot)
	{
		m_bot_list.push_back(m_ship_model->m_bot);
	}
	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i] != NULL)
		{
			for (Bot* bot : m_equipment[i]->m_bots)
				m_bot_list.push_back(bot);
		}
	}
}

void Ship::CleanGarbagedEquipments()
{
	size_t shipEquipmentsSize = m_garbageEquipments.size();
	for (size_t i = 0; i < shipEquipmentsSize; i++)
	{
		if (m_garbageEquipments[i] == NULL)
			continue;

		delete m_garbageEquipments[i];
	}
	m_garbageEquipments.clear();

	size_t shipWeaponsSize = m_garbageWeapons.size();
	for (size_t i = 0; i < shipWeaponsSize; i++)
	{
		if (m_garbageWeapons[i] == NULL)
			continue;

		delete m_garbageWeapons[i];
	}
	m_garbageWeapons.clear();
}

bool Ship::setShipEquipment(Equipment* equipment, bool overwrite_existing, bool no_save)
{
	assert(equipment != NULL);

	if (!overwrite_existing && m_equipment[equipment->m_equipmentType])
	{
		return false;
	}

	if (overwrite_existing && m_equipment[equipment->m_equipmentType] && !m_equipment[equipment->m_equipmentType]->m_bots.empty())
	{
		DestroyBots();
	}

	if (overwrite_existing && m_equipment[equipment->m_equipmentType])
	{
		m_garbageEquipments.push_back(m_equipment[equipment->m_equipmentType]);
	}

	m_equipment[equipment->m_equipmentType] = equipment;
	
	Ship::Init();

	if (!equipment->m_bots.empty())
	{
		GenerateBots(this);
	}
	
	if (!no_save)
		Ship::SaveItems(this);

	return true;
}

bool Ship::setShipWeapon(Weapon* weapon, bool overwrite_existing, bool no_save)
{
	assert(weapon != NULL);

	if (!overwrite_existing && m_weapon)
	{
		return false;
	}

	if (overwrite_existing && m_weapon)
	{
		m_garbageWeapons.push_back(m_weapon);
	}
		
	m_weapon = weapon;
		
	Init();

	if (!no_save)
		Ship::SaveItems(this);
	
	return true;
}

void Ship::cleanEquipment(int equipment_type, bool no_save)
{
	if (m_equipment[equipment_type])
	{
		if (!m_equipment[equipment_type]->m_bots.empty())
		{
			DestroyBots();
		}
		delete m_equipment[equipment_type];
		m_equipment[equipment_type] = NULL;

		this->Init();
	}

	if (!no_save)
		Ship::SaveItems(this);
}

void Ship::cleanWeapon(bool no_save)
{
	if (m_weapon)
	{
		delete m_weapon;
		m_weapon = NULL;

		Init();
	}

	if (!no_save)
		Ship::SaveItems(this);
}

void Ship::setShipModel(ShipModel* ship_model, bool no_save)
{
	assert(ship_model != NULL);

	if (ship_model->m_bot)
	{
		DestroyBots();
	}

	m_ship_model = ship_model;
	Init();

	if (ship_model->m_bot)
	{
		GenerateBots(this);
	}

	if (!no_save)
		Ship::SaveItems(this);
}

void Ship::GetInputState(bool input_guy_boolean, PlayerActions action)
{
	if (input_guy_boolean)
	{
		m_inputs_states[action] = m_inputs_states[action] == Input_Release ? Input_Tap : Input_Hold;
		
	}
	else
	{
		m_inputs_states[action] = Input_Release;
	}
}

//Inputs
void Ship::UpdateInputStates()
{
	GetInputState(InputGuy::isFiring(), Action_Firing);
	GetInputState(InputGuy::isBraking(), Action_Braking);
	GetInputState(InputGuy::isHyperspeeding(), Action_Hyperspeeding);
	GetInputState(InputGuy::isSlowMotion(), Action_Slowmotion);
	GetInputState(InputGuy::isRecalling(), Action_Recalling);
	GetInputState(InputGuy::isOpeningHud(), Action_OpeningHud);
	GetInputState(InputGuy::isChangingResolution(), Action_ChangingResolution);
	GetInputState(InputGuy::setAutomaticFire(), Action_AutomaticFire);
	GetInputState(InputGuy::isUsingDebugCommand(), Action_DebugCommand);
	GetInputState(InputGuy::getDirections().x < - 1.f * JOYSTICK_MIN_AXIS_VALUE / 100, Action_Left);
	GetInputState(InputGuy::getDirections().x > 1.f * JOYSTICK_MIN_AXIS_VALUE / 100, Action_Right);
	GetInputState(InputGuy::getDirections().y < - 1.f * JOYSTICK_MIN_AXIS_VALUE / 100, Action_Up);
	GetInputState(InputGuy::getDirections().y > 1.f * JOYSTICK_MIN_AXIS_VALUE / 100, Action_Down);
}

bool Ship::UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition)
{
	if (state_required == Input_Tap && condition && m_inputs_states[action] == Input_Tap)
	{
		m_actions_states[action] = !m_actions_states[action];
		return true;
	}
	else if (state_required == Input_Hold && condition)
	{
		m_actions_states[action] = m_inputs_states[action] == Input_Hold;
		return true;
	}
	else if (!condition)
	{
		m_actions_states[action] = false;
	}
	return false;
}

void Ship::ManageGrazingFeedback()
{
	if (m_shield_max > 0 && m_shield < m_shield_max && m_collision_timer <= 0)
	{
		float radius = GRAZE_DISTANCE;
		float angle_to_fill = 360.f * m_graze_count / (GRAZING_COUNT_TO_REGEN_SHIELD * (m_shield + 1));
		float thickness = 2;

		//position
		for (int i = 0; i < GRAZING_FEEDBACK_CIRCLE_POINTS * 2; i++)
		{
			float angle = 360.f * (i / 2) / (GRAZING_FEEDBACK_CIRCLE_POINTS - 1);

			m_graze_percent_points[i].position.x = getPosition().x + (radius + (i % 2) * thickness) * cos(- M_PI_2 + angle * M_PI / 180);
			m_graze_percent_points[i].position.y = getPosition().y + (radius + (i % 2) * thickness) * sin(- M_PI_2 + angle * M_PI / 180);

			//color
			if (angle_to_fill > 1.f / (GRAZING_FEEDBACK_CIRCLE_POINTS - 1) && angle <= angle_to_fill)
			{
				float s = 0.50 + 3 * abs(sin(m_graze_sinus_clock.getElapsedTime().asSeconds())) / 4;
				m_graze_percent_points[i].color = sf::Color(0, 0, 255, 200 * s);
			}
				
			else
			{
				float s = 0.25 + 3 * abs(sin(4 * m_graze_sinus_clock.getElapsedTime().asSeconds())) / 4;
				m_graze_percent_points[i].color = sf::Color(255, 255, 255, 20 * s);
			}
		}
	}
	else
	{
		for (int i = 0; i < GRAZING_FEEDBACK_CIRCLE_POINTS * 2; i++)
			m_graze_percent_points[i].color = sf::Color(0, 0, 0, 0);
	}
}

void Ship::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	float l_hyperspeedMuliplier = hyperspeedMultiplier < 1 ? hyperspeedMultiplier : 1;

	//Update feedback timers
	if (m_damage_feedbackTimer > 0)
	{
		m_damage_feedbackTimer -= deltaTime.asSeconds();
		if (m_damage_feedbackTimer <= 0)
		{
			setColor(Color(255, 255, 255, 255), true);
	
			if (m_ship_model->m_fake_textureName.empty() == false)
				m_fake_ship->setColor(Color(255, 255, 255, 255), true);
		}
	}

	ManageImmunity(deltaTime * l_hyperspeedMuliplier);

	if (m_visible == false)
		return;
	
	//clean the dust
	CleanGarbagedEquipments();
	
	//Resetting flags
	if (m_isCollidingWithInteractiveObject != PortalInteraction && m_is_asking_scene_transition == false)
		m_targetPortal = NULL;
	
	if (m_isCollidingWithInteractiveObject != ShopInteraction)
		m_targetShop = NULL;
	
	m_isCollidingWithInteractiveObject = No_Interaction;
	m_movingX = false;
	m_movingY = false;
	m_moving = false;
	m_is_asking_SFPanel = SFPanel_None;

	//Graze feedback
	ManageGrazingFeedback();
	
	sf::Vector2f directions = InputGuy::getDirections();
	
	//Update dialogs that have a limited lifespam
	if (m_targetDialogs.empty() == false && m_SFTargetPanel != NULL && m_SFTargetPanel->m_panel_type == SFPanel_Dialog && m_SFTargetPanel->GetDuration() > 0 && m_SFTargetPanel->GetDurationTimer() <= 0)
		ContinueDialog();

	//Manage inputs
	ManageInputs(deltaTime, hyperspeedMultiplier, directions);

	m_trail->m_visible = (hyperspeedMultiplier > 1.0f);
	
	GameObject::update(deltaTime, hyperspeedMultiplier);
	ScreenBorderConstraints();
	SettingTurnAnimations();
	
	ManageGhost(deltaTime * l_hyperspeedMuliplier);
	ManageJumpFeedbacks();
	
	//member objects follow
	if (m_combo_aura != NULL)
		m_combo_aura->update(deltaTime, hyperspeedMultiplier);
	
	if (m_trail != NULL)
		m_trail->update(deltaTime, hyperspeedMultiplier);
	
	if (m_fake_ship != NULL)
		m_fake_ship->update(deltaTime, hyperspeedMultiplier);
}

void Ship::Draw(sf::RenderTexture& screen)
{
	if (m_visible == true)
	{
		//if (m_combo_aura != NULL)
		//	m_combo_aura->Draw(screen);

		if (m_fake_ship != NULL)
			m_fake_ship->Draw(screen);

		if (m_trail != NULL)
			m_trail->Draw(screen);

		GameObject::Draw(screen);

		if ((*CurrentGame).m_is_in_hub == false && m_shield_max > 0 && m_shield < m_shield_max && m_collision_timer <= 0)
			screen.draw(m_graze_percent_points, GRAZING_FEEDBACK_CIRCLE_POINTS * 2, sf::TrianglesStrip);
	}
}

void Ship::ManageJumpFeedbacks()
{
	if (m_is_jumping == true)
		PlayStroboscopicEffect(sf::seconds(0.1), sf::seconds(0.01));
}

bool Ship::IsVisible()
{
	//no update on invisible ship (=dead)
	if (m_fake_ship)
	{
		return m_fake_ship->m_visible;
	}
	else
	{
		return m_visible;
	}
}

void Ship::ManageShieldRegen(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if ((*CurrentGame).m_waiting_for_dialog_validation == true)
	{
		AnimatedSprite::update(deltaTime);
		return;
	}

	//sheld regen if not maximum
	if (m_shield > 0 || m_shield_recovery_clock.getElapsedTime().asSeconds() > m_shield_recovery_time)
	{
		ShieldRegen(deltaTime, hyperspeedMultiplier);
	}
}

bool Ship::ManageFiring(sf::Time deltaTime, float hyperspeedMultiplier)
{
	bool firing = m_collision_timer <= 0 && m_disable_inputs == false && m_disable_fire == false && m_release_to_fire == false && m_HUD_state != HUD_OpeningEquipment && m_recall_text->m_visible == false && (*CurrentGame).m_end_dialog_clock.getElapsedTime().asSeconds() > END_OF_DIALOGS_DELAY
		&& (*CurrentGame).m_waiting_for_dialog_validation == false && hyperspeedMultiplier <= 1 && (m_actions_states[Action_Firing] == true || m_automatic_fire == true);

	//Fire function
	if (m_weapon != NULL)
	{
		//UPDATE WEAPON POSITION
		float target_angle = getRotation();//calculating the angle we want to face, if any
		if (m_weapon->m_target_homing != NO_HOMING || (m_weapon->m_target_homing == SEMI_HOMING && m_weapon->m_rafale_index == 0))
			target_angle = fmod(- (*CurrentGame).GetAngleToNearestGameObject(EnemyObject, getPosition()), 360);

		float current_angle = getRotation();
		float delta = current_angle - target_angle;
		if (delta > 180)
			delta -= 360;
		else if (delta < -180)
			delta += 360;

		float theta = getRotation() + m_weapon->m_angle_offset;

		if (m_weapon->m_target_homing != NO_HOMING)
			theta -= delta;

		if (m_weapon->hasLockingSalvoInProgress() == true)
		{
			//semi-HOMING and rafale not ended => no update of target or weapon position
		}
		else
		{
			//calcule weapon offset
			theta *= M_PI / 180;//switching to radians
			m_weapon->m_weapon_current_offset.x = m_weapon->m_weaponOffset.x * cos(theta) + m_size.y / 2 * sin(theta) * (- m_weapon->m_fire_direction);
			m_weapon->m_weapon_current_offset.y = m_weapon->m_weaponOffset.x * sin(theta) - m_size.y / 2 * cos(theta) * (- m_weapon->m_fire_direction);

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
				//(*CurrentGame).PlaySFX(SFX_Fire);//todo: use ammo->sound file
			}
		}

		//UPDATE BEAMS
		m_weapon->UpdateBeams(firing);
	}

	//Bots firing
	for (Bot* bot : m_bot_list)
		bot->Fire(deltaTime, (*CurrentGame).m_hyperspeedMultiplier, firing);

	//not considered "firing" between rafales
	if (firing == true && m_weapon != NULL)
	{
		if (m_weapon->m_rafale < 0 && m_weapon->m_beams.empty() == true)
			firing = false;
		else if (m_weapon->m_rafale > 0 && m_weapon->m_rafale_index > 0)
			firing = false;
	}

	//not considered firing if no weapon and no bot to fire
	if (m_weapon == NULL && m_bot_list.empty() == true)
		firing = false;

	return firing;
}

void Ship::UpdateHUDStates()
{
	if (!m_targetDialogs.empty())
	{
		m_is_asking_SFPanel = m_is_asking_SFPanel != SFPanel_DialogNext ? SFPanel_Dialog : SFPanel_DialogNext;
		if (m_targetDialogs.front()->m_duration == 0)
			m_HUD_state = HUD_Dialog;
	}
	else if (m_HUD_state != HUD_OpeningEquipment)
	{
		if (m_targetPortal && m_targetPortal->m_state == PortalOpen && !m_actions_states[Action_Firing])
			m_HUD_state = HUD_PortalInteraction;
		else if (m_targetShop != NULL)
		{
			if (m_HUD_state != HUD_ShopStellarMap && m_HUD_state != HUD_Trade && m_HUD_state != HUD_Upgrades)// m_HUD_state != HUD_ShopSellMenu && m_HUD_state != HUD_ShopStellarMap)
				m_HUD_state = HUD_ShopMainMenu;
		}
		else
			m_HUD_state = HUD_Idle;
	}
}

void Ship::ManageInputs(sf::Time deltaTime, float hyperspeedMultiplier, sf::Vector2f inputs_direction)
{
	//Registering inputs
	UpdateInputStates();

	if (m_disable_inputs == false)
	{
		if (m_release_to_fire == true && m_inputs_states[Action_Firing] == Input_Release)
			m_release_to_fire = false;

		if (m_release_to_throw == true && m_inputs_states[Action_Braking] == Input_Release)
			m_release_to_throw = false;

		//Debug command
		#ifndef NDEBUG
			if (m_inputs_states[Action_DebugCommand] == Input_Tap || m_inputs_states[Action_DebugCommand] == Input_Hold)
				(*CurrentGame).killGameObjectType(EnemyObject);
		#endif

		//Update HUD state
		UpdateHUDStates();

		//DIALOG
		if (m_HUD_state == HUD_Dialog && m_SFHudPanel)
		{
			//waiting for player input to continue
			if (m_SFTargetPanel && m_SFTargetPanel->GetDuration() == 0)
			{
				//meanwhile, close HUD menu if open and go back to normal hyperspeed
				if (m_SFHudPanel->GetCursor()->m_visible == true)
				{
					(*CurrentGame).m_hyperspeedMultiplier = 1;
					m_SFHudPanel->SetCursorVisible_v2(false);
				}

				//press A to continue
				if  (m_inputs_states[Action_Firing] == Input_Tap)
				{
					ContinueDialog();
				}
			}
		}
		//Enemy blocking movement?
		else if (m_input_blocker)
		{
			m_speed = sf::Vector2f(0, 0);
			return;
		}
		//EQUIPMENT HUD
		else if (m_HUD_state == HUD_OpeningEquipment && m_SFHudPanel)
		{
			//Cursor movement
			m_SFHudPanel->SetCursorVisible_v2(true);

			//Freeze cursor if throwing item, otherwise move it
			if (m_SFHudPanel->GetCursor()->m_currentAnimationIndex < Cursor_Focus1_8 || m_SFHudPanel->GetCursor()->m_currentAnimationIndex > Cursor_Focus8_8)
				MoveCursor(m_SFHudPanel->GetCursor(), inputs_direction, deltaTime, m_SFHudPanel);

			//Actions
			GridElement* highlighted_element = (m_SFTargetPanel == NULL || m_SFTargetPanel->GetHighlightedElement() == NULL) ? (m_SFHudPanel->GetHighlightedElement() == NULL ? NULL : m_SFHudPanel->GetHighlightedElement()) : m_SFTargetPanel->GetHighlightedElement();
			if (highlighted_element != NULL && highlighted_element->m_object != NULL)
			{
				//A: equip item
				if (m_inputs_states[Action_Firing] == Input_Tap && highlighted_element->m_grid == m_SFHudPanel->GetGrid_v2(Trade_StashGrid))
					EquipItem_v2(highlighted_element);

				//A: desequip from HUD panel
				else if (m_inputs_states[Action_Firing] == Input_Tap && highlighted_element->m_grid == m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid))
					DesquipItem_v2(highlighted_element);

				//A: desequip from HUD panel
				else if ((m_inputs_states[Action_Braking] == Input_Tap || m_inputs_states[Action_Braking] == Input_Hold))
					ThrowingItem_v2(highlighted_element);

				else
					m_SFHudPanel->SetPrioritaryFeedback(false);
			}

			//Weapon firing
			bool firing = ManageFiring(deltaTime, hyperspeedMultiplier);

			//Closing hud
			if (UpdateAction(Action_OpeningHud, Input_Tap, true))
			{
				m_HUD_state = HUD_Idle;
				if (m_SFHudPanel)
				{
					m_SFHudPanel->SetCursorVisible_v2(false);
				}
			}
		}
		//TRADE (BUY/SELL)
		else if (m_HUD_state == HUD_Trade && m_SFTargetPanel)
		{
			//Cursor movement
			MoveCursor(m_SFTargetPanel->GetCursor(), inputs_direction, deltaTime, m_SFTargetPanel);

			GridElement* highlighted_element = (m_SFTargetPanel == NULL || m_SFTargetPanel->GetHighlightedElement() == NULL) ? (m_SFHudPanel->GetHighlightedElement() == NULL ? NULL : m_SFHudPanel->GetHighlightedElement()) : m_SFTargetPanel->GetHighlightedElement();
			if (highlighted_element != NULL && highlighted_element->m_object != NULL)
			{
				//A: buy item
				if (m_inputs_states[Action_Firing] == Input_Tap && highlighted_element->m_grid == m_SFTargetPanel->GetGrid_v2(Trade_ShopGrid))
					BuyingItem_v2(highlighted_element, true);

				//X: buy item and equip it directly
				else if (m_inputs_states[Action_Braking] == Input_Tap && highlighted_element->m_grid == m_SFTargetPanel->GetGrid_v2(Trade_ShopGrid))
					BuyingItem_v2(highlighted_element, false);

				//X: equip item
				else if (m_inputs_states[Action_Braking] == Input_Tap && highlighted_element->m_grid == m_SFTargetPanel->GetGrid_v2(Trade_StashGrid))
					EquipItem_v2(highlighted_element);

				//A: sell
				else if (m_inputs_states[Action_Firing] == Input_Tap && (highlighted_element->m_grid == m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid) || highlighted_element->m_grid == m_SFTargetPanel->GetGrid_v2(Trade_StashGrid)))
					SellingItem_v2(highlighted_element);

				//X: desequip
				else if (m_inputs_states[Action_Braking] == Input_Tap && highlighted_element->m_grid == m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid))
					DesquipItem_v2(highlighted_element);
			}

			//exit
			if (m_inputs_states[Action_Slowmotion] == Input_Tap)
			{
				m_HUD_state = HUD_ShopMainMenu;
				m_SFTargetPanel->SetCursorVisible_v2(false);
			}
		}
		//STELLAR MAP
		else if (m_HUD_state == HUD_ShopStellarMap)
		{
			//Cursor movement
			MoveCursor(m_SFTargetPanel->GetCursor(), inputs_direction, deltaTime, m_SFTargetPanel);

			//Teleportation
			//if (m_SFTargetPanel->GetTeleportationCost() > 0 && m_money >= m_SFTargetPanel->GetTeleportationCost())
			//{
			//	if (m_inputs_states[Action_Firing] == Input_Tap)
			//	{
			//		Teleport(m_SFTargetPanel->GetTeleportationDestination());
			//		m_money -= m_SFTargetPanel->GetTeleportationCost();
			//		SavePlayerMoneyAndHealth(this);
			//		m_HUD_state = HUD_Idle;
			//	}
			//}

			//Center view
			if (m_inputs_states[Action_Braking] == Input_Tap)
			{
				CenterMapView();
			}

			//exit
			if (m_inputs_states[Action_Slowmotion] == Input_Tap)
			{
				m_HUD_state = HUD_ShopMainMenu;
				m_SFTargetPanel->SetCursorVisible_v2(true);
			}
		}
		//UPGRADES PANEL
		else if (m_HUD_state == HUD_Upgrades && m_SFTargetPanel)
		{	
			//A: buy upgrade
			if (m_inputs_states[Action_Firing] == Input_Tap)
			{
				m_SFTargetPanel->BuyUpgrade();
			}
			
			//exit
			if (m_inputs_states[Action_Slowmotion] == Input_Tap)
			{
				m_HUD_state = HUD_ShopMainMenu;
				m_SFTargetPanel->SetCursorVisible_v2(false);
			}
		}
		else//Idle combat, shop menu, portal menu (where ship can continue to move)
		{
			//Opening hud
			if (UpdateAction(Action_OpeningHud, Input_Tap, true))
			{
				m_HUD_state = HUD_OpeningEquipment;
				m_SFHudPanel->SetCursorVisible_v2(true);
				
				if (!m_disableSlowmotion)
				{
					(*CurrentGame).m_hyperspeedMultiplier = 1.0f / m_hyperspeed;
				}

				m_actions_states[Action_Firing] = false;
			}

			//Moving
			m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
			m_movingX = inputs_direction.x != 0;
			m_movingY = inputs_direction.y != 0;
			if (m_recall_text->m_visible == false)
			{
				ManageAcceleration(inputs_direction);
			}

			//IDLE (COMBAT)
			if (m_HUD_state == HUD_Idle)
			{
				//Recalling back to last hub
				UpdateAction(Action_Recalling, Input_Hold, !m_disableRecall);

				if (m_actions_states[Action_Recalling] == true && m_respawnSceneName.empty() == false)
					Recalling();
				else
					m_recall_text->m_visible = false;

				//Slow_motion and hyperspeed
				UpdateAction(Action_Hyperspeeding, Input_Hold, !m_disableHyperspeed && !m_actions_states[Action_Recalling]);
				UpdateAction(Action_Slowmotion, Input_Tap, !m_disableSlowmotion);

				//hyperspeed
				if (CanHyperspeed() && m_actions_states[Action_Hyperspeeding] && m_hyperspeed_fuel > 0)
				{
					(*CurrentGame).m_hyperspeedMultiplier = m_hyperspeed;
					m_hyperspeed_fuel -= m_hyperspeed * HYPERSPEED_CONSUMPTION_FOR_CRUISING * deltaTime.asSeconds();
					if (m_hyperspeed_fuel < 0)
					{
						m_hyperspeed_fuel = 0;
					}
				}

				//jump
				else if (CanJump() && m_inputs_states[Action_Hyperspeeding] == Input_Tap && !m_disableJump && m_hyperspeed_fuel >= SHIP_JUMPING_COST && m_moving)
				{
					m_hyperspeed_fuel -= SHIP_JUMPING_COST;

					Jump();
				}

				//bomb
				else if ((*CurrentGame).m_is_in_hub == false && GetNumberOfBombs() > 0 && m_inputs_states[Action_Slowmotion] == Input_Tap && m_hyperspeed_fuel > (m_hyperspeed_fuel_max / GetNumberOfBombs()) && !m_actions_states[Action_Recalling] && !m_immune)
				{
					m_hyperspeed_fuel -= m_hyperspeed_fuel_max / GetNumberOfBombs();
					Bomb();
				}
				else if (!m_actions_states[Action_Recalling])
				{
					(*CurrentGame).m_hyperspeedMultiplier = 1.0f;//resetting hyperspeed
				}

				//Auto fire option (F key)
				if (UpdateAction(Action_AutomaticFire, Input_Tap, !m_disable_fire))
				{
					//Bots automatic fire option
					if (m_inputs_states[Action_AutomaticFire] == Input_Tap)
						m_automatic_fire = !m_automatic_fire;
					for (Bot* bot : m_bot_list)
						bot->m_automatic_fire = m_automatic_fire;
				}

				//Firing button
				UpdateAction(Action_Firing, Input_Hold, !m_disable_fire);

				//Weapon firing
				bool firing = ManageFiring(deltaTime, hyperspeedMultiplier);

				//Bots firing
				for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
				{
					(*it)->Fire(deltaTime, hyperspeedMultiplier, firing);
				}

				//Braking and speed malus on firing
				UpdateAction(Action_Braking, Input_Hold, !m_actions_states[Action_Recalling]);
				//brake speed malus
				if (firing == true || (m_actions_states[Action_Braking] == Input_Tap || m_actions_states[Action_Braking] == Input_Hold))
				{
					m_speed.x *= SHIP_BRAKING_MALUS_SPEED;
					m_speed.y *= SHIP_BRAKING_MALUS_SPEED;
				}
			}
			//PORTAL
			else if (m_HUD_state == HUD_PortalInteraction)
			{
				//Up and down in options, IF LEVEL CHOOSER IS AVAILABLE
				if (m_SFTargetPanel)
				{
					if (m_inputs_states[Action_Braking] == Input_Tap && m_SFTargetPanel->GetSelectedOptionIndex() < m_targetPortal->m_max_unlocked_hazard_level)
					{
						m_SFTargetPanel->SetSelectedOptionIndex(m_SFTargetPanel->GetSelectedOptionIndex() + 1);
					}
					else if (m_inputs_states[Action_Hyperspeeding] == Input_Tap && m_SFTargetPanel->GetSelectedOptionIndex() > 0)
					{
						m_SFTargetPanel->SetSelectedOptionIndex(m_SFTargetPanel->GetSelectedOptionIndex() - 1);
					}
				}

				//Weapon firing
				bool firing = ManageFiring(deltaTime, hyperspeedMultiplier);

				//Bots firing
				for (Bot* bot : m_bot_list)
				{
					bot->Fire(deltaTime, (*CurrentGame).m_hyperspeedMultiplier, firing);
				}

				//brake speed malus
				if (firing == true || (m_actions_states[Action_Braking] == Input_Tap || m_actions_states[Action_Braking] == Input_Hold))
				{
					m_speed.x *= SHIP_BRAKING_MALUS_SPEED;
					m_speed.y *= SHIP_BRAKING_MALUS_SPEED;
				}

				//Entering portal
				if (m_inputs_states[Action_Firing] == Input_Tap)
				{
					m_is_asking_scene_transition = true;//this triggers transition in InGameState update
					//m_immune = true;
				}
			}
			//SHOP MAIN
			else if (m_HUD_state == HUD_ShopMainMenu && m_SFTargetPanel)
			{
				//A: Enter shop
				if (m_inputs_states[Action_Firing] == Input_Tap)
				{
					//m_HUD_state = HUD_Trade;
					m_HUD_state = HUD_Upgrades;
				}
				//X: Enter stellar map
				else if (m_inputs_states[Action_Braking] == Input_Tap)
				{
					m_HUD_state = HUD_ShopStellarMap;
				}
			}
		}
		
		IdleDecelleration(deltaTime);
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	if (m_is_jumping)
	{
		if (m_is_jumping && m_jump_clock.getElapsedTime().asSeconds() > SHIP_JUMPING_DISTANCE / SHIP_JUMPING_SPEED)
		{
			m_is_jumping = false;
			m_speed = sf::Vector2f(0, 0);
		}
		else
		{
			return;
		}
	}

	m_speed.x += inputs_direction.x*getFighterFloatStatValue(Fighter_Acceleration);
	m_speed.y += inputs_direction.y*getFighterFloatStatValue(Fighter_Acceleration);

	//max speed constraints
	GameObject::NormalizeVector(&m_speed, getFighterFloatStatValue(Fighter_MaxSpeed));
}

void Ship::ManageGhost(sf::Time deltaTime)
{
	//ghost status expires?
	if (m_ghost_timer > sf::seconds(0))
	{
		m_ghost_timer -= deltaTime;
		setGhost(m_ghost_timer >= sf::seconds(0));
	}
}

void Ship::ManageImmunity(sf::Time deltaTime)
{
	//immunity after hit taken
	if (m_collision_timer > 0)
	{
		m_collision_timer -= deltaTime.asSeconds();

		if (m_collision_timer > 0)
		{
			int alpha = (int)(255 * (sin(m_collision_timer * 10) + 1) / 2);
			setColor(sf::Color(m_color.r, m_color.g, m_color.b, alpha));
			if (m_fake_ship != NULL)
				m_fake_ship->setColor(sf::Color(m_color.r, m_color.g, m_color.b, alpha));

			//death
			if (m_armor <= 0)
				m_visible = false;
		}
		else
		{
			setColor(sf::Color(255, 255, 255, 255));
			if (m_fake_ship != NULL)
				m_fake_ship->setColor(sf::Color(255, 255, 255, 255));
		}
	}
}

void Ship::MoveCursor(GameObject* cursor, sf::Vector2f inputs_directions, sf::Time deltaTime, SFPanel* container)
{
	if (!cursor)
		return;

	cursor->m_speed.x = inputs_directions.x * HUD_CURSOR_SPEED;
	cursor->m_speed.y = inputs_directions.y * HUD_CURSOR_SPEED;

	cursor->update(deltaTime);
	
	if (container)
	{
		//panel constraints
		sf::Vector2f panel_size = container->getSize();
		sf::Vector2f panel_pos = container->getOrigin() == sf::Vector2f(0, 0) ? sf::Vector2f(container->getPosition().x + panel_size.x / 2, container->getPosition().y + panel_size.y / 2) : container->getPosition();
		sf::Vector2f cursor_pos = cursor->getPosition();
		if (cursor_pos.x < panel_pos.x - (panel_size.x / 2))
		{
			cursor->setPosition(panel_pos.x - (panel_size.x / 2), cursor_pos.y);
			cursor->m_speed.x = 0;
		}
		if (cursor_pos.x > panel_pos.x + (panel_size.x / 2))
		{
			cursor->setPosition(panel_pos.x + (panel_size.x / 2), cursor_pos.y);
			cursor->m_speed.x = 0;
		}
		if (cursor_pos.y < panel_pos.y - (panel_size.y / 2))
		{
			cursor->setPosition(cursor_pos.x, panel_pos.y - (panel_size.y / 2));
			cursor->m_speed.y = 0;
		}
		if (cursor_pos.y > panel_pos.y + (panel_size.y / 2))
		{
			cursor->setPosition(cursor_pos.x, panel_pos.y + (panel_size.y / 2));
			cursor->m_speed.y = 0;
		}
	}
}

bool Ship::BuyingItem_v2(GridElement* element, bool equip_directly)
{
	GameObject* item = element->m_object;
	int equipment_type = Grid::GetEquipmentType(item);
	int price = Grid::GetPrice(item);
	Grid* equipped_grid = m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid);
	Grid* stash_grid = m_SFHudPanel->GetGrid_v2(Trade_StashGrid);
	
	//check price
	if (price > m_money)
		return false;

	bool success = false;

	if (equip_directly == true)//try to equip it in priority
	{
		if (equipped_grid->InsertObject(item, equipment_type, false) == equipment_type)//try equip directly in the dedicated slot, success?
		{
			m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid)->InsertObject(item->Clone(), equipment_type, false);//clone in copy grid
			(*CurrentGame).PlaySFX(SFX_Equip);
			Equip_v2(item);

			success = true;
		}
		else if (stash_grid->InsertObject(equipped_grid->m_elements[equipment_type]->m_object, -1, false) >= 0)//try swapping equipped object with stash then equip it, success?
		{
			m_SFTargetPanel->GetGrid_v2(Trade_StashGrid)->InsertObject(m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid)->m_elements[equipment_type]->m_object, -1, false);//replicate in copy grid

			equipped_grid->InsertObject(item, equipment_type, true);
			m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid)->InsertObject(item->Clone(), equipment_type, true);//clone in copy grid

			(*CurrentGame).PlaySFX(SFX_Equip);
			Equip_v2(item);

			success = true;
		}
		else
			success = false;
	}
	else//try to stash it in priority
	{
		if (stash_grid->InsertObject(item, -1, false) >= 0)//try Inserting in the stash, success?)
		{
			m_SFTargetPanel->GetGrid_v2(Trade_StashGrid)->InsertObject(item->Clone(), -1, false);//clone in copy grid
			success = true;
		}
		else if (equipped_grid->InsertObject(item, equipment_type, false) == equipment_type)//try inserting in equipped slots, success?
		{
			m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid)->InsertObject(item->Clone(), equipment_type, false);//clone in copy grid
			(*CurrentGame).PlaySFX(SFX_Equip);
			Equip_v2(item);

			success = true;
		}
	}

	//pay the price and remove from shop
	if (success == true)
	{
		m_money -= price;
		//m_SFTargetPanel->GetGrid_v2(Trade_ShopGrid)->UpdateGreyMaskOnInsufficientCredits(m_money);//buggé
		
		(*CurrentGame).PlaySFX(SFX_BuyOrSell);

		//clear item from shop
		m_targetShop->m_grid_v2->InsertObject(NULL, element->m_index, true);

		Ship::SaveItems(this);
		Ship::SavePlayerMoneyAndHealth(this);
		//Shop::SaveShop(m_targetShop);
	}

	return success;
}

void Ship::SellingItem_v2(GridElement* element)
{
	GameObject* item = element->m_object;
	int equipment_type = Grid::GetEquipmentType(item);
	Grid* equipped_grid = m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid);
	Grid* stash_grid = m_SFHudPanel->GetGrid_v2(Trade_StashGrid);

	bool success = m_targetShop->m_grid_v2->InsertObject(item, -1, false) >= 0;//trying to insert in shop grid, success?

	//sell anyway
	element->m_grid->InsertObject(NULL, element->m_index, true);

	if (element->m_grid == m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid))//replicate in copy grid
		equipped_grid->InsertObject(NULL, element->m_index, true);
	else
		stash_grid->InsertObject(NULL, element->m_index, true);
	
	int price = Grid::GetPrice(item);
	m_money += price;
	//m_SFTargetPanel->GetGrid_v2(Trade_ShopGrid)->UpdateGreyMaskOnInsufficientCredits(m_money);//buggé
	
	(*CurrentGame).PlaySFX(SFX_BuyOrSell);

	if (success == false)//no space in shop, object has been sold and is lost permanently
		delete item;

	if (element->m_grid == m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid))
		Desequip_v2(item);

	Ship::SaveItems(this);
	Ship::SavePlayerMoneyAndHealth(this);
	//Shop::SaveShop(m_targetShop);
}

bool Ship::DesquipItem_v2(GridElement* element)
{
	GameObject* item = element->m_object;
	int equipment_type = Grid::GetEquipmentType(item);
	Grid* equipped_grid = element->m_grid->m_panel == m_SFHudPanel ? m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid) : m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid);
	Grid* stash_grid = element->m_grid->m_panel == m_SFHudPanel ? m_SFHudPanel->GetGrid_v2(Trade_StashGrid) : m_SFTargetPanel->GetGrid_v2(Trade_StashGrid);
	Grid* copy_equipped_grid = (m_SFTargetPanel != NULL && m_SFTargetPanel->m_panel_type == SFPanel_Trade) ? (element->m_grid == m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid) ? m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid) : m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)) : NULL;
	Grid* copy_stash_grid = (m_SFTargetPanel != NULL && m_SFTargetPanel->m_panel_type == SFPanel_Trade) ? (element->m_grid == m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid) ? m_SFTargetPanel->GetGrid_v2(Trade_StashGrid) : m_SFHudPanel->GetGrid_v2(Trade_StashGrid)) : NULL;
	
	if (stash_grid->InsertObject(item, -1, false) >= 0)
	{
		equipped_grid->InsertObject(NULL, element->m_index, true);

		if (copy_equipped_grid != NULL && copy_stash_grid != NULL)//replicate in copy grid if any
		{
			copy_stash_grid->InsertObject(copy_equipped_grid->m_elements[element->m_index]->m_object, -1, false);
			copy_equipped_grid->InsertObject(NULL, element->m_index, true);
		}

		Desequip_v2(item);

		return true;
	}

	return false;
}

bool Ship::EquipItem_v2(GridElement* element)
{
	GameObject* item = element->m_object;
	int equipment_type = Grid::GetEquipmentType(item);
	Grid* equipped_grid = element->m_grid->m_panel == m_SFHudPanel ? m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid) : m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid);
	Grid* stash_grid = element->m_grid->m_panel == m_SFHudPanel ? m_SFHudPanel->GetGrid_v2(Trade_StashGrid) : m_SFTargetPanel->GetGrid_v2(Trade_StashGrid);
	Grid* copy_equipped_grid = (m_SFTargetPanel != NULL && m_SFTargetPanel->m_panel_type == SFPanel_Trade) ? (element->m_grid == m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid) ? m_SFTargetPanel->GetGrid_v2(Trade_EquippedGrid) : m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)) : NULL;
	Grid* copy_stash_grid = (m_SFTargetPanel != NULL && m_SFTargetPanel->m_panel_type == SFPanel_Trade) ? (element->m_grid == m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid) ? m_SFTargetPanel->GetGrid_v2(Trade_StashGrid) : m_SFHudPanel->GetGrid_v2(Trade_StashGrid)) : NULL;

	if (equipped_grid->InsertObject(item, equipment_type, false) == equipment_type)//try equipping, success?
	{
		stash_grid->InsertObject(NULL, element->m_index, true);
		
		if (copy_equipped_grid != NULL && copy_stash_grid != NULL)//replicate in copy grid if any
		{
			copy_equipped_grid->InsertObject(copy_stash_grid->m_elements[element->m_index]->m_object, equipment_type, false);
			copy_stash_grid->InsertObject(NULL, element->m_index, true);
		}
	}
	else//swap item with stash
	{
		GameObject* item_tmp = equipped_grid->m_elements[equipment_type]->m_object;

		equipped_grid->InsertObject(item, equipment_type, true);
		stash_grid->InsertObject(item_tmp, element->m_index, true);

		if (copy_equipped_grid != NULL && copy_stash_grid != NULL)
		{
			GameObject* copy_item_tmp = copy_equipped_grid->m_elements[equipment_type]->m_object;//replicate in copy grid
			copy_equipped_grid->InsertObject(copy_stash_grid->m_elements[element->m_index]->m_object, equipment_type, true);
			copy_stash_grid->InsertObject(copy_item_tmp, element->m_index, true);
		}

		stash_grid->m_panel->SetHighlightedElement(NULL);//reset highlighted element in grid
	}

	Equip_v2(item);
	(*CurrentGame).PlaySFX(SFX_Equip);

	return false;
}

void Ship::ThrowingItem_v2(GridElement* element)
{
	if (m_inputs_states[Action_Braking] == Input_Tap)
	{
		m_brakingHoldingClock.restart();
		m_SFHudPanel->GetCursor()->setAnimationLine(Cursor_Focus1_8, false);
		m_SFHudPanel->SetPrioritaryFeedback(true);
	}
	else if (m_SFHudPanel->GetPrioritaryFeedback())
	{
		if (m_inputs_states[Action_Braking] != Input_Hold || m_release_to_throw == true)
		{
			m_brakingHoldingClock.restart();
			m_SFHudPanel->SetPrioritaryFeedback(false);
		}
		else
		{
			if (m_brakingHoldingClock.getElapsedTime() > sf::seconds(HUD_HOLD_TIME_BEFORE_REMOVE_ITEM))
			{
				m_SFHudPanel->GetCursor()->setAnimationLine(Cursor_Focus1_8);
				if (m_brakingHoldingClock.getElapsedTime().asSeconds() < HUD_HOLD_TIME_BEFORE_REMOVE_ITEM / 8)
					m_SFHudPanel->GetCursor()->setAnimationLine(Cursor_Focus1_8);

				//throw item
				if (element->m_grid == m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid))
					Desequip_v2(element->m_object);

				delete element->m_grid->m_elements[element->m_index]->m_object;
				element->m_grid->InsertObject(NULL, element->m_index, true);

				m_SFHudPanel->GetCursor()->setAnimationLine(Cursor_HighlightState, false);
				m_release_to_throw = true;
				
				//Save items
				SaveItems(this);

				m_brakingHoldingClock.restart();

			}
			//holding feedback
			else
			{
				for (int k = 0; k < HUD_CURSOR_HOLDING_FRACTIONS; k++)
				{
					if (m_brakingHoldingClock.getElapsedTime().asSeconds() < (1.0f * HUD_HOLD_TIME_BEFORE_REMOVE_ITEM / HUD_CURSOR_HOLDING_FRACTIONS) * (k + 1))
					{
						m_SFHudPanel->GetCursor()->setAnimationLine((CursorFeedbackStates)(Cursor_Focus1_8 + k));
						break;
					}
				}
			}
		}
	}
}

bool Ship::Equip_v2(GameObject* object)
{
	if (object->m_equipment_loot != NULL)
		return setShipEquipment(object->m_equipment_loot->Clone(), true, false);
	else
		return setShipWeapon(object->m_weapon_loot->Clone(), true, false);
}

void Ship::Desequip_v2(GameObject* object)
{
	if (object->m_equipment_loot != NULL)
		cleanEquipment(object->m_equipment_loot->m_equipmentType, false);
	else
		cleanWeapon(false);
}


void Ship::Recalling()
{
	if (!m_disableSlowmotion)
	{
		(*CurrentGame).m_hyperspeedMultiplier = 1.0f / m_hyperspeed;
	}

	ostringstream ss;
	ss << "Recalling...(" << (int)(m_recall_clock.getElapsedTime().asSeconds() * 100 / TIME_FOR_RECALL_TO_HUB) << "%)";
	m_recall_text->setString(ss.str());

	if (m_recall_clock.getElapsedTime().asSeconds() > MIN_TIME_FOR_RECALL_TO_HUB && !m_recall_text->m_visible)
	{
		m_recall_text->m_visible = true;
		m_recall_clock.restart();
	}

	if (m_recall_clock.getElapsedTime().asSeconds() > TIME_FOR_RECALL_TO_HUB)
	{
		Teleport(m_respawnSceneName);
		m_recall_clock.restart();
		m_recall_text->m_visible = false;
	}
}

void Ship::Bomb()
{
	int i = GetBombsDamage();
	(*CurrentGame).damageGameObjectType(EnemyObject, GetBombsDamage());
	(*CurrentGame).killGameObjectType(EnemyFire);

	//ghost
	if (m_ghost_timer < sf::seconds(SHIP_BOMBING_IMMUNITY_DURATION))
	{
		m_ghost_timer = sf::seconds(SHIP_BOMBING_IMMUNITY_DURATION);
	}
}

void Ship::Jump()
{
	ScaleVector(&m_speed, SHIP_JUMPING_SPEED);
	m_is_jumping = true;

	//ghost
	if (m_ghost_timer < sf::seconds(SHIP_JUMPING_IMMUNITY_DURATION))
	{
		m_ghost_timer = sf::seconds(SHIP_JUMPING_IMMUNITY_DURATION);
	}

	PlayStroboscopicEffect(sf::seconds(0.1), sf::seconds(0.01));

	m_jump_clock.restart();
}

void Ship::SettingTurnAnimations()
{
	//setting animation
	const float x = m_speed.x;

	if (!m_ship_model->m_fake_textureName.empty())
	{
		if (x > 0 && m_currentAnimationIndex != ShipTurningRight && !m_disable_inputs)
			m_currentAnimationIndex = ShipTurningRight;

		else if (x < 0 && m_currentAnimationIndex != ShipTurningLeft && !m_disable_inputs)
			m_currentAnimationIndex = ShipTurningLeft;

		else if ((x == 0 && m_currentAnimationIndex != ShipIdle) || m_disable_inputs)
			m_currentAnimationIndex = ShipIdle;
	}
}

void Ship::RotateShip(float angle)
{
	setRotation(angle);
	if (m_fake_ship)
	{
		m_fake_ship->setRotation(angle);
	}
	if (m_combo_aura)
	{
		m_combo_aura->setRotation(angle);
	}
	if (m_trail)
	{
		m_trail->setRotation(angle);
}	
	for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
	{
		(*it)->setRotation(angle);
	}

	if (m_recall_text)
	{
		m_recall_text->m_offset.y = -m_size.y * 0.5;
		//m_recall_text->setPosition(sf::Vector2f(m_recall_text->getPosition().x - m_recall_text->getGlobalBounds().width / 2, m_recall_text->getPosition().y));
	}
}

void Ship::ScreenBorderConstraints()
{
	sf::Vector2i offset = sf::Vector2i(0, 0);
	if (m_is_asking_scene_transition == false)
		offset = sf::Vector2i(SCREEN_BORDER_OFFSET_CONSTRAINT_X, SCREEN_BORDER_OFFSET_CONSTRAINT_Y);

	//screen borders contraints	correction
	if (getPosition().x < m_size.x / 2 + offset.x)
	{
		setPosition(m_size.x / 2 + offset.x, getPosition().y);
		m_speed.x = 0;
	}

	if (getPosition().x > SCENE_SIZE_X - (m_size.x / 2) - offset.x)
	{
		setPosition(SCENE_SIZE_X - (m_size.x / 2) - offset.x, getPosition().y);
		m_speed.x = 0;
	}

	if (getPosition().y < m_size.y / 2 + offset.y)
	{
		setPosition(getPosition().x, m_size.y / 2 + offset.y);
		m_speed.y = 0;
	}

	if (getPosition().y > SCENE_SIZE_Y - (m_size.y / 2) - offset.y)
	{
		setPosition(getPosition().x, SCENE_SIZE_Y - (m_size.y / 2) - offset.y);
		m_speed.y = 0;
	}
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	if (m_is_jumping)
	{
		return;
	}

	//idle deceleration
	if (!m_movingX || m_HUD_state == HUD_OpeningEquipment || m_recall_text->m_visible == true)
	{
		m_speed.x -= (m_speed.x) * deltaTime.asSeconds()*(getFighterFloatStatValue(Fighter_Deceleration) / 100);

		if (abs(m_speed.x) < SHIP_MIN_SPEED_X)
			m_speed.x = 0;
	}

	if (!m_movingY || m_HUD_state == HUD_OpeningEquipment || m_recall_text->m_visible == true)
	{
		m_speed.y -= (m_speed.y)*deltaTime.asSeconds()*(getFighterFloatStatValue(Fighter_Deceleration) / 100);

		if (abs(m_speed.y) < SHIP_MIN_SPEED_Y)
			m_speed.y = 0;
	}
}

/*
void Ship::FillShopWithRandomObjets(size_t num_spawned_objects, Shop* shop, EnemyClass loot_class, int equipment_type)
{
	//Specific equipment type requested?
	int equipment_type_roll = equipment_type;
	if (equipment_type_roll < 0 || equipment_type_roll > NBVAL_Equipment)
	{
		equipment_type_roll = rand() % ((int)NBVAL_Equipment + 1);//+1 is for the weapon type
	}

	//Generate random loots in shop
	for (size_t i = 0; i < num_spawned_objects; i++)
	{
		//double random_number = (double)rand() / (RAND_MAX);
		float random_beast_scale = RandomizeFloatBetweenValues(LootTable_BeastScale_Base[loot_class].x, LootTable_BeastScale_Base[loot_class].y);

		if (Enemy::AssignRandomEquipment((EquipmentType)equipment_type_roll, shop->m_level, shop, random_beast_scale))
		{
			GameObject* capsule = NULL;
			if (equipment_type_roll < NBVAL_Equipment)
			{
				capsule = Enemy::CloneEquipmentIntoGameObject(shop->m_equipment_loot);
				shop->m_equipment_loot = NULL;
			}
			else
			{
				capsule = Enemy::CloneWeaponIntoGameObject(shop->m_weapon_loot);
				shop->m_weapon_loot = NULL;
			}

			shop->m_grid_v2->InsertObject(capsule, -1, false);
		}
		else
		{
			printf("<!> Error: could not generate equipment in Shop, with Enemy::AssignRandomEquipment(), on i=%zu\n", i);
		}
	}
}
*/

bool Ship::ResplenishHealth()
{
	bool hasHealthToResplenish = false;
	if (m_armor < m_armor_max)
	{
		m_armor = m_armor_max;
		hasHealthToResplenish = true;
	}
	if (m_shield < m_shield_max)
	{
		m_shield = m_shield_max;
		hasHealthToResplenish = true;
	}
	if (m_hyperspeed_fuel < m_hyperspeed_fuel_max)
	{
		m_hyperspeed_fuel = m_hyperspeed_fuel_max;
		hasHealthToResplenish = true;
	}
	return hasHealthToResplenish;
}

void Ship::RegenHealthFast(sf::Time deltaTime, bool armor, bool shield, bool hyperspeed_fuel)
{
	if (HUB_FAST_REGEN_DURATION == 0)
	{
		ResplenishHealth();
		return;
	}

	float armor_regen_f = 1.0f * m_armor_max / HUB_FAST_REGEN_DURATION * deltaTime.asSeconds();
	int armor_regen = armor_regen_f + 1;
	if (m_armor < m_armor_max && armor)
	{
		m_armor = m_armor + armor_regen <= m_armor_max ? m_armor + armor_regen : m_armor_max;
	}

	float shield_regen_f = 1.0f * m_shield_max / HUB_FAST_REGEN_DURATION * deltaTime.asSeconds();
	int shield_regen = shield_regen_f + 1;
	if (m_shield < m_shield_max && shield)
	{
		m_shield = m_shield + armor_regen <= m_shield_max ? m_shield + shield_regen : m_shield_max;
	}

	float hyperspeed_fuel_regen_f = 1.0f * m_hyperspeed_fuel_max / HUB_FAST_REGEN_DURATION * deltaTime.asSeconds();
	int hyperspeed_fuel_regen = hyperspeed_fuel_regen_f + 1;
	if (m_hyperspeed_fuel < m_hyperspeed_fuel_max && hyperspeed_fuel)
	{
		m_hyperspeed_fuel = m_hyperspeed_fuel + hyperspeed_fuel_regen <= m_hyperspeed_fuel_max ? m_hyperspeed_fuel + hyperspeed_fuel_regen : m_hyperspeed_fuel_max;
	}
}

void Ship::setGhost(bool ghost)
{
	GameObject::setGhost(ghost);
	if (m_fake_ship)
	{
		m_fake_ship->GameObject::setGhost(ghost);
	}
	for (std::vector<Bot*>::iterator it = m_bot_list.begin(); it != m_bot_list.end(); it++)
	{
		(*it)->Bot::setGhost(ghost);
	}
}

void Ship::Respawn(bool no_save)
{
	//Init();
	////GenerateBots(this);
	SetVisibility(true);
	
	m_collision_timer = 0;
	m_graze_count = 0;
	m_disable_inputs = false;

	m_upgrades.clear();
	m_upgrades_short.clear();
	m_armor_max = 3;
	m_armor = m_armor_max;
	m_shield_max = 0;
	m_shield = 0;
	delete m_weapon;
	m_weapon = Enemy::LoadWeapon("laser", -1);
	for (Bot* bot : m_bot_list)
		delete bot;
	m_bot_list.clear();

	m_level = 1;
	m_money = 100;
	m_knownScenes.clear();

	//saving
	if (no_save == false)
	{
		Ship::SavePlayerMoneyAndHealth(this);
		Ship::SavePlayerUpgrades(this);
		Ship::SavePlayerScenes(this);
		Shop::SaveShopUpgrades(NULL);
	}
}

void Ship::SetVisibility(bool visible)
{
	m_visible = visible;
	if (m_fake_ship != NULL)
		m_fake_ship->m_visible = visible;

	SetBotsVisibility(visible);
}

void Ship::Death()
{
	FX* myFX = m_FX_death->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	(*CurrentGame).addToScene(myFX, true);

	SetVisibility(false);
	m_graze_count = 0;
	m_graze_level = 0;
	m_HUD_state = HUD_Idle;
	m_SFHudPanel->SetCursorVisible_v2(false);

	m_targetDialogs.clear();
	m_input_blocker = NULL;
	m_speed = sf::Vector2f(0, 0);
	m_disable_inputs = true;

	(*CurrentGame).PlaySFX(SFX_BigKill);

	//losing xp
	//int death_xp_penalty_ = floor(this->xp_max * XP_DEATH_MALUS_PERCENTAGE);
	//if (this->xp < death_xp_penalty_)
	//{
	//	this->xp = 0;
	//}
	//else
	//{
	//	this->xp -= death_xp_penalty_;
	//}
}

bool Ship::GetLoot(GameObject& object)
{
	//EQUIPMENT
	if (object.m_equipment_loot)
	{
		bool success = false;
		GameObject* capsule = Enemy::CloneEquipmentIntoGameObject(object.m_equipment_loot);
		//stash it
		if (m_equipment[object.m_equipment_loot->m_equipmentType] != NULL)
		{
			success = m_SFHudPanel->GetGrid_v2(Trade_StashGrid)->InsertObject(capsule, -1, false) >= 0;
		}
		//equip it
		else
		{
			setShipEquipment(object.m_equipment_loot->Clone(), true);
			//and update HUD
			success = m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)->InsertObject(capsule, object.m_equipment_loot->m_equipmentType, false);
		}
		
		if (success)
		{
			delete object.m_equipment_loot;
			object.m_equipment_loot = NULL;
			Ship::SaveItems(this);
			(*CurrentGame).PlaySFX(SFX_Equip);
		}

		return success;
	}
	//WEAPON
	else if (object.m_weapon_loot)
	{
		bool success = false;
		GameObject* capsule = Enemy::CloneWeaponIntoGameObject(object.m_weapon_loot);
		//stash it
		if (m_weapon != NULL)
		{
			success = m_SFHudPanel->GetGrid_v2(Trade_StashGrid)->InsertObject(capsule, -1, false) >= 0;
		}
		//equip it
		else
		{
			setShipWeapon(object.m_weapon_loot->Clone(), true);
			//and update HUD
			success = m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)->InsertObject(capsule, NBVAL_Equipment, false);
		}

		if (success)
		{
			delete object.m_weapon_loot;
			object.m_weapon_loot = NULL;
			Ship::SaveItems(this);
			(*CurrentGame).PlaySFX(SFX_Equip);
		}

		return success;
	}
	//MONEY
	else if (object.m_money > 0)
	{
		//feedback
		sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow
		SFText* text_feedback = new SFText((*CurrentGame).m_font[Font_Terminator], 18, _yellow, getPosition());
		ostringstream ss;
		ss << "$ " << object.m_money;
		text_feedback->setString(ss.str());
		sf::Vector2f size = m_fake_ship ? m_fake_ship->m_size : m_size;
		text_feedback->setPosition(getPosition());
		SFTextPop* pop_feedback = new SFTextPop(text_feedback, 0, MONEY_LOOT_DISPLAY_NOT_FADED_TIME, MONEY_LOOT_DISPLAY_FADE_OUT_TIME, NULL, MONEY_LOOT_DISPLAY_SPEED_Y, sf::Vector2f(0, (- size.y / 2 - TEXT_POP_OFFSET_Y)));
		pop_feedback->setPosition(sf::Vector2f(pop_feedback->getPosition().x - pop_feedback->getGlobalBounds().width / 2, pop_feedback->getPosition().y));
		delete text_feedback;
		(*CurrentGame).addToTextPops(pop_feedback);

		//transfer money
		get_money_from(object);

		(*CurrentGame).PlaySFX(SFX_MoneyLoot);

		Ship::SavePlayerMoneyAndHealth(this);
		return true;
	}

	return false;
}

void Ship::GetPortal(GameObject* object)
{
	if (m_HUD_state == HUD_OpeningEquipment)
		return;

	m_targetPortal = (Portal*)(object);
	m_isCollidingWithInteractiveObject = PortalInteraction;

	//if ((*CurrentGame).m_is_in_hub == true && (*CurrentGame).m_waiting_for_dialog_validation == false && m_is_asking_SFPanel != SFPanel_Dialog && m_targetPortal->m_currentAnimationIndex == PortalOpenIdle)
	//	m_is_asking_SFPanel = SFPanel_Portal;
	//
	//else if ((*CurrentGame).m_is_in_hub == false && (*CurrentGame).m_waiting_for_dialog_validation == false && m_is_asking_SFPanel != SFPanel_Dialog && m_targetPortal->m_currentAnimationIndex == PortalOpenIdle)
	//	m_is_asking_SFPanel = SFPanel_Action;

	if ( (*CurrentGame).m_waiting_for_dialog_validation == false && m_is_asking_SFPanel != SFPanel_Dialog && m_targetPortal->m_currentAnimationIndex == PortalOpenIdle)
		m_is_asking_SFPanel = SFPanel_Action;
}

void Ship::GetShop(GameObject* object)
{
	if (m_HUD_state == HUD_OpeningEquipment)
		return;

	m_targetShop = (Shop*)(object);
	m_isCollidingWithInteractiveObject = ShopInteraction;

	if (!(*CurrentGame).m_waiting_for_dialog_validation)
	{
		if (m_HUD_state == HUD_Trade)
			m_is_asking_SFPanel = SFPanel_Trade;
		else if (m_HUD_state == HUD_ShopStellarMap)
			m_is_asking_SFPanel = SFPanel_Map;
		else if (m_HUD_state == HUD_Upgrades)
			m_is_asking_SFPanel = SFPanel_Upgrades;
		else
			m_is_asking_SFPanel = SFPanel_Shop;
	}
}

GameObject* Ship::GetFakeShip()
{
	if (m_fake_ship != NULL)
		return m_fake_ship;
	else
		return this;
}

static int GrazeLevelsThresholds[NB_GRAZE_LEVELS] = { 0, 500, 1500, 3000 };
static float GrazeLevelsBeastBonus[NB_GRAZE_LEVELS] = { 0.0f, 0.2f, 0.4f, 0.6f };

void Ship::GetGrazing(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if ((*CurrentGame).m_waiting_for_dialog_validation || m_collision_timer > 0)
		return;

	//shield regen with grazing
	if (m_shield_max > 0 && m_shield < m_shield_max)
	{
		static double graze_count_buffer = 0;
		graze_count_buffer += GRAZE_PER_SECOND_AND_PER_BULLET * deltaTime.asSeconds() * hyperspeedMultiplier;

		if (graze_count_buffer > 1)
		{
			double intpart;
			graze_count_buffer = modf(graze_count_buffer, &intpart);
			m_graze_count += intpart;

			//regen shield
			if (m_graze_count >= GRAZING_COUNT_TO_REGEN_SHIELD)
			{
				m_shield++;
				m_graze_count = m_shield == m_shield_max ? GRAZING_COUNT_TO_REGEN_SHIELD * (m_shield + 1) : 0;
			}

			//Combo
			//AddComboCount(intpart);
		}
	}
	else
		m_graze_count = 0;

	/*
	if (m_graze_level < NB_GRAZE_LEVELS - 1)
	{
		if (m_graze_count >= GrazeLevelsThresholds[m_graze_level + 1])
		{
			//Graze level up
			m_graze_level++;
			sf::Color color = sf::Color::White;
			switch (m_graze_level)
			{
				case GRAZE_LEVEL_BLUE:
				{
					color = sf::Color(0, 155, 255, 20);
					m_graze_radius_feedback.setOutlineColor(color);
					break;
				}
				case GRAZE_LEVEL_RED:
				{
					color = sf::Color(255, 0, 0, 20);
					m_graze_radius_feedback.setOutlineColor(color);
					break;
				}
				case GRAZE_LEVEL_YELLOW:
				{
					color = sf::Color(255, 255, 255, 20);
					m_graze_radius_feedback.setOutlineColor(color);
					break;
				}
			}

			//Text feedback
			//SFText* text_feedback = new SFText((*CurrentGame).m_font[Font_Terminator], 14, color, getPosition());
			//ostringstream ss;
			//if (m_graze_level < NB_GRAZE_LEVELS - 1)
			//{
			//	ss << "Graze level+";
			//}
			//else
			//{
			//	ss << "Graze level MAX";
			//}
			//text_feedback->setString(ss.str());
			//sf::Vector2f size = m_fake_ship ? m_fake_ship->m_size : m_size;
			//text_feedback->setPosition(getPosition());
			//int s = (*CurrentGame).m_direction == DIRECTION_DOWN ? -1 : 1;
			//SFTextPop* pop_feedback = new SFTextPop(text_feedback, 0, GRAZE_UP_DISPLAY_NOT_FADED_TIME, GRAZE_UP_DISPLAY_NOT_FADED_TIME, NULL, s*MONEY_LOOT_DISPLAY_SPEED_Y, sf::Vector2f(0, s*(-size.y / 2 - TEXT_POP_OFFSET_Y)));
			//pop_feedback->setPosition(sf::Vector2f(pop_feedback->getPosition().x - pop_feedback->getGlobalBounds().width / 2, pop_feedback->getPosition().y));
			//delete text_feedback;
			//(*CurrentGame).addToFeedbacks(pop_feedback);

			//Aura
			if (m_combo_aura && m_graze_level > 0 && m_graze_level < NB_GRAZE_LEVELS)
			{
				//update aura
				if (m_graze_level > GRAZE_LEVEL_NONE + 1)
				{
					m_combo_aura->setAnimationLine(m_graze_level, true);
				}
				//"new" aura
				else
				{
					m_combo_aura->setAnimationLine(m_graze_level, false);
				}
				
			}
		}
	}
	*/
}

int Ship::getGrazeCount()
{
	return m_graze_count;
}

float Ship::getShipBeastScore()
{
	float bonus = 0;
	if (m_graze_level < NB_GRAZE_LEVELS && m_graze_level >= 0)
	{
		bonus = GrazeLevelsBeastBonus[m_graze_level];
	}
	else
	{
		LOGGER_WRITE(Logger::DEBUG, TextUtils::format("<!> Error, The ship has a 'graze_level' (%d) beyond existing values\n", m_graze_level));
		m_graze_count = 0;
		m_graze_level = 0;
	}

	return bonus;
}

void Ship::GetDamageFrom(GameObject& object)
{
	if (m_collision_timer > 0 || (*CurrentGame).m_waiting_for_dialog_validation || (*CurrentGame).m_waiting_for_scene_transition)
		return;

	if (object.m_damage == 0)
		return;

	if (object.m_collision_timer > 0)
		return;

	if (object.m_collider_type == EnemyFire)
	{
		//display impact FX regardless of damage
		Ammo& ammo = (Ammo&)object;
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

	if (object.m_collider_type != EnemyFire)//ship collision versus ship
	{
		//object.m_collision_timer = TIME_BETWEEN_COLLISION_DAMAGE_TICK;

		if (object.m_collider_type == EnemyObject)//FX mutal collision
		{
			string fx_name = "explosion_S";
			float duration = atof(((*CurrentGame).m_FXConfig[fx_name][FX_DURATION]).c_str());
			FX* fx = new FX(sf::Vector2f(0, 0), sf::Vector2f(0, 0), (*CurrentGame).m_FXConfig[fx_name][FX_FILENAME], sf::Vector2f(stoi((*CurrentGame).m_FXConfig[fx_name][FX_WIDTH]), stoi((*CurrentGame).m_FXConfig[fx_name][FX_HEIGHT])), 2, sf::seconds(duration));

			float angle = GameObject::GetAngleRadBetweenObjects(this, &object);
			fx->setPosition(getPosition().x - sin(angle)*GetShipSize().x / 2, getPosition().y + cos(angle)*GetShipSize().y / 2);
			(*CurrentGame).addToScene(fx, true);
		}
	}

	if (m_immune == true)
		return;

	//Getting hit
	//setColor(Color(255, 0, 0, 255), true);
	//if (m_ship_model->m_fake_textureName.empty() == false)
	//	m_fake_ship->setColor(Color(255, 0, 0, 255), true);
	//m_damage_feedbackTimer = DAMAGE_FEEDBACK_TIME;

	m_collision_timer = IMMUNE_DELAY_AFTER_HIT_TAKEN;
	(*CurrentGame).killGameObjectType(EnemyFire);

	int damage = 1;//object.m_damage
	if (damage > m_shield)
	{
		m_armor -= (damage - m_shield);
		m_shield = 0;
	}
	else
		m_shield -= damage;

	m_shield_recovery_clock.restart();
	
	//Combo
	//(*CurrentGame).m_playership->AddComboCount(-(*CurrentGame).m_playership->m_combo_count_max / 2);

	m_graze_count = 0;
	//m_graze_level = GRAZE_LEVEL_NONE;
	m_hits_taken++;
	//if (m_combo_aura != NULL)
	//	m_combo_aura->setAnimationLine(GRAZE_LEVEL_NONE);

	//if (m_armor <= 0)
	//	Death();
}


int Ship::GetFocusedPortalMaxUnlockedHazardLevel()
{
	if (m_targetPortal == NULL)
		return -1;
	
	return m_targetPortal->m_max_unlocked_hazard_level;
}

sf::Vector2f Ship::GetShipSize()
{
	return m_fake_ship != NULL ? m_fake_ship->m_size : m_size;
}

int Ship::UpdateShipLevel()
{
	int level_ = 1;
	int credits_ = 0;

	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i])
		{
			credits_ += m_equipment[i]->m_credits;
		}
	}
	if (m_weapon)
	{
		credits_ += m_weapon->m_credits;
	}

	while (credits_ >= ((*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_ + 1) - (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_)) * (NBVAL_Equipment + 1))
	{
		credits_ -= ((*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_ + 1) - (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_)) * (NBVAL_Equipment + 1);
		level_++;
	}

	m_level = level_;
	m_xp = credits_;
	m_xp_max = ((*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_ + 1) - (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_)) * (NBVAL_Equipment + 1);

	return level_;
}

//SAVING AND LOADING 
int Ship::SavePlayerScenes(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Saving scenes in profile.\n");
	assert(ship != NULL);

	ofstream data(string(getSavesPath()) + SCENES_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		// instructions
		for (map<string, int>::iterator it = ship->m_knownScenes.begin(); it != ship->m_knownScenes.end(); it++)
		{
			data << it->first.c_str();// << " " << it->second;
			if (it->first.c_str() == ship->m_respawnSceneName)
				data << " " << "!";
			
			data << endl;
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "Failed to open SCENES SAVE FILE !" << endl;
	}

	return 0;
}

string Ship::LoadPlayerScenes(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading scenes from profile.\n");
	assert(ship != NULL);

	string return_current_scene;

	std::ifstream  data(string(getSavesPath()) + SCENES_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		while (std::getline(data, line))
		{
			string scene;
			string current_scene;

			std::istringstream(line) >> scene >> current_scene;

			ship->m_knownScenes.insert(std::pair<string, int>(scene, 0));
			if (current_scene.compare("!") == 0)
				return_current_scene = scene;
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "Failed to open PLAYER SAVE FILE !" << endl;
	}

	return return_current_scene;
}

int Ship::SavePlayerMoneyAndHealth(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Saving money in profile.\n");
	assert(ship != NULL);

	ofstream data(string(getSavesPath()) + MONEY_AND_HEALTH_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		data << "Money " << ship->m_money << endl;
		data << "Health " << ship->m_armor << endl;
		data << "Shield " << ship->m_shield << endl;
		data << "Graze " << ship->m_graze_count << endl;
		data << "Level " << ship->m_level << endl;

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found for known scenes. A new file is going to be created.\n" << endl;
	}

	return 0;
}

bool Ship::LoadPlayerMoneyAndHealth(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading money and health from profile.\n");
	assert(ship != NULL);

	std::ifstream  data(string(getSavesPath()) + MONEY_AND_HEALTH_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		int i = 0;
		while (std::getline(data, line))
		{
			string ss;
			string value;
			std::istringstream(line) >> ss >> value;

			if (i == 0)
				ship->m_money = stoi(value);
			else if (i == 1)
				ship->m_armor = stoi(value);
			else if (i == 2)
				ship->m_shield = stoi(value);
			else if (i == 3)
				ship->m_graze_count = stoi(value);
			else if (i == 4)
				ship->m_level = stoi(value);
		
			i++;
		}
		
		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No MONEY SAVE FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

int Ship::SaveItems(Ship* ship)
{
	return 0;
	LOGGER_WRITE(Logger::DEBUG, "Saving items in profile.\n");
	assert(ship != NULL);

	ofstream data(string(getSavesPath()) + ITEMS_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		// instructions
		for (int i = 0; i < NBVAL_Equipment; i++)
		{
			switch (i)
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

			Enemy::SaveEquipmentData(data, ship->m_equipment[i], true);
		}

		data << "Weapon ";
		Enemy::SaveWeaponData(data, ship->m_weapon, true);

		//stash
		for (GridElement* element : ship->m_SFHudPanel->GetGrid_v2(Trade_StashGrid)->m_elements)
		{
			if (element->m_object == NULL)
			{
				data << "0";
				data << endl;
			}
			else if (element->m_object->m_equipment_loot != NULL)
				Enemy::SaveEquipmentData(data, element->m_object->m_equipment_loot, false);
			else
				Enemy::SaveWeaponData(data, element->m_object->m_weapon_loot, false);
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found for known scenes. A new file is going to be created.\n" << endl;
	}

	return 0;
}

bool Ship::LoadPlayerItems(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading items from profile.\n");
	assert(ship != NULL);

	std::ifstream  data(string(getSavesPath()) + ITEMS_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		//clear current equipment
		ship->cleanWeapon(true);
		for (int i = 0; i < NBVAL_Equipment; i++)
			ship->cleanEquipment(i, true);
		ship->m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)->ClearGrid();
		ship->m_SFHudPanel->GetGrid_v2(Trade_StashGrid)->ClearGrid();

		//load save
		std::string line;
		int i = 0;
		while (std::getline(data, line))
		{
			string equipment_type;
			string display_name;

			//Loading equipment
			if (i < NBVAL_Equipment)
			{
				std::istringstream(line) >> equipment_type >> display_name;
				if (display_name.compare("0") != 0)
				{
					Equipment* equipment = Enemy::LoadSavedEquipmentFromLine(line);
					ship->setShipEquipment(equipment, true, true);//Equipped items will be loaded later in HUD

					GameObject* capsule = Enemy::CloneEquipmentIntoGameObject(equipment);
					ship->m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)->InsertObject(capsule, i, false);
				}
			}
			//Loading weapon
			else if (i == NBVAL_Equipment)
			{
				std::istringstream(line) >> equipment_type >> display_name;
				if (display_name.compare("0") != 0)
				{
					Weapon* weapon = Enemy::LoadSavedWeaponFromLine(line);
					ship->setShipWeapon(weapon, true, true);//Equipped items will be loaded later in HUD

					GameObject* capsule = Enemy::CloneWeaponIntoGameObject(weapon);
					ship->m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)->InsertObject(capsule, NBVAL_Equipment, false);
				}
			}
			//Loading stash
			else
			{
				std::istringstream(line) >> equipment_type;

				int index = i - NBVAL_Equipment - 1;
				if (!ship->m_SFHudPanel)
				{
					printf("Error: <!> in Ship::LoadPlayerItems() <!> this ship doesn't have a HUD panel.\n");
				}
				else
				{
					if (equipment_type.compare("0") != 0)//Load stash in HUD
					{
						if (equipment_type.compare("Weapon") == 0)
						{
							Weapon* weapon = Enemy::LoadSavedWeaponFromLine(line);
							GameObject* capsule = Enemy::CloneWeaponIntoGameObject(weapon);
							ship->m_SFHudPanel->GetGrid_v2(Trade_StashGrid)->InsertObject(capsule, index, false);
							delete weapon;
						}
						else
						{
							Equipment* equipment = Enemy::LoadSavedEquipmentFromLine(line);
							GameObject* capsule = Enemy::CloneEquipmentIntoGameObject(equipment);
							ship->m_SFHudPanel->GetGrid_v2(Trade_StashGrid)->InsertObject(capsule, index, false);
							delete equipment;
						}
					}
				}
			}

			i++;
		}

		data.close();  // on ferme le fichier

		return i == NBVAL_Equipment + 1 + STASH_GRID_NB_LINES*STASH_GRID_NB_ROWS;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No ITEMS SAVE FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

int Ship::SavePlayerUpgrades(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Saving upgrades in profile.\n");
	assert(ship != NULL);

	ofstream data(string(getSavesPath()) + PLAYER_UPGRADES_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		for (vector<string>::iterator it = ship->m_upgrades.begin(); it != ship->m_upgrades.end(); it++)
		{
			data << *it;
			data << endl;
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found for player upgrades. A new file is going to be created.\n" << endl;
	}

	return 0;
}

bool Ship::LoadPlayerUpgrades(Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading upgrades from profile.\n");
	assert(ship != NULL);

	std::ifstream  data(string(getSavesPath()) + PLAYER_UPGRADES_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		ship->m_upgrades.clear();

		//load save
		std::string line;
		while (std::getline(data, line))
		{
			string upgrade_name;
			std::istringstream(line) >> upgrade_name;

			ship->SetUpgrade(upgrade_name);
		}
		
		data.close();  // on ferme le fichier

		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No ITEMS SAVE FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

void Ship::updatePostCollision()
{
	m_previouslyCollidingWithInteractiveObject = m_isCollidingWithInteractiveObject;
	m_isCollidingWithInteractiveObject = No_Interaction;
}

float Ship::getFighterFloatStatValue(FighterStats stat)
{
	float new_stat_value = 0;
	float equipment_value = 0;

	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i])
		{
			switch (stat)
			{
				case Fighter_Hyperspeed:
				{
					equipment_value += m_equipment[i]->m_hyperspeed;
					break;
				}
				case Fighter_MaxSpeed:
				{
					equipment_value += m_equipment[i]->m_max_speed;
					break;
				}
				case Fighter_Acceleration:
				{
					equipment_value += m_equipment[i]->m_acceleration;
					break;
				}
				case Fighter_Deceleration:
				{
					equipment_value += m_equipment[i]->m_deceleration;
					break;
				}
				case Fighter_ShieldRecovery:
				{
					equipment_value += m_equipment[i]->m_shield_recovery_time;
					break;
				}
				default:
				{
					equipment_value += 0;
				}
			}
		}
		else
		{
			equipment_value += 0;
		}
	}

	//adding ship model stats
	switch (stat)
	{
		case Fighter_Hyperspeed:
		{
			new_stat_value = m_ship_model->m_hyperspeed + equipment_value;
			break;
		}
		case Fighter_MaxSpeed:
		{
			new_stat_value = m_ship_model->m_max_speed + equipment_value;
			break;
		}
		case Fighter_Acceleration:
		{
			new_stat_value = m_ship_model->m_acceleration + equipment_value;
			break;
		}
		case Fighter_Deceleration:
		{
			new_stat_value = m_ship_model->m_deceleration + equipment_value;
			break;
		}
		case Fighter_ShieldRecovery:
		{
			new_stat_value = m_ship_model->m_shield_recovery_time + equipment_value;
			break;
		}
		default:
		{
			new_stat_value = equipment_value;
		}
	}

	return new_stat_value;
}

int Ship::getFighterIntStatValue(FighterStats stat)
{
	int new_stat_value = 0;
	int equipment_value = 0;

	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i])
		{
			switch (stat)
			{
				case Fighter_Armor:
				{
					return m_armor;
				}
				case Fighter_ArmorMax:
				{
					equipment_value += m_equipment[i]->m_armor;
					break;
				}
				case Fighter_Shield:
				{
					return m_shield;
				}
				case Fighter_ShieldMax:
				{
					equipment_value += m_equipment[i]->m_shield;
					break;
				}
				case Fighter_ShieldRegen:
				{
					equipment_value += m_equipment[i]->m_shield_regen;
					break;
				}
				case Fighter_HyperspeedFuel:
				{
					equipment_value += m_equipment[i]->m_hyperspeed_fuel;
					break;
				}
				case Fighter_ContactDamage:
				{
					equipment_value += m_equipment[i]->m_damage;
					break;
				}
				case Fighter_Credits:
				{
					equipment_value += m_equipment[i]->m_credits;
					break;
				}
				case Fighter_Level:
				{
					equipment_value += m_equipment[i]->m_level;
					break;
				}
				default:
				{
					equipment_value += 0;
				}
			}
		}
		else
		{
			equipment_value += 0;
		}
	}

	//adding ship model stats
	switch (stat)
	{
		case Fighter_ArmorMax:
		{
			new_stat_value = m_ship_model->m_armor + equipment_value;
			break;
		}
		case Fighter_ShieldMax:
		{
			new_stat_value = m_ship_model->m_shield + equipment_value;
			break;
		}
		case Fighter_ShieldRegen:
		{
			new_stat_value = m_ship_model->m_shield_regen + equipment_value;
			break;
		}
		case Fighter_HyperspeedFuel:
		{
			new_stat_value = m_ship_model->m_hyperspeed_fuel + equipment_value;
			break;
		}
		case Fighter_ContactDamage:
		{
			new_stat_value = m_ship_model->m_damage + equipment_value;
			break;
		}
		case Fighter_Credits:
		{
			new_stat_value = equipment_value;
			break;
		}
		case Fighter_Level:
		{
			new_stat_value = equipment_value;
			break;
		}
		default:
		{
			new_stat_value = equipment_value;
		}
	}

	return new_stat_value;
}

void Ship::GenerateBots(GameObject* target)
{
	int j = 0;
	for (Bot* bot : m_bot_list)
	{
		bot->m_automatic_fire = m_automatic_fire;
		//bot->m_spread = GameObject::getSize_for_Direction((*CurrentGame).m_direction, (*it)->m_spread);
		bot->setTarget(target);
		bot->m_visible = !m_disable_bots;

		(*CurrentGame).addToScene(bot, true);

		//bots auto spreading based on number of bots
		//int s = j % 2 == 0 ? 1 : -1;
		//int x = j / 2;
		//(*it)->m_spread.x *= s * (1 + x);
		//j++;

		//bots auto spreading based on number of bots
		//for (int i = 0; i < number_of_bots; i++)
		//{
		//	int s = i % 2 == 0 ? 1 : -1;
		//	int x = i / 2;
		//	bot->m_spread.x *= s * (1 + x);
		//}
	}
}

void Ship::SetBotsVisibility(bool visible)
{
	for (Bot* bot : m_bot_list)
	{
		bot->m_visible = visible;
		bot->setGhost(m_ghost);
	}
}

void Ship::DestroyBots()
{
	for (Bot* bot : m_bot_list)
	{
		bot->m_visible = false;
		bot->m_garbageMe = true;
	}

	m_bot_list.clear();
}

void Ship::ContinueDialog()
{
	if (m_SFTargetPanel != NULL && m_SFTargetPanel->GetDialog()->m_next_dialog_name.empty() == false && m_SFTargetPanel->GetDialog()->m_next_dialog_name.compare("0") != 0)
		m_is_asking_SFPanel = SFPanel_DialogNext;
	else if (m_targetDialogs.size() > 1)
		m_is_asking_SFPanel = SFPanel_DialogNext;
	else
	{
		if ((*CurrentGame).m_waiting_for_dialog_validation == true)
			m_release_to_fire = true;

		(*CurrentGame).m_waiting_for_dialog_validation = false;
		(*CurrentGame).m_end_dialog_clock.restart();
	}

	delete m_targetDialogs.front();
	m_targetDialogs.erase(m_targetDialogs.begin());
}

void Ship::Teleport(string destination_name)
{
	m_is_asking_teleportation = destination_name;

	(*CurrentGame).PlaySFX(SFX_Teleport);
}

void Ship::CenterMapView(sf::Vector2f offset)
{
	m_SFTargetPanel->SetMapViewOffset(offset);
}

void Ship::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (m_fake_ship != NULL)
		m_fake_ship->PlayStroboscopicEffect(effect_duration, time_between_poses);
	else
	{
		if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
		{
			Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
			(*CurrentGame).addToScene(strobo, true);

			m_stroboscopic_effect_clock.restart();
		}
	}
}

bool Ship::CanHyperspeed()
{
	if (m_ship_model && m_ship_model->m_can_hyperspeed)
	{
		return true;
	}
	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i] && m_equipment[i]->m_can_hyperspeed)
		{
			return true;
		}
	}

	return false;
}

bool Ship::CanJump()
{
	if (m_ship_model && m_ship_model->m_can_jump)
	{
		return true;
	}
	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i] && m_equipment[i]->m_can_jump)
		{
			return true;
		}
	}

	return false;
}

int Ship::GetNumberOfBombs()
{
	int bombs = 0;
	if (m_ship_model)
	{
		bombs += m_ship_model->m_bombs;
	}
	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i])
		{
			bombs += m_equipment[i]->m_bombs;
		}
	}

	return bombs;
}

int Ship::GetBombsDamage()
{
	return GetNumberOfBombs() > 0 ? BOMB_FIRST_LEVEL_DAMAGE*(*CurrentGame).GetEnemiesStatsMultiplierForLevel(m_level)*0.01 : 0;
}

void Ship::AddComboCount(int value)
{
	if (value == 0)
	{
		return;
	}
	
	int current_combo_level = m_combo_level;

	if (value > 0)
	{
		while (value > 0)
		{
			if (m_combo_count + value >= m_combo_count_max)
			{
				value -= (m_combo_count_max - m_combo_count);
				m_combo_level++;
				m_combo_count_max = COMBO_COUNT_FIRST_LEVEL * pow(1 + COMBO_LEVEL_COUNT_MULTIPLIER, m_combo_level);
				m_combo_count = 0;
			}
			else
			{
				m_combo_count += value;
				value = 0;
			}
		}
	}
	else if (value < 0)
	{
		value = -value;
		while (value > 0)
		{
			if (value >= m_combo_count + 1)
			{
				if (m_combo_level > 0)
				{
					value -= (m_combo_count + 1);
					m_combo_level--;
					m_combo_count_max = COMBO_COUNT_FIRST_LEVEL * pow(1 + COMBO_LEVEL_COUNT_MULTIPLIER, m_combo_level);
					m_combo_count = m_combo_count_max - 1;
				}
				else
				{
					m_combo_count = 0;
					value = 0;
				}
			}
			else
			{
				m_combo_count -= value;
				value = 0;
			}
		}
	}

	//Text feedback
	if (current_combo_level != m_combo_level)
	{
		sf::Color color;
		ostringstream ss;
		ss << "Combo x" << m_combo_level;
		if (current_combo_level < m_combo_level)
		{
			color = sf::Color::Green;
		}
		else
		{
			color = sf::Color::Red;
		}
		SFText* text_feedback = new SFText((*CurrentGame).m_font[Font_Terminator], 16, color, getPosition());
		text_feedback->setString(ss.str());
		sf::Vector2f size = m_fake_ship ? m_fake_ship->m_size : m_size;
		text_feedback->setPosition(getPosition());
		SFTextPop* pop_feedback = new SFTextPop(text_feedback, 0, GRAZE_UP_DISPLAY_NOT_FADED_TIME, GRAZE_UP_DISPLAY_NOT_FADED_TIME, NULL, MONEY_LOOT_DISPLAY_SPEED_Y, sf::Vector2f(0, (-size.y / 2 - TEXT_POP_OFFSET_Y)));
		pop_feedback->setPosition(sf::Vector2f(pop_feedback->getPosition().x - pop_feedback->getGlobalBounds().width / 2, pop_feedback->getPosition().y));
		delete text_feedback;
		(*CurrentGame).addToTextPops(pop_feedback);
	}
}

void Ship::AddDialog(Dialog* dialog)
{
	m_targetDialogs.push_back(dialog);
}

void Ship::SetInputBlocker(GameObject* blocker)
{
	m_input_blocker = blocker;
}

GameObject* Ship::GetInputBlocker()
{
	return m_input_blocker;
}

void Ship::SetAskingPanel(SFPanelTypes type)
{
	m_is_asking_SFPanel = type;
}

void Ship::SetUpgrade(string upgrade_name)
{
	//update "short" list of maxed upgrades
	bool permanent = (bool)(stoi((*CurrentGame).m_upgradesConfig[upgrade_name][UPGRADE_PERMANENT]));
	if (permanent == true)
	{
		m_upgrades.push_back(upgrade_name);

		string locked_by = (*CurrentGame).m_upgradesConfig[upgrade_name][UPGRADE_UNLOCKED_BY];
		if (locked_by.compare("0") != 0)
		{
			for (vector<string>::iterator it = m_upgrades_short.begin(); it != m_upgrades_short.end(); it++)
			{
				if (it->compare(locked_by) == 0)
				{
					*it = upgrade_name;
					break;
				}
			}
		}
		else
			m_upgrades_short.push_back(upgrade_name);
	}
	
	//apply effect
	if (upgrade_name.compare("Upgrade_hp_1") == 0)
	{
		m_armor_max++;
		m_armor++;
	}
	else if (upgrade_name.compare("Upgrade_hp_2") == 0)
	{
		m_armor_max++;
		m_armor++;
	}
	else if (upgrade_name.compare("Upgrade_hp_3") == 0)
	{
		m_armor_max++;
		m_armor++;
	}
	else if (upgrade_name.compare("Upgrade_hp_4") == 0)
	{
		m_armor_max++;
		m_armor++;
	}
	else if (upgrade_name.compare("Upgrade_hp_5") == 0)
	{
		m_armor_max++;
		m_armor++;
	}
	else if (upgrade_name.compare("Upgrade_shield_1") == 0)
		m_shield_max++;
	else if (upgrade_name.compare("Upgrade_shield_2") == 0)
		m_shield_max++;
	else if (upgrade_name.compare("Upgrade_shield_3") == 0)
		m_shield_max++;
	else if (upgrade_name.compare("Upgrade_shield_4") == 0)
		m_shield_max++;
	else if (upgrade_name.compare("Upgrade_shield_5") == 0)
		m_shield_max++;
	else if (upgrade_name.compare("Upgrade_laser_1") == 0)
		SetWeapon("laser1");
	else if (upgrade_name.compare("Upgrade_laser_2") == 0)
		SetWeapon("laser2");
	else if (upgrade_name.compare("Upgrade_laser_3") == 0)
		SetWeapon("laser3");
	else if (upgrade_name.compare("Upgrade_laser_4") == 0)
		SetWeapon("laser4");
	else if (upgrade_name.compare("Upgrade_laser_5") == 0)
		SetWeapon("laser5");
	else if (upgrade_name.compare("Upgrade_laserbeam_1") == 0)
		SetWeapon("laserbeam1");
	else if (upgrade_name.compare("Upgrade_laserbeam_2") == 0)
		SetWeapon("laserbeam2");
	else if (upgrade_name.compare("Upgrade_laserbeam_3") == 0)
		SetWeapon("laserbeam3");
	else if (upgrade_name.compare("Upgrade_laserbeam_4") == 0)
		SetWeapon("laserbeam4");
	else if (upgrade_name.compare("Upgrade_laserbeam_5") == 0)
		SetWeapon("laserbeam5");
	else if (upgrade_name.compare("Upgrade_minirocket_1") == 0)
		SetWeapon("minirocket1");
	else if (upgrade_name.compare("Upgrade_minirocket_2") == 0)
		SetWeapon("minirocket2");
	else if (upgrade_name.compare("Upgrade_minirocket_3") == 0)
		SetWeapon("minirocket3");
	else if (upgrade_name.compare("Upgrade_minirocket_4") == 0)
		SetWeapon("minirocket4");
	else if (upgrade_name.compare("Upgrade_minirocket_5") == 0)
		SetWeapon("minirocket5");
	else if (upgrade_name.compare("Upgrade_drone_laser_1") == 0)
		SetDrone("bot_laser", 1);
	else if (upgrade_name.compare("Upgrade_drone_laser_2") == 0)
		SetDrone("bot_laser", 2);
	else if (upgrade_name.compare("Upgrade_drone_laser_3") == 0)
		SetDrone("bot_laser", 3);
	else if (upgrade_name.compare("Upgrade_drone_laser_4") == 0)
		SetDrone("bot_laser", 4);
	else if (upgrade_name.compare("Upgrade_drone_laser_5") == 0)
		SetDroneWeapon("laser1");
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_1") == 0)
		SetDrone("bot_laserbeam", 1);
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_2") == 0)
		SetDroneWeapon("laserbeam1");
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_3") == 0)
		SetDroneWeapon("laserbeam2");
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_4") == 0)
		SetDroneWeapon("laserbeam3");
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_5") == 0)
	{
		SetDrone("bot_laserbeam", 2);
		SetDroneWeapon("laserbeam3");
	}
	else if (upgrade_name.compare("Upgrade_drone_minirocket_1") == 0)
		SetDrone("bot_minirocket", 1);
	else if (upgrade_name.compare("Upgrade_drone_minirocket_2") == 0)
		SetDroneWeapon("minirocket1");
	else if (upgrade_name.compare("Upgrade_drone_minirocket_3") == 0)
	{
		SetDrone("bot_minirocket", 2);
		SetDroneWeapon("minirocket1");
	}
	else if (upgrade_name.compare("Upgrade_drone_minirocket_4") == 0)
		SetDroneWeapon("minirocket2");
	else if (upgrade_name.compare("Upgrade_drone_minirocket_5") == 0)
		SetDroneWeapon("minirocket3");
	else if (upgrade_name.compare("Restores_hp_1") == 0)
		m_armor++;


	else if (upgrade_name.compare("Restores_hp_1") == 0)
		m_armor++;
}

void Ship::SetWeapon(string weapon_name)
{
	delete m_weapon;
	m_weapon = Enemy::LoadWeapon(weapon_name, -1);
}

void Ship::SetDrone(string drone_name, int number)
{
	for (Bot* bot : m_bot_list)
		delete bot;

	m_bot_list.clear();

	Bot* bot = Enemy::LoadBot(drone_name);
	m_bot_list.push_back(bot);

	for (int i = 1; i < number; i++)
	{
		Bot* new_bot = bot->Clone();

		//bots auto spreading based on number of bots
		int s = i % 2 == 0 ? 1 : -1;
		int x = i / 2;
		new_bot->m_spread.x *= s * (1 + x);
	}

	GenerateBots(this);
}

void Ship::SetDroneWeapon(string weapon_name)
{
	if (m_bot_list.empty() == false)
	{
		Weapon* weapon = Enemy::LoadWeapon(weapon_name, -1);
		
		for (Bot* bot : m_bot_list)
		{
			delete bot->m_weapon;
			bot->m_weapon = bot == m_bot_list.front() ? weapon : weapon->Clone();
		}
	}
}

void Ship::RandomizeUpgrades(Shop* target_shop)
{
	//short list
	//select eligible upgrades:
	//- not already possessed
	//- unlocked
	//- condition checked
	vector<string> random_upgrades;
	for (map<string, vector<string> >::iterator it = (*CurrentGame).m_upgradesConfig.begin(); it != (*CurrentGame).m_upgradesConfig.end(); it++)
	{
		if (stoi(it->second[UPGRADE_PERMANENT]) == 1)
		{
			//already possessed?
			bool found = false;
			for (vector<string>::iterator it2 = m_upgrades.begin(); it2 != m_upgrades.end(); it2++)
			{
				if (it2->compare(it->second[UPGRADE_NAME]) == 0)
				{
					found = true;
					break;
				}
			}

			if (found == true)
				continue;
		}

		//locked by
		bool unlocked = false;
		if (it->second[UPGRADE_UNLOCKED_BY].compare("0") != 0)
		{
			for (vector<string>::iterator it2 = m_upgrades.begin(); it2 != m_upgrades.end(); it2++)
			{
				if (it2->compare(it->second[UPGRADE_UNLOCKED_BY]) == 0)
				{
					unlocked = true;
					break;
				}
			}

			if (unlocked == false)
				continue;
		}

		//condition
		if (it->second[UPGRADE_CONDITION].compare("0") != 0)
		{
			FloatCompare result = ERROR_COMPARE;

			//compute results
			if (it->second[UPGRADE_CONDITION].compare("life") == 0)
			{
				if (stoi(it->second[UPGRADE_CONDITION_VALUE]) == m_armor)//case of "death" condition handled in method Death(), when the enemy dies precisely
					result = EQUAL_TO;
				else
					result = 100.0f * m_armor / m_armor_max >= stoi(it->second[UPGRADE_CONDITION_VALUE]) ? GREATER_THAN : LESSER_THAN;
			}
			else if (it->second[UPGRADE_CONDITION].compare("shield") == 0)
			{
				if (m_shield_max == 0)//careful of the case where shield_max == 0, we don't want to be dividing by 0
					result = stoi(it->second[UPGRADE_CONDITION_VALUE]) == 0 ? EQUAL_TO : LESSER_THAN;
				else
					result = 100.0f * m_shield / m_shield_max >= stoi(it->second[UPGRADE_CONDITION_VALUE]) ? GREATER_THAN : LESSER_THAN;
			}

			//check results
			if (result == EQUAL_TO || (it->second[UPGRADE_CONDITION_OP].compare("greater") == 0 && result == GREATER_THAN) || (it->second[UPGRADE_CONDITION_OP].compare("lesser") == 0 && result == LESSER_THAN))
			{
				//condition matched
			}
			else
				continue;
		}

		//add to the short list
		random_upgrades.push_back(it->second[UPGRADE_NAME]);
	}

	//randomize among the short list
	random_shuffle(random_upgrades.begin(), random_upgrades.end());

	for (int i = 0; i < NB_UPGRADE_CHOICES; i++)
	{
		target_shop->m_upgrades[i] = random_upgrades[i];
		target_shop->m_sold_out[i] = false;
	}
}