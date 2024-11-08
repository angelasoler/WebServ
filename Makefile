NAME			= webserv
FLAGS			= -Wall -Wextra -Werror -std=c++98 -g
SRC_PATH		= src/
SRC_FILES		= $(wildcard $(SRC_PATH)*.cpp)
OBJ_PATH		= obj/
OBJ_FILES		= $(SRC_FILES:%.cpp=$(OBJ_PATH)%.o)
HEADER_FILES	= $(wildcard includes/*.hpp)

all: $(NAME)

$(NAME): $(OBJ_FILES)
	c++ $(FLAGS) -I./includes $^ -o $(NAME)

$(OBJ_PATH)%.o : %.cpp $(HEADER_FILES)
	mkdir -p $(dir $@)
	c++ $(FLAGS) -c $< -o $@ -I includes

run: $(NAME)
	make re
	clear && ./$(NAME)

v: valgrind

valgrind: $(NAME)
	clear
	valgrind --show-leak-kinds=all --leak-check=full --track-fds=yes ./$(NAME) config.conf

git:
	make fclean
	git add .
	git status
	@read -p "Digite a mensagem do commit: " MESSAGE; \
	git commit -m "$$MESSAGE"

clean:
	@rm -rf $(OBJ_PATH)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

resetLogs:
	@rm -rf logs
	@mkdir logs

kill-process:
	@PID=$$(ps aux | grep $(NAME) | grep -v grep | awk '{print $$2}' | head -n 1); \
	if [ -z "$$PID" ]; then \
	  echo "Processo '$(NAME)' não encontrado!"; \
	else \
	  echo "Matando o processo '$(NAME)' com PID: $$PID"; \
	  kill -9 $$PID; \
	fi

.PHONY: all clean fclean re run git valgrind v resetLogs kill-process
