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
#include <algorithm>

#include "common/def.h"
#include "common/log.h"
#include "common/sys.h"
#include "common/base.h"

#include "dpcp_base.h"

using namespace dpcp;

class dpcp_parser_graph_node : public dpcp_base {
public:
    static void set_ecpri_test_attributes(parser_graph_node_attr& attributes)
    {
        // Node configuration:
        attributes.header_length_mode = PARSE_GRAPH_NODE_LEN_FIXED; // Using the constant header use case.
        attributes.header_length_base_value = 16; // eCPRI packet header constant part size is 16 bytes.
        attributes.header_length_field_offset = 0; // Reserved in this case.
        attributes.header_length_field_shift = 0; // Reserved in this case.
        attributes.header_length_field_mask = 0; // Reserved in this case.

        // Input arc configuration:
        parse_graph_arc_attr in_arc;
        in_arc.arc_parse_graph_node = PARSE_GRAPH_ARC_NODE_MAC; // Packet headers order: |MAC| --> |eCPRI|
        in_arc.start_inner_tunnel = false; // No tunneling in this case.
        in_arc.compare_condition_value = 0xAEFE; // eCPRI ether_type is the condition to take this node path.
        in_arc.parse_graph_node_handle = 0; // Not valid in this case.
        attributes.in_arcs.push_back(in_arc);

        // Samples configuration:

        // Sample 0 - octets 9,10,11,12 from eCPRI packet header:
        parse_graph_flow_match_sample_attr sample_0;

        sample_0.enabled = true;
        sample_0.offset_mode = PARSE_GRAPH_SAMPLE_OFFSET_FIXED; // The offset is fixed in eCPRI case.
        sample_0.field_offset = 0; // Reserved in this case.
        sample_0.field_offset_mask = 0; // Reserved in this case.
        sample_0.field_offset_shift = 0; // Reserved in this case.
        sample_0.field_base_offset = 8; // Starts from octet 9, so the offset is 8 bytes.
        sample_0.tunnel_mode = PARSE_GRAPH_FLOW_MATCH_SAMPLE_TUNNEL_FIRST; // Not a tunneled packet.
        sample_0.field_id = 0; // Don't set filed ID, get it through query.
        attributes.samples.push_back(sample_0);

        // Sample 1 - octets 13,14,15,16 from eCPRI packet header:
        parse_graph_flow_match_sample_attr sample_1;

        sample_1.enabled = true;
        sample_1.offset_mode = PARSE_GRAPH_SAMPLE_OFFSET_FIXED; // The offset is fixed in eCPRI case.
        sample_1.field_offset = 0; // Reserved in this case.
        sample_1.field_offset_mask = 0; // Reserved in this case.
        sample_1.field_offset_shift = 0; // Reserved in this case.
        sample_1.field_base_offset = 12; // Starts from octet 13, so the offset is 12 bytes.
        sample_1.tunnel_mode = PARSE_GRAPH_FLOW_MATCH_SAMPLE_TUNNEL_FIRST; // Not a tunneled packet.
        sample_1.field_id = 0; // Don't set filed ID, get it through query.
        attributes.samples.push_back(sample_1);
    };
};

/**
 * @test dpcp_parser_graph_node.ti_01_create
 * @brief
 *    Check parser_graph_node::create method
 * @details
 *
 */
TEST_F(dpcp_parser_graph_node, ti_01_create)
{
    adapter* ad = OpenAdapter();
    ASSERT_NE(nullptr, ad);

    status ret = ad->open();
    ASSERT_EQ(DPCP_OK, ret);

    adapter_hca_capabilities caps;
    ret = ad->get_hca_capabilities(caps);
    ASSERT_EQ(DPCP_OK, ret);

    bool is_parser_graph_node_supported = caps.general_object_types_parse_graph_node;
    if (!is_parser_graph_node_supported) {
        log_trace("parser_graph_node is not supported \n");
        delete ad;
        return;
    }

    /**
     * eCPRI simple use case test.
     */
    parser_graph_node_attr attributes;
    dpcp_parser_graph_node::set_ecpri_test_attributes(attributes);
    parser_graph_node _parser_graph_node(ad->get_ctx(), attributes);

    ret = _parser_graph_node.create();
    ASSERT_EQ(DPCP_OK, ret);

    uint32_t node_id = 0;
    ret = _parser_graph_node.get_id(node_id);
    ASSERT_EQ(DPCP_OK, ret);
    log_trace("node_id: 0x%x\n", node_id);

    delete ad;
}

/**
 * @test dpcp_parser_graph_node.ti_02_query
 * @brief
 *    Check parser_graph_node::query method
 * @details
 *
 */
TEST_F(dpcp_parser_graph_node, ti_02_query)
{
    adapter* ad = OpenAdapter();
    ASSERT_NE(nullptr, ad);

    status ret = ad->open();
    ASSERT_EQ(DPCP_OK, ret);

    adapter_hca_capabilities caps;
    ret = ad->get_hca_capabilities(caps);
    ASSERT_EQ(DPCP_OK, ret);

    bool is_parser_graph_node_supported = caps.general_object_types_parse_graph_node;
    if (!is_parser_graph_node_supported) {
        log_trace("parser_graph_node is not supported \n");
        delete ad;
        return;
    }

    /**
     * eCPRI Flex parser configuration use case test.
     */
    parser_graph_node_attr attributes;
    dpcp_parser_graph_node::set_ecpri_test_attributes(attributes);
    parser_graph_node _parser_graph_node(ad->get_ctx(), attributes);

    ret = _parser_graph_node.create();
    ASSERT_EQ(DPCP_OK, ret);

    ret = _parser_graph_node.query();
    ASSERT_EQ(DPCP_OK, ret);

    auto& sample_ids = _parser_graph_node.get_sample_ids();
    for (int index = 0; index < _parser_graph_node.get_num_of_samples(); index++) {
        log_trace("sample_id[%d]: %u\n", index, sample_ids[index]);
    }

    delete ad;
}

/**
 * @test dpcp_parser_graph_node.ti_03_query
 * @brief
 *    Check parser_graph_node::query method
 * @details
 *
 */
TEST_F(dpcp_parser_graph_node, ti_03_query)
{
    adapter* ad = OpenAdapter();
    ASSERT_NE(nullptr, ad);

    status ret = ad->open();
    ASSERT_EQ(DPCP_OK, ret);

    adapter_hca_capabilities caps;
    ret = ad->get_hca_capabilities(caps);
    ASSERT_EQ(DPCP_OK, ret);

    bool is_parser_graph_node_supported = caps.general_object_types_parse_graph_node;
    if (!is_parser_graph_node_supported) {
        log_trace("parser_graph_node is not supported \n");
        delete ad;
        return;
    }

    /**
     * eCPRI Flex parser configuration use case test.
     */
    parser_graph_node_attr attributes;
    dpcp_parser_graph_node::set_ecpri_test_attributes(attributes);

    parser_graph_node* out_parser_graph_node;
    ret = ad->create_parser_graph_node(attributes, out_parser_graph_node);
    ASSERT_EQ(DPCP_OK, ret);

    ret = out_parser_graph_node->query();
    ASSERT_EQ(DPCP_OK, ret);

    auto& sample_ids = out_parser_graph_node->get_sample_ids();
    for (int index = 0; index < out_parser_graph_node->get_num_of_samples(); index++) {
        log_trace("sample_id[%d]: %u\n", index, sample_ids[index]);
    }

    delete ad;
}
