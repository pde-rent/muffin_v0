#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

int		on_tick(t_env *env)
{
	float sma = on_deque::SMA<float>(env->data->px, 10, 0);
	return (1);
}
