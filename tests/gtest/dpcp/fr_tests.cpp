/*
Copyright (C) Mellanox Technologies, Ltd. 2020. ALL RIGHTS RESERVED.

This software product is a proprietary product of Mellanox Technologies, Ltd.
(the "Company") and all right, title, and interest in and to the software
product, including all associated intellectual property rights, are and shall
remain exclusively with the Company. All rights in or to the software product
are licensed, not sold. All rights not licensed are reserved.

This software product is governed by the End User License Agreement provided
with the software product.
*/

#include "common/def.h"
#include "common/log.h"
#include "common/sys.h"
#include "common/base.h"
#include "common/cmn.h"

#include "dpcp_base.h"

using namespace dpcp;

class dpcp_fr : /*public obj,*/ public dpcp_base {
public:
    static adapter* s_ad;
    static flow_rule* s_fr3t;
    static flow_rule* s_fr4t;

protected:
    match_params m_mask3 = { {}, 0xFFFF, 0, 0xFFFFFFFF, 0, 0xFFFF, 0, 0xFF, 0xF }; // DMAC, DST_IP, PORT, Protocol, IP Version
    match_params m_mask4 = { {}, 0xFFFF, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF, 0, 0xFF, 0xF }; // DMAC, DST_IP, SRC_IP, PORT, Protocol, IP Version
    rq_params m_rqp;

    void SetUp() override
    {
        if (errno) {
            log_trace("dpcp_rq::SetUp errno= %d\n", errno);
            errno = EOK;
        }
        if (nullptr == s_ad) {
            s_ad = OpenAdapter();
            ASSERT_NE(nullptr, s_ad);
        }
        if (nullptr == s_fr3t) {
            s_fr3t = new (std::nothrow) flow_rule(s_ad->get_ctx(), 10, m_mask3);
        }
        if (nullptr == s_fr4t) {
            s_fr4t = new (std::nothrow) flow_rule(s_ad->get_ctx(), 10, m_mask4);
        }
        m_rqp = {{2048, 16384, 0, 0}, 4, 0};

        m_rqp.wqe_sz = m_rqp.rq_at.buf_stride_num * m_rqp.rq_at.buf_stride_sz / 16; // in DS (16B)
    }
    void TearDown()
    {
    }
};
adapter* dpcp_fr::s_ad = nullptr;
flow_rule* dpcp_fr::s_fr3t = nullptr;
flow_rule* dpcp_fr::s_fr4t = nullptr;

/**
 * @test dpcp_fr.ti_01_Constructor
 * @brief
 *    Check flow_rule constructor
 * @details
 *
 */
TEST_F(dpcp_fr, ti_01_constructor)
{
    ASSERT_NE(nullptr, s_fr3t);
}
/**
 * @test dpcp_fr.ti_02_get_priority
 * @brief
 *    Check flow_rule::get_priority()
 * @details
 */
TEST_F(dpcp_fr, ti_02_get_priority)
{
    uint16_t pr = 0;
    status ret = s_fr3t->get_priority(pr);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(10, pr);
}
/**
 * @test dpcp_fr.ti_03_match_value
 * @brief
 *    Check flow_rule::set_match_value/get_match_value()
 * @details
 */
TEST_F(dpcp_fr, ti_03_match_value)
{
    match_params mv0 = {};
    match_params mvr = {};
    status ret = s_fr3t->get_match_value(mvr);
    ASSERT_EQ(DPCP_OK, ret);
    int i = memcmp(&mv0, &mvr, sizeof(mv0));
    ASSERT_EQ(0, i);

    match_params mv1 = { {}, 0x0800, 0, 0x12345678, 0, 0x4321, 0, 0x11, 4 };
    ret = s_fr3t->set_match_value(mv1);
    ASSERT_EQ(DPCP_OK, ret);

    ret = s_fr3t->get_match_value(mvr);
    ASSERT_EQ(DPCP_OK, ret);
    i = memcmp(&mv1, &mvr, sizeof(mv1));
    ASSERT_EQ(0, i);
}
/**
 * @test dpcp_fr.ti_04_flow_id
 * @brief
 *    Check flow_rule::set_flow_id/get_flow_id()
 * @details
 */
TEST_F(dpcp_fr, ti_04_flow_id)
{
    uint32_t fid0 = 0;
    uint32_t fidr = 0;
    status ret = s_fr3t->get_flow_id(fidr);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(0, fidr);

    fid0 = 1;
    ret = s_fr3t->set_flow_id(fid0);
    ASSERT_EQ(DPCP_OK, ret);

    ret = s_fr3t->get_flow_id(fidr);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(fidr, fid0);

    fid0 = 0xFFFFF;
    ret = s_fr3t->set_flow_id(fid0);
    ASSERT_EQ(DPCP_OK, ret);

    ret = s_fr3t->get_flow_id(fidr);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(fidr, fid0);

    fid0 = 0xFFFFF + 1;
    ret = s_fr3t->set_flow_id(fid0);
    ASSERT_EQ(DPCP_ERR_OUT_OF_RANGE, ret);

    ret = s_fr3t->get_flow_id(fidr);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(fidr, fid0 - 1);

    ret = s_fr3t->set_flow_id(0);
    ASSERT_EQ(DPCP_OK, ret);
    ret = s_fr3t->get_flow_id(fidr);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(0, fidr);
}
static striding_rq* s_srq1 = nullptr;
static striding_rq* s_srq2 = nullptr;
static tir* s_tir1 = nullptr;
static tir* s_tir2 = nullptr;
/**
 * @test dpcp_fr.ti_05_set_tir
 * @brief
 *    Check flow_rule::set_dest_tir
 * @details
 */
TEST_F(dpcp_fr, ti_05_set_dest_tir)
{
    status ret = s_ad->open();
    ASSERT_EQ(DPCP_OK, ret);
    s_srq1 = open_str_rq(s_ad, m_rqp);
    ASSERT_NE(s_srq1, nullptr);
    uint32_t rq_id = 0;
    ret = s_srq1->get_id(rq_id);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(0, rq_id);

    uint32_t nt = 0;
    ret = s_fr3t->get_num_tirs(nt);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(0, nt);

    ret = s_ad->create_tir(rq_id, s_tir1);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(nullptr, s_tir1);

    ret = s_fr3t->add_dest_tir(s_tir1);
    ASSERT_EQ(DPCP_OK, ret);
    ret = s_fr3t->get_num_tirs(nt);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(1, nt);

    s_srq2 = open_str_rq(s_ad, m_rqp);
    ASSERT_NE(nullptr, s_srq2);
    uint32_t rq2_id = 0;
    ret = s_srq2->get_id(rq2_id);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(0, rq2_id);

    ret = s_ad->create_tir(rq2_id, s_tir2);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_NE(nullptr, s_tir2);

    ret = s_fr3t->add_dest_tir(s_tir2);
    ASSERT_EQ(DPCP_OK, ret);
    ret = s_fr3t->get_num_tirs(nt);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(2, nt);
}
/**
 * @test dpcp_fr.ti_06_remove_dest_tir
 * @brief
 *    Check flow_rule::remove_dest_tir
 * @details
 */
TEST_F(dpcp_fr, ti_06_remove_dest_tir)
{
    uint32_t nt = 0;
    status ret = s_fr3t->get_num_tirs(nt);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(2, nt);

    ret = s_fr3t->remove_dest_tir(s_tir2);
    ASSERT_EQ(DPCP_OK, ret);
    ret = s_fr3t->get_num_tirs(nt);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(1, nt);

    ret = s_fr3t->remove_dest_tir(s_tir2);
    ASSERT_EQ(DPCP_ERR_INVALID_PARAM, ret);
    ret = s_fr3t->get_num_tirs(nt);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(1, nt);

    ret = s_fr3t->remove_dest_tir(s_tir1);
    ASSERT_EQ(DPCP_OK, ret);
    ret = s_fr3t->get_num_tirs(nt);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(0, nt);

    ret = s_fr3t->remove_dest_tir(s_tir1);
    ASSERT_EQ(DPCP_ERR_INVALID_PARAM, ret);
    ret = s_fr3t->get_num_tirs(nt);
    ASSERT_EQ(DPCP_OK, ret);
    ASSERT_EQ(0, nt);
}
/**
 * @test dpcp_fr.ti_07_apply_settings
 * @brief
 *    Check flow_rule::apply_settings
 * @details
 */
TEST_F(dpcp_fr, ti_07_apply_settings)
{
#ifdef __linux__
    SKIP_TRUE(sys_rootuser(), "This test requires root permissions (RAW_NET)\n");
#endif
    status ret = s_fr3t->add_dest_tir(s_tir2);
    ASSERT_EQ(DPCP_OK, ret);

    ret = s_fr3t->apply_settings();
    ASSERT_EQ(DPCP_OK, ret);
}
/**
 * @test dpcp_fr.ti_08_revoke_settings
 * @brief
 *    Check flow_rule::revoke_settings
 * @details
 *
 */
TEST_F(dpcp_fr, ti_08_revoke_settings)
{
    status ret = s_fr3t->revoke_settings();
    ASSERT_EQ(DPCP_OK, ret);
}
/**
* @test dpcp_fr.ti_09_4_tuple
* @brief
*    Check flow_rule::apply_settings for 4 tuple rule
* @details
*/
TEST_F(dpcp_fr, ti_09_4_tuple)
{
#ifdef __linux__
    SKIP_TRUE(sys_rootuser(), "This test requires root permissions (RAW_NET)\n");
#endif
    match_params mv0 = {};
    match_params mvr = {};
    status ret = s_fr4t->get_match_value(mvr);
    ASSERT_EQ(DPCP_OK, ret);
    int i = memcmp(&mv0, &mvr, sizeof(mv0));
    ASSERT_EQ(0, i);

    match_params mv1 = { {}, 0x0800, 0, 0x12345678, 0x87654321, 0x4321, 0, 0x11, 4 };
    ret = s_fr4t->set_match_value(mv1);
    ASSERT_EQ(DPCP_OK, ret);

    ret = s_fr4t->get_match_value(mvr);
    ASSERT_EQ(DPCP_OK, ret);
    i = memcmp(&mv1, &mvr, sizeof(mv1));
    ASSERT_EQ(0, i);

    ret = s_fr4t->add_dest_tir(s_tir2);
    ASSERT_EQ(DPCP_OK, ret);

    ret = s_fr4t->apply_settings();
    ASSERT_EQ(DPCP_OK, ret);

    ret = s_fr4t->revoke_settings();
    ASSERT_EQ(DPCP_OK, ret);

    delete s_tir2;
    delete s_tir1;
    delete s_srq2;
    delete s_srq1;
    delete s_ad;
}
