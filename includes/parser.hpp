#pragma once

namespace parser::str
{
	int			to_int(const char *str);
	long		to_long(const char *str);
	double		to_double(const char *str);
	time_t 		to_time(const char* str, const char* format = "%Y%m%d %H:%M:%S"); //+%f microseconds?
}

namespace parser::time
{
	std::string to_string(time_t epoch, const char* format = "%Y%m%d %H:%M:%S");
}

namespace parser::format
{
	int 	is_timestamp(const char c);
	int 	is_numeric(const char c);
}
