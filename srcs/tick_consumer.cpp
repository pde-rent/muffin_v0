#include "main.hpp"

namespace	stream::tick::consumer
{	
	int			listen(t_env *env)
	{
		while (1)
		{
			if (env->lock[0]) //buffer has been filled with new tick data, start strategy::run
			{
				while (env->lock[0])	//while tick remains available to use
					if (env->lock[2])	//if tick is processed and ready to be pulled by strategy::run
					{
						env->lock[2] = false; //data pulled, feeder is free to reload
						while (!env->lock[1]); //wait for feeder to consume data
						env->lock[1] = false; // data was consumed
						strategy::run(env); // call logic
					}
				break; //exit, test is finished, no more ticks to proceed
			}
		}
		return	(1);
	}
	int			flush(t_env *env)
	{
		env->ticks->epoch->resize(TICK_BUFF_SIZE);	//data->epoch->erase(TICK_BUFF_SIZE);
		env->ticks->bid->resize(TICK_BUFF_SIZE);		//data->px->erase(TICK_BUFF_SIZE);
		env->ticks->ask->resize(TICK_BUFF_SIZE);		//data->px->erase(TICK_BUFF_SIZE);
		env->ticks->volume->resize(TICK_BUFF_SIZE);;		//data->vol->erase(TICK_BUFF_SIZE);
		return (1);
	}
}
