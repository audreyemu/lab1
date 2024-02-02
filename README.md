## UID: 205997572

## Pipe Up

Pipe up is a program that performs like the pipe operator in linux by connecting the inputs/outputs between up to 8 different processes.

## Building

Simply run the ```make``` command to build this program

## Running

To run this program, run ./pipe followed by up to 8 (at least 1) arguments representing additional processes. Running ```./pipe ls cat wc``` should return something like ```7     7     63``` which is the same output produced by ```ls|cat|wc```

## Cleaning up

To clean up all binary files, run ```make clean```
