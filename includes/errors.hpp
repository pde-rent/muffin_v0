#pragma once

#define DEFAULT 0

// error codes defines
#define ERR_NOT_ENOUGH_ARGS		"Wrong parameter count, 2 parameters expected"
#define ERR_NO_HISTO_FILE		"Failed to open historical file, please check your path"
#define ERR_NO_CONFIG_FILE		"Failed to open config file, please check your path"

// usage
#define ERR_USAGE				"Usage: ./muffin [config_file]"

int		put_error(std::string str, int err_code);
