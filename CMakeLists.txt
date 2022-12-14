cmake_minimum_required(VERSION 3.14)

set(PROGRAM_NAME elzac)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROGRAM_NAME} LANGUAGES CXX)

FIND_PACKAGE (
  Qt6
  COMPONENTS
    Core
    Network
    LinguistTools
  REQUIRED )

FIND_PACKAGE ( ALSA REQUIRED )

set(TS_FILES
    translation/elzac_en_US.ts
)

set(SOURCE_FILES
    src/main.cpp
    src/alsa_card.cpp
    src/alsa_card_model.cpp
    src/alsa_ctl.cpp
    src/osc_client.cpp
    src/osc_message.cpp
    src/osc_server.cpp
    src/sigwatch.cpp
)

set(HEADER_FILES
    include/alsa_card.h
    include/alsa_card_model.h
    include/alsa_ctl.h
    include/osc_client.h
    include/osc_message.h
    include/osc_server.h
    include/sigwatch.h
)

include_directories(include/)

add_executable(${PROGRAM_NAME}
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${TS_FILES}
)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

install(TARGETS ${PROGRAM_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

TARGET_LINK_LIBRARIES ( ${PROGRAM_NAME}
  Qt6::Core
  Qt6::Network
  ${ALSA_LIBRARIES}
  udev
)
