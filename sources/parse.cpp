#include "../includes/stdafx.h"

namespace file
{
	bool			exists(string file_name)
	{
		if (FILE *file = fopen(file_name, "rb"))
		{
			fclose(file);
			return (true);
		}
		return (false);
	}
	unsigned long	count_lines(string file_name)
	{
		FILE			*histo;
		char			buf[BUFF_SIZE];
		size_t			neol = 0;
		int				handle;
		unsigned long	len = 0;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if (((histo = fopen(file_name, "r")) != NULL))
		{
			fseek(histo, 0, SEEK_END);
			len = (ftell(histo));
			fseek(histo, 0, SEEK_SET);
			while ((handle = fread(buf, 1, BUFF_SIZE, histo)))
			{
				char* ptr = buf;
				while ((ptr = (string)memchr(ptr, '\n', (buf + handle) - ptr)))
				{
					++ptr;
					++neol;
				}
			}
			printf("Characters to parse: %lu\nRows of data: %zu\nEstimate loading time: %zu ms\n",
				len, neol, neol / 290);
			fclose(histo);
			return (neol);
		}
		printf("Failed to open the data file\n");
		return (0);
	}
}
namespace fast
{
	size_t	parse_zu(string str)
	{
		size_t	res = 0;
		while (*str)
			res = res * 10 + (*str++ - '0');
		return (res);
	}
	double	parse_f(string str)
	{
		double res = 0.0;
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
			double f = 0.0;
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
