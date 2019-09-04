#include "main.hpp"

namespace init
{
	static int data(t_env *env)
	{
		// array version:
		if (env->ticker.price_mode == MODE_TICK) {
			// size_t double_arr_size = sizeof(double) * (size_t)TICK_BUFF_SIZE;
			// size_t ui_arr_size = sizeof(unsigned int) * (size_t)TICK_BUFF_SIZE;
			env->ticks = new t_tick_data;
			env->ticks->epoch = new unsigned int[TICK_BUFF_SIZE];
			env->ticks->bid = new double[TICK_BUFF_SIZE];
			env->ticks->ask = new double[TICK_BUFF_SIZE];
			env->ticks->volume = new double[TICK_BUFF_SIZE];
			env->price = env->ticks->bid;
			env->epoch = env->ticks->epoch;
			printf("TICK data store allocated\n");
		} else if (env->ticker.price_mode == MODE_OHLC) {
			// size_t double_arr_size = sizeof(double) * (size_t)OHLC_BUFF_SIZE;
			// size_t ui_arr_size = sizeof(unsigned int) * (size_t)OHLC_BUFF_SIZE;
			env->ohlc = new t_ohlc_data;
			env->ohlc->epoch = new unsigned int[OHLC_BUFF_SIZE];
			env->ohlc->open = new double[OHLC_BUFF_SIZE];
			env->ohlc->high = new double[OHLC_BUFF_SIZE];
			env->ohlc->low = new double[OHLC_BUFF_SIZE];
			env->ohlc->close = new double[OHLC_BUFF_SIZE];
			env->ohlc->volume = new double[OHLC_BUFF_SIZE];
			env->price = env->ohlc->close;
			env->epoch = env->ohlc->epoch;
			printf("OHLC data store allocated\n");
		} else {
			printf("mode unknown >> ERROR\n");
		}
		// deque version:
		// if (env->ticker.price_mode == MODE_TICK) {
		// 	env->ticks = (t_tick_data *)malloc(sizeof(t_tick_data));
		// 	env->ticks->epoch = new std::deque<unsigned int>(0);
		// 	env->ticks->bid = new std::deque<double>(0);
		// 	env->ticks->ask = new std::deque<double>(0);
		// 	env->ticks->volume = new std::deque<double>(0);
		// 	env->price = env->ticks->bid;
		// 	env->epoch = env->ticks->epoch;
		// } else if (env->ticker.price_mode == MODE_OHLC) {
		// 	env->ohlc = (t_ohlc_data *)malloc(sizeof(t_ohlc_data));
		// 	env->ohlc->epoch = new std::deque<unsigned int>(0);
		// 	env->ohlc->open = new std::deque<double>(0);
		// 	env->ohlc->high = new std::deque<double>(0);
		// 	env->ohlc->low = new std::deque<double>(0);
		// 	env->ohlc->close = new std::deque<double>(0);
		// 	env->ohlc->volume = new std::deque<double>(0);
		// 	env->price = env->ohlc->close;
		// 	env->epoch = env->ohlc->epoch;
		// }
		return (1);
	}
	static int account(t_env *env)
	{
		for (int pass = 0; pass < MAX_PASS; pass++)
		{
			// TODO: vector mode can be faster for small order amount >> to be profiled
			// deque mode
			// env->acc.order_book.active[pass] = std::deque<t_order>(0);
			// env->acc.order_book.history[pass] = std::deque<t_order>(0);
			// map mode
			// env->acc.order_book.active[pass] = {};
			// env->acc.order_book.history[pass] = {};
			// array version:
			for (int i = 0; i < MAX_PASS; i++) {
				for (int j = 0; j < ORDER_BOOK_SIZE; j++) {
					env->acc.order_book.order[i][j].status = MODE_EMPTY_SLOT;
				}
			}
			env->acc.order_book.nb_long[pass] = 0;
			env->acc.order_book.nb_short[pass] = 0;
		}
		return (1);
	}
	// static int ticker(t_ticker *ticker)
	// {
	// 	return (1);
	// }
	// static int risk(t_risk *risk)
	// {
	// 	return (1);
	// }
	static int thread_pool(t_env *env)
	{
		env->nb_cores = std::thread::hardware_concurrency();
		if (!env->nb_cores) { env->nb_cores = 4; } // defaults to 4 cores if none available in architecture
		// env->stream_worker = &stream_worker;
		// env->backtest_worker = &backtest_worker;
		// see https://github.com/progschj/ThreadPool/blob/master/example.cpp for wrapping yields
		env->thread_pool = new ThreadPool(env->nb_cores > MAX_PASS ? MAX_PASS : env->nb_cores);
		env->thread_yields = std::vector<std::future<int>>(0); // could use list, vector or whatever
		return (1);
	}
	static int stream_workers(t_env *env)
	{
		// data point producer and consumer will make busy 2 threads of the thread pool
		// leaving env->nb_cores - 2 threads running for backtest: id = 0
		std::thread producer;
		std::thread consumer;
		// #pragma omp parallel sections
		// 	#pragma omp section
		switch (env->ticker.price_mode) {
			case MODE_OHLC:
				printf("Starting stream::ohlc::producer\n");
				producer = std::thread(stream::ohlc::producer::generate, env);
				// env->thread_yields->emplace_back(
				// 	env->thread_pool->enqueue(stream::ohlc::producer::generate, env)
				// );
				printf("Starting stream::ohlc::consumer\n");
				consumer = std::thread(stream::ohlc::consumer::listen, env);
				// env->thread_yields->emplace_back(
				// 	env->thread_pool->enqueue(stream::ohlc::consumer::listen, env)
				// );
				break;
			case MODE_TICK:
				printf("Starting stream::tick::producer\n");
				producer = std::thread(stream::tick::producer::generate, env);
				// env->thread_yields->emplace_back(
				// 	env->thread_pool->enqueue(stream::tick::producer::generate, env)
				// );
				printf("Starting stream::tick::consumer\n");
				consumer = std::thread(stream::tick::consumer::listen, env);
				// env->thread_yields->emplace_back(
				// 	env->thread_pool->enqueue(stream::tick::consumer::listen, env)
				// );
				break;
		}
		producer.join();
		consumer.join();
		// printf("consumer and producer threads joined!!!!!!!!!!\n");
		return (1);
	}
	int backtest(t_env *env)
	{
		//start clockwatch
		auto t1 = std::chrono::high_resolution_clock::now();
		// start backtesting / stream workers
		init::stream_workers(env);
		//stop clockwatch once backtest is done >> threads are all finished
		auto t2 = std::chrono::high_resolution_clock::now();
		auto elapsed_ms = /*std::chrono::duration_cast<std::chrono::milliseconds>*/(t2 - t1);
		printf("Backtest took %d ms\n", (int)elapsed_ms.count());
		return ((int)elapsed_ms.count());
	}
	t_env *env(void)
	{
		// replaced all malloc() with new()
		t_env *env = new t_env();

		// these are now on stack:
		// env->risk = new t_risk();
		// env->acc = new t_account();
		// env->ticker = new t_ticker();
		// env->consumer_busy = true;
		env->new_data_available = false;
		printf("env address: %p\n", (void *)env);
		env->config_file_path = std::string(CONFIG_PATH);
		// env->config_file_path = std::string(CONFIG_PATH);
		printf("env->config_file_path: %s\n", env->config_file_path.c_str());
		// parse param file which path is in params header
		if (!config::get_parameters(env, env->config_file_path)) {
			return (NULL);
		}
		//init environment & load ticker
		init::data(env); // initiate data arrays
		init::account(env); // initiate parallel backtesting order books
		init::thread_pool(env);		
		return (env);
	}
}

namespace deinit
{
	static void	data(t_env *env)
	{
		// array version:
		// we could make everything point to NULL after free
		if (env->ticker.price_mode == MODE_TICK) {
			delete [] env->ticks->epoch;
			delete [] env->ticks->ask;
			delete [] env->ticks->bid;
			delete [] env->ticks->volume;
			delete env->ticks;
			printf("TICK data store freed\n");
		} else if (env->ticker.price_mode == MODE_OHLC) {
			delete [] env->ohlc->epoch;
			delete [] env->ohlc->open;
			delete [] env->ohlc->high;
			delete [] env->ohlc->low;
			delete [] env->ohlc->close;
			delete [] env->ohlc->volume;
			delete env->ohlc;
			printf("OHLC data store allocated\n");
		}
		// deque version:
		// if (env->ticker.price_mode == MODE_TICK) {
		// 	env->ticks->epoch->clear();
		// 	env->ticks->ask->clear();
		// 	env->ticks->bid->clear();
		// 	env->ticks->volume->clear();
		// 	delete(env->ticks->epoch);
		// 	delete(env->ticks->ask);
		// 	delete(env->ticks->bid);
		// 	delete(env->ticks->volume);
		// 	free(env->ticks);
		// } else if (env->ticker.price_mode == MODE_OHLC) {
		// 	env->ohlc->epoch->clear();
		// 	env->ohlc->open->clear();
		// 	env->ohlc->high->clear();
		// 	env->ohlc->low->clear();
		// 	env->ohlc->close->clear();
		// 	env->ohlc->volume->clear();
		// 	delete (env->ohlc->epoch);
		// 	delete (env->ohlc->open);
		// 	delete (env->ohlc->high);
		// 	delete (env->ohlc->low);
		// 	delete (env->ohlc->close);
		// 	delete (env->ohlc->volume);			
		// 	free(env->ohlc);
		// }
	}
	int			env(t_env *env)
	{
		//empty all envirnment components then free it
		deinit::data(env);
		//deinit::ohlc_data(env->data);
		// delete(&env->acc.order_book.active);
		// delete(&env->acc.order_book.history);
		// delete(&env->thread_yields);
		delete(&env->thread_pool);
		// delete(env->acc);
		// delete(env->risk);
		// delete(env->ticker);
		delete(env);
		return (1);
	}
}
