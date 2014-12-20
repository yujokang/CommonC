#include "xmath_tvs.h"

/* 0 * 0 = 0 */
static struct xmultiply_tv zero_zero = {
	.in_0 = 0,
	.in_1 = 0,
	.out_high = 0,
	.out_low = 0,
};

/* the default value to be used for factors and dividend halves */
#define DEFAULT	0xdead

/* 0 * a = 0 */
static struct xmultiply_tv zero_first = {
	.in_0 = 0,
	.in_1 = DEFAULT,
	.out_high = 0,
	.out_low = 0,
};

/* a * 0 = 0 */
static struct xmultiply_tv zero_second = {
	.in_0 = DEFAULT,
	.in_1 = 0,
	.out_high = 0,
	.out_low = 0,
};

/* a regular multiply without carries to the second half */
static struct xmultiply_tv normal = {
	.in_0 = DEFAULT,
	.in_1 = DEFAULT,
	.out_high = 0,
	.out_low = 0xc1b080e9,
};

/* the largest 64-bit integer, used for corner case testing */
#define LARGEST	(~((uint64_t) 0))

/* using the maximum number for the first factor, and a carry is needed */
static struct xmultiply_tv largest_first = {
	.in_0 = LARGEST,
	.in_1 = DEFAULT,
	.out_high = 0xdeac,
	.out_low = 0xffffffffffff2153,
};

/* using the maximum number for the second factor, and a carry is needed */
static struct xmultiply_tv largest_second = {
	.in_0 = DEFAULT,
	.in_1 = LARGEST,
	.out_high = 0xdeac,
	.out_low = 0xffffffffffff2153,
};

/* squaring the maximum number */
static struct xmultiply_tv largest_largest = {
	.in_0 = LARGEST,
	.in_1 = LARGEST,
	.out_high = 0xfffffffffffffffe,
	.out_low = 0x0000000000000001,
};

struct xmultiply_tv *xmultiply_tvs[N_XMULTIPLY_TVS] = {
	&zero_zero, &zero_first, &zero_second, &normal,
	&largest_first, &largest_second, &largest_largest
};

/* the default divisor used in most of the "xmod" tests */
#define DEFAULT_MOD	0x7532

/* 0 % a = 0 */
static struct xmod_tv zero_dividend = {
	.high = 0,
	.low = 0,
	.mod = DEFAULT_MOD,
	.result = 0,
};

/* have non-zero value in both halves */
static struct xmod_tv normal_dividend = {
	.high = DEFAULT,
	.low = DEFAULT,
	.mod = DEFAULT_MOD,
	.result = 0x33f,
};

/* have non-zero value only in high half */
static struct xmod_tv no_low_dividend = {
	.high = DEFAULT,
	.low = 0,
	.mod = DEFAULT_MOD,
	.result = 0xef6,
};

/* largest possible dividend */
static struct xmod_tv largest_dividend = {
	.high = LARGEST,
	.low = LARGEST,
	.mod = DEFAULT_MOD,
	.result = 0x5f47,
};

/*
 * When the divisor is a power of 2,
 * then all bits corresponding to equal or greater powers will be cleared.
 */
#define POWER_2_MOD	0x1000
static struct xmod_tv power_2_mod = {
	.high = DEFAULT,
	.low = DEFAULT,
	.mod = POWER_2_MOD,
	.result = 0xead,
};

struct xmod_tv *xmod_tvs[N_XMOD_TVS] = {
	&zero_dividend, &normal_dividend, &no_low_dividend, &largest_dividend,
	&power_2_mod
};
