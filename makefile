CPCC=g++
CFLAGS=-std=c++11 -Ofast
mws : main.cpp mapping.h
	$(CPCC) main.cpp -o mws $(CFLAGS)