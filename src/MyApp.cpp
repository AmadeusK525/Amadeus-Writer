#include "MyApp.h"

#include <iostream>

#include "wxmemdbg.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	m_locale.Init(wxLANGUAGE_DEFAULT);
	wxInitAllImageHandlers();

	am::SetExecutablePath(argv.GetArguments()[0]);

	wxString strProjectPath;
	if ( argc > 1 )
		strProjectPath = argv.GetArguments()[1];
	else
		strProjectPath = am::GetLastSavePath();

	if ( !am::Init(strProjectPath) )
	{
		amProjectWizard wizard(nullptr, 1234);
		return wizard.RunWizard(wizard.GetFirstPage());
	}

	return true;
}

int MyApp::OnExit()
{
	am::ShutDown();
	return wxApp::OnExit();
}

void MyApp::OnWizardFinished(const wxFileName& path)
{
	am::Init(path.GetFullPath());
}