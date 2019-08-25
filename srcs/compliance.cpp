#include "main.hpp"

namespace compliance
{
	// checkes if arguments passed are well formatted and valid
	int		argv(int ac, char **av)
	{
		//something is wrong, return 0!
		if (ac == 1)
			return (put_error(USAGE, 0));
		if (ac != 3)
			return (put_error(ERR_1, 0));
		if (!file::exists(av[1]))
			return (put_error(ERR_2, 0));
		if (!file::exists(av[2]))
			return (put_error(ERR_3, 0));
		//all good, return 1!
		return (1);
	}
}
