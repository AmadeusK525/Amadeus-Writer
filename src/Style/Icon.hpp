#ifndef ICON_H_
#define ICON_H_

#pragma once

#include <wx/bitmap.h>

enum class IconName
{
    AddCharacter,
    AddDocument,
    AddFile,
    AddFileLight,
    AddFolder,
    AddFolderLight,
    AddImage,
    AddItem,
    AddLocation,
    AddNote,
    AddText,
    AlignCenter,
    AlignCenterLight,
    AlignCenterJust,
    AlignCenterJustLight,
    AlignLeft,
    AlignLeftLight,
    AlignRight,
    AlignRightLight,
    ArrowDown,
    ArrowLeft,
    ArrowRight,
    ArrowUp,
    Bold,
    BoldLight,
    Book,
    Connection,
    Cursor,
    FullScreen,
    FullScreenLight,
    Italic,
    ItalicLight,
    Minus,
    NoteView,
    NoteViewLight,
    Plus,
    Research,
    Save,
    Trash,
    Underline,
    UnderlineLight
};

namespace Style {
    namespace Icon {
        wxBitmapBundle GetBmp(IconName iconName, const wxSize& size);
    }
}

#endif // ICON_H_
