# Project
project(plasma_transport)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (    ${QT_DEFINITIONS}
                     ${KDE4_DEFINITIONS}
                     )
include_directories( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${KDE4_INCLUDES}
                     )

# Source files
set(project_src transport.cpp service.cpp)

# Services
add_subdirectory(services)

# UI
kde4_add_ui_files(project_src config.ui)

# Plasma plugin
kde4_add_plugin(plasma_applet_transport ${project_src})
target_link_libraries(plasma_applet_transport ${KDE4_KDEUI_LIBS} ${QT_QTNETWORK_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_PLASMA_LIBS})

# Install paths
install(TARGETS plasma_applet_transport
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasma-applet-transport.desktop
        DESTINATION  ${SERVICES_INSTALL_DIR})

