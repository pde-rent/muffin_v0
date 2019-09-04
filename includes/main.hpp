#pragma once

// for msvc
#define _CRT_SECURE_NO_WARNINGS

//external inclusions
#include <cstdint>		//advanced int typedefs
#include <climits>     //advanced type limits definitions
#include <string>
// #include <tchar.h>   //ANSI, DBCS, or Unicode strings
// #include <iostream>	//C inherited i/o library
#include <cstdio>		//C inherited i/o library
#include <cstdlib>		//C inherited multi-usage stdlib
#include <cstring>		//C inherited string operations
#include <string>
#include <ctime>
#include <cmath>		//C inherited maths library
// #include <deque>		//C++ queuing/dequeuing lib // defines in data
#include <chrono>		//C++ chrono
// threading >> omp replaced with std::thread
// #include <omp.h>		//C multiprocessing/threading preprocessor utility
#include <thread>
#include <mutex>
//Project specific inclusions
#include "params.hpp"		//backtester parameters header
#include "modes.hpp"		//MODE_* header

//This project is c style, so structs are used over classes (we gotta make the switch tho)
// structs
// #include "t_worker.hpp"
#include "t_env.hpp"		//env struct
#include "t_ticker.hpp"
#include "t_account.hpp"
#include "t_order.hpp"
#include "t_risk.hpp"
#include "t_data.hpp"
// classes
#include "ThreadPool.hpp"

#include "files.hpp"		//file objects header
#include "parser.hpp"		//parser header
#include "print.hpp"		//print header
#include "config.hpp"		//config header
#include "price_tick.hpp"	//tick objects header
#include "price_ohlc.hpp"	//ohlc objects header
#include "errors.hpp"		//error management header
#include "init.hpp"			//init core functions
#include "orders.hpp"		//orders header
#include "compliance.hpp"	//compliance header
#include "risk.hpp"			//risk header
#include "indicators.hpp"	//indicators header
#include "strategy.hpp"		//strategy header

#ifdef _MSC_VER
#include "targetver.h"	//Windows specific header
#endif

// Malloc related defines
#define IS_INT(x) if (!x) return (0);
#define IS_CHAR(x) if (!x) return (NULL);
#define IS_VOID(x) if (!x) return ;
