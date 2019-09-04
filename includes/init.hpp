#pragma once

#include "t_env.hpp"

namespace init //constructors
{
	//int	account(t_env *env);
	//int	ticker(t_env *env);
	//int	risk(t_env *env);
	//int 	data(t_env *env);
	//int 	thread_pool(t_env *env);
	//int 	stream_workers(t_env *env);
	t_env 	*env(void);
	int 	backtest(t_env *env);
}

namespace deinit //destructors
{
	int		env(t_env *env);
}
