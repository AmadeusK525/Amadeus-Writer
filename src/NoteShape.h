#ifndef NOTESHAPE_H_
#define NOTESHAPE_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include "AutoWrapTextShape.h"
#include "Corkboard.h"

class NoteShape: public wxSFRoundRectShape {
protected:
	AutoWrapTextShape* content = nullptr;

public:
	wxWindowID curColour = CorkboardCanvas::MENU_NoteDefault;

public:
	// Enable RTTI and cloneability
	XS_DECLARE_CLONABLE_CLASS(NoteShape);

	NoteShape();
	NoteShape(const NoteShape& other);

	virtual ~NoteShape() { }

	void changeColour(wxWindowID id);

	void willCountLines(bool count) { content->willCountLines(count); }
	void willClip(bool will) { content->willClip(will); }
	
	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	virtual void OnHandle(wxSFShapeHandle& handle);
	virtual void OnEndHandle(wxSFShapeHandle& handle);

	virtual void OnBeginDrag(const wxPoint& pos);
	virtual void OnEndDrag(const wxPoint& pos);
};

#endif