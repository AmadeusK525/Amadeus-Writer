#include "ProjectWizard.h"

#include <wx\wx.h>
#include <wx\radiobox.h>

amdProjectWizard::amdProjectWizard(wxWindow* parent, wxWindowID id) {
	SetExtraStyle(wxWIZARD_EX_HELPBUTTON);
	Create(parent, id, "Project Wizard", wxNullBitmap, wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	SetIcon(wxICON(amadeus));

	SetBitmapPlacement(wxWIZARD_VALIGN_CENTRE);
	//SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

	wxWizardPageSimple* page2 = new wxWizardPageSimple(this);
	wxWizardPageSimple* page3 = new wxWizardPageSimple(this);
	m_page1 = new amdFirstWizardPage(this, page2, page3);

	wxStaticText* label = new wxStaticText(page2, -1, "Please select a folder where\nyour project will be located:");
	label->SetForegroundColour(wxColour(255, 255, 255));
	label->SetFont(wxFontInfo(10).Bold());
	m_dirPicker = new wxDirPickerCtrl(page2, -1, "", "Select a folder", wxDefaultPosition,
		wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_SMALL | wxDIRP_DIR_MUST_EXIST);

	wxStaticText* label2 = new wxStaticText(page3, -1, "Please select an .amp file for\nloading your project:");
	label2->SetForegroundColour(wxColour(255, 255, 255));
	label2->SetFont(wxFontInfo(10).Bold());
	m_filePicker = new wxFilePickerCtrl(page3, -1, "", "Select a file", "Amadeus Project files (*.amp)|*.amp",
		wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_SMALL | wxFLP_FILE_MUST_EXIST);

	wxBoxSizer* siz2 = new wxBoxSizer(wxVERTICAL);
	siz2->Add(label, wxSizerFlags(0).Expand());
	siz2->AddSpacer(5);
	siz2->Add(m_dirPicker, wxSizerFlags(0));
	siz2->AddStretchSpacer(1);
	page2->SetSizerAndFit(siz2);

	wxBoxSizer* siz3 = new wxBoxSizer(wxVERTICAL);
	siz3->Add(label2, wxSizerFlags(0).Expand());
	siz3->AddSpacer(5);
	siz3->Add(m_filePicker, wxSizerFlags(0));
	siz3->AddStretchSpacer(1);
	page3->SetSizerAndFit(siz3);

	page2->SetNext(nullptr);
	page3->SetNext(nullptr);
	page2->SetPrev(m_page1);
	page3->SetPrev(m_page1);

	GetPageAreaSizer()->Add(m_page1);

	SetBackgroundColour(wxColour(40, 40, 40));
}

wxFileName amdProjectWizard::GetFileName() {
	return m_dirPicker->GetDirName();
}



amdFirstWizardPage::amdFirstWizardPage(wxWizard* parent, wxWizardPage* doLoad, wxWizardPage* doNew) :
	wxWizardPage(parent) {

	m_load = doLoad;
	m_new = doNew;

	wxStaticText* label1 = new wxStaticText(this, -1, _("Would you like to load an existing\nproject or start a new one?"));
	label1->SetForegroundColour(wxColour(255, 255, 255));
	label1->SetFont(wxFontInfo(10).Bold());

	wxArrayString choices;
	choices.Add(_("Load project."));
	choices.Add(_("New project."));

	m_rb = new wxRadioBox(this, -1, _("Choose an option and hit next"),
		wxDefaultPosition, wxDefaultSize, choices);
	m_rb->SetForegroundColour(wxColour(255, 255, 255));
	m_rb->GetChildren()[0]->SetForegroundColour(wxColour(255, 255, 255));
	m_rb->GetChildren()[1
	]->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* siz1 = new wxBoxSizer(wxVERTICAL);
	siz1->AddSpacer(10);
	siz1->Add(label1, wxSizerFlags(0).Expand());
	siz1->AddSpacer(10);
	siz1->Add(m_rb, wxSizerFlags(0).Expand());
	siz1->AddStretchSpacer(1);

	SetSizerAndFit(siz1);
}

wxWizardPage* amdFirstWizardPage::GetNext() const {
	return m_rb->GetSelection() ? m_load : m_new;
}