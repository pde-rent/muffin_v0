#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

int	put_error(string str, int err_code)
{
	printf("%s", str);
	return (err_code);
}