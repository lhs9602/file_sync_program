# 대상 실행 파일 및 소스 파일 설정
add_executable(server server.c)

set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
# 필요시 라이브러리 링크
target_link_libraries(server PRIVATE func_lib OpenSSL::Crypto  pthread ZLIB::ZLIB)

# 헤더 파일 포함 디렉토리 설정
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
