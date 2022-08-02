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

#ifndef SRC_DCMD_LINUX_UMEM_H_
#define SRC_DCMD_LINUX_UMEM_H_

#include "dcmd/base/base_umem.h"

namespace dcmd {

class umem : public base_umem {
public:
    umem()
    {
        m_handle = nullptr;
    }
    umem(ctx_handle handle, struct umem_desc* desc);
    virtual ~umem();

    uint32_t get_id();

private:
    umem_handle m_handle;
};

} /* namespace dcmd */

#endif /* SRC_DCMD_LINUX_UMEM_H_ */
