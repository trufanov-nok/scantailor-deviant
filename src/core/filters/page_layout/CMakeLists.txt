PROJECT("Page Layout Filter")

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_BINARY_DIR}")

SET(
        sources
        Utils.cpp Utils.h
        ImageView.cpp ImageView.h
        Filter.cpp Filter.h
        alignmentwidget.cpp alignmentwidget.h
        OptionsWidget.cpp OptionsWidget.h
        Task.cpp Task.h
        CacheDrivenTask.cpp CacheDrivenTask.h
        Params.cpp Params.h
        Settings.cpp Settings.h
        Thumbnail.cpp Thumbnail.h
        ApplySettingsWidget.cpp ApplySettingsWidget.h
        Alignment.cpp Alignment.h
        OrderByWidthProvider.cpp OrderByWidthProvider.h
        OrderByHeightProvider.cpp OrderByHeightProvider.h
        OrderByAlignment.cpp OrderByAlignment.h
)
SOURCE_GROUP("Sources" FILES ${sources})
FILE(GLOB ui_files "ui/*.ui")
SOURCE_GROUP("UI Files" FILES ${ui_files})

ADD_LIBRARY(page_layout STATIC ${sources} ${ui_sources})
TARGET_LINK_LIBRARIES(page_layout Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml)
TRANSLATION_SOURCES(scantailor-deviant ${sources} ${ui_files})
