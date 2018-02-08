#pragma once

#include "stdafx.h"

namespace on_deque
{
	//trend
	template<class T>
	T SMA(std::deque<T, auto> *dq, int period, int shift = 0) //single mode
	{
		static T sum = 0;
		if (!sum)	//compute the whole sum
		{
			sum = dq->at(shift);
			period += shift;
			while (++shift < (period))
				sum += dq->at(shift);
		}
		else            //if the cache exists, no need to compute the whole sum
		{
			sum -= dq->at(shift + period);	//take of oldest price
			sum += dq->at(shift);				//add up last price
		}
		return (sum / (T)period);
		/* WORKS WITH VECTORS BUT NOT WITH DEQUE (contiguous allocation)
		std::deque<float>::iterator last = data->px->begin(); //auto could be use for readability
		double	sum = *last;

		w hile (shift){ ++last; --shift; }
		while ((++shift < period) && (sum += *last)) ++last;
		printf("start: %zu sum: %f\n", *(data->epoch->begin()), sum);
		return (sum/(double)period);
		*/
	}
	template<class T>
	T EMA(std::deque<T> *dq, int period, int shift)
	{
		static T prev = 0;
		if (!prev) prev = SMA<T>(dq, period, shift);
		return ((prev = ((dq->at(shift) + prev) / (T)period)));
	}
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
	double ATR(t_tick_data *data, int p, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
	double SDEV(t_tick_data *data, int p, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
}
namespace on_array
{
	//trend
	double SMA(t_tick_data *data, int p, int shift = 0); //single mode
	double EMA(t_tick_data *data, int p, int shift = 0); //single mode
	/*
	double LWMA(t_tick_data *data, int p, int shift); //single mode
	double HMA(t_tick_data *data, int p, int shift); //single mode
	//volatility
	double ATR(t_tick_data *data, int p, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
	double SDEV(t_tick_data *data, int p, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
	*/
}
