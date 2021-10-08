#pragma once

#include <climits>

// come from lua code

const int EOZ = -1;

//accept UniCode IDentifiers
const unsigned char NONA = 0x01;

const unsigned char luai_ctype_[UCHAR_MAX + 2] = {
	0x00, /* EOZ */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0. */
	0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 1. */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, /* 2. */
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, /* 3. */
	0x16, 0x16, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x05, /* 4. */
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, /* 5. */
	0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x05,
	0x04, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x05, /* 6. */
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, /* 7. */
	0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x00,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* 8. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* 9. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* a. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* b. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	0x00, 0x00, NONA, NONA, NONA, NONA, NONA, NONA, /* c. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* d. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* e. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, 0x00, 0x00, 0x00, /* f. */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


#define ALPHABIT	0
#define DIGITBIT	1
#define PRINTBIT	2
#define SPACEBIT	3
#define XDIGITBIT	4


#define MASK(B)		(1 << (B))


/*
** add 1 to char to allow index -1 (EOZ)
*/
#define testprop(c,p)	(luai_ctype_[(c)+1] & (p))

/*
** 'lalpha' (Lua alphabetic) and 'lalnum' (Lua alphanumeric) both include '_'
*/
#define lislalpha(c)	testprop(c, MASK(ALPHABIT))
#define lislalnum(c)	testprop(c, (MASK(ALPHABIT) | MASK(DIGITBIT)))
#define lisdigit(c)	testprop(c, MASK(DIGITBIT))
#define lisspace(c)	testprop(c, MASK(SPACEBIT))
#define lisprint(c)	testprop(c, MASK(PRINTBIT))
#define lisxdigit(c)	testprop(c, MASK(XDIGITBIT))


/*
** Priority table for binary operators.
*/
static const struct
{
	unsigned char left; /* left priority for each binary operator */
	unsigned char right; /* right priority */
} priority[] = {
	/* ORDER OPR */
	{10, 10}, {10, 10}, /* '+' '-' */
	{11, 11}, {11, 11}, /* '*' '%' */
	{14, 13}, /* '^' (right associative) */
	{11, 11}, {11, 11}, /* '/' '//' */
	{6, 6}, {4, 4}, {5, 5}, /* '&' '|' '~' */
	{7, 7}, {7, 7}, /* '<<' '>>' */
	{9, 8}, /* '..' (right associative) */
	{3, 3}, {3, 3}, {3, 3}, /* ==, <, <= */
	{3, 3}, {3, 3}, {3, 3}, /* ~=, >, >= */
	{2, 2}, {1, 1} /* and, or */
};

const int UNARY_PRIORITY = 12; /* priority for unary operators */
