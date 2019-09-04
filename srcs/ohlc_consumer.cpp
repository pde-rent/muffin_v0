#include "main.hpp"

namespace stream::ohlc::consumer
{	
	int		listen(t_env *env)
	{
		env->consumer_busy = false;
		// printf("consumer: I'm free and waiting for first data points\n");
		while (1) {
			if (env->new_data_available == true) // buffer has been filled with new tick data, start strategy::run
			{
				env->stream_mutex.lock();
				// printf("env->stream_mutex.lock()\n");
				env->consumer_busy = true;
				env->new_data_available = false; // consume data while other thread is locked
				// env->stream_mutex.unlock();
				// check for stop signal
				// printf("waiting for pool\n");
				strategy::wait_for_thread_pool(env);
				if (!EPOCH[0]) { // PRICE[0]
					for (int pass = 0; pass < env->nb_pass; pass++) {
						int sum = order::close_all(env, MODE_DEFAULT, pass, "[residual close]"); // mode default closes all trades
						printf("closed %d residual orders for pass %d\n", sum, pass);
					}
					printf("stream::ohlc::consumer::terminate()\n");
					delete env->thread_pool;
					return (1); // finished
				}
				// printf("consumer: I'm consuming data points >> I'm busy\n");
				strategy::split_to_thread_pool(env); // call logic
				// printf("pool consumed\n");
				// wait for thread_pool to finish processing then close residual orders
				// env->stream_mutex.lock();
				env->consumer_busy = false; // data consumed
				env->stream_mutex.unlock();
				// printf("env->stream_mutex.unlock()\n");
				// printf(env->new_data_available
				// 	? "consumer: Thanks for waiting, I'll deal with it"
				// 	: "consumer: I'm free and waiting for new data points\n");
			}
		}
	}
}
