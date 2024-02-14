#pragma once
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "file_func.h"
#include "serialize_func.h"
#include "hash_table_func.h"

#define COMPRESS_TYPE 10
#define COMPRESS_BOUNDARY 0

#ifdef __cplusplus

extern "C"
{
#endif
    void serialized_data_decompress(unsigned char **serialized_data, transfer_header_t *transfer_header);
    unsigned long serialized_data_compress(unsigned char **serialized_data, transfer_header_t *transfer_header, unsigned long serialized_data_size);
#ifdef __cplusplus
}
#endif
