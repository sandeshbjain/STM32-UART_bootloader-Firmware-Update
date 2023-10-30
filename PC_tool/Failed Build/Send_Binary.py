import serial
import time
# Configure the serial port
ser = serial.Serial('COM11', 115200)  # Change 'COM1' to your specific port and baud rate

# Open the binary file for reading
with open('Firmware_Update_Binary.bin', 'rb') as file:
    while True:
        packet = file.read(1024)  # Read 128 bytes at a time (adjust packet size as needed)
        if not packet:
            break  # End of file
        ser.write(packet)  # Send the packet over UART
        time.sleep(0.1)

ser.close()
