# 현재 디렉토리의 모든 소스 파일 가져오기
file(GLOB COMMON_FUNC_SOURCES *.c)

# 대상 라이브러리 및 소스 파일 설정
add_library(func_lib ${COMMON_FUNC_SOURCES})

# 헤더 파일 포함 디렉토리 설정
target_include_directories(func_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}  
)