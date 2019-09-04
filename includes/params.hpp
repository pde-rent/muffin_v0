#pragma once

// config file location (either absolute or relative from working dir)
#define CONFIG_PATH			"./resources/config.yml"
// HISTORICAL DATA READING/LOADING BUFFERS
#define BUFF_SIZE			(1024 * 16)
// DATA LOOKBACK PERIOD
#define OHLC_BUFF_SIZE		1440
#define TICK_BUFF_SIZE		50001 // at least 100* more memory required than ohlc data for similar term
// ORDER BOOK MAX SIZE
#define ORDER_BOOK_SIZE		1024 // no more than 1k orders in memory
// INDICATORS PARAMETERS >> using ohlc
// indicators need to have much bigger calculation period for tick data (> *50)
#define NB_INDICATORS		1
#define MAX_PASS			2   // maximum 50 combo tested
// OHLC test:
#define MIN_MA1				10   // 10 minutes if ohlc frequency = 1min
#define MAX_MA1				64  // 12 hours if ...
// TICK test:
// #define MIN_MA1				1000
// #define MAX_MA1				50000
