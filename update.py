from os import system

system("g++ -std=c++11 -Wall -c motor.cpp -lwiringPi -lm")
system("ar -rc libmotor.a motor.o")