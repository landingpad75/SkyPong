all:
	g++ src/**/*.cpp src/**/**/**/*.cpp src/**/**/*.cpp -o out/pong -L./lib -I./includes -lraylib -ldiscord-rpc -Wall -Wextra