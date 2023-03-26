project(wxSF)

set(WXSF_INCLUDES
	"include"
	"src")
set(WXSF_SRCS
	"src/ArrowBase.cpp"
	"src/AutoLayout.cpp"
	"src/BitmapShape.cpp"
	"src/BitmapShapeXml.cpp"
	"src/CanvasHistory.cpp"
	"src/CanvasState.cpp"
	"src/CircleArrow.cpp"
	"src/CircleShape.cpp"
	"src/CommonFcn.cpp"
	"src/ControlShape.cpp"
	"src/CurveShape.cpp"
	"src/DiagramManager.cpp"
	"src/DiamondArrow.cpp"
	"src/DiamondShape.cpp"
	"src/EditTextShape.cpp"
	"src/EllipseShape.cpp"
	"src/FixedRectShape.cpp"
	"src/FlexGridShape.cpp"
	"src/GUI.fbp"
	"src/GridShape.cpp"
	"src/LineShape.cpp"
	"src/MultiSelRect.cpp"
	"src/OpenArrow.cpp"
	"src/OrthoShape.cpp"
	"src/Printout.cpp"
	"src/PolygonShape.cpp"
	"src/PolygonShapeXml.cpp"
	"src/RectShape.cpp"
	"src/RoundRectShape.cpp"
	"src/RoundOrthoShape.cpp"
	"src/SFEvents.cpp"
	"src/ScaledDC.cpp"
	"src/ShapeBase.cpp"
	"src/SolidArrow.cpp"
	"src/ShapeCanvas.cpp"
	"src/ShapeHandle.cpp"
	"src/ShapeDockpoint.cpp"
	"src/ShapeBaseXml.cpp"
	"src/ShapeDataObject.cpp"
	"src/Thumbnail.cpp"
	"src/TextShape.cpp"
	"src/TextShapeXml.cpp"
	"src/wxxmlserializer/PropertyIO.cpp"
	"src/wxxmlserializer/XmlSerializer.cpp")

find_package(wxWidgets COMPONENTS all REQUIRED)

if (wxWidgets_FOUND)
	include(${wxWidgets_USE_FILE})

	list(APPEND WXSF_INCLUDES ${wxWidgets_INCLUDE_DIRS})
	set(WXSF_LIBRARIES ${wxWidgets_LIBRARIES})

	add_library(wxSF ${WXSF_SRCS})

	target_include_directories(wxSF PUBLIC ${WXSF_INCLUDES})
	target_link_libraries(wxSF PRIVATE ${WXSF_LIBRARIES})
endif()
