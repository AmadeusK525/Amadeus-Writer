#include "Outline.h"

#include "boost/filesystem.hpp"

#include "MovableNote.h"
#include "NotesBox.h"
#include "CorkboardImage.h"
#include "MainFrame.h"

namespace fs = boost::filesystem;

Outline::Outline(wxWindow* parent) : wxNotebook(parent, -1) {
    corkboard = new Corkboard(this);
    timeline = new wxPanel(this);

    AddPage(corkboard, "Corkboard");
    AddPage(timeline, "Timeline");
}

void Outline::clearAll() {
    for (unsigned int i = 0; i < corkboard->notes.size(); i++)
        corkboard->doDeleteNote(i);

    for (unsigned int i = 0; i < corkboard->images.size(); i++)
        corkboard->doDeleteImage(i);

    for (unsigned int i = 0; i < corkboard->boxes.size(); i++)
        corkboard->doDeleteBox(i);

    pnoi = 0;
}

void Outline::saveOutline(std::ofstream& out, int& progress, wxProgressDialog* dialog) {
    for (auto it : corkboard->boxes) {
        it->save(out);
        dialog->Update(progress++);
    }

    for (auto it : corkboard->notes) {
        it->save(out);
        dialog->Update(progress++);
    }

    for (int i = 0; i < pnoi; i++) {
        if (fs::exists(MainFrame::currentDocFolder + "\\Images\\Outline\\" + std::to_string(i) + ".jpg"))
            fs::remove(MainFrame::currentDocFolder + "\\Images\\Outline\\" + std::to_string(i) + ".jpg");

        if (fs::exists(MainFrame::currentDocFolder + "\\Images\\Outline\\" + std::to_string(i) + ".jpeg"))
            fs::remove(MainFrame::currentDocFolder + "\\Images\\Outline\\" + std::to_string(i) + ".jpeg");

        if (fs::exists(MainFrame::currentDocFolder + "\\Images\\Outline\\" + std::to_string(i) + ".png"))
            fs::remove(MainFrame::currentDocFolder + "\\Images\\Outline\\" + std::to_string(i) + ".png");
    }

    pnoi = noi;

    for (auto it : corkboard->images) {
        it->save(out);
        dialog->Update(progress++);
    }

}

void Outline::loadOutline(std::ifstream& in, int& progress, wxProgressDialog* dialog) {
    clearAll();

    for (int i = 0; i < nob; i++) {
        NotesBox* box = new NotesBox(corkboard->main, wxDefaultPosition, wxDefaultSize, corkboard->curBox++);
        box->load(in);
        box->initBox();
        corkboard->boxes.push_back(box);

        dialog->Update(progress++);
    }

    for (int i = 0; i < non; i++) {
        MovableNote* note = new MovableNote(corkboard->main, wxDefaultPosition, wxSize(230, 150), corkboard->curNote++);
        note->Layout();
        corkboard->notes.push_back(note);
        corkboard->notes[i]->load(in);
    }

    for (int i = 0; i < noi; i++) {
        CorkboardImage* image = new CorkboardImage(corkboard->main, wxImage(),
            wxDefaultPosition, wxDefaultSize, 1, 1, corkboard->curImage++);
        image->load(in);
        corkboard->images.push_back(image);
    }

    for (int i = 0; i < non; i++) {
        corkboard->notes[i]->initNote();
        dialog->Update(progress++);
    }

    for (int i = 0; i < noi; i++) {
        corkboard->images[i]->initImage();
        dialog->Update(progress++);
    }

    pnoi = noi;

    corkboard->main->Refresh();
    corkboard->main->Update();
}

void Outline::saveQuantities(std::ofstream& out) {
    if (out.is_open()) {
        out.write((char*)&non, sizeof(int));

        out.write((char*)&noi, sizeof(int));

        out.write((char*)&nob, sizeof(int));
    }
}

void Outline::loadQuantities(std::ifstream& in) {
    if (in.is_open()) {
        in.read((char*)&non, sizeof(int));

        in.read((char*)&noi, sizeof(int));

        in.read((char*)&nob, sizeof(int));
    }
}
