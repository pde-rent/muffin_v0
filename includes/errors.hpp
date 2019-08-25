#pragma once

#define DEFAULT 0

//error codes defines
#define ERR_1 "Wrong parameter count, 2 parameters expected"
#define ERR_2 "Failed to open historical file, please check your path"
#define ERR_3 "Failed to open config file, please check your path"
#define USAGE "Usage: ./muffin [csv_file] [config_file]"

int		put_error(std::string str, int err_code);
