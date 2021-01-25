#ifndef NOTESHAPE_H_
#define NOTESHAPE_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include "AutoWrapTextShape.h"
#include "Corkboard.h"

class NoteShape: public wxSFRoundRectShape {
protected:
	AutoWrapTextShape* m_content = nullptr;

public:
	wxWindowID m_currentColour = CorkboardCanvas::MENU_NoteDefault;

public:
	// Enable RTTI and cloneability
	XS_DECLARE_CLONABLE_CLASS(NoteShape);

	NoteShape();
	NoteShape(const NoteShape& other);

	virtual ~NoteShape() { }

	void ChangeColour(wxWindowID id);

	void ShouldCountLines(bool count) { m_content->ShouldCountLines(count); }
	void ShouldClip(bool will) { m_content->ShouldClip(will); }
	
	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	virtual void OnHandle(wxSFShapeHandle& handle);
	virtual void OnEndHandle(wxSFShapeHandle& handle);

	virtual void OnBeginDrag(const wxPoint& pos);
	virtual void OnEndDrag(const wxPoint& pos);
};

#endif