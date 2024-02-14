#include "hash_test.h"

TEST_F(hash_test, find_file_data1)
{
    ::testing::internal::CaptureStdout();

    EXPECT_NE(find_file_data(file_list, absolute_path_change("../../test_files/1.txt")), nullptr);
    EXPECT_NE(find_file_data(file_list, absolute_path_change("../../test_files/2.html")), nullptr);
    EXPECT_NE(find_file_data(file_list, absolute_path_change("../../test_files/3.exe")), nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();
}
TEST_F(hash_test, find_file_data2)
{
    ::testing::internal::CaptureStdout();

    EXPECT_EQ(find_file_data(file_list, "../../test_files/2.txt"), nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(hash_test, find_file_data3)
{
    ::testing::internal::CaptureStdout();

    EXPECT_EQ(find_file_data(NULL, "../../test_files/2.txt"), nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();
}
TEST_F(hash_test, find_file_data4)
{
    ::testing::internal::CaptureStdout();

    EXPECT_EQ(find_file_data(file_list, NULL), nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(hash_test, add_path)
{
    ::testing::internal::CaptureStdout();

    file_list_t *test_list = NULL;

    add_path(&test_list, absolute_path_change((char *)file_path[0]));
    EXPECT_NE(find_file_data(file_list, absolute_path_change((char *)file_path[0])), nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(hash_test, delete_file_data)
{
    ::testing::internal::CaptureStdout();

    delete_file_data(&file_list, (char *)file_path[0]);
    EXPECT_EQ(find_file_data(file_list, (char *)file_path[0]), nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(hash_test, clear_file_list)
{
    ::testing::internal::CaptureStdout();

    clear_file_list(&file_list);
    EXPECT_EQ(find_file_data(file_list, (char *)file_path[0]), nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(hash_test, change_state1)
{
    ::testing::internal::CaptureStdout();

    change_state(file_list, 0);
    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        EXPECT_EQ(current_file_data->state, 0);
    }

    std::string output = ::testing::internal::GetCapturedStdout();
}
TEST_F(hash_test, change_state2)
{
    ::testing::internal::CaptureStdout();

    change_state(file_list, 2);
    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        EXPECT_EQ(current_file_data->state, 2);
    }

    std::string output = ::testing::internal::GetCapturedStdout();
}
TEST_F(hash_test, change_state3)
{
    ::testing::internal::CaptureStdout();

    change_state(file_list, -1);
    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        EXPECT_EQ(current_file_data->state, -1);
    }

    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(hash_test, delete_state_clear)
{
    ::testing::internal::CaptureStdout();

    change_state(file_list, -1);
    delete_state_clear(&file_list);
    EXPECT_EQ(file_list, nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();
}