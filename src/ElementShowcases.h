#ifndef ELEMENTSHOWCASE_H_
#define ELEMENTSHOWCASE_H_
#pragma once

#include <wx\wx.h>
#include <wx\wrapsizer.h>
#include <wx\scrolwin.h>

#include "ImagePanel.h"
#include "StoryElements.h"
#include "SwitchCtrl.h"
#include "amUtility.h"

using std::pair;

class amElementShowcase;
class amCharacterShowcase;
class amLocationShowcase;
class amItemShowcase;

class amRelatedElementsContainer : public wxScrolledWindow
{
protected:
	amElementShowcase* m_showcase = nullptr;

	Element* m_owner = nullptr;
	wxStaticText* m_label = nullptr;
	wxButton* m_addRelated = nullptr;

	wxBoxSizer* m_vertical = nullptr;
	wxWrapSizer* m_grid = nullptr;

	struct amElementButton : public wxButton
	{
		Element* element = nullptr;

		amElementButton(wxWindow* parent, Element* element, const wxString& label) : 
			wxButton(parent, -1, label, wxDefaultPosition, wxSize(80, 100))
		{
			SetElement(element);
		}

		void SetElement(Element* element)
		{
			this->element = element;

			SetFont(wxFontInfo(9));
			SetForegroundColour(wxColour(255, 255, 255));
			SetBackgroundColour(wxColour(0, 0, 0));

			wxString label = element->name;
			label.Replace(" ", "\n");

			if ( GetBitmap().IsOk() )
			{
				SetBitmap(wxNullBitmap);
			}

			if ( element->image.IsOk() )
				SetBitmap(amGetScaledImage(70, 70, element->image), wxTOP);

			SetLabel(label);
			Refresh();
		}
	};

public:
	amRelatedElementsContainer(wxWindow* parent, amElementShowcase* showcase);

	void OnElementButtonsClicked(wxCommandEvent& event);

	void LoadAllElements(Element* element);

	virtual void LoadCharacters(Element* element);
	virtual void LoadLocations(Element* element);
	virtual void LoadLocationsByType(Element* element);
	virtual void LoadItems(Element* element);

	void DoLoad(Element* element, bool (*ShouldAdd)(Element*));
};

class amElementShowcase : public wxWindow
{
protected:
	wxScrolledWindow* m_mainPanel = nullptr;
	wxScrolledWindow* m_secondPanel = nullptr;

	wxButton* m_nextPanel = nullptr;
	wxButton* m_prevPanel = nullptr;

	wxBoxSizer* m_mainSizer = nullptr;

	ImagePanel* m_image = nullptr;

	wxStaticText* m_name = nullptr,
		* m_role = nullptr;

	wxVector<pair<wxStaticText*, wxTextCtrl*>> m_custom{};

	wxBoxSizer* m_mainVerSizer = nullptr;
	wxBoxSizer* m_secondVerSizer = nullptr;

	amRelatedElementsContainer* m_relatedCharacters = nullptr;

public:
	amElementShowcase(wxWindow* parent);
	virtual void SetData(Element* element);
	virtual void ClearAll();

	void OnNextPanel(wxCommandEvent& event);
	void OnPreviousPanel(wxCommandEvent& event);

	void ShowPage(int index);

	inline void EmptyMouseEvent(wxMouseEvent& event) {}
};

///////////////////////////////////////////////////////////////////
///////////////////////// CharacterShowcase ///////////////////////
///////////////////////////////////////////////////////////////////

class amCharacterShowcase : public amElementShowcase
{
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

	wxSwitchCtrl* m_isAlive = nullptr;

	wxVector<pair<wxStaticText*, wxTextCtrl*>> m_custom{};

public:
	amCharacterShowcase(wxWindow* parent);
	virtual void SetData(Character* character);
	virtual void ClearAll() override;
};


///////////////////////////////////////////////////////////////////
///////////////////////// LocationShowcase ////////////////////////
///////////////////////////////////////////////////////////////////


class amLocationShowcase : public amElementShowcase
{
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
	amLocationShowcase(wxWindow* parent);
	virtual void SetData(Location* location);
	virtual void ClearAll() override;
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Item Showcase ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////


class amItemShowcase : public amElementShowcase
{
private:

public:
	amItemShowcase(wxWindow* parent);
	virtual void SetData(Item* item);
	virtual void ClearAll() override;
};

#endif