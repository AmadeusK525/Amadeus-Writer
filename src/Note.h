#ifndef NOTE_H_
#define NOTE_H_
#pragma once

#include <wx\wx.h>

struct Note {
	wxString content{ "" };
	wxString name{ "" };

	bool isDone = false;

	Note(wxString content, wxString name);
};

#endif