# Arduino Oscilloscope

This project is going to implement a simple oscilloscope using an Arduino Mega 2560. The oscilloscope samples up to 8 ADC channels and streams the data to a PC for visualization.

## Features

- Timed sampling of up to 8 channels on the ADC
- Configurable sampling frequency
- Continuous sampling mode
- Buffered sampling mode with trigger conditions
- Asynchronous serial communication using UART interrupts

## Components

- Arduino Mega 2560
- whatever sensor to test the oscilloscope (simple wires are also ok)

## Project Structure

- Arduino/: Contains the Arduino code for the oscilloscope.
- Linux/: Contains the PC client code for controlling the oscilloscope and visualizing the data.

## prerequisites

- linux

    ```sh
    sudo apt-get install arduino arduino-mk gnuplot make
    ```

- arch

    ```sh
    sudo pacman -S arduino gnuplot make
    ```

## Compilation

The following command works boyh in main directory (`arduino_oscilloscope`) to compile both Arduino and Linux, and in the single directories (`Linux` or `Arduino`) to compile the single part of the project.

- only compiling

    ```sh
    make build
    ```
- only clean

    ```sh
    make clean
    ```

- clean + build + run

    ```sh
    make
    ```

## Runing the code

- run with default values:

    ```sh
    make run
    ```

- run with certain values:

    ```sh
    make run <device> <baudrate> [blocking]
    ```

    default vaules are 
    - device:   /dev/ttyUSB0
    - baudrate: 9600
    - blocking: true