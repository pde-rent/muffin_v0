#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

int		on_tick(t_env *env)
{
	SMA(env, 1000, 0);
	return (1);
}
