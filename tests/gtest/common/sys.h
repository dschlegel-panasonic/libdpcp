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

#ifndef TESTS_GTEST_COMMON_SYS_H_
#define TESTS_GTEST_COMMON_SYS_H_

/* Minimum and maximum macros */
#define sys_max(a, b) (((a) > (b)) ? (a) : (b))
#define sys_min(a, b) (((a) < (b)) ? (a) : (b))

static INLINE int sys_is_big_endian(void)
{
    return (htonl(1) == 1);
}

static INLINE double sys_gettime(void)
{
#ifdef __linux__
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double)(tv.tv_sec * 1000000 + tv.tv_usec);
#else
    SYSTEMTIME now = {};
    FILETIME ft_now = {}; // in 100ns accuracy
    GetSystemTime(&now);
    SystemTimeToFileTime(&now, &ft_now);
    uint64_t* tt = (uint64_t*)&ft_now;
    return (double)(*tt / 10.);
#endif
}

static INLINE uint64_t sys_rdtsc(void)
{
    unsigned long long int result = 0;

#if defined(__i386__)
    __asm volatile(".byte 0x0f, 0x31" : "=A"(result) :);

#elif defined(__x86_64__)
    unsigned hi, lo;
    __asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    result = hi;
    result = result << 32;
    result = result | lo;

#elif defined(__powerpc__)
    unsigned long int hi, lo, tmp;
    __asm volatile("0:                 \n\t"
                   "mftbu   %0         \n\t"
                   "mftb    %1         \n\t"
                   "mftbu   %2         \n\t"
                   "cmpw    %2,%0      \n\t"
                   "bne     0b         \n"
                   : "=r"(hi), "=r"(lo), "=r"(tmp));
    result = hi;
    result = result << 32;
    result = result | lo;

#endif

    return (result);
}

void sys_hexdump(const char* tag, void* ptr, int buflen);

int sys_get_addr(char* dst, struct sockaddr_in* addr);

char* sys_addr2dev(struct sockaddr_in* addr, char* buf, size_t size);

int sys_dev2addr(char* dev, struct sockaddr_in* addr);

int sys_gateway(struct sockaddr_in* addr);

#if defined(__linux__)
pid_t sys_procpid(const char* name);
#endif

static INLINE char* sys_addr2str(struct sockaddr_in* addr)
{
    static thread_local char addrbuf[100];
    inet_ntop(AF_INET, &addr->sin_addr, addrbuf, sizeof(addrbuf));
    sprintf(addrbuf, "%s:%d", addrbuf, ntohs(addr->sin_port));

    return addrbuf;
}

static INLINE int sys_rootuser(void)
{
#if defined(__linux__)
    return (geteuid() == 0);
#else
    return IsUserAnAdmin();
#endif
}

static INLINE int64_t sys_getpid(void)
{
#if defined(__linux__)
    return getpid();
#else
    return static_cast<int64_t>(GetCurrentProcessId());
#endif
}

#endif /* TESTS_GTEST_COMMON_SYS_H_ */
