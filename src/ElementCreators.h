#ifndef ELEMENTCREATORS_H_
#define ELEMENTCREATORS_H_
#pragma once

#include <wx\wx.h>
#include <wx\wrapsizer.h>

#include "StoryElements.h"
#include "ProjectManager.h"
#include "ImagePanel.h"

using std::pair;

class amElementCreator : public wxFrame
{
protected:
	amProjectManager* m_manager = nullptr;

	wxPanel* m_panel1 = nullptr,
		* m_panel3 = nullptr,
		* m_btnPanel = nullptr;
	wxScrolledWindow* m_panel2 = nullptr;

	ImagePanel* m_imagePanel = nullptr;

	std::unordered_map<wxStaticText*, wxTextCtrl*> m_mShortAttr;
	std::unordered_map<wxStaticText*, wxTextCtrl*> m_mPreDefLongAttr;
	std::unordered_map<wxTextCtrl*, wxTextCtrl*> m_mLongAttr;
	wxVector<pair<wxTextCtrl*, wxTextCtrl*>> m_custom{};
	wxVector<wxButton*> m_minusButtons{};

	wxButton* m_addCustom = nullptr;
	wxBoxSizer* m_customSizer = nullptr;

	wxButton* m_back = nullptr;
	wxButton* m_cancel = nullptr;
	wxButton* m_next = nullptr;

	wxButton* m_chooseImage = nullptr;
	wxButton* m_removeImage = nullptr;

	wxBoxSizer* m_mainSizer = nullptr;
	StoryElement* m_elementEdit = nullptr;

public:
	amElementCreator() = default;
	amElementCreator(wxWindow* parent,
		amProjectManager* manager,
		long id = wxID_ANY,
		const wxString& label = "Create element",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(700, 700),
		long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
		wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);

	virtual bool Create(wxWindow* parent,
		amProjectManager* manager,
		long id = wxID_ANY,
		const wxString& label = "Create element",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(700, 700),
		long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
		wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);

	virtual void StartEditing(StoryElement* element) = 0;
	virtual void DoEdit(wxCommandEvent& event) = 0;

	virtual void AddCustomAttr(wxCommandEvent& event) = 0;
	virtual void RemoveCustomAttr(wxCommandEvent& event);

	void Next(wxCommandEvent& event);
	void Back(wxCommandEvent& event);
	void Cancel(wxCommandEvent& event);

	void SetImage(wxCommandEvent& event);
	void RemoveImage(wxCommandEvent& event);

	virtual wxVector<std::pair<wxString, wxString>> GetShortAttributes();
	virtual wxVector<std::pair<wxString, wxString>>  GetLongAttributes();

	virtual void OnCreateElement(wxCommandEvent& event) = 0;
	virtual void OnClose(wxCloseEvent& event);

	enum
	{
		BUTTON_RemoveImage,
		BUTTON_SetImage,

		BUTTON_Cancel,

		BUTTON_Back1,
		BUTTON_Back2,

		BUTTON_BackEdit1,
		BUTTON_BackEdit2,

		BUTTON_CreateEdit,

		BUTTON_Next1,
		BUTTON_Next2,

		BUTTON_NextEdit1,
		BUTTON_NextEdit2,

		BUTTON_Create
	};

	DECLARE_EVENT_TABLE()
	wxDECLARE_ABSTRACT_CLASS(amElementCreator);
};


//////////////////////////////////////////////////////////////////////
//////////////////////// Character Creator /////////////////////////
/////////////////////////////////////////////////////////////////////


class amCharacterCreator : public amElementCreator
{
private:
	wxTextCtrl* ncFullName = nullptr,
		* ncNickname = nullptr,
		* ncAge = nullptr,
		* ncSex = nullptr,
		* ncNationality = nullptr,
		* ncHeight = nullptr,
		* ncBackstory = nullptr,
		* ncAppearance = nullptr,
		* ncPersonality = nullptr;

	wxStaticText* label8 = nullptr;

	wxRadioButton* ncMain = nullptr,
		* ncSupp = nullptr,
		* ncVillian = nullptr,
		* ncSecon = nullptr;

public:
	amCharacterCreator() = default;
	amCharacterCreator(wxWindow* parent,
		amProjectManager* manager,
		long id = wxID_ANY,
		const wxString& label = "Create character",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(700, 700),
		long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
		wxCLIP_CHILDREN | wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT);

	virtual bool Create(wxWindow* parent,
		amProjectManager* manager,
		long id = wxID_ANY,
		const wxString& label = "Create element",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(700, 700),
		long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
		wxCLIP_CHILDREN | wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT) override;

	virtual void StartEditing(StoryElement* editChar);
	virtual void DoEdit(wxCommandEvent& event);

	virtual void AddCustomAttr(wxCommandEvent& event);

	virtual void OnCreateElement(wxCommandEvent& event);
	virtual void OnClose(wxCloseEvent& event);

	inline virtual wxSize DoGetBestClientSize() const override { return wxSize(700, 700); }

	wxDECLARE_DYNAMIC_CLASS(amCharacterCreator);
};


//////////////////////////////////////////////////////////////////////
////////////////////////// Location Creator //////////////////////////
//////////////////////////////////////////////////////////////////////


class amLocationCreator : public amElementCreator
{
private:
	wxTextCtrl* nlName = nullptr,
		* nlGeneral = nullptr,
		* nlNatural = nullptr,
		* nlArchitecture = nullptr,
		* nlPolitics = nullptr,
		* nlEconomy = nullptr,
		* nlCulture = nullptr;

	wxRadioButton* nlHigh = nullptr,
		* nlLow = nullptr;

	wxStaticText* pEconomyLabel = nullptr;
	wxWrapSizer* m_nlcustomSizer = nullptr;

public:
	amLocationCreator() = default;
	amLocationCreator(wxWindow* parent,
		amProjectManager* manager,
		long id,
		const wxString& label = "Create location",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(900, 650), 
		long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
		wxCLIP_CHILDREN | wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT | wxBORDER_SIMPLE);

	virtual bool Create(wxWindow* parent,
		amProjectManager* manager,
		long id = wxID_ANY,
		const wxString& label = "Create location",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(900, 650),
		long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
		wxCLIP_CHILDREN | wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT) override;

	virtual void StartEditing(StoryElement* editLoc);
	virtual void DoEdit(wxCommandEvent& event);

	virtual void AddCustomAttr(wxCommandEvent& event);
	virtual void RemoveCustomAttr(wxCommandEvent& event);
	void RecolorCustoms();

	virtual void OnCreateElement(wxCommandEvent& event);
	virtual void OnClose(wxCloseEvent& event);

	inline virtual wxSize DoGetBestClientSize() const override { return wxSize(1100, 750); }

	wxDECLARE_DYNAMIC_CLASS(amLocationCreator);
};


//////////////////////////////////////////////////////////////////////
//////////////////////////// Item Creator ////////////////////////////
//////////////////////////////////////////////////////////////////////


class amItemCreator : public amElementCreator
{
private:
	wxTextCtrl* niName = nullptr,
		* niGeneral = nullptr,
		* niAppearance = nullptr,
		* niBackstory = nullptr,
		* niOrigin = nullptr,
		* niUsage = nullptr,
		* niWidth = nullptr,
		* niHeight = nullptr,
		* niDepth = nullptr;

	wxStaticText* meaLabel = nullptr;

	wxRadioButton* niMagic = nullptr,
		* niNonMagic = nullptr,
		* niNatural = nullptr,
		* niManMade = nullptr,
		* niHigh = nullptr,
		* niLow = nullptr;

	wxBoxSizer* niMeaSizer = nullptr;

public:
	amItemCreator() = default;
	amItemCreator(wxWindow* parent,
		amProjectManager* manager,
		long id = wxID_ANY,
		const wxString& label = "Create item",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(900, 720),
		long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
		wxCLIP_CHILDREN | wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT | wxBORDER_SIMPLE);

	virtual bool Create(wxWindow* parent,
		amProjectManager* m_manager,
		long id = wxID_ANY,
		const wxString& label = "Create item",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(900, 720),
		long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
		wxCLIP_CHILDREN | wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT) override;

	virtual void StartEditing(StoryElement* editItem);
	virtual void DoEdit(wxCommandEvent& event);

	virtual void AddCustomAttr(wxCommandEvent& event);

	virtual void OnCreateElement(wxCommandEvent& event);
	virtual void OnClose(wxCloseEvent& event);

	void OnPaint(wxPaintEvent& event);

	inline virtual wxSize DoGetBestClientSize() const override { return wxSize(1000, 750); }

	wxDECLARE_DYNAMIC_CLASS(amItemCreator);
};
#endif