#include "amUtility.h"

#include "MyApp.h"
#include "MainFrame.h"

#ifdef __WXMSW__
wxDataViewItemAttr amTreeModelNode::m_hoverAttr{};
#endif

void am::DoSubWindowFullscreen(wxWindow* window, wxWindow* originalParent, bool fs,
	wxBoxSizer* originalSizer, wxSizerFlags flags, int sizerIndex)
{
	amMainFrame* mainFrame = am::GetMainFrame();
	wxPanel* mainPanel = mainFrame->GetMainPanel();
	wxBoxSizer* holderSizer = mainFrame->GetHolderSizer();

	if ( fs )
	{
		if ( originalSizer )
			originalSizer->Detach(window);

		window->Reparent(mainFrame);

		holderSizer->Replace(mainPanel, window);
		mainFrame->ShowFullScreen(true);

	}
	else
	{
		holderSizer->Replace(window, mainPanel);
		window->Reparent(originalParent);

		if ( originalSizer )
			originalSizer->Insert(sizerIndex, window, flags);

		if ( !mainFrame->IsFrameFullScreen() )
			mainFrame->ShowFullScreen(false);
	}

	mainPanel->Show(!fs);
	mainFrame->Layout();
	if ( originalSizer )
		originalSizer->Layout();
}
