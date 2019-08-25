#pragma once

//HISTORICAL DATA READING/LOADING BUFFERS
#define BUFF_SIZE			(1024 * 16)
#define OHLC_BUFF_SIZE		1001
#define TICK_BUFF_SIZE		50001 // at least 100* more memory required than ohlc data for similar term

//INDICATORS PARAMETERS >> using ohlc
//indicators need to have much bigger calculation period for tick data (> *50)
#define NB_INDICATORS		1
#define MAX_PASS			50   // maximum 50 combo tested
// OHLC test:
#define MIN_MA1				10   // 10 minutes if ohlc frequency = 1min
#define MAX_MA1				720  // 12 hours if ...
// TICK test:
// #define MIN_MA1				1000
// #define MAX_MA1				50000
