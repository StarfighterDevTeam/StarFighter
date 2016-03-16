#include "Dialog.h"

Dialog::Dialog()
{
	
}

Dialog* Dialog::Clone()
{
	Dialog* new_dialog = new Dialog();

	new_dialog->m_name = this->m_name;
	new_dialog->m_fade_in = this->m_fade_in;
	new_dialog->m_fade_out = this->m_fade_out;
	new_dialog->m_enemy_speaking = this->m_enemy_speaking;
	new_dialog->m_duration = this->m_duration;
	new_dialog->m_title = this->m_title;
	new_dialog->m_body = this->m_body;
	new_dialog->m_picture_name = this->m_picture_name;
	new_dialog->m_next_dialog_name = this->m_next_dialog_name;

	return new_dialog;
}