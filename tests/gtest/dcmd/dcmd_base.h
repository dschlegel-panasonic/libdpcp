/*
 * Copyright © 2020-2022 NVIDIA CORPORATION & AFFILIATES. ALL RIGHTS RESERVED.
 *
 * This software product is a proprietary product of Nvidia Corporation and its affiliates
 * (the "Company") and all right, title, and interest in and to the software
 * product, including all associated intellectual property rights, are and
 * shall remain exclusively with the Company.
 *
 * This software product is governed by the End User License Agreement
 * provided with the software product.
 */

#ifndef TESTS_GTEST_DCMD_BASE_H_
#define TESTS_GTEST_DCMD_BASE_H_

#include "src/dcmd/dcmd.h"

/**
 * DCMD Base class for tests
 */
class dcmd_base : public testing::Test, public test_base {
protected:
    virtual void SetUp();
    virtual void TearDown();
};

#endif /* TESTS_GTEST_DCMD_BASE_H_ */
