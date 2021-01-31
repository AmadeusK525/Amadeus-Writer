#ifndef PROJECTWIZARD_H_
#define PROJECTWIZARD_H_
#pragma once

#include <wx\wizard.h>
#include <wx\filepicker.h>
#include <wx\radiobut.h>

class amdFirstWizardPage : public wxWizardPage {
private:
	wxRadioButton* m_loadBtn = nullptr,
		* m_newBtn = nullptr;

	wxWizardPage* m_load = nullptr,
		* m_new = nullptr;

public:
	amdFirstWizardPage(wxWizard* parent, wxWizardPage* doLoad, wxWizardPage* doNew);

	virtual wxWizardPage* GetPrev() const { return nullptr; }
	virtual wxWizardPage* GetNext() const;

	bool IsLoading() { return m_loadBtn->GetValue(); }
};


class amdProjectWizard : public wxWizard {
private:
	amdFirstWizardPage* m_page1 = nullptr;

	wxDirPickerCtrl* m_dirPicker = nullptr;
	wxFilePickerCtrl* m_filePicker = nullptr;
	wxTextCtrl* m_title = nullptr;

public:
	amdProjectWizard(wxWindow* parent, wxWindowID id);

	wxWizardPage* GetFirstPage() { return m_page1; }
	wxFileName GetFileName();

	bool IsLoading() { return m_page1->IsLoading(); }
};

#endif