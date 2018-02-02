#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

double SMA(t_env *env, int period, int shift)
{
	std::deque<float>::iterator front = env->data->px->begin(); //auto could be use for readability
	double	sum = *front;

	while (shift){ ++front; --shift; }
    while ((++shift < period) && (sum += *front)) ++front;
    return (sum/(double)period);
}