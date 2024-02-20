#include "compress_test.h"

TEST_F(compress_test, serialized_data_compress1)
{
    ::testing::internal::CaptureStdout();
    update_header_set(file_list, &transfer_header, 3);

    unsigned char *serialized_data = nullptr;
    file_serialized(&serialized_data, file_list, transfer_header);

    unsigned long serialized_data_size = transfer_header.total_size;
    transfer_header.total_size = serialized_data_compress(&serialized_data, &transfer_header, transfer_header.total_size);

    ASSERT_LT(transfer_header.total_size, serialized_data_size);

    std::string output = ::testing::internal::GetCapturedStdout();
    free(serialized_data);
}
TEST_F(compress_test, serialized_data_compress2)
{
    ::testing::internal::CaptureStdout();

    transfer_header.total_size = serialized_data_compress(nullptr, &transfer_header, transfer_header.total_size);

    ASSERT_EQ(transfer_header.total_size, 0);

    std::string output = ::testing::internal::GetCapturedStdout();
}
TEST_F(compress_test, serialized_data_compress)
{
    ::testing::internal::CaptureStdout();
    update_header_set(file_list, &transfer_header, 3);

    unsigned char *serialized_data = nullptr;
    file_serialized(&serialized_data, file_list, transfer_header);

    unsigned long serialized_data_size = transfer_header.total_size;
    transfer_header.total_size = serialized_data_compress(&serialized_data, &transfer_header, transfer_header.total_size);

    unsigned char *decompress_data = (unsigned char *)malloc(transfer_header.total_size);

    memcpy(decompress_data, serialized_data + sizeof(transfer_header_t), transfer_header.total_size);

    serialized_data_decompress(&decompress_data, &transfer_header);

    file_deserialized(&decompress_data, transfer_header.file_count, NULL);

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        EXPECT_EQ(file_path_check(current_file_data->path), 1);
    }
    std::string output = ::testing::internal::GetCapturedStdout();
    free(serialized_data);
    free(decompress_data);
}