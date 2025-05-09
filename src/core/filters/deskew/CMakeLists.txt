PROJECT("Deskew Filter")

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_BINARY_DIR}")

SET(
        sources
        CacheDrivenTask.cpp CacheDrivenTask.h
        Dependencies.cpp Dependencies.h
        DewarpingParams.cpp DewarpingParams.h
        DewarpingThumbnail.cpp DewarpingThumbnail.h
        DewarpingView.cpp DewarpingView.h
        DistortionType.cpp DistortionType.h
        Filter.cpp Filter.h
        ImageView.cpp ImageView.h
        NoDistortionView.cpp NoDistortionView.h
        OptionsWidget.cpp OptionsWidget.h
        OrderByPaperBendProvider.cpp OrderByPaperBendProvider.h
        OrderByCameraFovProvider.cpp OrderByCameraFovProvider.h
        OrderByDewarpedHeightProvider.cpp OrderByDewarpedHeightProvider.h
        OrderByDewarpedWidthProvider.cpp OrderByDewarpedWidthProvider.h
        OrderByDistortionTypeProvider.cpp OrderByDistortionTypeProvider.h
        Params.cpp Params.h
        PerspectiveParams.cpp PerspectiveParams.h
        RotationParams.cpp RotationParams.h
        RotationThumbnail.cpp RotationThumbnail.h
        Settings.cpp Settings.h
        DewarpThumbnailMaker.cpp DewarpThumbnailMaker.h
        ThumbnailVersionGenerator.h ThumbnailVersionGenerator.cpp
        STEX_PolylineModelShape.cpp STEX_PolylineModelShape.h
        Task.cpp Task.h
        Utils.cpp Utils.h
)
SOURCE_GROUP("Sources" FILES ${sources})
FILE(GLOB ui_files "ui/*.ui")
SOURCE_GROUP("UI Files" FILES ${ui_files})

ADD_LIBRARY(deskew STATIC ${sources} ${ui_sources})
TARGET_LINK_LIBRARIES(deskew Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml)

TRANSLATION_SOURCES(scantailor-deviant ${sources} ${ui_files})
