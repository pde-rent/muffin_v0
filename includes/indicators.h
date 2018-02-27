#pragma once

#include "stdafx.h"

namespace on_deque
{
	//trend
	template<class T>
	T SMA(std::deque<T> *list, int period, int shift = 0, int magic = 0) //single mode
	{
		static T sum[MAX_PASS] = {0};
		period += shift;
		if (!sum[magic])	//compute the whole sum
		{
			sum[magic] = list->at(shift);
			while (++shift < (period))
				sum[magic] += list->at(shift);
		}
		else            //if the cache exists, no need to compute the whole sum
		{
			//printf("tick: %f period: %f\n", list->at(shift), list->at(period));
			sum[magic] += (list->at(shift) - list->at(period)); //take of oldest price and add up last price
		}
		//printf("+ %f - %f\n", list->at(shift), list->at(period));
		return (sum[magic] / (T)(period - shift));
	}
	template<class T>
	T MA_UP(std::deque<T> *list, int period, int shift = 0, int magic = 0) //single mode
	{
		static T prev[MAX_PASS] = {};
		T x = 2 / ((T)period + 1);
		if (!prev[magic])	//compute the UP SMA
		{
			T sum = 0;
			sum = list->at(shift);
			while (++shift < (period))
				if (list->at(shift) > list->at(shift + 1))
					sum += (list->at(shift) - list->at(shift + 1));
			return ((prev[magic] = sum / period));
		}
		else if (list->at(shift) > list->at(shift + 1))
			return ((prev[magic] = (((list->at(shift) - list->at(shift + 1)) * x) + (prev[magic] * (1 - x))))); //take of oldest price and add up last price
		return ((prev[magic] = (prev[magic] * (1 - x))));
	}
	template<class T>
	T MA_DOWN(std::deque<T> *list, int period, int shift = 0, int magic = 0) //single mode
	{
		static T prev[MAX_PASS] = {};
		T x = 2 / ((T)period + 1);
		if (!prev[magic])	//compute the UP SMA
		{
			T sum = 0;
			sum = list->at(shift);
			while (++shift < (period))
				if (list->at(shift) < list->at(shift + 1))
					sum += (list->at(shift + 1) - list->at(shift));
			return ((prev[magic] = sum / period));
		}
		else if (list->at(shift) < list->at(shift + 1))
			return ((prev[magic] = (((list->at(shift + 1) - list->at(shift)) * x) + (prev[magic] * (1 - x))))); //take of oldest price and add up last price
		return ((prev[magic] = (prev[magic] * (1 - x))));
	}
	/*ALTERNATIVE WAY TO SMA CALCULATIONS(SLOWER but cleaner)
	template<class T>
	T SMA(std::deque<T> *list, int period, int shift = 0) //single mode
	{
		T sum = 0;
		for (std::deque<T>::iterator last = list->begin() + shift; last != list->begin() + shift + period; ++last)
			sum += *last;
		return (sum / (T)period);
	}
	*/
	template<class T>
	T EMA(std::deque<T> *list, int period, int shift = 0, int magic = 0)	//EXPONENTIAL MA
	{
		static T prev[MAX_PASS] = {};
		T x = 2 / ((T)period + 1);
		if (!prev[magic])
			prev[magic] = SMA<T>(list, period, shift);
		return ((prev[magic] = ((list->at(shift) * x) + (prev[magic] * (1 - x)))));
	}
	template<class T>
	T WWMA(std::deque<T> *list, int period, int shift = 0, int magic = 0) //WELLES WILDERS MA
	{
		static T prev[MAX_PASS] = {};
		T x = 1 / ((T)period);
		if (!prev[magic])
			prev[magic] = SMA<T>(list, period, shift);
		return ((prev[magic] = ((list->at(shift) * x) + (prev[magic] * (1 - x)))));
	}
	/*
	template<class T>
	T LWMA(std::deque<T> *list, int period, int shift)	//LINEAR WEIGHTED MA
	{
	}
	template<class T>
	T HEMA(std::deque<T> *list, int period, int shift)	//HULL MA
	{
	}
	template<class T>
	T DEMA(std::deque<T> *list, int period, int shift)	//DOUBLE EXP SMOOTHING MA
	{
	}
	template<class T>
	T TEMA(std::deque<T> *list, int period, int shift)	//TRIPLE EXP SMOOTHING MA
	{
	}
	template<class T>
	T LSMA(std::deque<T> *list, int period, int shift)	//LEAST SQUARES MA
	{
	}
	template<class T>
	T WMA(std::deque<T> *list, int period, int shift)	//WEIGHTED MA
	{
	}
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
	float ATR(t_tick_data *data, int p, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
	float SDEV(t_tick_data *data, int p, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
	//oscillator
	template<class T>
	T RSI(std::deque<T> *list, int period, int shift = 0, int magic = 0) //single mode
	{
		
		static T prev_up[MAX_PASS] = {};
		static T prev_down[MAX_PASS] = {};
		T x = 2 / ((T)period + 1);
		if (!prev_down[magic] && !prev_up[magic]) //first RS calculation (RS = average up over N / average down over N)
		{
			prev_up[magic] = MA_UP<T>(list, period, shift, magic);
			prev_down[magic] = MA_DOWN<T>(list, period, shift, magic);
		}
		else if (list->at(shift) > list->at(shift + 1)) //positive diff
		{
			prev_up[magic] = prev_up[magic] * (1 - x) + (list->at(shift) - list->at(shift + 1)) * x;
			prev_down[magic] *= (1 - x);
		}
		else if (list->at(shift) < list->at(shift + 1))	//negative diff
		{
			prev_down[magic] = prev_down[magic] * (1 - x) + (list->at(shift + 1) - list->at(shift)) * x;
			prev_up[magic] *= (1 - x);
		}
		else //no diff
		{
			prev_up[magic] *= (1 - x);
			prev_down[magic] *= (1 - x);
		}
		return (100 - (100 / (1 + (prev_up[magic] / prev_down[magic]))));
	}
}
namespace on_array
{
	float SMA(std::deque<float> *list, int period, int shift = 0);
	float EMA(std::deque<float> *list, int period, int shift = 0);
	/*
	float LWMA(t_tick_data *data, int p, int shift); //single mode
	float HMA(t_tick_data *data, int p, int shift); //single mode
	*/
	//volatility
	float ATR(t_tick_data *data, int p, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
	float SDEV(t_tick_data *data, int p, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
}
