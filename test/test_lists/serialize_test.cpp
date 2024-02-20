#include "serialize_test.h"

TEST_F(serialize_test, file_list_serialized1)
{

    ::testing::internal::CaptureStdout();
    update_header_set(file_list, &transfer_header, 1);

    unsigned char *serialized_data = nullptr;
    file_list_serialized(&serialized_data, &transfer_header, file_list);

    ASSERT_NE(serialized_data, nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();

    free(serialized_data);
}
TEST_F(serialize_test, file_list_serialized2)
{
    ::testing::internal::CaptureStdout();

    unsigned char *serialized_data = nullptr;
    file_list_serialized(&serialized_data, &transfer_header, nullptr);

    ASSERT_EQ(serialized_data, nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();

    free(serialized_data);
}

TEST_F(serialize_test, file_list_deserialized)
{
    ::testing::internal::CaptureStdout();
    update_header_set(file_list, &transfer_header, 1);

    unsigned char *serialized_data = nullptr;
    file_list_serialized(&serialized_data, &transfer_header, file_list);

    change_state(file_list, 0);

    unsigned char *serialized_data_ptr = NULL;
    serialized_data_ptr = serialized_data + sizeof(transfer_header_t);

    file_list_deserialized(&serialized_data_ptr, file_list, transfer_header.file_count, (char *)sync_file_path);

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        EXPECT_EQ(current_file_data->state, 0);
    }
    std::string output = ::testing::internal::GetCapturedStdout();

    free(serialized_data);
}

TEST_F(serialize_test, file_path_serialized1)
{
    ::testing::internal::CaptureStdout();
    update_header_set(file_list, &transfer_header, 2);

    unsigned char *serialized_data = nullptr;
    file_path_serialized(&serialized_data, &transfer_header, file_list);

    ASSERT_NE(serialized_data, nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();

    free(serialized_data);
}
TEST_F(serialize_test, file_path_serialized2)
{
    ::testing::internal::CaptureStdout();
    unsigned char *serialized_data = nullptr;
    file_path_serialized(&serialized_data, &transfer_header, nullptr);

    ASSERT_EQ(serialized_data, nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();

    free(serialized_data);
}
TEST_F(serialize_test, file_path_deserialized)
{
    ::testing::internal::CaptureStdout();
    update_header_set(file_list, &transfer_header, 2);

    unsigned char *serialized_data = nullptr;
    file_path_serialized(&serialized_data, &transfer_header, file_list);

    change_state(file_list, 0);

    unsigned char *serialized_data_ptr = NULL;
    serialized_data_ptr = serialized_data + sizeof(transfer_header_t);

    file_path_deserialized(file_list, &serialized_data_ptr, transfer_header.file_count);

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        EXPECT_EQ(current_file_data->state, 1);
    }
    std::string output = ::testing::internal::GetCapturedStdout();

    free(serialized_data);
}

TEST_F(serialize_test, file_serialized1)
{
    ::testing::internal::CaptureStdout();
    update_header_set(file_list, &transfer_header, 3);

    unsigned char *serialized_data = nullptr;
    file_serialized(&serialized_data, file_list, transfer_header);

    ASSERT_NE(serialized_data, nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();

    free(serialized_data);
}
TEST_F(serialize_test, file_serialized2)
{
    ::testing::internal::CaptureStdout();
    unsigned char *serialized_data = nullptr;
    file_serialized(&serialized_data, nullptr, transfer_header);

    ASSERT_EQ(serialized_data, nullptr);
    std::string output = ::testing::internal::GetCapturedStdout();

    free(serialized_data);
}

TEST_F(serialize_test, file_deserialized)
{
    ::testing::internal::CaptureStdout();
    update_header_set(file_list, &transfer_header, 3);

    unsigned char *serialized_data = nullptr;
    file_serialized(&serialized_data, file_list, transfer_header);

    unsigned char *serialized_data_ptr = NULL;
    serialized_data_ptr = serialized_data + sizeof(transfer_header_t);

    file_deserialized(&serialized_data_ptr, transfer_header.file_count, NULL);

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        EXPECT_EQ(file_path_check(current_file_data->path), 1);
    }

    std::string output = ::testing::internal::GetCapturedStdout();

    free(serialized_data);
}

TEST_F(serialize_test, update_header_set1)
{
    update_header_set(file_list, &transfer_header, 3);
    EXPECT_GT(transfer_header.total_size, 0);
}
TEST_F(serialize_test, update_header_set2)
{
    update_header_set(file_list, &transfer_header, 2);
    EXPECT_GT(transfer_header.total_size, 0);
}
TEST_F(serialize_test, update_header_set3)
{
    update_header_set(file_list, &transfer_header, 3);
    EXPECT_GT(transfer_header.total_size, 0);
}
TEST_F(serialize_test, update_header_set4)
{
    update_header_set(file_list, &transfer_header, 0);
    EXPECT_EQ(transfer_header.total_size, 0);
}