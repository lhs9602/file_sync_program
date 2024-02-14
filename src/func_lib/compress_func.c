
/**
 * @file compress_func.c
 * @brief 압축 관련 함수 라이브러리
 *
 */

#include "compress_func.h"

/**
 * @brief serialized_data_compress
 * 직렬화 데이터를 압축해제 함수
 *
 * @param unsigned char **serialized_data
 *
 * @param transfer_header_t *transfer_header
 *
 * @return void
 */
void serialized_data_decompress(unsigned char **serialized_data, transfer_header_t *transfer_header)
{
    if (NULL == serialized_data)
    {
        printf("serialized_data_decompress의 매개변수가 올바르지 않습니다.\n");
        return;
    }
    unsigned char *serialized_data_ptr = *serialized_data;
    unsigned long decompress_size = 0;

    memcpy(&decompress_size, serialized_data_ptr, sizeof(decompress_size));
    serialized_data_ptr += sizeof(decompress_size);

    z_stream strm;
    memset(&strm, 0, sizeof(z_stream));

    // z_stream 초기화
    inflateInit(&strm);

    // 원본 데이터 크기만큼 할당
    unsigned char *decompress_data = (unsigned char *)malloc(decompress_size);

    // z_stream  설정
    strm.next_in = serialized_data_ptr;
    strm.avail_in = transfer_header->total_size - sizeof(decompress_size);
    strm.next_out = decompress_data;
    strm.avail_out = decompress_size;

    // 압축 해제 작업 수행
    inflate(&strm, Z_FINISH);

    // zlib 종료 함수 호출
    inflateEnd(&strm);

    // 기존 직렬화 데이터 해제 및 압축데이터를 할당
    if (NULL != *serialized_data)
    {
        free(*serialized_data);
        *serialized_data = NULL;
    }
    *serialized_data = decompress_data;
}

/**
 * @brief serialized_data_compress
 * 직렬화 데이터를 압축하는 함수
 *
 * @param unsigned char **serialized_data
 *
 * @param transfer_header_t *transfer_header
 *
 * @param unsigned long serialized_data_size
 *
 * @return unsigned long
 */
unsigned long serialized_data_compress(unsigned char **serialized_data, transfer_header_t *transfer_header, unsigned long serialized_data_size)
{
    if (NULL == serialized_data)
    {
        printf("serialized_data_compress 매개변수가 올바르지 않습니다.\n");
        return 0;
    }
    z_stream strm;
    memset(&strm, 0, sizeof(z_stream));

    // z_stream 초기화 및 압축 레벨 설정
    deflateInit(&strm, Z_DEFAULT_COMPRESSION);

    // 압축 데이터 저장변수 할당과 압축 크기 측정
    unsigned char *compress_data = NULL;
    unsigned long compressed_data_size = compressBound(serialized_data_size);
    compress_data = (unsigned char *)malloc(compressed_data_size + sizeof(transfer_header_t) + sizeof(unsigned long));

    // 압축 데이터 포인터 설정 및 헤더만큼 이동
    unsigned char *compress_data_ptr = compress_data;
    compress_data_ptr += sizeof(transfer_header_t) + sizeof(unsigned long);

    // 직렬화 데이터 포인터 설정 및 헤더만큼 이동
    unsigned char *serialized_data_ptr = *serialized_data;
    serialized_data_ptr += sizeof(transfer_header_t);

    // z_stream 구조체 설정
    strm.next_in = serialized_data_ptr;
    strm.avail_in = serialized_data_size;
    strm.next_out = compress_data_ptr;
    strm.avail_out = compressed_data_size;

    // 압축 및 압축 데이터 크기 저장
    deflate(&strm, Z_FINISH);
    compressed_data_size = strm.total_out;

    // 헤더의 정보 수정 및 저장.
    transfer_header->data_type += COMPRESS_TYPE;
    transfer_header->total_size = compressed_data_size + sizeof(serialized_data_size);

    compress_data_ptr = compress_data;
    memcpy(compress_data_ptr, transfer_header, sizeof(transfer_header_t));

    compress_data_ptr += sizeof(transfer_header_t);
    memcpy(compress_data_ptr, &serialized_data_size, sizeof(serialized_data_size));

    printf("압축 전 크기:%lu\n", serialized_data_size);
    printf("압축 후 크기:%lu\n", compressed_data_size);
    // 기존 직렬화 데이터 해제 및 압축데이터를 할당
    if (NULL != *serialized_data)
    {
        free(*serialized_data);
        *serialized_data = NULL;
    }
    *serialized_data = compress_data;

    // zlib 종료 함수 호출
    deflateEnd(&strm);

    // 전체 데이터 크기 반환
    return compressed_data_size + sizeof(unsigned long);
}