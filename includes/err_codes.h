#pragma once

#define DEFAULT 0

//error codes defines
#define ERR_1 "Wrong parameter count, 2 parameters expected\n\0"
#define ERR_2 "Failed to open historical file, please check your path\n\0"
#define ERR_3 "Failed to open config file, please check your path\n\0"
#define USAGE "Usage: ./muffin [csv_file] [config_file]\n\0"

//trade defines
#define OP_BUY 1
#define OP_SELL -1
#define OP_BUY_LIMIT 2
#define OP_SELL_LIMIT -2
#define OP_BUY_STOP 3
#define OP_SELL_STOP -3

//tp / sl types defines
#define NO_TP 0
#define HARD_PRICE_TP 1
#define PCT_TICKER_TP 2
#define PCT_ACC_TP 3
#define NO_SL 0
#define HARD_PRICE_SL 1
#define PCT_TICKER_SL 2
#define PCT_ACC_SL 3
