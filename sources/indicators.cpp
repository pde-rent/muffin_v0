#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

namespace on_deque
{
	/*
	float LWMA(t_tick_data *data, int p, int shift); //single mode
	float HMA(t_tick_data *data, int p, int shift); //single mode
	float MACD(t_tick_data *data, int fast_p, int slow_p, int signal_p, int shift); //modes fast/slow/signal
	//oscillators
	float ADX(t_tick_data *data, int p, int shift, int mode); //modes DI+/DI-
	float ADXMA(t_tick_data *data, int p, int shift, int mode);
	float RSI(t_tick_data *data, int p, int shift); //single mode
	float MFI(t_tick_data *data, int p, int shift); //single mode
	float CCI(t_tick_data *data, int p, int shift); //single mode
	float STOCH(t_tick_data *data, int k_p, int d_p, int slow_p, int mode, int shift);
	float QQE(t_tick_data *data, int p, int sf_p, float wp, int shift);
	*/
	//volatility
	float ATR(t_tick_data *list, int period, int shift, int ma_type) //modes SMA, EMA ...
	{
		return 0;
	}

	float SDEV(t_tick_data *data, int p, int shift, int ma_type) //modes SMA, EMA ...
	{
		return 0;
	}
}
namespace on_array
{

}