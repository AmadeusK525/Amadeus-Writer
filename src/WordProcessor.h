#ifndef WORDPROCESSOR_H_
#define WORDPROCESSOR_H_
#pragma once

#include <wx\richtext\richtextctrl.h>


/////////////////////////////////////////////////////////////////////
//////////////////////////// amWPComment ////////////////////////////
/////////////////////////////////////////////////////////////////////


class amWPCommentTag : public wxRichTextFieldType
{
	DECLARE_DYNAMIC_CLASS(amWPCommentTag)

public:
	amWPCommentTag() {}
	amWPCommentTag(const wxString& name);
	virtual ~amWPCommentTag() {}

	amWPCommentTag(const amWPCommentTag& obj) : wxRichTextFieldType(obj) { Copy(obj); }

protected:
	void Copy(const amWPCommentTag& obj) { wxRichTextFieldType::Copy(obj); }
	virtual wxRichTextFieldType* Clone() const { return new amWPCommentTag(*this); }

	virtual bool GetRangeSize(wxRichTextField* obj,
		const wxRichTextRange& range,
		wxSize& size, int& descent,
		wxDC& dc,
		wxRichTextDrawingContext& context,
		int flags,
		const wxPoint& position = wxPoint(0, 0),
		const wxSize& parentSize = wxDefaultSize,
		wxArrayInt* partialExtents = nullptr) const;

	virtual bool Layout(wxRichTextField* obj,
		wxDC& dc,
		wxRichTextDrawingContext& context,
		const wxRect& rect,
		const wxRect& parentRect,
		int style);

	virtual bool Draw(wxRichTextField* obj,
		wxDC& dc,
		wxRichTextDrawingContext& context,
		const wxRichTextRange& range,
		const wxRichTextSelection& selection,
		const wxRect& rect,
		int descent,
		int style);
};


/////////////////////////////////////////////////////////////////////
/////////////////////////// WordProcessor ///////////////////////////
/////////////////////////////////////////////////////////////////////


class amWordProcessor : public wxRichTextCtrl
{
private:


public:
	amWordProcessor(wxWindow* parent, wxWindowID id = -1, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxRE_MULTILINE, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxASCII_STR(wxTextCtrlNameStr));


	void OnChar(wxKeyEvent& event);
	void ApplyCommentToSelection();

	virtual void SetupScrollbars(bool atTop = false, bool fromOnPaint = false);

	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////////////////
//////////////////////// amSpecialDrawingHandler //////////////////////
///////////////////////////////////////////////////////////////////////


class amSpecialDrawingHandler : public wxRichTextDrawingHandler
{

};

#endif