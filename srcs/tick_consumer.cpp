#include "main.hpp"

// copy of stream::ohlc::consumer as no different for now
namespace	stream::tick::consumer
{	
	int		listen(t_env *env)
	{
		env->consumer_busy = false;
		// printf("consumer: I'm free and waiting for first data points\n");
		while (1){
			if (env->new_data_available == true) // buffer has been filled with new tick data, start strategy::run
			{
				env->stream_mutex.lock();
				env->consumer_busy = true;
				env->new_data_available = false; // consume data while other thread is locked
				env->stream_mutex.unlock();
				// check for stop signal
				if (!EPOCH[0]) { // PRICE[0]
					printf("stream::ohlc::consumer::terminate()\n");
					return (1); // finished
				}
				// printf("consumer: I'm consuming data points >> I'm busy\n");
				strategy::split_to_thread_pool(env); // call logic
				env->stream_mutex.lock();
				env->consumer_busy = false; // data consumed
				env->stream_mutex.unlock();
				// printf(env->new_data_available
				// 	? "consumer: Thanks for waiting, I'll deal with it"
				// 	: "consumer: I'm free and waiting for new data points\n");
			}
		}
	}
}
