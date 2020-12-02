#pragma once

#include "string"
#include "vector"

#include "wx/wx.h"

#include "ImagePanel.h"

using std::string;
using std::vector;

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

	wxBoxSizer* vertical = nullptr;

public:
	CharacterShowcase(wxWindow* parent);

	void setData(wxImage& image, vector<string>& charData);
};

