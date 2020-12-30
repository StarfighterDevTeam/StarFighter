#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

Ship::Ship(string textureName, sf::Vector2f size, string fake_textureName, sf::Vector2f fake_size, int fake_frameNumber, int fake_animationNumber) : GameObject(Vector2f(0, 0), Vector2f(0, 0), textureName, size, Vector2f(size.x * 0.5, size.y * 0.5), 1, 1)
{
	m_collider_type = PlayerShip;
	m_layer = PlayerShipLayer;

	m_transparent = false;
	m_automatic_fire = false;
	m_display_name = "Wisteria";
	m_weapon = NULL;
	m_FX_death = NULL;
	m_moving = false;
	m_movingX = m_movingY = false;
	m_visible = true;
	m_DontGarbageMe = true;
	m_damage = 0;
	m_armor = 1;
	m_shield = 0;
	m_shield_regen = 0;
	m_disable_inputs = false;
	m_disable_fire = false;
	m_disableSlowmotion = false;
	m_disableRecall = false;
	m_disableSpecial = false;
	m_graze_count = 0;
	m_graze_level = 0;
	m_disable_bots = true;
	m_is_asking_scene_transition = false;
	m_input_blocker = NULL;
	m_hits_taken = 0;
	m_release_to_fire = false;
	m_release_to_throw = false;
	m_ghost_timer = 0;
	m_jump_ghost_timer = 0;
	m_jump_timer = 0;
	m_jump_cooldown = 0;
	m_cloak_ghost_timer = 0;
	m_cloak_cooldown = 0;

	m_level = 1;
	m_xp = 0;
	m_crystals = 0;
	
	m_combo_count = 0;
	m_combo_count_max = COMBO_COUNT_FIRST_LEVEL;
	m_combo_level = 0;

	m_fake_ship = new FakeShip(this, fake_textureName, fake_size, fake_frameNumber, fake_animationNumber);
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
		m_graze_percent_points[i * 2].color = sf::Color(0, 0, 0, 0);
		m_graze_percent_points[i * 2 + 1].color = m_graze_percent_points[i * 2].color;
	}

	for (int i = 0; i < GRAZING_FEEDBACK_CIRCLE_POINTS; i++)
	{
		m_special_percent_points[i * 2].color = sf::Color(0, 0, 0, 0);
		m_special_percent_points[i * 2 + 1].color = m_special_percent_points[i * 2].color;
	}

	m_targetPortal = NULL;
	m_targetShop = NULL;

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
}

Ship::~Ship()
{
	delete m_weapon;
	delete m_FX_death;

	for (Bot* bot : m_bot_list)
		bot->Death();

	//game objects
	delete m_combo_aura;
	delete m_fake_ship;;
	delete m_trail;

	if (m_recall_text != NULL)
		m_recall_text->GarbageMe();
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


void Ship::ManageSpecialFeedback()
{
	float angle_to_fill;
	if ((m_jump_cooldown > 0 && m_jump_cooldown < SHIP_JUMPING_COOLDOWN) || (m_cloak_cooldown > 0 && m_cloak_cooldown < SHIP_CLOAK_COOLDOWN))
	{
		float radius = GRAZE_DISTANCE - 8;
		float thickness = 2;

		if (m_jump_cooldown > 0)
			angle_to_fill = 90 + 180.f * (float)(SHIP_JUMPING_COOLDOWN - m_jump_cooldown) / SHIP_JUMPING_COOLDOWN;
		else if (m_cloak_cooldown > 0)
			angle_to_fill = 90 + 180.f * (float)(SHIP_CLOAK_COOLDOWN - m_cloak_cooldown) / SHIP_CLOAK_COOLDOWN;

		//position
		for (int i = 0; i < GRAZING_FEEDBACK_CIRCLE_POINTS * 2; i++)
		{
			float angle = 90 + 360.f * (i / 2) / (GRAZING_FEEDBACK_CIRCLE_POINTS - 1);

			m_special_percent_points[i].position.x = getPosition().x + (radius + (i % 2) * thickness) * cos(-M_PI_2 + angle * M_PI / 180);
			m_special_percent_points[i].position.y = getPosition().y + (radius + (i % 2) * thickness) * sin(-M_PI_2 + angle * M_PI / 180);

			//color
			if (angle_to_fill > 1.f / (GRAZING_FEEDBACK_CIRCLE_POINTS - 1) && angle <= angle_to_fill)
			{
				float s = 0.50 + 1 * abs(sin(2 * m_graze_sinus_clock.getElapsedTime().asSeconds())) / 4;
				m_special_percent_points[i].color = sf::Color(223, 183, 19, 200 * s);
			}
			else if (angle <= 270)
			{
				float s = 0.25 + 3 * abs(sin(4 * m_graze_sinus_clock.getElapsedTime().asSeconds())) / 4;
				m_special_percent_points[i].color = sf::Color(255, 255, 255, 20 * s);
			}
		}
	}
	else
	{
		for (int i = 0; i < GRAZING_FEEDBACK_CIRCLE_POINTS * 2; i++)
			m_special_percent_points[i].color = sf::Color(0, 0, 0, 0);
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
	
			if (m_fake_ship != NULL)
				m_fake_ship->setColor(Color(255, 255, 255, 255), true);
		}
	}

	ManageImmunity(deltaTime * l_hyperspeedMuliplier);

	if (m_visible == false)
		return;
	
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

	ManageGrazingFeedback();
	ManageSpecialFeedback();

	sf::Vector2f directions = InputGuy::getDirections();
	
	//Update dialogs that have a limited lifespam
	if (m_targetDialogs.empty() == false && m_SFTargetPanel != NULL && m_SFTargetPanel->m_panel_type == SFPanel_Dialog && m_SFTargetPanel->GetDuration() > 0 && m_SFTargetPanel->GetDurationTimer() <= 0)
		ContinueDialog();

	ManageGhost(deltaTime * l_hyperspeedMuliplier);
	ManageJump(deltaTime  * l_hyperspeedMuliplier);
	ManageCloak(deltaTime  * l_hyperspeedMuliplier);

	//Manage inputs
	ManageInputs(deltaTime, hyperspeedMultiplier, directions);

	m_trail->m_visible = (hyperspeedMultiplier > 1.0f);
	
	GameObject::update(deltaTime, hyperspeedMultiplier);
	ScreenBorderConstraints();
	SettingTurnAnimations();
	
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

		if ((*CurrentGame).m_gameloop_state == SCROLLING || (*CurrentGame).m_gameloop_state == BOSS_FIGHT)
			if ((*CurrentGame).m_is_in_hub == false && m_shield_max > 0 && m_shield < m_shield_max && m_collision_timer <= 0)
				screen.draw(m_graze_percent_points, GRAZING_FEEDBACK_CIRCLE_POINTS * 2, sf::TrianglesStrip);

		screen.draw(m_special_percent_points, GRAZING_FEEDBACK_CIRCLE_POINTS * 2, sf::TrianglesStrip);
	}
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

bool Ship::ManageFiring(sf::Time deltaTime, float hyperspeedMultiplier)
{
	bool firing = m_visible == true && m_collision_timer <= 0 && m_disable_inputs == false && m_disable_fire == false && m_release_to_fire == false && m_HUD_state != HUD_OpeningEquipment && m_recall_text->m_visible == false && (*CurrentGame).m_end_dialog_clock.getElapsedTime().asSeconds() > END_OF_DIALOGS_DELAY
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
				(*CurrentGame).killGameObjectType(EnemyObject, false);
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
		//else if (m_HUD_state == HUD_OpeningEquipment && m_SFHudPanel)
		//{
		//	//Cursor movement
		//	m_SFHudPanel->SetCursorVisible_v2(true);
		//
		//	//Freeze cursor if throwing item, otherwise move it
		//	if (m_SFHudPanel->GetCursor()->m_currentAnimationIndex < Cursor_Focus1_8 || m_SFHudPanel->GetCursor()->m_currentAnimationIndex > Cursor_Focus8_8)
		//		MoveCursor(m_SFHudPanel->GetCursor(), inputs_direction, deltaTime, m_SFHudPanel);
		//
		//
		//	//Weapon firing
		//	bool firing = ManageFiring(deltaTime, hyperspeedMultiplier);
		//
		//	//Closing hud
		//	if (UpdateAction(Action_OpeningHud, Input_Tap, true))
		//	{
		//		m_HUD_state = HUD_Idle;
		//		if (m_SFHudPanel)
		//		{
		//			m_SFHudPanel->SetCursorVisible_v2(false);
		//		}
		//	}
		//}
		////TRADE (BUY/SELL)
		//else if (m_HUD_state == HUD_Trade && m_SFTargetPanel)
		//{
		//	//Cursor movement
		//	MoveCursor(m_SFTargetPanel->GetCursor(), inputs_direction, deltaTime, m_SFTargetPanel);
		//
		//
		//	//exit
		//	if (m_inputs_states[Action_Slowmotion] == Input_Tap)
		//	{
		//		m_HUD_state = HUD_ShopMainMenu;
		//		m_SFTargetPanel->SetCursorVisible_v2(false);
		//	}
		//}
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
			if (m_inputs_states[Action_Hyperspeeding] == Input_Tap)
			{
				CenterMapView();
			}

			//exit
			if (m_inputs_states[Action_Braking] == Input_Tap)
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
			if (m_inputs_states[Action_Braking] == Input_Tap)
			{
				m_HUD_state = HUD_ShopMainMenu;
				m_SFTargetPanel->SetCursorVisible_v2(false);
			}
		}
		else//Idle combat, shop menu, portal menu (where ship can continue to move)
		{
			//Opening hud
			//if (UpdateAction(Action_OpeningHud, Input_Tap, true))
			//{
			//	m_HUD_state = HUD_OpeningEquipment;
			//	m_SFHudPanel->SetCursorVisible_v2(true);
			//	
			//	if (!m_disableSlowmotion)
			//	{
			//		(*CurrentGame).m_hyperspeedMultiplier = 1.0f / m_hyperspeed;
			//	}
			//
			//	m_actions_states[Action_Firing] = false;
			//}

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
				UpdateAction(Action_Slowmotion, Input_Tap, !m_disableSlowmotion);

				//jump
				if (m_can_jump == true && m_inputs_states[Action_Hyperspeeding] == Input_Tap && m_disableSpecial == false && m_jump_cooldown <= 0 && m_collision_timer <=0)
					Jump();

				if (m_jump_timer == 1.f * SHIP_JUMPING_DISTANCE / SHIP_JUMPING_SPEED)
					if (m_moving == true)
					{
						ScaleVector(&m_speed, SHIP_JUMPING_SPEED);
						m_jump_timer -= deltaTime.asSeconds();
					}

				//cloak
				if (m_can_cloak == true && m_inputs_states[Action_Hyperspeeding] == Input_Tap && m_disableSpecial == false && m_cloak_cooldown <= 0 && m_collision_timer <= 0)
					Cloak();

				//bomb
				//else if ((*CurrentGame).m_is_in_hub == false && m_bombs > 0 && m_inputs_states[Action_Slowmotion] == Input_Tap && !m_actions_states[Action_Recalling] && !m_immune)
				//{
				//	Bomb();
				//}
				
				if (!m_actions_states[Action_Recalling])
					(*CurrentGame).m_hyperspeedMultiplier = 1.0f;//resetting hyperspeed

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
				for (Bot* bot : m_bot_list)
					bot->Fire(deltaTime, hyperspeedMultiplier, firing);

				//Braking and speed malus on firing
				UpdateAction(Action_Braking, Input_Hold, !m_actions_states[Action_Recalling]);
				//brake speed malus
				if (m_jump_timer <= 0)
				{
					if (firing == true || (m_actions_states[Action_Braking] == Input_Tap || m_actions_states[Action_Braking] == Input_Hold))
					{
						m_speed.x *= SHIP_BRAKING_MALUS_SPEED;
						m_speed.y *= SHIP_BRAKING_MALUS_SPEED;
					}
				}
			}
			//PORTAL
			else if (m_HUD_state == HUD_PortalInteraction)
			{
				//Up and down in options, IF LEVEL CHOOSER IS AVAILABLE
				//if (m_SFTargetPanel)
				//{
				//	if (m_inputs_states[Action_Braking] == Input_Tap && m_SFTargetPanel->GetSelectedOptionIndex() < m_targetPortal->m_max_unlocked_hazard_level)
				//	{
				//		m_SFTargetPanel->SetSelectedOptionIndex(m_SFTargetPanel->GetSelectedOptionIndex() + 1);
				//	}
				//	else if (m_inputs_states[Action_Hyperspeeding] == Input_Tap && m_SFTargetPanel->GetSelectedOptionIndex() > 0)
				//	{
				//		m_SFTargetPanel->SetSelectedOptionIndex(m_SFTargetPanel->GetSelectedOptionIndex() - 1);
				//	}
				//}

				//Weapon firing
				bool firing = ManageFiring(deltaTime, hyperspeedMultiplier);

				//Bots firing
				for (Bot* bot : m_bot_list)
					bot->Fire(deltaTime, (*CurrentGame).m_hyperspeedMultiplier, firing);

				//brake speed malus
				if (m_jump_timer <= 0)
				{
					if (firing == true || (m_actions_states[Action_Braking] == Input_Tap || m_actions_states[Action_Braking] == Input_Hold))
					{
						m_speed.x *= SHIP_BRAKING_MALUS_SPEED;
						m_speed.y *= SHIP_BRAKING_MALUS_SPEED;
					}
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
					m_HUD_state = HUD_Upgrades;
				}
				//X: Enter stellar map
				else if (m_inputs_states[Action_Hyperspeeding] == Input_Tap)
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
	if (m_jump_timer > 0 && m_jump_timer < 1.f * SHIP_JUMPING_DISTANCE / SHIP_JUMPING_SPEED)//jumping
		return;

	m_speed.x += inputs_direction.x * m_acceleration;
	m_speed.y += inputs_direction.y * m_acceleration;

	//max speed constraints
	GameObject::NormalizeVector(&m_speed, m_max_speed);
}

void Ship::ManageGhost(sf::Time deltaTime)
{
	//ghost status expires?
	if (m_ghost_timer > 0)
	{
		m_ghost_timer -= deltaTime.asSeconds();
		setGhost(m_ghost_timer > 0);
	}
}

void Ship::ManageJump(sf::Time deltaTime)
{
	if (m_jump_ghost_timer > 0)
	{
		m_jump_ghost_timer -= deltaTime.asSeconds();

		if (m_ghost_timer < m_jump_ghost_timer)
			m_ghost_timer = m_jump_ghost_timer;
	}

	if (m_jump_cooldown > 0 && m_jump_ghost_timer <= 0)
		m_jump_cooldown -= deltaTime.asSeconds();

	if (m_jump_timer > 0 && m_jump_timer < 1.f * SHIP_JUMPING_DISTANCE / SHIP_JUMPING_SPEED)
	{
		PlayStroboscopicEffect(sf::seconds(0.1), sf::seconds(0.01), m_ghost_timer > 0 ? GHOST_ALPHA_VALUE : 255);

		m_jump_timer -= deltaTime.asSeconds();

		if (m_jump_timer <= 0)//end of jump
			m_speed = sf::Vector2f(0, 0);
	}
}

void Ship::ManageCloak(sf::Time deltaTime)
{
	if (m_cloak_ghost_timer > 0)
	{
		m_cloak_ghost_timer -= deltaTime.asSeconds();

		if (m_ghost_timer < m_cloak_ghost_timer)
			m_ghost_timer = m_cloak_ghost_timer;
	}

	if (m_cloak_cooldown > 0 && m_cloak_ghost_timer <= 0)
		m_cloak_cooldown -= deltaTime.asSeconds();
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

			SetBotsVisibility(false);

			//death
			if (m_armor <= 0)
				SetVisibility(false);
		}
		else if (m_armor > 0)
		{
			SetVisibility(true);

			setColor(sf::Color(255, 255, 255, 255));
			if (m_fake_ship != NULL)
				m_fake_ship->setColor(sf::Color(255, 255, 255, 255));

			//enemy phase "player unhit" condition
			for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[EnemyObject])
			{
				Enemy* enemy = (Enemy*)object;

				if (enemy->m_currentPhase != NULL)
				{
					for (ConditionTransition* cond : enemy->m_currentPhase->m_transitions_list)
					{
						if (cond->m_condition == PlayerUnhit)
						{
							enemy->setPhase(enemy->getPhase(cond->m_nextPhase_name));
							break;
						}
					}
				}
			}
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
	(*CurrentGame).killGameObjectType(EnemyFire, false);
	(*CurrentGame).killGameObjectType(EnemyObject, true);
}

void Ship::Jump()
{
	m_jump_timer = 1.f * SHIP_JUMPING_DISTANCE / SHIP_JUMPING_SPEED;
	m_jump_ghost_timer = SHIP_JUMPING_GHOST_DURATION;
	m_jump_cooldown = SHIP_JUMPING_COOLDOWN;

	FX* fx_jump = new FX(getPosition(), sf::Vector2f(0, 0), "2D/FX/FX_jump.png", sf::Vector2f(100, 100), false, 4);
	(*CurrentGame).addToScene(fx_jump, true);
}

void Ship::Cloak()
{
	m_cloak_ghost_timer = SHIP_CLOAK_GHOST_DURATION;
	m_cloak_cooldown = SHIP_CLOAK_COOLDOWN;

	FX* fx_jump = new FX(getPosition(), sf::Vector2f(0, 0), "2D/FX/FX_jump.png", sf::Vector2f(100, 100), false, 4);
	(*CurrentGame).addToScene(fx_jump, true);
}

void Ship::SettingTurnAnimations()
{
	//setting animation
	const float x = m_speed.x;

	if (m_fake_ship != NULL)
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

	sf::Vector2f size = m_fake_ship != NULL ? m_fake_ship->m_size : m_size;

	//screen borders contraints	correction
	if (getPosition().x < size.x / 2 + offset.x)
	{
		setPosition(size.x / 2 + offset.x, getPosition().y);
		m_speed.x = 0;
	}

	if (getPosition().x > SCENE_SIZE_X - (size.x / 2) - offset.x)
	{
		setPosition(SCENE_SIZE_X - (size.x / 2) - offset.x, getPosition().y);
		m_speed.x = 0;
	}

	if (getPosition().y < size.y / 2 + offset.y)
	{
		setPosition(getPosition().x, size.y / 2 + offset.y);
		m_speed.y = 0;
	}

	if (getPosition().y > SCENE_SIZE_Y - (size.y / 2) - offset.y)
	{
		setPosition(getPosition().x, SCENE_SIZE_Y - (size.y / 2) - offset.y);
		m_speed.y = 0;
	}
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	if (m_jump_timer > 0)//jumping
		return;

	//idle deceleration
	if (!m_movingX || m_HUD_state == HUD_OpeningEquipment || m_recall_text->m_visible == true)
	{
		m_speed.x -= m_speed.x * deltaTime.asSeconds() * (m_deceleration / 100);

		if (abs(m_speed.x) < SHIP_MIN_SPEED_X)
			m_speed.x = 0;
	}

	if (!m_movingY || m_HUD_state == HUD_OpeningEquipment || m_recall_text->m_visible == true)
	{
		m_speed.y -= m_speed.y * deltaTime.asSeconds() * (m_deceleration / 100);

		if (abs(m_speed.y) < SHIP_MIN_SPEED_Y)
			m_speed.y = 0;
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
	SetVisibility(true);
	
	m_collision_timer = 0;
	m_graze_count = 0;
	m_disable_inputs = false;

	m_upgrades.clear();
	m_upgrades_short.clear();
	m_SFHudPanel->UpdateUpgradeIcons();

	m_max_speed = 750;
	m_deceleration = 2000;
	m_acceleration = 800;

	m_armor_max = 3;
	m_armor = m_armor_max;
	m_shield_max = 0;
	m_shield = 0;
	delete m_weapon;
	m_weapon = Enemy::LoadWeapon("laser", -1);
	for (Bot* bot : m_bot_list)
		bot->Death();
	m_bot_list.clear();

	m_level = 1;
	m_money = 100;
	m_crystals = 0;
	m_knownScenes.clear();

	m_can_jump = false;
	m_can_cloak = false;
	m_bombs = 0;

	m_automatic_fire = false;

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

void Ship::Death(bool give_money)
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
	sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow
	SFText* text_feedback = new SFText((*CurrentGame).m_font[Font_Terminator], 18, _yellow, getPosition());
	ostringstream ss;
		
	//+1 hp
	if (object.m_display_name.compare("hp_1") == 0)
	{
		if (m_armor < m_armor_max)
			m_armor++;
			
		ss << "+1 hp";
	}
	else if (object.m_display_name.compare("crystal_1") == 0)
	{
		m_crystals++;
		ss << "+1 crystal";
	}
		
	text_feedback->setString(ss.str());
	sf::Vector2f size = m_fake_ship ? m_fake_ship->m_size : m_size;
	text_feedback->setPosition(getPosition());
	SFTextPop* pop_feedback = new SFTextPop(text_feedback, 0, MONEY_LOOT_DISPLAY_NOT_FADED_TIME, MONEY_LOOT_DISPLAY_FADE_OUT_TIME, NULL, MONEY_LOOT_DISPLAY_SPEED_Y, sf::Vector2f(0, (-size.y / 2 - TEXT_POP_OFFSET_Y)));
	pop_feedback->setPosition(sf::Vector2f(pop_feedback->getPosition().x - pop_feedback->getGlobalBounds().width / 2, pop_feedback->getPosition().y));
	delete text_feedback;
	(*CurrentGame).addToTextPops(pop_feedback);
		
	return true;
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

void Ship::GetDamageFrom(GameObject& object)
{
	if (m_collision_timer > 0 || (*CurrentGame).m_waiting_for_dialog_validation || (*CurrentGame).m_waiting_for_scene_transition)
		return;

	if (object.m_damage == 0)
		return;

	if (object.m_collision_timer > 0)
		return;

	if (m_ghost_timer > 0)
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
			FX* fx = new FX(sf::Vector2f(0, 0), sf::Vector2f(0, 0), (*CurrentGame).m_FXConfig[fx_name][FX_FILENAME], sf::Vector2f(stoi((*CurrentGame).m_FXConfig[fx_name][FX_WIDTH]), stoi((*CurrentGame).m_FXConfig[fx_name][FX_HEIGHT])), (bool)stoi((*CurrentGame).m_FXConfig[fx_name][FX_IS_PERMANENT]), 2);

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

	int damage = 1;//object.m_damage
	if (damage > m_shield)
	{
		m_armor -= (damage - m_shield);
		m_shield = 0;
	}
	else
		m_shield -= damage;
	
	//Combo
	//(*CurrentGame).m_playership->AddComboCount(-(*CurrentGame).m_playership->m_combo_count_max / 2);

	m_graze_count = 0;
	//m_graze_level = GRAZE_LEVEL_NONE;
	m_hits_taken++;
	//if (m_combo_aura != NULL)
	//	m_combo_aura->setAnimationLine(GRAZE_LEVEL_NONE);

	if (m_armor > 0)
	{	
		//bomb
		(*CurrentGame).killGameObjectType(EnemyFire, false);
		(*CurrentGame).killGameObjectType(EnemyObject, true);
	}
	else
	{
		//Death. Remove all existing beams fired by player
		if (m_weapon != NULL && m_weapon->m_beams.empty() == false)
		{
			for (Ammo* beam : m_weapon->m_beams)
				beam->Death();

			m_weapon->m_beams.clear();
		}

		for (Bot* bot : m_bot_list)
			if (bot->m_weapon != NULL && bot->m_weapon->m_beams.empty() == false)
			{
				for (Ammo* beam : bot->m_weapon->m_beams)
					beam->Death();

				bot->m_weapon->m_beams.clear();
			}
	}

	//enemy phase "player hit" condition
	for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[EnemyObject])
	{
		Enemy* enemy = (Enemy*)object;

		if (enemy->m_currentPhase != NULL)
		{
			for (ConditionTransition* cond : enemy->m_currentPhase->m_transitions_list)
			{
				if (cond->m_condition == PlayerHit)
				{
					enemy->setPhase(enemy->getPhase(cond->m_nextPhase_name));
					break;
				}
			}
		}
	}
		
	//if (m_armor <= 0)
	//Death();
}

sf::Vector2f Ship::GetShipSize()
{
	return m_fake_ship != NULL ? m_fake_ship->m_size : m_size;
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
		cerr << "DEBUG: Failed to open PLAYER SAVE FILE !" << endl;
	}

	printf("DEBUG: Loaded %d known scenes from profile. Respawn scene: %s.\n", ship->m_knownScenes.size(), return_current_scene.c_str());

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
		data << "Crystal " << ship->m_crystals << endl;

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
			else if (i == 5)
				ship->m_crystals = stoi(value);
		
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

		//update HUD
		ship->m_SFHudPanel->UpdateUpgradeIcons();
		
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
		bot->m_visible = visible;
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

void Ship::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses, int max_alpha)
{
	if (m_fake_ship != NULL)
		m_fake_ship->PlayStroboscopicEffect(effect_duration, time_between_poses, max_alpha);
	else
	{
		if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
		{
			Stroboscopic* strobo = new Stroboscopic(effect_duration, this, max_alpha);
			(*CurrentGame).addToScene(strobo, true);

			m_stroboscopic_effect_clock.restart();
		}
	}
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

		//keep a short list of the most advances upgrades
		bool replaced = false;
		string unlocked_by = (*CurrentGame).m_upgradesConfig[upgrade_name][UPGRADE_UNLOCKED_BY];
		while (unlocked_by.compare("0") != 0)
		{
			for (vector<string>::iterator it = m_upgrades_short.begin(); it != m_upgrades_short.end(); it++)
			{
				if (it->compare(unlocked_by) == 0)
				{
					*it = upgrade_name;
					unlocked_by = "0";
					replaced = true;
					break;
				}
			}

			if (replaced == false)
				unlocked_by = (*CurrentGame).m_upgradesConfig[unlocked_by][UPGRADE_UNLOCKED_BY];
		}
		
		if (replaced == false)
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
	{
		SetDrone("bot_laser", 4);
		SetDroneWeapon("laser1");
	}
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_1") == 0)
		SetDrone("bot_laserbeam", 1);
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_2") == 0)
	{
		SetDrone("bot_laserbeam", 1);
		SetDroneWeapon("laserbeam1");
	}
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_3") == 0)
	{
		SetDrone("bot_laserbeam", 1);
		SetDroneWeapon("laserbeam2");
	}
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_4") == 0)
	{
		SetDrone("bot_laserbeam", 1);
		SetDroneWeapon("laserbeam3");
	}
	else if (upgrade_name.compare("Upgrade_drone_laserbeam_5") == 0)
	{
		SetDrone("bot_laserbeam", 2);
		SetDroneWeapon("laserbeam3");
	}
	else if (upgrade_name.compare("Upgrade_drone_minirocket_1") == 0)
		SetDrone("bot_minirocket", 1);
	else if (upgrade_name.compare("Upgrade_drone_minirocket_2") == 0)
	{
		SetDrone("bot_minirocket", 1);
		SetDroneWeapon("minirocket1");
	}
	else if (upgrade_name.compare("Upgrade_drone_minirocket_3") == 0)
	{
		SetDrone("bot_minirocket", 2);
		SetDroneWeapon("minirocket1");
	}
	else if (upgrade_name.compare("Upgrade_drone_minirocket_4") == 0)
	{
		SetDrone("bot_minirocket", 2);
		SetDroneWeapon("minirocket2");
	}
	else if (upgrade_name.compare("Upgrade_drone_minirocket_5") == 0)
	{
		SetDrone("bot_minirocket", 2);
		SetDroneWeapon("minirocket3");
	}
	else if (upgrade_name.compare("Restores_hp_1") == 0)
		m_armor++;
	else if (upgrade_name.compare("Upgrade_jump_1") == 0)
		m_can_jump = true;
	else if (upgrade_name.compare("Upgrade_cloak_1") == 0)
		m_can_cloak = true;
}

void Ship::SetWeapon(string weapon_name)
{
	delete m_weapon;
	m_weapon = Enemy::LoadWeapon(weapon_name, -1);
}

void Ship::SetDrone(string drone_name, int number)
{
	for (Bot* bot : m_bot_list)
		bot->Death();

	m_bot_list.clear();

	Bot* bot = Enemy::LoadBot(drone_name);
	m_bot_list.push_back(bot);

	for (int i = 1; i < number; i++)
	{
		Bot* new_bot = Enemy::LoadBot(drone_name);

		//bots auto spreading based on number of bots
		int s = i % 2 == 0 ? 1 : -1;
		int x = i / 2;
		new_bot->m_spread.x *= s * (1 + x);
		if (new_bot->m_pattern.m_pattern_type == Circle_)
		{
			new_bot->m_pattern.m_starting_point += 6;
			new_bot->m_pattern.m_starting_point = new_bot->m_pattern.m_starting_point % 12;
			new_bot->m_pattern.resetPattern();
		}
		
		m_bot_list.push_back(new_bot);
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
	//- not forbidden
	//- condition true
	vector<string> random_upgrades;
	for (map<string, vector<string> >::iterator it = (*CurrentGame).m_upgradesConfig.begin(); it != (*CurrentGame).m_upgradesConfig.end(); it++)
	{
		if (stoi(it->second[UPGRADE_PERMANENT]) == 1)
		{
			//already possessed? forbidden by another possessed upgrade?
			bool found = false;
			for (vector<string>::iterator it2 = m_upgrades.begin(); it2 != m_upgrades.end(); it2++)
			{
				if (it2->compare(it->first) == 0 || (*CurrentGame).m_upgradesConfig[*it2][UPGRADE_FORBIDS_1].compare(it->first) == 0 || (*CurrentGame).m_upgradesConfig[*it2][UPGRADE_FORBIDS_2].compare(it->first) == 0)
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
		random_upgrades.push_back(it->first);
	}

	//randomize among the short list
	random_shuffle(random_upgrades.begin(), random_upgrades.end());

	for (int i = 0; i < NB_UPGRADE_CHOICES; i++)
	{
		target_shop->m_upgrades[i] = random_upgrades[i];
		target_shop->m_sold_out[i] = false;
	}
}