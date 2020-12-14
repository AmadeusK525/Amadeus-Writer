#include "Outline.h"

#include <boost/filesystem.hpp>

#include "MainFrame.h"
#include "Corkboard.h"
#include "OutlineFiles.h"

namespace fs = boost::filesystem;

Outline::Outline(wxWindow* parent) : wxNotebook(parent, -1) {
    corkHolder = new wxPanel(this);
    corkboard = new Corkboard(corkHolder);
    timeline = new wxPanel(this);
    files = new OutlineFilesPanel(this);

    // Hack for doing corkboard fullscreen
    corkHolderSizer = new wxBoxSizer(wxVERTICAL);
    corkHolderSizer->Add(corkboard, wxSizerFlags(1).Expand());
    corkHolder->SetSizer(corkHolderSizer);

    AddPage(corkHolder, "Corkboard");
    AddPage(timeline, "Timeline");
    AddPage(files, "Files");
}

void Outline::saveOutline(int& progress, wxProgressDialog* dialog) {
    corkboard->save();
    dialog->Update(progress++);
    files->save();
}

void Outline::loadOutline(int& progress, wxProgressDialog* dialog) {
    corkboard->load();
    files->load();
    dialog->Update(progress++);
}

void Outline::clearAll() {
    corkboard->getCanvas()->GetDiagramManager()->Clear();
    corkboard->getCanvas()->Refresh(true);

    files->clearAll();
}