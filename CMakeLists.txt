cmake_minimum_required(VERSION 3.20)
project(AffichageSynchrone CXX)

# Configuration C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")

# Gestion des Dépendances (Spécifique à l'OS)
if(WIN32)
    message(STATUS "Configuration pour Windows détectée")

    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

    # 1. Téléchargement de Raylib (Windows n'a pas de gestionnaire de paquets natif)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)

    # Définitions spécifiques pour Windows
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX -DNOGDI -DNOUSER)

elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuration pour Linux détectée")

    # 1. Utilisation de PkgConfig pour trouver les libs système (pacman/paru)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED IMPORTED_TARGET raylib)
    pkg_check_modules(VLC REQUIRED IMPORTED_TARGET libvlc)
    pkg_check_modules(FFTW REQUIRED IMPORTED_TARGET fftw3)
endif()

# Raygui (Header-only, commun aux deux)
set(RAYGUI_HEADER_PATH "${CMAKE_SOURCE_DIR}/libs/raygui/raygui.h")
if(NOT EXISTS "${RAYGUI_HEADER_PATH}")
    message(STATUS "Téléchargement de raygui.h...")
    file(DOWNLOAD
            "https://raw.githubusercontent.com/raysan5/raygui/4.0/src/raygui.h"
            "${RAYGUI_HEADER_PATH}"
    )
endif()

# Fichiers du Projet
set(SOURCE_FILES
        src/main.cpp
        src/Vue/V_Master.cpp
        src/Modele/M_ExpediteurUDP.cpp
        src/Modele/M_VideoComplexe.cpp
        src/Modele/M_ServeurTFTP.cpp
        src/Modele/M_SessionLecture.cpp
        src/Controleur/C_LecteurPhysiqueLocal.cpp
        src/Modele/M_LecteurPhysique.cpp
        src/Modele/M_DecouverteReseau.cpp
        src/Modele/M_ConfigReseau.cpp
        src/Modele/M_JsonUtil.cpp
        src/Modele/M_ReceveurUDP.cpp
        src/Modele/M_TFTP_W.cpp
)

set(HEADER_FILES
        include/Vue/V_Master.h
        include/Modele/M_ExpediteurUDP.h
        include/Modele/M_VideoComplexe.h
        include/Modele/M_ServeurTFTP.h
        include/Modele/M_SessionLecture.h
        include/Controleur/C_LecteurPhysiqueLocal.h
        include/Modele/M_LecteurPhysique.h
        include/Modele/M_DecouverteReseau.h
        include/Modele/M_ConfigReseau.h
        include/Modele/M_JsonUtil.h
        include/Modele/M_ReceveurUDP.h
        include/Modele/M_ProtocoleReseau.h
        include/Modele/M_TFTP_W.h
)

# Création de l'Exécutable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Inclusions et Liens
target_include_directories(${PROJECT_NAME} PRIVATE
        include
        "libs/raygui"
)

if(WIN32)
    target_include_directories(${PROJECT_NAME} PRIVATE
            "libs/LibVLC/include"
            "libs/LibVLC/include/vlc"
            "libs/FFTW"
    )
    target_link_directories(${PROJECT_NAME} PRIVATE "libs/LibVLC/lib")

    target_link_libraries(${PROJECT_NAME} PRIVATE
            raylib
            winmm
            gdi32
            ws2_32
            wsock32
            libvlc
            libvlccore
            iphlpapi
            "${CMAKE_SOURCE_DIR}/libs/FFTW/libfftw3-3.dll"
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
            PkgConfig::RAYLIB
            PkgConfig::VLC
            PkgConfig::FFTW
            m
            pthread
    )
endif()

# ==========================================
# DOCUMENTATION (Doxygen & Sphinx)
# ==========================================

find_package(Doxygen)

if (DOXYGEN_FOUND)
    # Commande pour exécuter Doxygen
    add_custom_target(doxygen_generate
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
            COMMENT "Génération du XML Doxygen..."
    )

    # Commande pour exécuter Sphinx
    add_custom_target(sphinx_generate
            COMMAND sphinx-build -b html source build/html
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
            COMMENT "Génération du HTML Sphinx..."
            DEPENDS doxygen_generate # Sphinx attend que Doxygen ait fini
    )

    # Grouper dans "Documentation"
    set_target_properties(doxygen_generate sphinx_generate PROPERTIES FOLDER "Documentation")

endif ()