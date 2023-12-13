#!/usr/bin/env python3

import serial
import matplotlib.pyplot as plt

def main():
    # Open the serial port
    ser = serial.Serial('/dev/ttyUSB0', 115200)  # Adjust the port and baud rate accordingly

    # Initialize variables for plotting
    num_samples = 88200  # Adjust as needed
    samples = []

    # Read data from serial port
    for _ in range(num_samples):
        data = ser.readline()
        samples.append(data)

    # Close the serial port
    ser.close()

    # Plot the samples
    plt.plot(samples)
    plt.title('Audio Samples from INMP441 Microphone')
    plt.xlabel('Sample Index')
    plt.ylabel('Amplitude')
    plt.show()

if __name__ == "__main__":
    main()
