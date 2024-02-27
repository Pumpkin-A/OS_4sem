#!/bin/bash
g++ -o child child.cpp -lpthread
g++ -o parent parent.cpp -lpthread
./parent
