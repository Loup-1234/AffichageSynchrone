cmake_minimum_required(VERSION 4.1)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(AffichageSynchrone)

set(CMAKE_CXX_STANDARD 23)

# ==========================================
# DÉPENDANCES COMMUNES (FetchContent)
# ==========================================
include(FetchContent)

# Téléchargement de Google Test
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)


# ==========================================
# CONFIGURATION PAR SYSTÈME D'EXPLOITATION
# ==========================================
if (WIN32) # CONFIGURATION WINDOWS

    message(STATUS "Configuration des dépendances pour Windows...")
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)

    # Téléchargement de Raylib
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)

    # Téléchargement de Raygui
    set(RAYGUI_HEADER_PATH "include/raygui/raygui.h")
    if(NOT EXISTS "${RAYGUI_HEADER_PATH}")
        message(STATUS "Téléchargement de raygui.h (version 4.0) depuis GitHub...")
        file(DOWNLOAD
                "https://raw.githubusercontent.com/raysan5/raygui/4.0/src/raygui.h"
                "${RAYGUI_HEADER_PATH}"
        )
    endif()

else () # CONFIGURATION LINUX

    message(STATUS "Configuration des dépendances pour Linux...")

    find_package(PkgConfig REQUIRED)

    # Recherche des paquets système
    pkg_check_modules(FFTW REQUIRED IMPORTED_TARGET fftw3)
    pkg_check_modules(LIBVLC REQUIRED libvlc)

    find_package(raylib CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)

endif ()


# ==========================================
# FICHIERS DU PROJET
# ==========================================
set(SOURCE_FILES
        src/main.cpp
        include/Master/Master.cpp
        include/ExpediteurUDP_W/ExpediteurUDP_W.cpp
        include/VideoComplexe/VideoComplexe.cpp
)

set(HEADER_FILES
        include/Master/Master.h
        include/ExpediteurUDP_W/ExpediteurUDP_W.h
        include/VideoComplexe/VideoComplexe.h
)


# ==========================================
# CIBLES DE COMPILATION (Exécutables)
# ==========================================

# 1. Application Principale
add_executable(AffichageSynchrone ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(AffichageSynchrone PRIVATE include)

# 2. Tests Unitaires (Google Test)
add_executable(Gtest test/TestXcorr.cpp include/VideoComplexe/VideoComplexe.cpp)
target_link_libraries(Gtest PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(Gtest)


# ==========================================
# ÉDITION DES LIENS (Linker)
# ==========================================
if (WIN32) # WINDOWS

    # Inclusions spécifiques Windows
    target_include_directories(AffichageSynchrone PRIVATE "include/LibVLC/include" "${CMAKE_SOURCE_DIR}/include/FFTW")
    target_include_directories(Gtest PRIVATE "${CMAKE_SOURCE_DIR}/include/FFTW")

    target_link_directories(AffichageSynchrone PRIVATE "include/LibVLC/lib")

    # Liens pour l'application principale
    target_link_libraries(AffichageSynchrone PRIVATE
            raylib
            winmm
            gdi32
            ws2_32
            libvlc
            libvlccore
            ${CMAKE_SOURCE_DIR}/include/FFTW/libfftw3-3.dll
    )

    # Liens pour les tests unitaires
    target_link_libraries(Gtest PRIVATE ${CMAKE_SOURCE_DIR}/include/FFTW/libfftw3-3.dll)

else () # LINUX

    # Inclusions spécifiques Linux
    target_include_directories(AffichageSynchrone PRIVATE ${LIBVLC_INCLUDE_DIRS})

    # Liens pour l'application principale
    target_link_libraries(AffichageSynchrone PRIVATE
            raylib
            ${LIBVLC_LIBRARIES}
            PkgConfig::FFTW
            glfw
            OpenGL::GL
            Threads::Threads
            m
            dl
            rt
    )

    # Liens pour les tests unitaires
    target_link_libraries(Gtest PRIVATE PkgConfig::FFTW)

endif ()


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