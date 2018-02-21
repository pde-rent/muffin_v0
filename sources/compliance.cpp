#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

namespace compliance
{
	int		argv(int ac, char **av)
	{
		//something is wrong, return 0!
		if (ac == 1)
			return (put_error((string)USAGE, 0));
		if (ac != 3)
			return (put_error((string)ERR_1, 0));
		if (!file::exists(av[1]))
			return (put_error((string)ERR_2, 0));
		if (!file::exists(av[2]))
			return (put_error((string)ERR_3, 0));
		//all good, return 1!
		return (1);
	}
}