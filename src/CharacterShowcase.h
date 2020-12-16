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
	ImagePanel* image = nullptr;
	wxStaticText* name = nullptr;

	wxStaticText* age = nullptr,
		* sex = nullptr,
		* nat = nullptr,
		* height = nullptr,
		* nick = nullptr,
		* role = nullptr;

	wxTextCtrl* appearance = nullptr,
		* personality = nullptr,
		* backstory = nullptr;

	vector<pair<wxStaticText*, wxTextCtrl*>> custom{};

	wxBoxSizer* vertical = nullptr;

public:
	CharacterShowcase(wxWindow* parent);

	void setData(Character& character);
};

