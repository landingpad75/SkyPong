all:
	g++ src/**/*.cpp src/**/**/**/*.cpp src/**/**/*.cpp -o out/pong -L./lib -I./includes --std=c++17 -lraylib -ldiscord-rpc -lpthread