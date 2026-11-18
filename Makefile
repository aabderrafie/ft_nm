CC = cc
CFLAGS = -Wall -Wextra -Werror 


NAME = ft_nm


src = $(wildcard src/*.c)
obj = $(src:.c=.o)

all: $(NAME)

$(NAME): $(obj)
	$(CC)  -o $(NAME) $(obj)


%.o: %.c ft_nm.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(obj)

fclean: clean
	rm -f $(NAME) 

re: fclean all

.PHONY: clean 