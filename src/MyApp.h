#ifndef MYAPP_H_
#define MYAPP_H_

#pragma once

#include <wx/wx.h>

#include "ProjectManaging.h"
#include "ProjectWizard.h"

class MyApp : public wxApp
{
private:
	amProjectWizard* m_wizard = nullptr;
	wxLocale m_locale{};

public:
	virtual bool OnInit();
	virtual int OnExit();

	void OnWizardFinished(const wxFileName& path);
};

wxDECLARE_APP(MyApp);
#endif
