cmake_minimum_required(VERSION 4.1)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(AffichageSynchrone)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Téléchargement de Raylib
include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# 3. Téléchargement de Raygui
set(RAYGUI_HEADER_PATH "libs/raygui/raygui.h")
if(NOT EXISTS "${RAYGUI_HEADER_PATH}")
    message(STATUS "Téléchargement de raygui.h (version 4.0) depuis GitHub...")
    file(DOWNLOAD
            "https://raw.githubusercontent.com/raysan5/raygui/4.0/src/raygui.h"
            "${RAYGUI_HEADER_PATH}"
    )
endif()

# ==========================================
# FICHIERS DU PROJET
# ==========================================

set(SOURCE_FILES
        src/main.cpp
        src/Vue/V_Master.cpp
        src/Modele/M_ExpediteurUDP_W.cpp
        src/Modele/M_VideoComplexe.cpp
        src/Modele/M_ServeurTFTP_W.cpp
        src/Modele/M_SessionLecture.cpp
        src/Controleur/C_LecteurPhysiqueLocal.cpp
        src/Modele/M_LecteurPhysique.cpp
)

set(HEADER_FILES
        include/Vue/V_Master.h
        include/Modele/M_ExpediteurUDP_W.h
        include/Modele/M_VideoComplexe.h
        include/Modele/M_ServeurTFTP_W.h
        include/Modele/M_SessionLecture.h
        include/Controleur/C_LecteurPhysiqueLocal.h
        include/Modele/M_LecteurPhysique.h
)

add_executable(AffichageSynchrone ${SOURCE_FILES} ${HEADER_FILES})

target_compile_definitions(AffichageSynchrone PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    NOGDI
    NOUSER
)

# ==========================================
# INCLUSIONS ET LIENS
# ==========================================

target_include_directories(AffichageSynchrone PRIVATE
        include
        "libs/LibVLC/include"
        "libs/LibVLC/include/vlc"
        "${CMAKE_SOURCE_DIR}/libs/FFTW"
)

target_link_directories(AffichageSynchrone PRIVATE "libs/LibVLC/lib")

target_link_libraries(AffichageSynchrone PRIVATE
        raylib
        winmm
        gdi32
        ws2_32
        wsock32
        libvlc
        libvlccore
        ${CMAKE_SOURCE_DIR}/libs/FFTW/libfftw3-3.dll
)

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