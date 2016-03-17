#ifndef DIALOG_H_INCLUDED
#define DIALOG_H_INCLUDED

#include "GameObject.h"
#include "Weapon.h"
#include "TextUtils.h"

enum DialogData
{
	DIALOG_NAME,
	DIALOG_FADE_IN,
	DIALOG_FADE_OUT,
	DIALOG_ENEMY_SPEAKING,
	DIALOG_DURATION,
	DIALOG_TITLE,
	DIALOG_BODY,
	DIALOG_PICTURE,
	DIALOG_NEXT,
};

class Dialog
{
public:
	Dialog();
	Dialog* Clone();
	string m_name;
	bool m_fade_in;
	bool m_fade_out;
	bool m_enemy_speaking;
	float m_duration;
	string m_title;
	string m_body;
	string m_picture_name;
	string m_next_dialog_name;
};

#endif//DIALOG_H_INCLUDED