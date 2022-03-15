#ifndef CORKBOARDSHAPES_H_
#define CORKBOARDSHAPES_H_
#pragma once

#include <wx/wxsf/wxShapeFramework.h>

class AutoWrapTextShape : public wxSFEditTextShape
{
private:
	wxString m_textToDraw{ "" };

	double m_topSpace = 0.0;
	int m_height = -1;

public:
	XS_DECLARE_CLONABLE_CLASS(AutoWrapTextShape);

	AutoWrapTextShape();
	AutoWrapTextShape(const AutoWrapTextShape& other);

	virtual ~AutoWrapTextShape() {}

	virtual void UpdateRectSize();
	virtual void DrawTextContent(wxDC& dc);

	/// <summary>
	/// Only works if shape is valigned to bottom!
	/// </summary>
	/// <param name="points"></param>
	inline void SetTopSpace(int points) { m_topSpace = points; }
	void SetHeight(int height);

	inline void EmtpyDrawText() { m_textToDraw.Empty(); }

	void CalcWrappedText();
	void DoCalcWrappedText(int length, int height);

	void MarkSerializableDataMembers();
};


////////////////////////////////////////////////////////////////////
//////////////////////////// NoteShape /////////////////////////////
////////////////////////////////////////////////////////////////////


class NoteShape : public wxSFRoundRectShape
{
protected:
	AutoWrapTextShape* m_content = nullptr;

public:
	wxWindowID m_currentColour;

public:
	// Enable RTTI and cloneability
	XS_DECLARE_CLONABLE_CLASS(NoteShape);

	NoteShape();
	NoteShape(const NoteShape& other);

	virtual ~NoteShape() {}

	void ChangeColour(wxWindowID id);
	void EmtpyDrawText() { m_content->EmtpyDrawText(); }

	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	virtual void OnHandle(wxSFShapeHandle& handle);
	virtual void OnEndHandle(wxSFShapeHandle& handle);

	virtual void OnEndDrag(const wxPoint& pos);
};


////////////////////////////////////////////////////////////////////
//////////////////////////// ImageShape ////////////////////////////
////////////////////////////////////////////////////////////////////


class ImageShape : public wxSFBitmapShape
{
private:
	double ratio = 99999.9;

public:
	ImageShape();
	ImageShape(const ImageShape& other);
	virtual ~ImageShape();

	void Create(const wxString& path, wxBitmapType type = wxBITMAP_TYPE_ANY);

	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	virtual void OnHandle(wxSFShapeHandle& handle);
	virtual void OnEndHandle(wxSFShapeHandle& handle);

	virtual void OnEndDrag(const wxPoint& pos);

	XS_DECLARE_CLONABLE_CLASS(ImageShape)
};


////////////////////////////////////////////////////////////////////
///////////////////////////// TextShape ////////////////////////////
////////////////////////////////////////////////////////////////////


class TextShape : public wxSFEditTextShape
{
public:
	TextShape();
	TextShape(const TextShape& other);

	virtual void OnEndHandle(wxSFShapeHandle& handle);
	virtual void OnEndDrag(const wxPoint& pos);
};


#endif