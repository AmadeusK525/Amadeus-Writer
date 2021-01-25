#ifndef ELEMENTSHOWCASE_H_
#define ELEMENTSHOWCASE_H_
#pragma once

#include <wx\wx.h>
#include <wx\scrolwin.h>

#include "ImagePanel.h"
#include "Elements.h"

#include <vector>

using std::vector;
using std::pair;

class ElementShowcase: public wxScrolledWindow {
protected:
    ImagePanel* m_image = nullptr;

    wxStaticText* m_name = nullptr,
        * m_role = nullptr;

    vector<pair<wxStaticText*, wxTextCtrl*>> m_custom{};
	wxBoxSizer* m_vertical = nullptr;

public:
    ElementShowcase(wxWindow* parent);
	virtual void SetData(const Element& element);
};

///////////////////////////////////////////////////////////////////
///////////////////////// CharacterShowcase ///////////////////////
///////////////////////////////////////////////////////////////////

class CharacterShowcase : public ElementShowcase {
private:
	wxStaticText* m_ageLabel = nullptr,
		* m_sexLabel = nullptr,
		* m_natLabel = nullptr,
		* m_heightLabel = nullptr,
		* m_nickLabel = nullptr,
		* m_appLabel = nullptr,
		* m_perLabel = nullptr,
		* m_bsLabel = nullptr;

	wxStaticText* m_age = nullptr,
		* m_sex = nullptr,
		* m_nat = nullptr,
		* m_height = nullptr,
		* m_nick = nullptr;

	wxTextCtrl* m_appearance = nullptr,
		* m_personality = nullptr,
		* m_backstory = nullptr;

	vector<pair<wxStaticText*, wxTextCtrl*>> m_custom{};

public:
	CharacterShowcase(wxWindow* parent);
	virtual void SetData(const Element& charToSet);
};


///////////////////////////////////////////////////////////////////
///////////////////////// LocationShowcase ////////////////////////
///////////////////////////////////////////////////////////////////


class LocationShowcase : public ElementShowcase {
private:
    wxStaticText* m_generalLabel = nullptr,
        * m_natLabel = nullptr,
        * m_archLabel = nullptr,
        * m_poliLabel = nullptr,
        * m_ecoLabel = nullptr,
        * m_culLabel = nullptr;

    wxTextCtrl* m_general = nullptr,
        * m_natural = nullptr,
        * m_architecture = nullptr,
        * m_politics = nullptr,
        * m_economy = nullptr,
        * m_culture = nullptr;

    wxBoxSizer* m_firstColumn = nullptr,
        * m_secondColumn = nullptr;

    bool m_first = true;

public:
    LocationShowcase(wxWindow* parent);

    virtual void SetData(const Element& locToSet);
};


#endif