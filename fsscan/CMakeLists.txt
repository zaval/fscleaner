set(CMAKE_CXX_STANDARD 20)

add_executable(fsscan
    main.cpp
    database.cpp
    database.h
    argumentparser.cpp
    argumentparser.h
    fshelper.cpp
    fshelper.h
    threadpool.cpp
    threadpool.h
)

target_link_libraries(fsscan PRIVATE
    SQLite::SQLite3
#    PkgConfig::LIBSQLITE3
)

if(APPLE)
    set(BUNDLE_NAME $<TARGET_FILE_NAME:appfsCleaner>)
    install(TARGETS fsscan
            RUNTIME DESTINATION ${BUNDLE_NAME}.app/Contents/MacOS
    )
else()
    install(TARGETS fsscan
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()