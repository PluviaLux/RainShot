add_executable(RainShot
    ${SPECTACLE_SRCS}
    Main.cpp
    MainWindow.cpp

    MainWindow.h

    MainWindow.ui
)

target_link_libraries(RainShot PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
