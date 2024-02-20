#include "serialize_func.h"
#include "file_func.h"
#include "file_test.h"
#include "hash_test.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <sys/types.h>
#include <vector>
#include <utime.h>

class serialize_test : public hash_test
{
protected:
    transfer_header_t transfer_header = {
        0,
    };
    void SetUp() override
    {
        hash_test::SetUp();
    }

    void TearDown() override
    {
        hash_test::TearDown();
    }
};