#pragma once

#include "t_env.hpp"
#include "t_data.hpp"

namespace stream::ohlc::consumer
{
	// events
	int		listen(t_env *env);
	int		flush(t_env *env);
}

namespace stream::ohlc::producer
{
	// load data
	int		generate(t_env *env);
	// inlined:
	// void	push(t_env *env, time_t epoch, double ohlcv[5]);
}
