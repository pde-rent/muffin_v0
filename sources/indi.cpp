#include "../includes/stdafx.h"

float SMA(t_env *env, int period, int shift)
{
    std::deque<float>::iterator it = env->data->px->begin(); //auto could be use for readability
    float   sum = 0;

    while (shift--) ++it;
    while ((shift++ < period) && (sum += *it)) ++it;
    return (sum/period);
}