#include "file_test.h"

// file_path_check 테스트 메서드의 정의
TEST_F(file_test, file_path_check1)
{
    ::testing::internal::CaptureStdout();

    for (int i = 0; i < num_files; ++i)
    {
        EXPECT_EQ(file_path_check((char *)file_path[i]), 1);
    }

    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(file_test, file_path_check2)
{
    ::testing::internal::CaptureStdout();

    EXPECT_EQ(file_path_check((char *)fake_path), 0);

    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(file_test, file_path_check3)
{
    ::testing::internal::CaptureStdout();

    char *fake_path = nullptr;
    EXPECT_EQ(file_path_check(fake_path), 0);

    std::string output = ::testing::internal::GetCapturedStdout();
}
// relative_path_check 테스트 메서드의 정의
TEST_F(file_test, relative_path_check1)
{
    ::testing::internal::CaptureStdout();

    for (int i = 0; i < num_files; ++i)
    {
        EXPECT_EQ(relative_path_check((char *)file_path[i]), 2);
    }
    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(file_test, relative_path_check2)
{
    EXPECT_EQ(relative_path_check((char *)"../../test_files/1.txt"), 2);
}

TEST_F(file_test, relative_path_check3)
{
    ::testing::internal::CaptureStdout();

    EXPECT_EQ(relative_path_check(nullptr), -1);

    std::string output = ::testing::internal::GetCapturedStdout();
}

// check_sum_generater 테스트 메서드의 정의
TEST_F(file_test, check_sum_generater1)
{
    ::testing::internal::CaptureStdout();

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    for (int file_path_index = 0; file_path_index < sizeof(file_path) / sizeof(file_path[0]); file_path_index++)
    {
        unsigned char check_sum[SHA256_DIGEST_LENGTH] = {0};
        int result = check_sum_generater((char *)file_path[file_path_index], check_sum, &sha256);

        ASSERT_EQ(result, 1);

        for (int file_path_index = 0; file_path_index < 3; file_path_index++)
        {

            // 실제 값의 두 번째 요소 추출
            unsigned char actual_checksum_value = check_sum[file_path_index];

            // 두 요소 비교
            EXPECT_TRUE(actual_checksum_value);
        }
    }
    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(file_test, check_sum_generater2)
{
    ::testing::internal::CaptureStdout();

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char check_sum[SHA256_DIGEST_LENGTH] = {0};
    int result = check_sum_generater((char *)fake_path, check_sum, &sha256);

    ASSERT_EQ(result, -1);
    std::string output = ::testing::internal::GetCapturedStdout();
}

// flie_name_remover 테스트 메서드의 정의
TEST_F(file_test, flie_name_remover1)
{
    ::testing::internal::CaptureStdout();
    char path[] = "../../test_files/1/2/3";

    flie_name_remover(path);
    EXPECT_STREQ(path, "../../test_files/1/2");
    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(file_test, flie_name_remover2)
{
    ::testing::internal::CaptureStdout();
    char path[] = "../../test_files/4/5/6.txt";

    flie_name_remover(path);
    EXPECT_STREQ(path, "../../test_files/4/5");
    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(file_test, flie_name_remover3)
{
    ::testing::internal::CaptureStdout();

    char *path = nullptr;

    EXPECT_EQ(flie_name_remover(path), -1);

    std::string output = ::testing::internal::GetCapturedStdout();
}

// dir_path_create 테스트 메서드의 정의
TEST_F(file_test, dir_path_create1)
{
    const char *path = "../../test_files/1/2/3";

    dir_path_create((char *)path, 0);

    ASSERT_EQ(0, access("../../test_files/1", F_OK));
    ASSERT_EQ(0, access("../../test_files/1/2", F_OK));
    ASSERT_EQ(0, access("../../test_files/1/2/3", F_OK));
}
TEST_F(file_test, dir_path_create2)
{
    const char *path = "../../test_files/4/5/6.txt";

    dir_path_create((char *)path, 1);

    ASSERT_EQ(0, access("../../test_files/4", F_OK));
    ASSERT_EQ(0, access("../../test_files/4/5", F_OK));
}
TEST_F(file_test, process_sync_file)
{
    ::testing::internal::CaptureStdout();

    file_list_t *file_list = nullptr;
    process_sync_file(&file_list, (char *)sync_file_path);
    EXPECT_NE(file_list, nullptr);

    std::string output = ::testing::internal::GetCapturedStdout();
}
TEST_F(file_test, update_check_sync_file)
{
    ::testing::internal::CaptureStdout();

    file_list_t *file_list = nullptr;
    process_sync_file(&file_list, (char *)sync_file_path);
    EXPECT_EQ(update_check_sync_file(&file_list, (char *)sync_file_path), 0);

    std::string output = ::testing::internal::GetCapturedStdout();
}

TEST_F(file_test, absolute_path_change)
{

    EXPECT_STRNE(absolute_path_change("../../test_files/1.txt"), "../../test_files/1.txt");
}