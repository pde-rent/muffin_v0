#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

namespace on_deque
{
	/*
	double LWMA(t_tick_data *data, int p, int shift); //single mode
	double HMA(t_tick_data *data, int p, int shift); //single mode
	double MACD(t_tick_data *data, int fast_p, int slow_p, int signal_p, int shift); //modes fast/slow/signal
	//oscillators
	double ADX(t_tick_data *data, int p, int shift, int mode); //modes DI+/DI-
	double ADXMA(t_tick_data *data, int p, int shift, int mode);
	double RSI(t_tick_data *data, int p, int shift); //single mode
	double MFI(t_tick_data *data, int p, int shift); //single mode
	double CCI(t_tick_data *data, int p, int shift); //single mode
	double STOCH(t_tick_data *data, int k_p, int d_p, int slow_p, int mode, int shift);
	double QQE(t_tick_data *data, int p, int sf_p, double wp, int shift);
	*/
	//volatility
	double ATR(t_tick_data *data, int p, int shift, int ma_type) //modes SMA, EMA ...
	{

	}

	double SDEV(t_tick_data *data, int p, int shift, int ma_type) //modes SMA, EMA ...
	{


	}
}
namespace on_array
{

}