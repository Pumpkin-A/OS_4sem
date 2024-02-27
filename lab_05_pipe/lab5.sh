#!/bin/bash
g++ -o pipe pipe.cpp -lpthread
g++ -o pipe2 pipe2.cpp -lpthread
g++ -o pipeWithFcntl pipeWithFcntl.cpp -lpthread