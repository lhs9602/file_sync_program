#pragma once

#include "file_test.h"
#include "hash_table_func.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <sys/types.h>
#include <vector>
#include <utime.h>
#include <iomanip>
#include <cstdlib>

class hash_test : public file_test
{

protected:
    file_list_t *file_list = nullptr;
    void SetUp() override
    {
        ::testing::internal::CaptureStdout();

        file_test::SetUp();
        process_sync_file(&file_list, (char *)sync_file_path);
        std::string output = ::testing::internal::GetCapturedStdout();
    }

    void TearDown() override
    {
        ::testing::internal::CaptureStdout();

        file_test::TearDown();
        clear_file_list(&file_list);
        std::string output = ::testing::internal::GetCapturedStdout();
    }
};
