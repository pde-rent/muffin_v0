#include "main.hpp"

namespace parser::format
{
	int 	is_timestamp(const char c)
	{
		return (((c >= '0' && c <= '9') || c == ' ' || c == ':') ? 1 : 0);
	}
	int 	is_numeric(const char c)
	{
		return (((c >= '0' && c <= '9') || c == '.') ? 1 : 0);
	}
}

namespace parser::str
{
	time_t	to_time(const char* str, const char* format)
	{
		std::tm time;
		memset(&time, 0, sizeof(time));
		strptime(str, format, &time);
		return mktime(&time);
	}
	int		to_int(const char *str)
	{
		int 	res = 0;
		char	*s = (char *)str;

		while (*s)
			res = res * 10 + (*s++ - '0');
		return (res);
	}
	long	to_long(const char *str)
	{
		long 	res = 0;
		char	*s = (char *)str;

		while (*s)
			res = res * 10 + (*s++ - '0');
		return (res);
	}
	double	to_double(const char *str)
	{
		double res = 0.0;
		bool neg = false;
		char *s = (char *)str;

		if (*s == '-')
		{
			neg = true;
			++s;
		}
		while (*s >= '0' && *s <= '9')
		{
			res = (res * 10.0) + (*s - '0');
			++s;
		}
		if (*s == '.')
		{
			double f = 0.0;
			int n = 0;
			++s;
			while (*s >= '0' && *s <= '9')
			{
				f = (f*10.0) + (*s - '0');
				++s;
				++n;
			}
			res += f / pow(10.0, n);
		}
		return (neg ? -res : res);
	}
}
