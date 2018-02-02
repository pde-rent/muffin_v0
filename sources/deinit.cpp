#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

namespace deinit
{
	static void	tick_data(t_tick_data *data)
	{
		data->epoch->clear();
		data->px->clear();
		data->vol->clear();
		delete(data->epoch);
		delete(data->px);
		delete(data->vol);
		free(data);
	}
	static void	ohlc_data(t_ohlc_data *data)
	{
		data->epoch->clear();
		data->open->clear();
		data->high->clear();
		data->low->clear();
		data->close->clear();
		data->vol->clear();
		delete (data->epoch);
		delete (data->open);
		delete (data->high);
		delete (data->low);
		delete (data->close);
		delete (data->vol);
		free(data);
	}
	int			env(t_env *env)
	{
		//empty all envirnment components then free it
		deinit::tick_data(env->data);
		//deinit::ohlc_data(env->data);
		free(env->rsk);
		free(env->acc);
		free(env->ticker);
		delete(env->order_book);
		delete(env->order_history);
		free(env);
		return (1);
	}
}