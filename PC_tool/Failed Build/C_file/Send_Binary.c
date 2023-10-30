#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <serialport.h>

int main() {
    const char* portname = "COM11";  // Change to your specific port
    const int baudrate = 115200;
    const char* filename = "Firmware_Update_Binary.bin";

    sp_port *serial_port;
    sp_return error;

    // Open the serial port
    error = sp_get_port_by_name(portname, &serial_port);
    if (error != SP_OK) {
        fprintf(stderr, "Error finding the serial port\n");
        return 1;
    }

    error = sp_open(serial_port, SP_MODE_WRITE);
    if (error != SP_OK) {
        fprintf(stderr, "Error opening the serial port\n");
        return 1;
    }

    sp_set_baudrate(serial_port, baudrate);
    sp_set_dtr(serial_port, SP_DTR_ON);

    // Open the binary file for reading
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening the binary file\n");
        return 1;
    }

    // Send the start command
    sp_blocking_write(serial_port, "START", 5, 1000);

    // Send the file packet by packet
    size_t packet_size;
    uint8_t packet[1024];
    while ((packet_size = fread(packet, 1, sizeof(packet), file)) > 0) {
        sp_blocking_write(serial_port, packet, packet_size, 1000);
    }

    // Send the end command
    sp_blocking_write(serial_port, "END", 3, 1000);

    // Close the file and serial port
    fclose(file);
    sp_close(serial_port);
    sp_free_port(serial_port);

    return 0;
}
