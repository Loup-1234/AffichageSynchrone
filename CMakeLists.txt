cmake_minimum_required(VERSION 3.20)
project(AffichageSynchrone CXX C)

# ==========================================
# CONFIGURATION GLOBALE
# ==========================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GOOGLE TEST
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# ==========================================
# SOURCES ET EN-TÊTES
# ==========================================
set(SOURCE_FILES
        src/main.cpp
        src/Vue/V_Master.cpp
        src/Controleur/C_LecteurPhysiqueLocal.cpp
        src/Modele/M_VideoComplexe.cpp
        src/Modele/M_TFTP.cpp
        src/Modele/M_SessionLecture.cpp
        src/Modele/M_LecteurPhysique.cpp
        src/Modele/M_BDD.cpp
        src/Modele/M_ConfigReseau.cpp
        src/Modele/M_UDP.cpp
)

set(HEADER_FILES
        include/Vue/V_Master.h
        include/Controleur/C_LecteurPhysiqueLocal.h
        include/Modele/M_VideoComplexe.h
        include/Modele/M_TFTP.h
        include/Modele/M_SessionLecture.h
        include/Modele/M_LecteurPhysique.h
        include/Modele/M_BDD.h
        include/Modele/M_ConfigReseau.h
        include/Modele/M_UDP.h
)

# ==========================================
# DÉPENDANCES COMMUNES
# ==========================================
# SQLite3 (Compilation statique)
add_library(sqlite_lib OBJECT libs/sqlite3/sqlite3.c)
target_include_directories(sqlite_lib PUBLIC libs/sqlite3)

# Raygui (Téléchargement dynamique si manquant)
set(RAYGUI_HEADER_PATH "${CMAKE_SOURCE_DIR}/libs/raygui/raygui.h")
if(NOT EXISTS "${RAYGUI_HEADER_PATH}")
    message(STATUS "Téléchargement de raygui.h...")
    file(DOWNLOAD
            "https://raw.githubusercontent.com/raysan5/raygui/4.0/src/raygui.h"
            "${RAYGUI_HEADER_PATH}"
    )
endif()

# ==========================================
# CIBLE PRINCIPALE (EXÉCUTABLE)
# ==========================================
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# GOOGLE TEST
add_executable(Gtest
        test/M_SessionLecture_Test.cpp
        test/M_VideoComplexe_Test.cpp
        test/M_UDP_Expediteur_Test.cpp
        src/Vue/V_Master.cpp
        src/Controleur/C_LecteurPhysiqueLocal.cpp
        src/Modele/M_VideoComplexe.cpp
        src/Modele/M_TFTP.cpp
        src/Modele/M_SessionLecture.cpp
        src/Modele/M_LecteurPhysique.cpp
        src/Modele/M_BDD.cpp
        src/Modele/M_ConfigReseau.cpp
        src/Modele/M_UDP.cpp)
target_link_libraries(Gtest PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(Gtest)

# Inclusions globales du projet
target_include_directories(${PROJECT_NAME} PRIVATE
        include
        "libs/raygui"
)

target_include_directories(Gtest PRIVATE
        include
        "libs/raygui"
)

# ==========================================
# GESTION DES OS & LINKING (Windows / Unix)
# ==========================================
if(WIN32)
    message(STATUS "Configuration pour Windows détectée")
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

    # Raylib via FetchContent
    include(FetchContent)
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)

    # Remplacement de add_definitions par la méthode moderne liée à la target
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            WIN32_LEAN_AND_MEAN NOMINMAX NOGDI NOUSER
    )

    # Inclusions spécifiques Windows
    target_include_directories(${PROJECT_NAME} PRIVATE
            "libs/LibVLC/include"
            "libs/LibVLC/include/vlc"
            "libs/FFTW"
    )
    target_link_directories(${PROJECT_NAME} PRIVATE "libs/LibVLC/lib")

    # Linking Windows
    target_link_libraries(${PROJECT_NAME} PRIVATE
            raylib
            winmm
            gdi32
            ws2_32
            wsock32
            libvlc
            libvlccore
            iphlpapi
            sqlite_lib
            "${CMAKE_SOURCE_DIR}/libs/FFTW/libfftw3-3.dll"
    )

    target_link_options(${PROJECT_NAME} PRIVATE
            "-static-libgcc"
            "-static-libstdc++"
            "-static"
            "-Wl,--allow-multiple-definition"
    )

    # GOOGLE TEST
    # Remplacement de add_definitions par la méthode moderne liée à la target
    target_compile_definitions(Gtest PRIVATE
            WIN32_LEAN_AND_MEAN NOMINMAX NOGDI NOUSER
    )

    # Inclusions spécifiques Windows
    target_include_directories(Gtest PRIVATE
            "libs/LibVLC/include"
            "libs/LibVLC/include/vlc"
            "libs/FFTW"
    )
    target_link_directories(Gtest PRIVATE "libs/LibVLC/lib")

    # Linking Windows
    target_link_libraries(Gtest PRIVATE
            raylib
            winmm
            gdi32
            ws2_32
            wsock32
            libvlc
            libvlccore
            iphlpapi
            sqlite_lib
            "${CMAKE_SOURCE_DIR}/libs/FFTW/libfftw3-3.dll"
    )

    target_link_options(Gtest PRIVATE
            "-static-libgcc"
            "-static-libstdc++"
            "-static"
            "-Wl,--allow-multiple-definition"
    )

elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuration pour Linux détectée")

    # Dépendances système via PkgConfig
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED IMPORTED_TARGET raylib)
    pkg_check_modules(VLC REQUIRED IMPORTED_TARGET libvlc)
    pkg_check_modules(FFTW REQUIRED IMPORTED_TARGET fftw3)

    # Linking Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE
            PkgConfig::RAYLIB
            PkgConfig::VLC
            PkgConfig::FFTW
            sqlite_lib
            m
            pthread
    )

    # GOOGLE TEST
    target_link_libraries(Gtest PRIVATE
            PkgConfig::RAYLIB
            PkgConfig::VLC
            PkgConfig::FFTW
            sqlite_lib
            m
            pthread
    )
endif()

# ==========================================
# DOCUMENTATION (Doxygen & Sphinx)
# ==========================================
find_package(Doxygen)

if(DOXYGEN_FOUND)
    # 1. Génération XML (Doxygen)
    add_custom_target(doxygen_generate
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
            COMMENT "Génération du XML Doxygen..."
    )

    # 2. Génération HTML (Sphinx)
    add_custom_target(sphinx_generate
            COMMAND sphinx-build -b html source build/html
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
            COMMENT "Génération du HTML Sphinx..."
            DEPENDS doxygen_generate
    )

    # Organisation dans l'IDE
    set_target_properties(doxygen_generate sphinx_generate PROPERTIES FOLDER "Documentation")
endif()