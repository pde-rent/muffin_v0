#pragma once

// generic default
#define MODE_DEFAULT			0

// price modes
#define MODE_OHLC				0
#define MODE_TICK				1

// order trade entry modes
#define MODE_BUY				1
#define MODE_SELL				-1
#define MODE_BUY_LIMIT			2
#define MODE_SELL_LIMIT			-2
#define MODE_BUY_STOP			3
#define MODE_SELL_STOP			-3

// order trade status modes
#define MODE_OPEN				0
#define MODE_ACTIVE				0 // same as above
#define MODE_CLOSED				1
// #define MODE_HISTORY			1 // ""
#define MODE_CANCELED			2
#define MODE_EXPIRED			3
#define MODE_TICKET_CHANGED		4
#define MODE_EMPTY_SLOT		5

// order trade exit sort modes
#define MODE_FIFO				0
#define MODE_LIFO				1
#define MODE_RANDOM				2

// order trade exit batch modes
#define MODE_CLOSE_SINGLE		0
#define MODE_CLOSE_ALL			1
#define MODE_CLOSE_ALL_LONG		2
#define MODE_CLOSE_ALL_SHORT	3

//tp / sl types defines
#define MODE_TP_NONE			0
#define MODE_TP_PRICE			1
#define MODE_TP_PCT_TICKER		2
#define MODE_TP_PCT_ACC			3
#define MODE_SL_NONE			0
#define MODE_SL_PRICE			1
#define MODE_SL_PCT_TICKER		2
#define MODE_SL_PCT_ACC			3
