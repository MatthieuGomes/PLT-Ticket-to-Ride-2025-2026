cmake_minimum_required(VERSION 3.16)
project(cards_test LANGUAGES CXX)

# ✅ Activer C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Inclure le dossier courant pour les headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Récupérer automatiquement tous les fichiers .cpp
file(GLOB SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Créer l’exécutable principal
add_executable(cards_test ${SRC_FILES})
