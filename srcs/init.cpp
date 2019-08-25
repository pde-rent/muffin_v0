#include "main.hpp"

namespace init
{
	int data(t_env *env)
	{
		if (env->ticker->price_mode == MODE_TICK) {
			env->ticks = (t_tick_data *)malloc(sizeof(t_tick_data));
			env->ticks->epoch = new std::deque<size_t>(0);
			env->ticks->bid = new std::deque<double>(0);
			env->ticks->ask = new std::deque<double>(0);
			env->ticks->volume = new std::deque<double>(0);
			env->price = env->ticks->bid;
			env->epoch = env->ticks->epoch;
		} else if (env->ticker->price_mode == MODE_OHLC) {
			env->ohlc = (t_ohlc_data *)malloc(sizeof(t_tick_data));
			env->ohlc->epoch = new std::deque<size_t>(0);
			env->ohlc->open = new std::deque<double>(0);
			env->ohlc->high = new std::deque<double>(0);
			env->ohlc->low = new std::deque<double>(0);
			env->ohlc->close = new std::deque<double>(0);
			env->ohlc->volume = new std::deque<double>(0);
			env->price = env->ohlc->close;
			env->epoch = env->ohlc->epoch;
		}
		return (1);
	}
	// static t_ohlc_data		*ohlc_data(void)
	// {
	// 	t_ohlc_data	*data;
	// 	data = (t_ohlc_data *)malloc(sizeof(t_ohlc_data));
	// 	return (data);
	// }
	// already initialized on stack (not malloc)
	// static int account(t_account *acc)
	// {
	// 	return (1);
	// }
	// static int ticker(t_ticker *ticker)
	// {
	// 	return (1);
	// }
	// static int risk(t_risk *risk)
	// {
	// 	return (1);
	// }
	int env(t_env *env)
	{
		// initiate parallel backtesting order books
		for (int pass = 0; pass < MAX_PASS; pass++)
		{
			env->order_book[pass] = new std::deque<t_order>(0);
			env->order_history[pass] = new std::deque<t_order>(0);
			NB_LONG(pass) = 0;
			NB_SHORT(pass) = 0;
		}
		//param_parse("...");
		//t_tick_data *data = static_cast<t_tick_data*>(histo); //shall result depend on parameters parsing
		return (1);
	}
	void	split_process(t_env *env)
	{
		#pragma omp parallel sections
		{
			#pragma omp section		// data producing thread
			{
				switch (env->ticker->price_mode) {
					case MODE_OHLC:
						stream::ohlc::producer::generate(env);
						break;
					case MODE_TICK:
						stream::tick::producer::generate(env);
						break;
				}
			}
			#pragma omp section		//strategy::run backtester thread
			{
				switch (env->ticker->price_mode) {
					case MODE_OHLC:
						stream::ohlc::consumer::listen(env);
						break;
					case MODE_TICK:
						stream::tick::consumer::listen(env);
						break;
				}
			}
		}
	}
}

namespace deinit
{
	static void	data(t_env *env)
	{
		if (env->ticker->price_mode == MODE_TICK) {
			env->ticks->epoch->clear();
			env->ticks->ask->clear();
			env->ticks->bid->clear();
			env->ticks->volume->clear();
			delete(env->ticks->epoch);
			delete(env->ticks->ask);
			delete(env->ticks->bid);
			delete(env->ticks->volume);
			free(env->ticks);
		} else if (env->ticker->price_mode == MODE_OHLC) {
			env->ohlc->epoch->clear();
			env->ohlc->open->clear();
			env->ohlc->high->clear();
			env->ohlc->low->clear();
			env->ohlc->close->clear();
			env->ohlc->volume->clear();
			delete (env->ohlc->epoch);
			delete (env->ohlc->open);
			delete (env->ohlc->high);
			delete (env->ohlc->low);
			delete (env->ohlc->close);
			delete (env->ohlc->volume);			
			free(env->ohlc);
		}
	}
	int			env(t_env *env)
	{
		//empty all envirnment components then free it
		deinit::data(env);
		//deinit::ohlc_data(env->data);
		delete(&env->order_book);
		delete(&env->order_history);
		return (1);
	}
}
