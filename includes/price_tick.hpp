#pragma once

#include "t_env.hpp"

namespace stream::tick::consumer
{
	// events
	int		listen(t_env *env);
	int		flush(t_env *env);
}

namespace stream::tick::producer
{

	// load data
	int		generate(t_env *env);
}
