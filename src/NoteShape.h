#ifndef NOTESHAPE_H_
#define NOTESHAPE_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include "AutoWrapTextShape.h"

class NoteShape: public wxSFRoundRectShape {
protected:
	AutoWrapTextShape* content = nullptr;

public:
	// Enable RTTI and cloneability
	XS_DECLARE_CLONABLE_CLASS(NoteShape);

	NoteShape();
	NoteShape(static NoteShape& other);

	virtual ~NoteShape() { }

	void willCountLines(bool count) { content->willCountLines(count); }
	void willClip(bool will) { content->willClip(will); }
	
	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	virtual void OnHandle(wxSFShapeHandle& handle);
	virtual void OnEndHandle(wxSFShapeHandle& handle);

	virtual void OnBeginDrag(const wxPoint& pos);
	virtual void OnEndDrag(const wxPoint& pos);
};

#endif