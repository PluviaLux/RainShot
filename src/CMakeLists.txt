add_executable(RainShot
    ${SPECTACLE_SRCS}
    Main.cpp
    RWidget.cpp

    RWidget.h
)

target_link_libraries(RainShot
    Qt::Widgets
)
