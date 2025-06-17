cmake_minimum_required(VERSION 3.18)
project(nx_hbmenu VERSION 3.6.0)

if (NOT NINTENDO_SWITCH)
  find_package(SDL2 REQUIRED)
  find_package(SDL2 CONFIG COMPONENTS SDL2main)
endif()

find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(PhysFS REQUIRED)
find_package(libjpeg-turbo REQUIRED)
find_package(libconfig REQUIRED)

add_compile_options(
    -DVERSION=\"3.6.0\"
    -iquote $ENV{DEVKITPRO}/libnx/include
)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    common/menu-list.c
    common/worker.c
    common/math.c
    common/menu.c
    common/font.c
    common/launch.c
    common/menu-entry.c
    common/theme.c
    common/message-box.c
    common/text.c
    common/ui.c
    common/status.c
    common/language.c
    common/netloader.c
    common/assets.c
)

if (NINTENDO_SWITCH)
    target_sources(${PROJECT_NAME} PRIVATE
        nx_main/nx_power.c
        nx_main/nx_netstatus.c
        nx_main/nx_audio.c
        nx_main/nx_graphics.c
        nx_main/nx_touch.c
        nx_main/main.c
        nx_main/nx_thermalstatus.c
        nx_main/nx_launch.c
        nx_main/loaders/builtin.c
    )



nx_generate_nacp (${PROJECT_NAME}.nacp
    NAME   "NX HB Menu"
    AUTHOR "SwitchBrew"
)

nx_create_nro (${PROJECT_NAME}
    NACP  ${PROJECT_NAME}.nacp
    ICON  ${CMAKE_SOURCE_DIR}/icon.jpg
    ROMFS ${CMAKE_SOURCE_DIR}/romfs
)

else()
    target_sources(${PROJECT_NAME} PRIVATE
        pc_main/main.cpp
        pc_main/pc_launch.c
        pc_main/pc_netstatus.c
        pc_main/pc_thermalstatus.c
        pc_main/pc_power.c
    )

endif()

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
endif()

if (NOT NINTENDO_SWITCH)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
  )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Freetype::Freetype
    PNG
    PhysFs
    z
    bz2
    libjpeg-turbo::turbojpeg-static
    config
)

if(NINTENDO_SWITCH)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        deko3d
    )
endif()
