#include "ElementsNotebook.h"

#include "ElementCreators.h"
#include "ElementShowcases.h"

#include "MainFrame.h"
#include "Outline.h"
#include "OutlineFiles.h"

#include "MyApp.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(amdElementsNotebook, wxNotebook)

EVT_LIST_ITEM_FOCUSED(LIST_CharList, amdElementsNotebook::OnCharacterSelected)
EVT_LIST_ITEM_SELECTED(LIST_LocList, amdElementsNotebook::OnLocationSelected)

EVT_LIST_ITEM_RIGHT_CLICK(LIST_CharList, amdElementsNotebook::OnCharRightClick)
EVT_LIST_ITEM_ACTIVATED(LIST_CharList, amdElementsNotebook::OnCharacterActivated)
EVT_MENU(LISTMENU_EditChar, amdElementsNotebook::OnEditCharacter)
EVT_LIST_END_LABEL_EDIT(LIST_CharList, amdElementsNotebook::OnEditCharName)
EVT_MENU(LISTMENU_DeleteChar, amdElementsNotebook::OnDeleteCharacter)

EVT_LIST_ITEM_RIGHT_CLICK(LIST_LocList, amdElementsNotebook::OnLocRightClick)
EVT_LIST_ITEM_ACTIVATED(LIST_LocList, amdElementsNotebook::OnLocationActivated)
EVT_MENU(LISTMENU_EditLoc, amdElementsNotebook::OnEditLocation)
EVT_MENU(LISTMENU_DeleteLoc, amdElementsNotebook::OnDeleteLocation)

EVT_NOTEBOOK_PAGE_CHANGED(NOTEBOOK_THIS, amdElementsNotebook::SetSearchAC)

END_EVENT_TABLE()

wxListView* amdElementsNotebook::m_charList;
wxListView* amdElementsNotebook::m_locList;
wxListView* amdElementsNotebook::m_itemsList;

amdElementsNotebook::amdElementsNotebook(wxWindow* parent) :
    wxNotebook(parent, NOTEBOOK_THIS) {
    m_manager = amdGetManager();

    //Setting up first notebook tab with a characters list
    wxPanel* charFrame = new wxPanel(this, PANEL_Char);
    charFrame->SetBackgroundColour(wxColour(20, 20, 20));

    m_charList = new wxListView(charFrame, LIST_CharList, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
    m_charList->InsertColumn(0, "Name", wxLIST_FORMAT_CENTER, 190);
    m_charList->InsertColumn(1, "Sex", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    m_charList->InsertColumn(2, "Role", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    m_charList->InsertColumn(3, "First Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_charList->InsertColumn(4, "Last Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_charList->InsertColumn(5, "Chapters", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);

    m_charList->SetBackgroundColour(wxColour(45, 45, 45));
    m_charList->SetForegroundColour(wxColour(245, 245, 245));

    wxArrayString sortBy;
    sortBy.Add("Role");
    sortBy.Add("Name (A-Z)");
    sortBy.Add("Name (Z-A)");
    sortBy.Add("Chapters");
    sortBy.Add("First appearance");
    
    wxStaticText* cSortByLabel = new wxStaticText(charFrame, -1, "Sort by:");
    cSortByLabel->SetForegroundColour(wxColour(250, 250, 250));
    cSortByLabel->SetFont(wxFontInfo(11).Bold());

    m_cSortBy = new wxChoice(charFrame, -1, wxDefaultPosition, wxDefaultSize, sortBy);
    m_cSortBy->Bind(wxEVT_CHOICE, &amdElementsNotebook::OnCharactersSortBy, this);
    m_cSortBy->SetSelection(0);

    wxBoxSizer* cSortBySizer = new wxBoxSizer(wxHORIZONTAL);
    cSortBySizer->Add(cSortByLabel, wxSizerFlags(0).CenterVertical());
    cSortBySizer->AddSpacer(5);
    cSortBySizer->Add(m_cSortBy, wxSizerFlags(0).CenterVertical());

    wxBoxSizer* cLeftSizer = new wxBoxSizer(wxVERTICAL);
    cLeftSizer->Add(m_charList, wxSizerFlags(1).Expand());
    cLeftSizer->AddSpacer(10);
    cLeftSizer->Add(cSortBySizer, wxSizerFlags(0).Left());

    m_charShow = new CharacterShowcase(charFrame);

    wxBoxSizer* charSizer = new wxBoxSizer(wxHORIZONTAL);
    charSizer->Add(cLeftSizer, wxSizerFlags(4).Expand().Border(wxLEFT | wxTOP | wxBOTTOM, 10));
    charSizer->Add(m_charShow, 3, wxGROW |wxTOP, 10);
    charFrame->SetSizer(charSizer);

    this->AddPage(charFrame, "Characters");

    //Setting up second notebook tab with a locations list
    wxPanel* locFrame = new wxPanel(this, wxID_ANY);
    locFrame->SetBackgroundColour(wxColour(20, 20, 20));
    m_locList = new wxListView(locFrame, LIST_LocList, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
    m_locList->InsertColumn(0, "Name of location", wxLIST_FORMAT_CENTER, 125);
    m_locList->InsertColumn(1, "Importance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    m_locList->InsertColumn(2, "First Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_locList->InsertColumn(3, "First Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_locList->InsertColumn(4, "Chapters", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);

    m_locList->SetBackgroundColour(wxColour(45, 45, 45));
    m_locList->SetForegroundColour(wxColour(245, 245, 245));
    m_locList->SetMinSize(wxSize(300, 400));

    wxStaticText* lSortByLabel = new wxStaticText(locFrame, -1, "Sort by:");
    lSortByLabel->SetForegroundColour(wxColour(250, 250, 250));
    lSortByLabel->SetFont(wxFontInfo(11).Bold());

    m_lSortBy = new wxChoice(locFrame, -1, wxDefaultPosition, wxDefaultSize, sortBy);
    m_lSortBy->Bind(wxEVT_CHOICE, &amdElementsNotebook::OnLocationsSortBy, this);
    m_lSortBy->SetSelection(0);

    wxBoxSizer* lSortBySizer = new wxBoxSizer(wxHORIZONTAL);
    lSortBySizer->Add(lSortByLabel, wxSizerFlags(0).CenterVertical());
    lSortBySizer->AddSpacer(5);
    lSortBySizer->Add(m_lSortBy, wxSizerFlags(0).CenterVertical());

    wxBoxSizer* lLeftSizer = new wxBoxSizer(wxVERTICAL);
    lLeftSizer->Add(m_locList, wxSizerFlags(1).Expand());
    lLeftSizer->AddSpacer(10);
    lLeftSizer->Add(lSortBySizer, wxSizerFlags(0).Left());

    m_locShow = new LocationShowcase(locFrame);

    wxBoxSizer* locSizer = new wxBoxSizer(wxHORIZONTAL);
    locSizer->Add(lLeftSizer, wxSizerFlags(4).Expand().Border(wxLEFT | wxTOP | wxBOTTOM, 10));
    locSizer->Add(m_locShow, wxSizerFlags(5).Expand().Border(wxTOP | wxBOTTOM, 10));
    locFrame->SetSizer(locSizer);
    this->AddPage(locFrame, "Locations");

    //Setting up third notebook tab
    wxPanel* itemsFrame = new wxPanel(this, wxID_ANY);
    itemsFrame->SetBackgroundColour(wxColour(20, 20, 20));
    m_itemsList = new wxListView(itemsFrame, LIST_ItemsList, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxBORDER_NONE);
    m_itemsList->SetBackgroundColour(wxColour(45, 45, 45));
    m_itemsList->SetForegroundColour(wxColour(245, 245, 245));
    m_itemsList->InsertColumn(0, "Name of item", wxLIST_FORMAT_CENTER, 120);

    m_itemsList->SetMinSize(wxSize(300, 400));

    wxBoxSizer* itemSizer = new wxBoxSizer(wxHORIZONTAL);
    itemSizer->Add(m_itemsList, 1, wxGROW | wxALL, 10);
    itemsFrame->SetSizer(itemSizer);
    this->AddPage(itemsFrame, "Items");
}

void amdElementsNotebook::OnCharRightClick(wxListEvent& WXUNUSED(event)) {
    wxMenu menu;
    menu.Append(LISTMENU_EditChar, "&Edit");
    menu.Append(LISTMENU_DeleteChar, "&Delete");
    PopupMenu(&menu, wxDefaultPosition);
}

void amdElementsNotebook::OnEditCharacter(wxCommandEvent& WXUNUSED(event)) {
    auto it = m_manager->GetCharacters().begin();
    for (int i = 0; i < m_charList->GetFirstSelected(); i++) {
        it++;
    }

    amdCharacterCreator* edit = new amdCharacterCreator(m_manager->GetMainFrame(), m_manager, -1,
        "Create character", wxDefaultPosition, FromDIP(wxSize(650, 650)));

    edit->CenterOnParent();
    edit->SetEdit(&*it);
    edit->Show(true);

    m_manager->GetMainFrame()->Enable(false);
}

void amdElementsNotebook::OnEditCharName(wxListEvent& event) {
    auto it = m_manager->GetCharacters().begin();
    for (int i = 0; i < m_charList->GetFirstSelected(); i++) {
        it++;
    }

    it->name = event.GetLabel();
}

void amdElementsNotebook::OnDeleteCharacter(wxCommandEvent& WXUNUSED(event)) {
    long sel = m_charList->GetFirstSelected();

    wxMessageDialog deleteCheck(m_manager->GetMainFrame(), "Are you sure you want to delete '" + m_charList->GetItemText(sel) + "'?",
        "Delete character", wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
 
    if (deleteCheck.ShowModal() == wxID_YES) {
        m_charList->DeleteItem(sel);

        auto it = m_manager->GetCharacters().begin();
        for (int i = 0; i < sel; i++) {
            it++;
        }

        m_charNames.Remove(it->name);

        for (auto it2 : it->chapters)
            it2->characters.Remove(it->name);

        m_manager->GetMainFrame()->GetOutline()->GetOutlineFiles()->DeleteCharacter(*it);
        m_manager->GetCharacters().erase(it);

        m_manager->SetSaved(false);
    }
}

void amdElementsNotebook::OnCharacterActivated(wxListEvent& WXUNUSED(event)) {
    OnEditCharacter(wxCommandEvent());
}

void amdElementsNotebook::OnLocRightClick(wxListEvent& WXUNUSED(event)) {
    wxMenu menu;
    menu.Append(LISTMENU_EditLoc, "&Edit");
    menu.Append(LISTMENU_DeleteLoc, "Delete");
    PopupMenu(&menu, wxDefaultPosition);
}

void amdElementsNotebook::OnEditLocation(wxCommandEvent& WXUNUSED(event)) {

    auto it = m_manager->GetLocations().begin();
    for (int i = 0; i < m_locList->GetFirstSelected(); i++) {
        it++;
    }

    amdLocationCreator* edit = new amdLocationCreator(m_manager->GetMainFrame(), m_manager, -1,
        "Edit character - ''", wxDefaultPosition, FromDIP(wxSize(900, 650)));

    edit->CenterOnParent();
    edit->SetEdit(&*it);
    edit->Show(true);

    m_manager->GetMainFrame()->Enable(false);
}

void amdElementsNotebook::OnDeleteLocation(wxCommandEvent& WXUNUSED(event)) {
    long sel = m_locList->GetFirstSelected();

    wxMessageDialog deleteCheck(m_manager->GetMainFrame(), "Are you sure you want to delete '" + m_locList->GetItemText(sel) + "'?",
        "Delete character", wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

    if (deleteCheck.ShowModal() == wxID_YES) {
        m_locList->DeleteItem(sel);

        auto it = m_manager->GetLocations().begin();
        for (int i = 0; i < sel; i++) {
            it++;
        }

        m_locNames.Remove(it->name);

        for (auto it2 : it->chapters)
            it2->characters.Remove(it->name);

        m_manager->GetMainFrame()->GetOutline()->GetOutlineFiles()->DeleteLocation(*it);
        m_manager->GetLocations().erase(it);

        m_manager->SetSaved(false);
    }
}

void amdElementsNotebook::OnLocationActivated(wxListEvent& event) {
    OnEditLocation(wxCommandEvent());
}

void amdElementsNotebook::OnCharacterSelected(wxListEvent& event) {
    long sel = m_charList->GetFirstSelected();

    if (sel == -1) {
        m_charShow->SetData(Character());
        return;
    }

    auto it = m_manager->GetCharacters().begin();
    for (int i = 0; i < sel; i++) {
        it++;
    }
    m_charShow->SetData(*it);
}

void amdElementsNotebook::OnLocationSelected(wxListEvent& WXUNUSED(event)) {
    long sel = m_locList->GetFirstSelected();

    auto it = m_manager->GetLocations().begin();
    for (int i = 0; i < sel; i++) {
        it++;
    }

    m_locShow->SetData(*it);
}

void amdElementsNotebook::OnCharactersSortBy(wxCommandEvent& event) {
    m_cSortBy->SetSelection(event.GetInt());
    Character::cCompType = (CompType)event.GetInt();

    wxVectorSort(m_manager->GetCharacters());
    UpdateCharacterList();
}

void amdElementsNotebook::OnLocationsSortBy(wxCommandEvent& event) {
    m_lSortBy->SetSelection(event.GetInt());
    Location::lCompType = (CompType)event.GetInt();

    wxVectorSort(m_manager->GetLocations());
    UpdateLocationList();
}

void amdElementsNotebook::ClearAll() {
    m_charList->DeleteAllItems();
    m_locList->DeleteAllItems();
    m_charShow->SetData(Character());
    m_locShow->SetData(Location());
    m_charNames.clear();
    m_locNames.clear();
}

void amdElementsNotebook::UpdateCharacter(int n, Character& character) {
    m_charList->SetItem(n, 0, character.name);
    m_charList->SetItem(n, 1, character.sex);

    string role;
    switch (character.role) {
    case cProtagonist:
        role = "Protagonist";
        break;

    case cSupporting:
        role = "Supporting";
        break;

    case cVillian:
        role = "Villian";
        break;

    case cSecondary:
        role = "Secondary";
        break;

    default:
        role = "-";
    }

    m_charList->SetItem(n, 2, role);

    if (!character.chapters.IsEmpty()) {
        int first = 99999;
        int last = -1;
        for (auto& it : character.chapters) {
            if (it->position < first)
                first = it->position;

            if (it->position > last)
                last = it->position;
        }

        m_charList->SetItem(n, 3, wxString("Chapter ") << first);
        m_charList->SetItem(n, 4, wxString("Chapter ") << last);
    } else {
        m_charList->SetItem(n, 3, "-");
        m_charList->SetItem(n, 4, "-");
    }

    m_charList->SetItem(n, 5, std::to_string(character.chapters.Count()));
}

void amdElementsNotebook::UpdateLocation(int n, Location& location) {
    m_locList->SetItem(n, 0, location.name);

    string role("");
    switch (location.role) {
    case lHigh:
        role = "Main";
        break;

    case lLow:
        role = "Secondary";
        break;

    default:
        role = "-";
    }

    m_locList->SetItem(n, 1, role);

    if (!location.chapters.IsEmpty()) {
        int first = 999999;
        int last = -1;

        for (auto& it : location.chapters) {
            if (it->position < first)
                first = it->position;
        
            if (it->position > last)
                last = it->position;
        }

        m_locList->SetItem(n, 2, wxString("Chapter ") << first);
        m_locList->SetItem(n, 3, wxString("Chapter ") << last);
    } else {
        m_locList->SetItem(n, 2, "-");
        m_locList->SetItem(n, 3, "-");
    }
    m_locList->SetItem(n, 4, std::to_string(location.chapters.Count()));
}

void amdElementsNotebook::UpdateCharacterList() {
    m_charList->Freeze();

    int i = 0;
    int tlsize = m_charList->GetItemCount();
    int mfsize = m_manager->GetCharacters().size();
    int dif;

    if (tlsize > mfsize) {
        dif = tlsize - mfsize;
        for (int j = 0; j < dif; j++)
            m_charList->DeleteItem(mfsize - j + 1);
    } else if (mfsize > tlsize) {
        dif = mfsize - tlsize;
        for (int j = 0; j < dif; j++)
            m_charList->InsertItem(0, "");
    }

    for (auto& it : m_manager->GetCharacters())
        UpdateCharacter(i++, it);

    m_charList->Thaw();
}

void amdElementsNotebook::UpdateLocationList() {
    m_locList->Freeze();

    int i = 0;
    int tlsize = m_locList->GetItemCount();
    int mfsize = m_manager->GetLocations().size();
    int dif;

    if (tlsize > mfsize) {
        dif = tlsize - mfsize;
        for (int j = 0; j < dif; j++)
            m_locList->DeleteItem(mfsize - j + 1);
    } else if (mfsize > tlsize) {
        dif = mfsize - tlsize;
        for (int j = 0; j < dif; j++)
            m_locList->InsertItem(0, "");
    }

    for (auto& it : m_manager->GetLocations())
        UpdateLocation(i++, it);

    m_locList->Thaw();
}

void amdElementsNotebook::UpdateAll() {
    UpdateCharacterList();
    UpdateLocationList();
}

void amdElementsNotebook::SetSearchAC(wxBookCtrlEvent& WXUNUSED(event)) {
    int sel = this->GetSelection();

    switch (sel) {
    case 0:
        m_searchBar->AutoComplete(m_charNames);
        m_searchBar->SetDescriptiveText("Search character");
        break;

    case 1:
        m_searchBar->AutoComplete(m_locNames);
        m_searchBar->SetDescriptiveText("Search location");
        break;

    case 2:
        m_searchBar->AutoComplete(wxArrayString());
        m_searchBar->SetDescriptiveText("Search item");
        break;

    case 3:
        m_searchBar->AutoComplete(wxArrayString());
        m_searchBar->SetDescriptiveText("Search scene");
        break;
    }

    m_searchBar->Clear();
}

void amdElementsNotebook::AddCharName(string& name) {
    m_charNames.Add(name);
}

void amdElementsNotebook::AddLocName(string& name) {
    m_locNames.Add(name);
}

void amdElementsNotebook::RemoveCharacterName(string& name) {
    m_charNames.Remove(name);
}

void amdElementsNotebook::RemoveLocationName(string& name) {
    m_locNames.Remove(name);
}
