#include "Views/Wizards/ProjectWizard.h"
#include "MyApp.h"

#include <wx/wx.h>

#include "Utils/wxmemdbg.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../Assets/OSX/Amadeus.xpm"
#endif

amProjectWizard::amProjectWizard(wxWindow* parent, wxWindowID id)
{
	SetExtraStyle(wxWIZARD_EX_HELPBUTTON);
	Create(parent, id, "Project Wizard", wxNullBitmap, wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	m_btnNext->Bind(wxEVT_BUTTON, &amProjectWizard::OnNextBtn, this);

	SetIcon(wxICON(amadeus));

	SetBitmapPlacement(wxWIZARD_VALIGN_CENTRE);
	//SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

	wxWizardPageSimple* page2 = new wxWizardPageSimple(this);
	m_pLoadPage = new wxWizardPageSimple(this);
	wxWizardPageSimple* page4 = new wxWizardPageSimple(this);
	m_page1 = new amFirstWizardPage(this, page2, m_pLoadPage);

	wxStaticText* label = new wxStaticText(page2, -1, _("Please select a folder where\nyour project will be located:"));
	label->SetForegroundColour(wxColour(255, 255, 255));
	label->SetFont(wxFontInfo(10).Bold());
	m_dirPicker = new wxDirPickerCtrl(page2, -1, "", "Select a folder", wxDefaultPosition,
		wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL | wxDIRP_DIR_MUST_EXIST);

	wxStaticText* label2 = new wxStaticText(m_pLoadPage, -1, _("Please select an .amp file for\nloading your project:"));
	label2->SetForegroundColour(wxColour(255, 255, 255));
	label2->SetFont(wxFontInfo(10).Bold());
	m_filePicker = new wxFilePickerCtrl(m_pLoadPage, -1, "", _("Select a file"), _("Amadeus Project files (*.amp)|*.amp"),
		wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_SMALL | wxFLP_FILE_MUST_EXIST);

	wxStaticText* label3 = new wxStaticText(page4, -1, "Project Title:");
	label3->SetForegroundColour(wxColour(255, 255, 255));
	label3->SetFont(wxFontInfo(11).Bold());
	m_title = new wxTextCtrl(page4, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE | wxTE_CENTRE);
	m_title->SetBackgroundColour(wxColour(20, 20, 20));
	m_title->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* siz2 = new wxBoxSizer(wxVERTICAL);
	siz2->Add(label, wxSizerFlags(0).CenterHorizontal());
	siz2->AddSpacer(5);
	siz2->Add(m_dirPicker, wxSizerFlags(0).Expand());
	siz2->AddStretchSpacer(1);
	page2->SetSizerAndFit(siz2);

	wxBoxSizer* siz3 = new wxBoxSizer(wxVERTICAL);
	siz3->Add(label2, wxSizerFlags(0).CenterHorizontal());
	siz3->AddSpacer(5);
	siz3->Add(m_filePicker, wxSizerFlags(0).Expand());
	siz3->AddStretchSpacer(1);
	m_pLoadPage->SetSizerAndFit(siz3);

	wxBoxSizer* siz4 = new wxBoxSizer(wxVERTICAL);
	siz4->Add(label3, wxSizerFlags(0).CenterHorizontal());
	siz4->AddSpacer(5);
	siz4->Add(m_title, wxSizerFlags(0).Expand());
	siz4->AddStretchSpacer(1);
	page4->SetSizerAndFit(siz4);

	page2->SetNext(page4);
	m_pLoadPage->SetNext(nullptr);
	page4->SetNext(nullptr);
	page2->SetPrev(m_page1);
	m_pLoadPage->SetPrev(m_page1);
	page4->SetPrev(page2);

	GetPageAreaSizer()->Add(m_page1);

	SetPageSize(wxSize(300, 300));
	SetBackgroundColour(wxColour(40, 40, 40));
}

wxFileName amProjectWizard::GetFileName()
{
	if ( IsLoading() )
		return m_filePicker->GetFileName();
	else
	{
		wxFileName fileName(m_dirPicker->GetPath() << wxFileName::GetPathSeparator());
		fileName.SetFullName(m_title->GetValue() + ".amp");

		return fileName;
	}
}

void amProjectWizard::OnNextBtn(wxCommandEvent& event)
{
	if ( !GetCurrentPage()->GetNext() )
	{
		wxFileName fileName(GetFileName());

		if ( (IsLoading() && (!wxFileName::Exists(fileName.GetFullPath()) || fileName.GetExt() != "amp")) 
			|| !wxFileName::Exists(fileName.GetPath()) )
		{ 
			wxMessageBox("Path to .amp file isn't valid! Plase chose a valid path.");
			return;
		}
		else
		{
			wxGetApp().OnWizardFinished(fileName);
		}
	}

	event.Skip();
}


////////////////////////////////////////////////////////////////////////
//////////////////////////// FirstWizardPage ///////////////////////////
////////////////////////////////////////////////////////////////////////


amFirstWizardPage::amFirstWizardPage(wxWizard* parent, wxWizardPage* doLoad, wxWizardPage* doNew) :
	wxWizardPage(parent)
{

	m_load = doLoad;
	m_new = doNew;

	wxStaticText* label1 = new wxStaticText(this, -1, _("Would you like to load an existing\nproject or start a new one?"));
	label1->SetForegroundColour(wxColour(255, 255, 255));
	label1->SetFont(wxFontInfo(10).Bold());

	wxArrayString choices;
	choices.Add(_("Load project."));
	choices.Add(_("New project."));

	m_loadBtn = new wxRadioButton(this, -1, _("Load project"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_loadBtn->SetForegroundColour(wxColour(255, 255, 255));
	m_loadBtn->SetFont(wxFontInfo(9).Bold());
	m_newBtn = new wxRadioButton(this, -1, _("New project"));
	m_newBtn->SetForegroundColour(wxColour(255, 255, 255));
	m_newBtn->SetFont(wxFontInfo(9).Bold());

	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
	btnSizer->Add(m_loadBtn, wxSizerFlags(0));
	btnSizer->AddSpacer(10);
	btnSizer->Add(m_newBtn, wxSizerFlags(0));

	wxBoxSizer* siz1 = new wxBoxSizer(wxVERTICAL);
	siz1->AddSpacer(10);
	siz1->Add(label1, wxSizerFlags(0).Expand());
	siz1->AddSpacer(10);
	siz1->Add(btnSizer, wxSizerFlags(0).Expand());
	siz1->AddStretchSpacer(1);

	SetSizerAndFit(siz1);
}

wxWizardPage* amFirstWizardPage::GetNext() const
{
	return m_loadBtn->GetValue() ? m_new : m_load;
}
