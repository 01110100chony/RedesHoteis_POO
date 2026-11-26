cmake_minimum_required(VERSION 3.16)
project(HotelParadise LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(FONTES_DOMINIO
    model/Pessoa.h
    model/Pessoa.cpp
    model/Atendente.h
    model/Atendente.cpp
    model/Cliente.h
    model/Cliente.cpp
    model/TiposHotel.h
    model/TiposHotel.cpp
    model/EstrategiaDesconto.h
    model/EstrategiaDesconto.cpp
    model/Reserva.h
    model/Reserva.cpp
    core/GerenciadorReservas.h
    core/GerenciadorReservas.cpp
    core/PersistenciaArquivo.h
    core/PersistenciaArquivo.cpp
)

set(FONTES_PROJETO
    main.cpp
    ${FONTES_DOMINIO}
    ui/TelaLogin.h
    ui/TelaLogin.cpp
    ui/TelaPrincipal.h
    ui/TelaPrincipal.cpp
    ui/TelaReserva.h
    ui/TelaReserva.cpp
    ui/TelaResumoReserva.h
    ui/TelaResumoReserva.cpp
    ui/TelaListagemReservas.h
    ui/TelaListagemReservas.cpp
)

add_executable(HotelParadise WIN32 ${FONTES_PROJETO})
target_include_directories(HotelParadise PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(HotelParadise PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
install(TARGETS HotelParadise RUNTIME DESTINATION bin)

add_executable(teste_regras tests/teste_regras.cpp ${FONTES_DOMINIO})
target_include_directories(teste_regras PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(teste_regras PRIVATE Qt${QT_VERSION_MAJOR}::Core)

if(MSVC)
    target_compile_options(HotelParadise PRIVATE /utf-8)
    target_compile_options(teste_regras PRIVATE /utf-8)
elseif(MINGW OR CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(HotelParadise PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
    target_compile_options(teste_regras PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()
