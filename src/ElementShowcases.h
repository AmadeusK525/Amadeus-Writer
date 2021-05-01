#ifndef ELEMENTSHOWCASE_H_
#define ELEMENTSHOWCASE_H_
#pragma once

#include <wx\wx.h>
#include <wx\wrapsizer.h>
#include <wx\listctrl.h>
#include <wx\notebook.h>
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

class amRelatedElementsContainer;


///////////////////////////////////////////////////////////////////
///////////////////// RelatedElementsContainer ////////////////////
///////////////////////////////////////////////////////////////////


class amRelatedElementsContainer : public wxPanel
{
protected:
	amElementShowcase* m_showcase = nullptr;

	Element* m_owner = nullptr;
	wxStaticText* m_label = nullptr;

	wxBoxSizer* m_vertical = nullptr;
	wxWrapSizer* m_grid = nullptr;

	struct amElementButton : public wxButton
	{
		Element* element = nullptr;

		amElementButton(wxWindow* parent, Element* element, const wxString& label) : 
			wxButton(parent, -1, label, wxDefaultPosition, wxSize(90, 140))
		{
			SetFont(wxFontInfo(8).Bold());
			SetForegroundColour(wxColour(255, 255, 255));
			SetBackgroundColour(wxColour(0, 0, 0));

			SetElement(element);
		}

		void SetElement(Element* element)
		{
			this->element = element;

			wxString label = element->name;
			label.Replace(" ", "\n");

			if ( GetBitmap().IsOk() )
			{
				SetBitmap(wxNullBitmap);
			}

			if ( element->image.IsOk() )
				SetBitmap(amGetScaledImage(80, 80, element->image), wxTOP);

			SetLabel(label);
			Refresh();
		}
	};

public:
	amRelatedElementsContainer(wxWindow* parent, amElementShowcase* showcase);

	void LoadCharacters(Element* element);
	void LoadLocations(Element* element);
	void LoadLocationsByType(Element* element);
	void LoadItems(Element* element);

	void DoLoad(Element* element, bool (*ShouldAdd)(Element*));
	void ClearAll();

	void OnAddElement(wxCommandEvent& event);
	void OnRemoveElement(wxCommandEvent& event);
	void OnElementButtons(wxCommandEvent& event);
	void LoadAllElements(Element* element);
};


///////////////////////////////////////////////////////////////////
////////////////////// RelatedElementsDialog  /////////////////////
///////////////////////////////////////////////////////////////////


class amRelatedElementsDialog : public wxFrame
{
public:
	enum MODE
	{
		ADD,
		REMOVE
	};

private:
	Element* m_element = nullptr;
	amRelatedElementsContainer* m_container = nullptr;
	MODE m_mode;

	wxNotebook* m_notebook = nullptr;

	wxListView* m_characters = nullptr,
		* m_locations = nullptr,
		* m_items = nullptr;

	wxImageList* m_characterImages = nullptr,
		* m_locationImages = nullptr,
		* m_itemImages = nullptr;

public:
	amRelatedElementsDialog(wxWindow* parent, Element* element, amRelatedElementsContainer* container, MODE mode);

	void LoadPossibleCharacters();
	void LoadPossibleLocations();
	void LoadPossibleItems();

	void OnMainButton(wxCommandEvent& event);
	void OnRemoveElement(wxCommandEvent& event);

	void OnListResize(wxSizeEvent& event);
};


///////////////////////////////////////////////////////////////////
////////////////////////// ElementShowcase ////////////////////////
///////////////////////////////////////////////////////////////////


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
		* m_nameSecondPanel = nullptr,
		* m_role = nullptr;

	wxVector<pair<wxStaticText*, wxTextCtrl*>> m_custom{};

	wxBoxSizer* m_mainVerSizer = nullptr;
	wxBoxSizer* m_secondVerSizer = nullptr;

	amRelatedElementsContainer* m_relatedElements = nullptr;

public:
	amElementShowcase(wxWindow* parent);

	virtual void SetData(Element* element);
	virtual bool LoadFirstPanel(Element* element) = 0;
	virtual bool LoadSecondPanel(Element* element) = 0;
	virtual void ClearAll() = 0;

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

	virtual bool LoadFirstPanel(Element* element) override;
	virtual bool LoadSecondPanel(Element* element) override;

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
	
	virtual bool LoadFirstPanel(Element* element) override;
	virtual bool LoadSecondPanel(Element* element) override;
	
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

	virtual bool LoadFirstPanel(Element* element) override;
	virtual bool LoadSecondPanel(Element* element) override;

	virtual void ClearAll() override;
};

#endif