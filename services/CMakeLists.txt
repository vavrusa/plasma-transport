find_package(KDE4 REQUIRED)
add_definitions (${KDE4_DEFINITIONS})

install(FILES cz_vlak.js
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma_engine_transport/services/)
