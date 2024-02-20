#pragma once

#include "serialize_test.h"
#include "compress_func.h"
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

class compress_test : public serialize_test
{

protected:
    void SetUp() override
    {

        serialize_test::SetUp();
    }

    void TearDown() override
    {
        serialize_test::TearDown();
    }
};
