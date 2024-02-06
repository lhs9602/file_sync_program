enable_testing()
find_package(GTest REQUIRED)
# 테스트 실행 파일 생성
add_executable(test_main test_main.cpp)

set_target_properties(test_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# test_lists 디렉토리 내의 테스트 파일 목록 가져오기
file(GLOB TEST_LISTS_FILES "test_lists/*.cpp")

# test_lists 디렉토리에 있는 테스트 파일을 test_main과 함께 빌드
target_sources(test_main PRIVATE ${TEST_LISTS_FILES})
target_include_directories(test_main PRIVATE . /usr/include) 

target_include_directories(test_main PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}  
)

# test_main을 실행하기 위한 테스트 추가
add_test(NAME MyTests COMMAND test_main)

# src/common_func 라이브러리와 의존성 추가
target_link_libraries(test_main PRIVATE func_lib OpenSSL::Crypto ZLIB::ZLIB)
target_link_libraries(test_main PRIVATE GTest::GTest GTest::Main)
