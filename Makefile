# Makefile build 1.0.0
# by Paul de Renty

NAME    	= muffin
SRC_PATH	= srcs/
OBJ_PATH	= objs/
FLAGS 		= -std=c++17# version
FLAGS		+= -Wall -Werror -Wextra# -Wshadow -Wnon-virtual-dtor #error flags
#FLAGS		+= -pedantic -ainsi #normative flags
FLAGS		+= -g# debugging
FLAGS		+= -fopenmp # OPENMP multithreading preprocessors flag
#FLAGS		+= -O0 #optimisation flag (up to -O2, max unrolling and inlining)
CC			= g++ $(FLAGS)
NAME_P		= $(shell echo $(NAME) | tr ' ' '\n' | sed "s/\.[acoh]$///g" | tr '\n' ' ' | sed "s/ $///g")
SRC_SUFFIX	= .cpp
COMMON =	main \
			error \
			parser_str \
			parser_time \
			init \
			compliance \
			files \
			get_config \
			indicators \
			orders \
			strategy \
			tick_consumer \
			ohlc_consumer \
			tick_producer \
			ohlc_producer

SRC_FILES = $(addsuffix $(SRC_SUFFIX),$(COMMON))
OBJ_FILES = $(SRC_FILES:.cpp=.o)
SRC		= $(addprefix $(SRC_PATH),$(SRC_FILES))
OBJ		= $(addprefix $(OBJ_PATH),$(OBJ_FILES))

#color
YELLOW		= "\\033[33m"
BLUE		= "\\033[34m"
RED			= "\\033[31m"
WHITE		= "\\033[0m"
CYAN		= "\\033[36m"
GREEN		= "\\033[32m"
BOLD		= "\\033[1m"
PINK		= "\\033[95m"

#command
EOLCLR		= "\\033[0K"
#unicode
CHECK		= "✔️"#"\\xE2\\x9C\\x94"
OK			= " $(CYAN)$(CHECK)$(WHITE)"

all : $(NAME)

$(NAME) : $(OBJ)
	@printf "\r$(EOLCLR)[$(NAME_P)] >>>>>>>>>>>>>>\t$(YELLOW)$(BOLD)"\
	"$(NAME_P) compiled\t\t"$(OK)'\n'
	@$(CC) -I./includes $(OBJ) -o $@
	@printf "\r$(EOLCLR)[$(NAME_P)] >>>>>>>>>>>>>>\t$(GREEN)$(BOLD)"\
	"build successful\t"$(OK)'\n'

$(OBJ) : | $(OBJ_PATH)

$(OBJ_PATH) : 
	@mkdir -p $(OBJ_PATH)

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@printf "\r$(EOLCLR)[$(NAME_P)] compiling\t$(BOLD)$(YELLOW)$<$(WHITE)"
	@$(CC) -I./includes -o $@ -c $<
	@printf '\t'$(OK)

clean :
	@printf "[$(NAME_P)] removing\t$(PINK)all obj file$(WHITE)"
	@rm -rf $(OBJ_PATH)
	@printf '\t\t'$(OK)'\n'

fclean : clean
	@printf "[$(NAME_P)] erasing\t$(PINK)$(NAME)$(WHITE)"
	@rm -f $(NAME)
	@printf '\t\t\t'$(OK)'\n'
# 	@rm -rf muffin.dSYM

re : fclean all

.PHONY: all clean fclean re norm
