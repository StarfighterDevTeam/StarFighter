#pragma once
#ifndef _ENUMS_H_INCLUDED
#define _ENUMS_H_INCLUDED

enum SFX_Bank
{
	SFX_Laser,
	NBVAL_SFX_BANK,
};

enum Music_Bank
{
	Music_None,
	Music_Main,
	NBVAL_MUSIC_BANK,
};

enum FontsStyle
{
	Font_Arial,//0
	Font_Terminator,//1
	NBVAL_FontsStyle,//2
};

enum PlayerActions
{
	Action_Idle,
	Action_Firing,
	Action_Muting,
	Action_Pausing,
	NBVAL_PlayerActions,
};

enum PlayerInputStates
{
	Input_Release,//0
	Input_Tap,//1
	Input_Hold,//2
};

enum LayerType {
	BackgroundLayer,
	PortalLayer,
	PlayerStroboscopicLayer,
	ExplosionLayer,
	EnemyObjectLayer,
	AuraLayer,
	FeedbacksLayer,
	FriendlyFireLayer,
	PlayerShipLayer,
	EnemyFireLayer,

	PanelLayer,
	HudObject,
	HudCursor,
	NBVAL_Layer
};

enum GameObjectType {
	BackgroundObject,
	PortalObject,
	ShopObject,
	LootObject,
	PlayerShip,
	FakePlayerShip,
	FriendlyFire,
	Neutral,
	EnemyFire,
	EnemyObject,
	NBVAL_GameObject
};

enum SFPanelTypes
{
	SFPanel_None,
	SFPanel_Specific,
};

enum ActionButtons
{
	ActionButton_A,
	ActionButton_X,
	ActionButton_Y,
	ActionButton_B,
	NBVAL_ActionButtons,//4
};

#endif // _ENUMS_H_INCLUDED
