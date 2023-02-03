#pragma once

#include "LuaIdentify.h"
#include <climits>

const int EOZ = -1;

//accept UniCode IDentifiers
const unsigned char NONA = 0x01;

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
