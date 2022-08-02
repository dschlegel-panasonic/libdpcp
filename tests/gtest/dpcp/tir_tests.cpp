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

#include "common/def.h"
#include "common/log.h"
#include "common/sys.h"
#include "common/base.h"

#include "dpcp_base.h"

using namespace dpcp;

class dpcp_tir : public dpcp_base {};

/**
 * @test dpcp_tir.ti_01_Constructor
 * @brief
 *    Check constructor
 * @details
 */
TEST_F(dpcp_tir, ti_01_Constructor)
{
    status ret = DPCP_OK;
    adapter* adapter_obj = OpenAdapter();
    ASSERT_NE(nullptr, adapter_obj);

    tir tir_obj(adapter_obj->get_ctx());

    uintptr_t handle = 0;
    ret = tir_obj.get_handle(handle);
    ASSERT_EQ(DPCP_ERR_CREATE, ret);
    ASSERT_EQ(0, handle);

    uint32_t id = 0;
    ret = tir_obj.get_id(id);
    ASSERT_EQ(DPCP_ERR_INVALID_ID, ret);
    ASSERT_EQ(0, id);

    delete adapter_obj;
}

/**
 * @test dpcp_tir.ti_02_create
 * @brief
 *    Check tir::create method
 * @details
 *
 */
TEST_F(dpcp_tir, ti_02_create)
{
    adapter* adapter_obj = OpenAdapter();
    ASSERT_NE(nullptr, adapter_obj);

    status ret = adapter_obj->open();
    ASSERT_EQ(DPCP_OK, ret);

    uint32_t tdn = adapter_obj->get_td();
    ASSERT_NE(0, tdn);

    striding_rq* srq_obj = open_str_rq(adapter_obj, m_rqp);
    ASSERT_NE(nullptr, srq_obj);

    uint32_t rqn = 0;
    ret = srq_obj->get_id(rqn);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(0, rqn);
    log_trace("tdn: 0x%x rqn: 0x%x\n", tdn, rqn);

    tir tir_obj(adapter_obj->get_ctx());

    ret = tir_obj.create(tdn, rqn);
    ASSERT_EQ(DPCP_OK, ret);

    delete srq_obj;
    delete adapter_obj;
}

/**
 * @test dpcp_tir.ti_03_create_by_attr
 * @brief
 *    Check tir::create method
 * @details
 *
 */
TEST_F(dpcp_tir, ti_03_create_by_attr)
{
    adapter* adapter_obj = OpenAdapter();
    ASSERT_NE(nullptr, adapter_obj);

    status ret = adapter_obj->open();
    ASSERT_EQ(DPCP_OK, ret);

    uint32_t tdn = adapter_obj->get_td();
    ASSERT_NE(0, tdn);

    striding_rq* srq_obj = open_str_rq(adapter_obj, m_rqp);
    ASSERT_NE(nullptr, srq_obj);

    uint32_t rqn = 0;
    ret = srq_obj->get_id(rqn);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(0, rqn);
    log_trace("tdn: 0x%x rqn: 0x%x\n", tdn, rqn);

    tir tir_obj(adapter_obj->get_ctx());

    struct tir::attr tir_attr;
    memset(&tir_attr, 0, sizeof(tir_attr));
    tir_attr.flags = TIR_ATTR_INLINE_RQN | TIR_ATTR_TRANSPORT_DOMAIN;
    tir_attr.inline_rqn = rqn;
    tir_attr.transport_domain = tdn;
    ret = tir_obj.create(tir_attr);
    ASSERT_EQ(DPCP_OK, ret);

    delete srq_obj;
    delete adapter_obj;
}

/**
 * @test dpcp_tir.ti_04_create_twice
 * @brief
 *    tir can not be created more than once
 * @details
 *
 */
TEST_F(dpcp_tir, ti_04_create_twice)
{
    adapter* adapter_obj = OpenAdapter();
    ASSERT_NE(nullptr, adapter_obj);

    status ret = adapter_obj->open();
    ASSERT_EQ(DPCP_OK, ret);

    uint32_t tdn = adapter_obj->get_td();
    ASSERT_NE(0, tdn);

    striding_rq* srq_obj = open_str_rq(adapter_obj, m_rqp);
    ASSERT_NE(nullptr, srq_obj);

    uint32_t rqn = 0;
    ret = srq_obj->get_id(rqn);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(0, rqn);
    log_trace("tdn: 0x%x rqn: 0x%x\n", tdn, rqn);

    tir tir_obj(adapter_obj->get_ctx());

    struct tir::attr tir_attr;
    memset(&tir_attr, 0, sizeof(tir_attr));
    tir_attr.flags = TIR_ATTR_INLINE_RQN | TIR_ATTR_TRANSPORT_DOMAIN;
    tir_attr.inline_rqn = rqn;
    tir_attr.transport_domain = tdn;
    ret = tir_obj.create(tir_attr);
    ASSERT_EQ(DPCP_OK, ret);

    ret = tir_obj.create(tir_attr);
    ASSERT_EQ(DPCP_ERR_INVALID_PARAM, ret);

    delete srq_obj;
    delete adapter_obj;
}

/**
 * @test dpcp_tir.ti_05_create_invalid
 * @brief
 *    Check tir creation with invalid parameters
 * @details
 *
 */
TEST_F(dpcp_tir, ti_05_create_invalid)
{
    adapter* adapter_obj = OpenAdapter();
    ASSERT_NE(nullptr, adapter_obj);

    status ret = adapter_obj->open();
    ASSERT_EQ(DPCP_OK, ret);

    tir tir_obj(adapter_obj->get_ctx());

    struct tir::attr tir_attr;
    memset(&tir_attr, 0, sizeof(tir_attr));
    ret = tir_obj.create(tir_attr);
    ASSERT_EQ(DPCP_ERR_CREATE, ret);

    tir_attr.flags = TIR_ATTR_INLINE_RQN;
    tir_attr.inline_rqn = 0xFF;
    ret = tir_obj.create(tir_attr);
    ASSERT_EQ(DPCP_ERR_CREATE, ret);

    delete adapter_obj;
}

/**
 * @test dpcp_tir.ti_06_query
 * @brief
 *    Check tir::query method
 * @details
 *
 */
TEST_F(dpcp_tir, ti_06_query)
{
    adapter* adapter_obj = OpenAdapter();
    ASSERT_NE(nullptr, adapter_obj);

    status ret = adapter_obj->open();
    ASSERT_EQ(DPCP_OK, ret);

    uint32_t tdn = adapter_obj->get_td();
    ASSERT_NE(0, tdn);

    striding_rq* srq_obj = open_str_rq(adapter_obj, m_rqp);
    ASSERT_NE(nullptr, srq_obj);

    uint32_t rqn = 0;
    ret = srq_obj->get_id(rqn);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(0, rqn);
    log_trace("tdn: 0x%x rqn: 0x%x\n", tdn, rqn);

    tir tir_obj(adapter_obj->get_ctx());

    struct tir::attr tir_attr;
    memset(&tir_attr, 0, sizeof(tir_attr));
    tir_attr.flags = TIR_ATTR_INLINE_RQN | TIR_ATTR_TRANSPORT_DOMAIN;
    tir_attr.inline_rqn = rqn;
    tir_attr.transport_domain = tdn;
    ret = tir_obj.create(tir_attr);
    ASSERT_EQ(DPCP_OK, ret);

    memset(&tir_attr, 0, sizeof(tir_attr));
    ret = tir_obj.query(tir_attr);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_TRUE((tir_attr.flags & TIR_ATTR_INLINE_RQN) != 0);
    ASSERT_EQ(rqn, tir_attr.inline_rqn);
    ASSERT_TRUE((tir_attr.flags & TIR_ATTR_TRANSPORT_DOMAIN) != 0);
    ASSERT_EQ(tdn, tir_attr.transport_domain);

    delete srq_obj;
    delete adapter_obj;
}

/**
 * @test dpcp_tir.ti_07_modify
 * @brief
 *    Check tir::modify method
 * @details
 *
 */
TEST_F(dpcp_tir, ti_07_modify)
{
    adapter* adapter_obj = OpenAdapter();
    ASSERT_NE(nullptr, adapter_obj);

    status ret = adapter_obj->open();
    ASSERT_EQ(DPCP_OK, ret);

    uint32_t tdn = adapter_obj->get_td();
    ASSERT_NE(0, tdn);

    striding_rq* srq_obj = open_str_rq(adapter_obj, m_rqp);
    ASSERT_NE(nullptr, srq_obj);

    uint32_t rqn = 0;
    ret = srq_obj->get_id(rqn);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(0, rqn);
    log_trace("tdn: 0x%x rqn: 0x%x\n", tdn, rqn);

    tir tir_obj(adapter_obj->get_ctx());

    struct tir::attr tir_attr;
    memset(&tir_attr, 0, sizeof(tir_attr));
    tir_attr.flags = TIR_ATTR_INLINE_RQN | TIR_ATTR_TRANSPORT_DOMAIN;
    tir_attr.inline_rqn = rqn;
    tir_attr.transport_domain = tdn;
    ret = tir_obj.create(tir_attr);
    ASSERT_EQ(DPCP_OK, ret);

    memset(&tir_attr, 0, sizeof(tir_attr));
    ret = tir_obj.query(tir_attr);
    ASSERT_EQ(DPCP_OK, ret);

    adapter_hca_capabilities caps;
    ret = adapter_obj->get_hca_capabilities(caps);
    ASSERT_EQ(DPCP_OK, ret);

    if (caps.lro_cap) {
        uint32_t enable_mask_original = tir_attr.lro.enable_mask;
        tir_attr.flags = TIR_ATTR_LRO;
        tir_attr.lro.enable_mask = (enable_mask_original ? 0 : 1);
        ret = tir_obj.modify(tir_attr);
        ASSERT_EQ(DPCP_OK, ret);

        memset(&tir_attr, 0, sizeof(tir_attr));
        ret = tir_obj.query(tir_attr);
        ASSERT_EQ(DPCP_OK, ret);
        ASSERT_TRUE((tir_attr.flags & TIR_ATTR_LRO) != 0U);
        ASSERT_NE(enable_mask_original, tir_attr.lro.enable_mask);
    }

    delete srq_obj;
    delete adapter_obj;
}

/**
 * @test dpcp_tir.ti_08_create_tls
 * @brief
 *    Check tir::create with tls
 * @details
 *
 */
TEST_F(dpcp_tir, ti_08_create_tls)
{
    std::unique_ptr<adapter> adapter_obj(OpenAdapter());
    ASSERT_NE(nullptr, adapter_obj);

    status ret = adapter_obj->open();
    ASSERT_EQ(DPCP_OK, ret);

    uint32_t tdn = adapter_obj->get_td();
    ASSERT_NE(0, tdn);

    std::unique_ptr<striding_rq> srq_obj(open_str_rq(adapter_obj.get(), m_rqp));
    ASSERT_NE(nullptr, srq_obj);

    uint32_t rqn = 0;
    ret = srq_obj->get_id(rqn);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(0, rqn);
    log_trace("tdn: 0x%x rqn: 0x%x\n", tdn, rqn);

    tir tir_obj(adapter_obj->get_ctx());

    struct tir::attr tir_attr;
    memset(&tir_attr, 0, sizeof(tir_attr));
    tir_attr.flags = TIR_ATTR_INLINE_RQN | TIR_ATTR_TRANSPORT_DOMAIN;
    tir_attr.inline_rqn = rqn;
    tir_attr.transport_domain = tdn;

    adapter_hca_capabilities caps;
    ret = adapter_obj->get_hca_capabilities(caps);
    ASSERT_EQ(DPCP_OK, ret);

    if (caps.tls_rx) {
        tir_attr.flags |= TIR_ATTR_TLS;
        tir_attr.tls_en = 1U;
        log_trace("TLS-RX supported\n");
    } else {
        log_info("TLS-RX NOT supported\n");
        return;
    }

    ret = tir_obj.create(tir_attr);
    ASSERT_EQ(DPCP_OK, ret);

    memset(&tir_attr, 0, sizeof(tir_attr));
    ret = tir_obj.query(tir_attr);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(tir_attr.tls_en, 1);

    if (caps.tls_rx && caps.lro_cap) {
        uint32_t enable_mask_original = tir_attr.lro.enable_mask;
        tir_attr.flags = TIR_ATTR_LRO;
        tir_attr.lro.enable_mask = (enable_mask_original ? 0 : 1);
        tir_attr.lro.max_msg_sz = (32768U >> 8U);
        tir_attr.lro.timeout_period_usecs = 32U;
        ret = tir_obj.modify(tir_attr);
        ASSERT_EQ(DPCP_OK, ret);

        memset(&tir_attr, 0, sizeof(tir_attr));
        ret = tir_obj.query(tir_attr);
        ASSERT_EQ(DPCP_OK, ret);
        ASSERT_TRUE((tir_attr.flags & TIR_ATTR_LRO) != 0U);
        ASSERT_NE(enable_mask_original, tir_attr.lro.enable_mask);
        ASSERT_EQ(tir_attr.tls_en, 1);
    }
}
