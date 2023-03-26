cmake_minimum_required(VERSION 3.10)

project(Amadeus-Writer)

add_subdirectory(sdk/wxSQLite3)
add_subdirectory(sdk/wxShapeFramework)

set(AMADEUS_INCLUDES
	"src"
	"sdk/wxSQLite3/include"
	"sdk/wxShapeFramework/include")
set(AMADEUS_SRCS
	"src/MyApp.cpp"
	"src/ProjectManaging.cpp"
	"src/Controls/DragList.cpp"
	"src/Controls/ImagePanel.cpp"
	"src/Controls/SwitchCtrl.cpp"
	"src/Elements/BookElements.cpp"
	"src/Elements/StoryElements.cpp"
	"src/Utils/amUtility.cpp"
	"src/Style/Icon.cpp"
	"src/Views/MainFrame.cpp"
	"src/Views/Elements/DocumentCreator.cpp"
	"src/Views/Elements/ElementCreators.cpp"
	"src/Views/Elements/ElementShowcases.cpp"
	"src/Views/Elements/ElementsNotebook.cpp"
	"src/Views/Outline/Outline.cpp"
	"src/Views/Outline/OutlineFiles.cpp"
	"src/Views/Outline/Corkboard/Corkboard.cpp"
	"src/Views/Outline/Corkboard/CorkboardShapes.cpp"
	"src/Views/Outline/Timeline/Timeline.cpp"
	"src/Views/Outline/Timeline/TimelineDialogs.cpp"
	"src/Views/Outline/Timeline/TimelineShapes.cpp"
	"src/Views/Overview/Overview.cpp"
	"src/Views/Release/Release.cpp"
	"src/Views/Story/StoryNotebook.cpp"
	"src/Views/StoryWriter/StoryWriter.cpp"
	"src/Views/Wizards/BookWizard.cpp"
	"src/Views/Wizards/ProjectWizard.cpp")
set(AMADEUS_RESOURCES
	"Assets/Common/AddCharacter.svg"
	"Assets/Common/AddDocument.svg"
	"Assets/Common/AddFile-light.svg"
	"Assets/Common/AddFile.svg"
	"Assets/Common/AddFolder-light.svg"
	"Assets/Common/AddFolder.svg"
	"Assets/Common/AddImage.svg"
	"Assets/Common/AddItem.svg"
	"Assets/Common/AddLocation.svg"
	"Assets/Common/AddNote.svg"
	"Assets/Common/AddText.svg"
	"Assets/Common/AlignCenter-light.svg"
	"Assets/Common/AlignCenter.svg"
	"Assets/Common/AlignCenterJust-light.svg"
	"Assets/Common/AlignCenterJust.svg"
	"Assets/Common/AlignLeft-light.svg"
	"Assets/Common/AlignLeft.svg"
	"Assets/Common/AlignRight-light.svg"
	"Assets/Common/AlignRight.svg"
	"Assets/Common/ArrowDown.svg"
	"Assets/Common/ArrowLeft.svg"
	"Assets/Common/ArrowRight.svg"
	"Assets/Common/ArrowUp.svg"
	"Assets/Common/Bold-light.svg"
	"Assets/Common/Bold.svg"
	"Assets/Common/Book.svg"
	"Assets/Common/Connection.svg"
	"Assets/Common/Cursor.svg"
	"Assets/Common/FullScreen-light.svg"
	"Assets/Common/FullScreen.svg"
	"Assets/Common/Italic-light.svg"
	"Assets/Common/Italic.svg"
	"Assets/Common/Minus.svg"
	"Assets/Common/NoteView-light.svg"
	"Assets/Common/NoteView.svg"
	"Assets/Common/Plus.svg"
	"Assets/Common/Research.svg"
	"Assets/Common/Save.svg"
	"Assets/Common/Trash.svg"
	"Assets/Common/Underline-light.svg"
	"Assets/Common/Underline.svg"
)
list(APPEND AMADEUS_SRCS ${AMADEUS_RESOURCES})

set(AMADEUS_LIBRARIES
	wxSQLite3
	wxSF)

find_package(wxWidgets COMPONENTS all REQUIRED)

if (wxWidgets_FOUND)
	include(${wxWidgets_USE_FILE})

	list(APPEND AMADEUS_INCLUDES ${wxWidgets_INCLUDE_DIRS})
	list(APPEND AMADEUS_LIBRARIES ${wxWidgets_LIBRARIES})

	add_executable(Amadeus-Writer WIN32 MACOSX_BUNDLE ${AMADEUS_SRCS})

	set_property(TARGET Amadeus-Writer PROPERTY CXX_STANDARD 20)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		MACOSX_BUNDLE TRUE
		MACOSX_FRAMEWORK_IDENTIFIER org.amadeus.Amadeus-Writer
		RESOURCE "${AMADEUS_RESOURCES}")

	target_include_directories(Amadeus-Writer PUBLIC ${AMADEUS_INCLUDES})
	target_link_libraries(Amadeus-Writer PUBLIC ${AMADEUS_LIBRARIES} ${wxWidgets_LIBRARIES})

	message(${CMAKE_CURRENT_BINARY_DIR})
endif()
