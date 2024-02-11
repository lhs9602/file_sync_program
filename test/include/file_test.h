#pragma once

#include "file_func.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <fstream>

class file_test : public ::testing::Test
{
protected:
    const char *file_path[3] = {"../../test_files/1.txt", "../../test_files/2.html", "../../test_files/3.exe"};
    const char *sync_server_ip[5] = {"10.0.0.50", "10.0.0.51", "10.0.0.52", "2001:0db8:85a3:0000:0000:8a2e:0370:7334", "fe80:0000:0000:0000:0202:b3ff:fe1e:8329"};
    const char *mode = "r";
    const char *write_mode = "wb";
    const char *sync_file_path = "../../test_files/file_list.txt";
    const char *sync_server_path = "../../test_files/sync_server.txt";
    const char *test_dir = "../../test_files";
    const char *fake_path = "/home/nesusa/test_files/dgssg";
    int num_files = sizeof(file_path) / sizeof(file_path[0]);

    void SetUp() override
    {
        dir_path_create((char *)test_dir, 0);

        for (int i = 0; i < num_files; ++i)
        {
            const char *current_file_path = file_path[i];
            std::ofstream dummy_file(current_file_path);
            for (int j = 0; j < 6; ++j)
            {
                dummy_file << "This is a dummy file for testing." << i << "\n";
            }
            dummy_file.close();
        }

        std::ofstream sync_file(sync_file_path);
        for (int i = 0; i < num_files; ++i)
        {
            sync_file << file_path[i] << "\n";
        }
        sync_file.close();

        std::ofstream sync_server_file(sync_server_path);
        for (int i = 0; i < 5; ++i)
        {
            sync_server_file << sync_server_ip[i] << "\n";
        }
        sync_server_file.close();
    }

    void TearDown() override
    {
        for (int i = 0; i < num_files; ++i)
        {
            std::remove(file_path[i]);
        }
        std::remove(sync_file_path);
        std::remove(sync_server_path);
        rmdir("../../test_files/1/2/3");
        rmdir("../../test_files/1/2");
        rmdir("../../test_files/1");
        rmdir("../../test_files/4/5");
        rmdir("../../test_files/4");
        rmdir("../../test_files");
    }
};
