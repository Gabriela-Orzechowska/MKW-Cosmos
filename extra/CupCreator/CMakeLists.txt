cmake_minimum_required(VERSION 3.8)
project(CosmosCupCreator)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES
        src/ImGuiBase.h
        src/ImGuiBase.cpp
        src/editor/EditorBase.h
        src/editor/EditorBase.cpp
        src/low/brlyt.h
        src/low/lyt_types.h
        src/low/lmath.h
        src/lyt/Pane.h
        src/lyt/Pane.cpp
        src/math/gMath.h
        src/math/constants.h
        src/system/File.cpp
        src/system/File.h
        src/system/FileDialogManager.cpp
        src/system/FileDialogManager.h
        src/editor/yaz.cpp
)

add_executable(CosmosCupCreator src/main.cpp
        ${SOURCE_FILES}
        src/editor/yaz.h
        src/editor/ImageLoader.cpp
        src/editor/ImageLoader.h
)

add_executable(
        CosmosCupCreator__test
        ${SOURCE_FILES}
        test/test_math.cpp
        test/main_test.cpp
)

enable_testing()

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(boost_uuid REQUIRED CONFIG)
find_package(Stb REQUIRED)

target_link_libraries(CosmosCupCreator PRIVATE glad::glad)
target_link_libraries(CosmosCupCreator PRIVATE glfw)
target_link_libraries(CosmosCupCreator PRIVATE imgui::imgui)
target_link_libraries(CosmosCupCreator PRIVATE CURL::libcurl)
target_link_libraries(CosmosCupCreator PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(CosmosCupCreator PRIVATE Boost::uuid)
target_include_directories(CosmosCupCreator PRIVATE ${Stb_INCLUDE_DIR})

target_link_libraries(CosmosCupCreator__test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(CosmosCupCreator__test PRIVATE glad::glad)
target_link_libraries(CosmosCupCreator__test PRIVATE glfw)
target_link_libraries(CosmosCupCreator__test PRIVATE imgui::imgui)