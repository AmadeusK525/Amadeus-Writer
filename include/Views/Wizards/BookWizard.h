#ifndef AM_BOOKWIZARD_H_
#define AM_BOOKWIZARD_H_
#pragma once

#include <wx/wx.h>
#include <wx/wizard.h>
#include <wx/dataview.h>

#include "ProjectManaging.h"
#include "Views/StoryWriter/StoryWriter.h"
#include "Utils/amUtility.h"

struct amFirstBookWizardPage : public wxWizardPage
{
	wxWizardPage* m_pNextPage = nullptr;

	wxTextCtrl* m_tcTitle = nullptr,
		* m_tcGenre = nullptr,
		* m_tcDescription = nullptr,
		* m_tcAuthor = nullptr,
		* m_tcPublisher = nullptr;

	amFirstBookWizardPage(wxWizard* parent, wxWizardPage* next);

	//am::Book* CreateBook();
	bool CanCreateBook();

	virtual wxWizardPage* GetNext() const override { return m_pNextPage; }
	virtual wxWizardPage* GetPrev() const override { return nullptr; }
};


class BookWizardTreeModel : public StoryTreeModel
{
private:
	am::Book* m_pBookToInsert = nullptr;

public:
	BookWizardTreeModel(am::Book* pBookToInsert);

	virtual void OnBeginDrag(wxDataViewEvent& event, wxDataViewCtrl* dvc) override;
	virtual void OnDropPossible(wxDataViewEvent& event, wxDataViewCtrl* dvc) override;
	virtual void OnDrop(wxDataViewEvent& event, wxDataViewCtrl* dvc) override;
};


class amBookWizard : public wxWizard
{
private:
	am::Book* m_pBookToInsert = nullptr;
	bool m_bSucceeded = false;

	amFirstBookWizardPage* m_pMainPage = nullptr;
	wxWizardPageSimple* m_pPosPage = nullptr;

#ifdef __WXMSW__
	amHotTrackingDVCHandler m_posDVCHandler;
#endif
	wxDataViewCtrl* m_pPosDVC = nullptr;
	wxObjectDataPtr<BookWizardTreeModel> m_posDVCModel;

public:
	amBookWizard(wxWindow* parent);
	virtual ~amBookWizard();

	wxWizardPage* GetFirstPage() { return m_pMainPage; }
	bool CanCreateBook();

	void OnNextButton(wxCommandEvent& event);
};

#endif