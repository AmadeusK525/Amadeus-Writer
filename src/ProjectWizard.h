#ifndef PROJECTWIZARD_H_
#define PROJECTWIZARD_H_
#pragma once

#include <wx\wizard.h>
#include <wx\filepicker.h>

class amdFirstWizardPage : public wxWizardPage {
private:
	wxRadioBox* m_rb = nullptr;
	wxWizardPage* m_load = nullptr,
		* m_new = nullptr;

public:
	amdFirstWizardPage(wxWizard* parent, wxWizardPage* doLoad, wxWizardPage* doNew);

	virtual wxWizardPage* GetPrev() const { return nullptr; }
	virtual wxWizardPage* GetNext() const;
};


class amdProjectWizard : public wxWizard {
private:
	amdFirstWizardPage* m_page1 = nullptr;

	wxDirPickerCtrl* m_dirPicker = nullptr;
	wxFilePickerCtrl* m_filePicker = nullptr;

public:
	amdProjectWizard(wxWindow* parent, wxWindowID id);

	wxWizardPage* GetFirstPage() { return m_page1; }
	wxFileName GetFileName();
};

#endif