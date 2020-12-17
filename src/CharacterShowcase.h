#pragma once

#include <string>
#include <vector>

#include <wx/wx.h>

#include "Character.h"
#include "ImagePanel.h"

using std::string;
using std::vector;
using std::pair;

class CharacterShowcase : public wxScrolledWindow {
private:
	ImagePanel* m_image = nullptr;
	wxStaticText* m_name = nullptr;

	wxStaticText* m_age = nullptr,
		* m_sex = nullptr,
		* m_nat = nullptr,
		* m_height = nullptr,
		* m_nick = nullptr,
		* m_role = nullptr;

	wxTextCtrl* m_appearance = nullptr,
		* m_personality = nullptr,
		* m_backstory = nullptr;

	vector<pair<wxStaticText*, wxTextCtrl*>> m_custom{};

	wxBoxSizer* m_vertical = nullptr;

public:
	CharacterShowcase(wxWindow* parent);

	void setData(Character& character);
};

