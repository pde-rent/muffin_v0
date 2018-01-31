#pragma once

#define _CRT_SECURE_NO_WARNINGS
//external inclusions
# include <cstdint>		//advanced int typedefs
# include <climits>     //advanced type limits defines
# include <string>
//# include <tchar.h>     //ANSI, DBCS, or Unicode strings
# include <cstdio>		//C inherited i/o library
# include <cstdlib>		//C inherited multi-usage stdlib
# include <cstring>		//C inherited string operations
# include <cmath>		//C inherited maths library
# include <deque>		//C++ vectorization lib
# include <chrono>

//Project specific inclusions
//# include "targetver.h"	//Windows specific header
# include "env.h"			//environement header
# include "muffin.h"		//all other functions
# include "indicators.h"	//indicators header
# include "err_codes.h"	//indicators header

//Malloc related defines
# define IS_INT(x) if (!x) return (0);
# define IS_CHAR(x) if (!x) return (NULL);
# define IS_VOID(x) if (!x) return ;

//Bool related defines
# define True 1
# define False 0
