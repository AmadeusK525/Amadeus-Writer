#ifndef WORDPROCESSOR_H_
#define WORDPROCESSOR_H_
#pragma once

#include <wx\richtext\richtextctrl.h>

class amWordProcessor : public wxRichTextCtrl {
private:


public:
	amWordProcessor(wxWindow* parent, wxWindowID id = -1, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxRE_MULTILINE, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxASCII_STR(wxTextCtrlNameStr));
};

#endif