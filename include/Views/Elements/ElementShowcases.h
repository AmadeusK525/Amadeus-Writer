#ifndef ELEMENTSHOWCASE_H_
#define ELEMENTSHOWCASE_H_
#pragma once

#include <wx/wx.h>
#include <wx/wrapsizer.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/scrolwin.h>

#include "Controls/ImagePanel.h"
#include "Elements/StoryElements.h"
#include "Views/StoryWriter/StoryWriter.h"
#include "Controls/SwitchCtrl.h"
#include "Utils/amUtility.h"

using std::pair;

class amTangibleElementShowcase;
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
	amTangibleElementShowcase* m_showcase = nullptr;

	am::StoryElement* m_owner = nullptr;
	wxStaticText* m_label = nullptr;

	wxBoxSizer* m_vertical = nullptr;
	wxWrapSizer* m_grid = nullptr;

	struct amElementButton : public wxButton
	{
		am::StoryElement* element = nullptr;

		amElementButton(wxWindow* parent, am::StoryElement* element, const wxString& label) : 
			wxButton(parent, -1, label, wxDefaultPosition, wxSize(90, 140))
		{
			SetFont(wxFontInfo(8).Bold());
			SetForegroundColour(wxColour(255, 255, 255));
			SetBackgroundColour(wxColour(0, 0, 0));

			SetElement(element);
		}

		void SetElement(am::StoryElement* element)
		{
			this->element = element;

			wxString label = element->name;
			label.Replace(" ", "\n");

			if ( GetBitmap().IsOk() )
			{
				SetBitmap(wxNullBitmap);
			}

			if ( element->image.IsOk() )
				SetBitmap(am::GetScaledImage(80, 80, element->image), wxTOP);

			SetLabel(label);
			Refresh();
		}
	};

public:
	amRelatedElementsContainer(wxWindow* parent, amTangibleElementShowcase* showcase);

	void LoadCharacters(am::StoryElement* element);
	void LoadLocations(am::StoryElement* element);
	void LoadLocationsByType(am::StoryElement* element);
	void LoadItems(am::StoryElement* element);

	void DoLoad(am::StoryElement* element, bool (*ShouldAdd)(am::StoryElement*));
	void ClearAll();

	void OnAddElement(wxCommandEvent& event);
	void OnRemoveElement(wxCommandEvent& event);
	void OnElementButtons(wxCommandEvent& event);
	void LoadAllElements(am::StoryElement* element);
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
	am::StoryElement* m_element = nullptr;
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
	amRelatedElementsDialog(wxWindow* parent, am::StoryElement* element, amRelatedElementsContainer* container, MODE mode);

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
	am::StoryElement* m_element = nullptr;

public:
	virtual bool Create(wxWindow* parent);

	virtual void SetData(am::StoryElement* element) = 0;
	virtual bool LoadFirstPanel(am::TangibleElement* element) = 0;
	virtual bool LoadSecondPanel(am::TangibleElement* element) = 0;
	virtual void ClearAll() = 0;

	virtual void ShowPage(int index) = 0;

	am::StoryElement* GetElement() { return m_element; }

	wxDECLARE_ABSTRACT_CLASS(amElementShowcase);
};

class amTangibleElementShowcase : public amElementShowcase
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

	wxVector<std::pair<wxStaticText*, wxTextCtrl*>> m_custom{};

	wxBoxSizer* m_mainVerSizer = nullptr;
	wxWrapSizer* m_pShortAttrSizer = nullptr;
	wxBoxSizer* m_secondVerSizer = nullptr;

	wxDataViewCtrl* m_documentView = nullptr;
	wxObjectDataPtr<StoryTreeModel> m_documentViewModel;
	wxObjectDataPtr<StoryTreeModel> m_addDocumentViewModel;
#ifdef __WXMSW__
	amHotTrackingDVCHandler m_documentViewHandler;
	amHotTrackingDVCHandler m_addDocumentViewHandler;
#endif /*__WXMSW__*/

	//wxListView* m_documents = nullptr;
	wxButton* m_addDocumentBtn = nullptr;

	amRelatedElementsContainer* m_relatedElements = nullptr;

public:
	amTangibleElementShowcase() = default;
	amTangibleElementShowcase(wxWindow* parent);

	virtual bool Create(wxWindow* parent);

	virtual void SetData(am::StoryElement* element);
	virtual bool LoadFirstPanel(am::TangibleElement* element);
	virtual bool LoadSecondPanel(am::TangibleElement* element);
	virtual void ClearAll() = 0;

	void ShowPage(int index);

	void OnDocumentActivated(wxDataViewEvent& event);

	void OnAddDocument(wxCommandEvent& event);
	void OnRemoveDocument(wxCommandEvent& event);
	void OnOpenDocument(wxCommandEvent& event);

	void OnNextPanel(wxCommandEvent& event);
	void OnPreviousPanel(wxCommandEvent& event);

	inline void EmptyMouseEvent(wxMouseEvent& event) {}
	
	wxDECLARE_ABSTRACT_CLASS(amTangibleElementShowcase);

protected:
	virtual void LoadShortAttr(am::StoryElement* element);
	virtual void LoadLongAttr(am::StoryElement* element);
};


///////////////////////////////////////////////////////////////////
///////////////////////// CharacterShowcase ///////////////////////
///////////////////////////////////////////////////////////////////


class amCharacterShowcase : public amTangibleElementShowcase
{
private:
	wxSwitchCtrl* m_isAlive = nullptr;

public:
	amCharacterShowcase() = default;
	amCharacterShowcase(wxWindow* parent);

	virtual bool Create(wxWindow* parent);

	virtual bool LoadFirstPanel(am::TangibleElement* element) override;
	virtual void ClearAll() override;

	void OnIsAlive(wxCommandEvent& event);

	wxDECLARE_DYNAMIC_CLASS(amCharacterShowcase);
};


///////////////////////////////////////////////////////////////////
///////////////////////// LocationShowcase ////////////////////////
///////////////////////////////////////////////////////////////////


class amLocationShowcase : public amTangibleElementShowcase
{
private:
	wxBoxSizer* m_firstColumn = nullptr,
		* m_secondColumn = nullptr;

	bool m_bAddInFirstCol = true;

public:
	amLocationShowcase() = default;
	amLocationShowcase(wxWindow* parent);
	
	virtual bool Create(wxWindow* parent);

	virtual bool LoadFirstPanel(am::TangibleElement* element) override;
	virtual void ClearAll() override;

	wxDECLARE_DYNAMIC_CLASS(amLocationShowcase);

protected:
	virtual void LoadLongAttr(am::StoryElement* element) override;
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// am::Item Showcase ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////


class amItemShowcase : public amTangibleElementShowcase
{
private:
	wxStaticText* m_stManMade = nullptr,
		* m_stMagic = nullptr;

public:
	amItemShowcase() = default;
	amItemShowcase(wxWindow* parent);
	
	virtual bool Create(wxWindow* parent);

	virtual bool LoadFirstPanel(am::TangibleElement* element) override;
	virtual void ClearAll() override;

	wxDECLARE_DYNAMIC_CLASS(amItemShowcase);
};

#endif