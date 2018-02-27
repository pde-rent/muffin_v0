#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

namespace fast
{
	size_t	parse_zu(string str)
	{
		size_t	res = 0;
		while (*str)
			res = res * 10 + (*str++ - '0');
		return (res);
	}
	float	parse_f(string str)
	{
		float res = 0.0;
		bool neg = false;
		if (*str == '-')
		{
			neg = true;
			++str;
		}
		while (*str >= '0' && *str <= '9')
		{
			res = (res * 10.0) + (*str - '0');
			++str;
		}
		if (*str == '.')
		{
			float f = 0.0;
			int n = 0;
			++str;
			while (*str >= '0' && *str <= '9')
			{
				f = (f*10.0) + (*str - '0');
				++str;
				++n;
			}
			res += f / pow(10.0, n);
		}
		return (neg ? -res : res);
	}
}
