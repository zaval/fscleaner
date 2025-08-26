add_library(core
    SHARED
        helper.h
        helper.cpp
        hashcalculator.h
        hashcalculator.cpp
        mediafileinfo.cpp
        mediafileinfo.h
        mediainfoextractor.cpp
        mediainfoextractor.h
)

target_include_directories(core PRIVATE ${LIBAV_INCLUDE_DIRS})

target_link_libraries(core
    PRIVATE
        Qt6::Core
        PkgConfig::LIBAV
)


if(APPLE)
    set(BUNDLE_NAME $<TARGET_FILE_NAME:appfsCleaner>)
    install(TARGETS core
            LIBRARY DESTINATION ${BUNDLE_NAME}.app/Contents/Frameworks
    )
else()
    install(TARGETS core
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()