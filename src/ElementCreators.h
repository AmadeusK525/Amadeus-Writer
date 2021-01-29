#ifndef ELEMENTCREATORS_H_
#define ELEMENTCREATORS_H_
#pragma once

#include <wx\wx.h>
#include <wx\wrapsizer.h>

#include "Elements.h"
#include "Project.h"
#include "ImagePanel.h"

using std::pair;

class amdElementCreator: public wxFrame {
protected:
    amdProjectManager* m_manager = nullptr;

    wxPanel* m_panel1 = nullptr,
        * m_panel3 = nullptr,
        * m_btnPanel = nullptr;
    wxScrolledWindow* m_panel2 = nullptr;

    ImagePanel* m_imagePanel = nullptr;

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
    Element* m_elementEdit = nullptr;

public:
    amdElementCreator(wxWindow* parent,
        amdProjectManager* m_manager,
        long id = wxID_ANY,
        const wxString& label = "Create element",
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(600, 600),
        long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
        wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);

    virtual wxVector<wxString> GetValues() = 0;
    wxVector<pair<wxString, wxString>> GetCustom();

    virtual void SetEdit(Element* element) = 0;
    virtual void DoEdit(wxCommandEvent& event) = 0;

    virtual void AddCustomAttr(wxCommandEvent& event) = 0;
    virtual void RemoveCustomAttr(wxCommandEvent& event);

    void Next(wxCommandEvent& event);
    void Back(wxCommandEvent& event);
    void Cancel(wxCommandEvent& event);

    void SetImage(wxCommandEvent& event);
    void RemoveImage(wxCommandEvent& event);

    virtual void Create(wxCommandEvent& event) = 0;
    virtual void CheckClose(wxCloseEvent& event) = 0;

    enum {
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
};


//////////////////////////////////////////////////////////////////////
//////////////////////// Character Creator /////////////////////////
/////////////////////////////////////////////////////////////////////


class amdCharacterCreator : public amdElementCreator {
private:
    wxTextCtrl* ncFullName = nullptr,
        * ncNickname = nullptr,
        * ncAge = nullptr,
        * ncNationality = nullptr,
        * ncHeight = nullptr,
        * ncBackstory = nullptr,
        * ncAppearance = nullptr,
        * ncPersonality = nullptr;

    wxStaticText* label8 = nullptr;

    wxRadioButton* ncMale = nullptr,
        * ncFemale = nullptr;

    wxRadioButton* ncMain = nullptr,
        * ncSupp = nullptr,
        * ncVillian = nullptr,
        * ncSecon = nullptr;

public:
    amdCharacterCreator(wxWindow* parent,
        amdProjectManager* manager,
        long id = wxID_ANY,
        const wxString& label = "Create character",
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(520, 585),
        long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
        wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);

    virtual wxVector<wxString> GetValues();

    virtual void SetEdit(Element* editChar);
    virtual void DoEdit(wxCommandEvent& event);

    virtual void AddCustomAttr(wxCommandEvent& event);

    virtual void Create(wxCommandEvent& event);
    virtual void CheckClose(wxCloseEvent& event);
};


//////////////////////////////////////////////////////////////////////
////////////////////////// Location Creator //////////////////////////
//////////////////////////////////////////////////////////////////////


class amdLocationCreator : public amdElementCreator {
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

    wxStaticText* label6 = nullptr;
    wxWrapSizer* m_nlcustomSizer = nullptr;

public:
    amdLocationCreator(wxWindow* parent,
        amdProjectManager* manager,
        long id,
        const wxString& label,
        const wxPoint& pos,
        const wxSize& size,
        long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
        wxCLIP_CHILDREN | wxFRAME_SHAPED | wxFRAME_FLOAT_ON_PARENT | wxBORDER_SIMPLE);

    virtual wxVector<wxString> GetValues();

    virtual void SetEdit(Element* editLoc);
    virtual void DoEdit(wxCommandEvent& event);

    virtual void AddCustomAttr(wxCommandEvent& event);
    virtual void RemoveCustomAttr(wxCommandEvent& event);
    void RecolorCustoms();

    virtual void Create(wxCommandEvent& event);
    virtual void CheckClose(wxCloseEvent& event);
};


//////////////////////////////////////////////////////////////////////
//////////////////////////// Item Creator ////////////////////////////
//////////////////////////////////////////////////////////////////////


class amdItemCreator : public amdElementCreator {
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
    amdItemCreator(wxWindow* parent,
        amdProjectManager* manager,
        long id,
        const wxString& label,
        const wxPoint& pos,
        const wxSize& size,
        long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
        wxCLIP_CHILDREN | wxFRAME_SHAPED | wxFRAME_FLOAT_ON_PARENT | wxBORDER_SIMPLE);

    virtual wxVector<wxString> GetValues();

    virtual void SetEdit(Element* editItem);
    virtual void DoEdit(wxCommandEvent& event);

    virtual void AddCustomAttr(wxCommandEvent& event);

    virtual void Create(wxCommandEvent& event);
    virtual void CheckClose(wxCloseEvent& event);

    void OnPaint(wxPaintEvent& event);
};
#endif