#pragma once

#include "t_env.hpp"

namespace init //constructors
{
	//t_account		*account();
	//t_ticker		*ticker();
	//t_risk		*risk();
	int 	env(t_env *env);
	int 	data(t_env *env);
	void	split_process(t_env *env);
}

namespace deinit //destructors
{
	int		env(t_env *env);
}

int		get_config(t_env *env, std::string file_name);
int		prepare_bt(t_env *env);
