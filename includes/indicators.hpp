#pragma once

#include "main.hpp"

namespace on_deque
{
	//trend
	template<class T>
	T SMA(std::deque<T> *price, int period, int shift = 0, int magic = 0) //single mode
	{
		static T sum[MAX_PASS] = {0};
		period += shift;
		if (!sum[magic])	//compute the whole sum
		{
			sum[magic] = price->at(shift);
			while (++shift < (period))
				sum[magic] += price->at(shift);
		}
		else            //if the cache exists, no need to compute the whole sum
		{
			//printf("tick: %f period: %f\n", price->at(shift), price->at(period));
			sum[magic] += (price->at(shift) - price->at(period)); //take of oldest price and add up last price
		}
		//printf("+ %f - %f\n", price->at(shift), price->at(period));
		return (sum[magic] / (T)(period - shift));
	}
	template<class T>
	T MA_UP(std::deque<T> *price, int period, int shift = 0, int magic = 0) //single mode
	{
		static T prev[MAX_PASS] = {};
		T x = 2 / ((T)period + 1);
		if (!prev[magic])	//compute the UP SMA
		{
			T sum = 0;
			sum = price->at(shift);
			while (++shift < (period))
				if (price->at(shift) > price->at(shift + 1))
					sum += (price->at(shift) - price->at(shift + 1));
			return ((prev[magic] = sum / period));
		}
		else if (price->at(shift) > price->at(shift + 1))
			return ((prev[magic] = (((price->at(shift) - price->at(shift + 1)) * x) + (prev[magic] * (1 - x))))); //take of oldest price and add up last price
		return ((prev[magic] = (prev[magic] * (1 - x))));
	}
	template<class T>
	T MA_DOWN(std::deque<T> *price, int period, int shift = 0, int magic = 0) //single mode
	{
		static T prev[MAX_PASS] = {};
		T x = 2 / ((T)period + 1);
		if (!prev[magic])	//compute the UP SMA
		{
			T sum = 0;
			sum = price->at(shift);
			while (++shift < (period))
				if (price->at(shift) < price->at(shift + 1))
					sum += (price->at(shift + 1) - price->at(shift));
			return ((prev[magic] = sum / period));
		}
		else if (price->at(shift) < price->at(shift + 1))
			return ((prev[magic] = (((price->at(shift + 1) - price->at(shift)) * x) + (prev[magic] * (1 - x))))); //take of oldest price and add up last price
		return ((prev[magic] = (prev[magic] * (1 - x))));
	}
	/*ALTERNATIVE WAY TO SMA CALCULATIONS(SLOWER but cleaner)
	template<class T>
	T SMA(std::deque<T> *price, int period, int shift = 0) //single mode
	{
		T sum = 0;
		for (std::deque<T>::iterator last = price->begin() + shift; last != price->begin() + shift + period; ++last)
			sum += *last;
		return (sum / (T)period);
	}
	*/
	template<class T>
	T EMA(std::deque<T> *price, int period, int shift = 0, int magic = 0)	//EXPONENTIAL MA
	{
		static T prev[MAX_PASS] = {};
		T x = 2 / ((T)period + 1);
		if (!prev[magic])
			prev[magic] = SMA<T>(price, period, shift);
		return ((prev[magic] = ((price->at(shift) * x) + (prev[magic] * (1 - x)))));
	}
	template<class T>
	T WWMA(std::deque<T> *price, int period, int shift = 0, int magic = 0) //WELLES WILDERS MA
	{
		static T prev[MAX_PASS] = {};
		T x = 1 / ((T)period);
		if (!prev[magic])
			prev[magic] = SMA<T>(price, period, shift);
		return ((prev[magic] = ((price->at(shift) * x) + (prev[magic] * (1 - x)))));
	}
	/*
	template<class T>
	T LWMA(std::deque<T> *price, int period, int shift)	//LINEAR WEIGHTED MA
	{
	}
	template<class T>
	T HEMA(std::deque<T> *price, int period, int shift)	//HULL MA
	{
	}
	template<class T>
	T DEMA(std::deque<T> *price, int period, int shift)	//DOUBLE EXP SMOOTHING MA
	{
	}
	template<class T>
	T TEMA(std::deque<T> *price, int period, int shift)	//TRIPLE EXP SMOOTHING MA
	{
	}
	template<class T>
	T LSMA(std::deque<T> *price, int period, int shift)	//LEAST SQUARES MA
	{
	}
	template<class T>
	T WMA(std::deque<T> *price, int period, int shift)	//WEIGHTED MA
	{
	}
	double MACD(t_tick_data *data, int fast_p, int slow_p, int signal_p, int shift); //modes fast/slow/signal
	//oscillators
	double ADX(t_tick_data *data, int period, int shift, int mode); //modes DI+/DI-
	double ADXMA(t_tick_data *data, int period, int shift, int mode);
	double RSI(t_tick_data *data, int period, int shift); //single mode
	double MFI(t_tick_data *data, int period, int shift); //single mode
	double CCI(t_tick_data *data, int period, int shift); //single mode
	double STOCH(t_tick_data *data, int k_p, int d_p, int slow_p, int mode, int shift);
	double QQE(t_tick_data *data, int period, int sf_p, double wp, int shift);
	*/
	//volatility
	template<class T>
	double ATR(std::deque<T> *price, std::deque<T> *volume, int period, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
	template<class T>
	double SDEV(std::deque<T> *price, int period, int shift = 0, int ma_type = 1); //modes SMA, EMA ...
	//oscillator
	template<class T>
	T RSI(std::deque<T> *price, int period, int shift = 0, int magic = 0) //single mode
	{
		
		static T prev_up[MAX_PASS] = {};
		static T prev_down[MAX_PASS] = {};
		T x = 2 / ((T)period + 1);
		if (!prev_down[magic] && !prev_up[magic]) //first RS calculation (RS = average up over N / average down over N)
		{
			prev_up[magic] = MA_UP<T>(price, period, shift, magic);
			prev_down[magic] = MA_DOWN<T>(price, period, shift, magic);
		}
		else if (price->at(shift) > price->at(shift + 1)) //positive diff
		{
			prev_up[magic] = prev_up[magic] * (1 - x) + (price->at(shift) - price->at(shift + 1)) * x;
			prev_down[magic] *= (1 - x);
		}
		else if (price->at(shift) < price->at(shift + 1))	//negative diff
		{
			prev_down[magic] = prev_down[magic] * (1 - x) + (price->at(shift + 1) - price->at(shift)) * x;
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

// array function calls are 
namespace on_array
{
	//trend
	// printf("ASK: %f MA: %f\n", ASK(0), MA1[pass]);
	// printf("MA[%d]=%f PMA[%d]=%f PPMA[%d]=%f\n"
		// , pass, MA1[pass], pass, PMA1[pass], pass, PPMA1[pass]);
	template<class T>
	T SMA(T price[], int period, int shift = 0, int magic = 0) //single mode
	{
		static T sum[MAX_PASS] = {0};
		period += shift;
		if (!sum[magic])	//compute the whole sum
		{
			sum[magic] = price[shift];
			while (++shift < (period))
				sum[magic] += price[shift];
		}
		else            //if the cache exists, no need to compute the whole sum
		{
			//printf("tick: %f period: %f\n", price->at(shift), price->at(period));
			sum[magic] += (price[shift] - price[period]); //take of oldest price and add up last price
		}
		//printf("+ %f - %f\n", price->at(shift), price->at(period));
		return (sum[magic] / (T)(period - shift));
	}
}

// set the indicators namespace to on_array
namespace indicators
{
	using namespace on_array;
}
