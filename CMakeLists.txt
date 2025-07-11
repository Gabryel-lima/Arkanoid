cmake_minimum_required(VERSION 3.15)
project(arkanoid C)

# Localize a instalação do raylib (ou use add_subdirectory se o código estiver incluído)
find_package(raylib 5.0 REQUIRED)   # adapta-se à versão disponível

add_executable(arkanoid src/main.c)

target_link_libraries(arkanoid PRIVATE raylib)

# Incluir caminho para headers se for instalação não-padrão
# target_include_directories(arkanoid PRIVATE /caminho/do/raylib/include) 