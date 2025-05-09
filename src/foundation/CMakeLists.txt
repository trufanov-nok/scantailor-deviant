PROJECT("Foundation library")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/core)

SET(
        sources
        NonCopyable.h IntrusivePtr.h RefCountable.h
        AlignedArray.h
        FastQueue.h
        FuzzyEquals.cpp FuzzyEquals.h
        SafeDeletingQObjectPtr.h
        ScopedIncDec.h ScopedDecInc.h
        Span.h VirtualFunction.h FlagOps.h
        AutoRemovingFile.cpp AutoRemovingFile.h
        Proximity.cpp Proximity.h
        Property.h
        PropertyFactory.cpp PropertyFactory.h
        PropertySet.cpp PropertySet.h
        PerformanceTimer.cpp PerformanceTimer.h
        QtSignalForwarder.cpp QtSignalForwarder.h
        GridLineTraverser.cpp GridLineTraverser.h
        RoundingHasher.cpp RoundingHasher.h
        StaticPool.h
        DynamicPool.h
        MultipleTargetsSupport.h
        NumericTraits.h
        CopyableByMemcpy.h
        MinMaxAccumulator.h
        VecNT.h
        VecT.h
        MatMNT.h
        MatT.h
        PriorityQueue.h
        Grid.h
        GridAccessor.h
        IndexSequence.h
        ValueConv.h
)
SOURCE_GROUP("Sources" FILES ${sources})

ADD_LIBRARY(foundation STATIC ${sources})
TARGET_LINK_LIBRARIES(foundation Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml)
