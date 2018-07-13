from os import system

system('''
g++ -std=c++11 -Wall main.cpp -o main \
-I/home/dennis/Programs/C/lib/dennis/motor \
-L/home/dennis/Programs/C/lib/dennis/motor \
-lwiringPi -lmotor
''')