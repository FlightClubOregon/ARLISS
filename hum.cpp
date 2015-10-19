#include <iostream>
#include <unistd.h>
#include "mraa.h"
#include <stdio.h>
#include "math.h"
#include "hum.h"
#include "fstream"
#include "cstring"

using namespace std;

unsigned char Hum_H;
unsigned char Hum_L;
unsigned char Temp_H;
unsigned char Temp_L;

Hum::Hum()
{
      strcpy(humName, "not name");
}

Hum::Hum(char tempHumName[])
{
      strcpy(humName, tempHumName);
}

Hum::~Hum()
{

}

void Hum::humRead()
{
    mraa_init();

    ofstream outDataTemp, outDataHum;
    bool fileOpen;








      mraa_i2c_context i2c;
      i2c = mraa_i2c_init(1);

      mraa_i2c_address(i2c, 0x27); //set the i2c address to 0x27 (given in the sensor datasheet)
      mraa_i2c_write_byte(i2c, 0x27); //Write a single byte to the address 0x27.
      	  	  	  	  	  	  	  	  //The sensor wants its address and then a write bit (0).
      	  	  	  	  	  	  	  	  // This triggers it to take a data measurement.
      	  	  	  	  	  	  	  	  // I'm assuming the write byte() function is sending the write bit like Wire.h does

      usleep(75000); // sleep for 100 milliseconds. The data measurement on the sensor itself takes about 40ms

      uint8_t rx_tx_buf[4]; // Create a buffer for the i2c read data.
      	  	  	  	  	  	  // If you hover over, you can see how uint8_t is typedef'd to unsigned char variable type
      	  	  	  	  	  	  // [4] specifies 4 bytes-worth of data in the buffer, I stole this from Intel_i2c_GitHub example, line 87

      mraa_i2c_address(i2c, 0x27); // Sets the i2c slave address to 0x27 again.
      	  	  	  	  	  	  	  	  // The docs say to use this function before every i2c command
      mraa_i2c_read(i2c, rx_tx_buf, 4); // mraa read function. Reads from the i2c slave, stores the data in the buffer variable, reads 4 bytes-worth of data

      //cout << endl; //Print an end line character (makes a line space)

      Hum_H = rx_tx_buf[0]; // Set unsigned char variable "Hum_H" to the first byte data of the buffer

      Hum_L = rx_tx_buf[1]; // Set unsigned char variable "Hum_H" to the second byte data of the buffer

      Temp_H = rx_tx_buf[2]; // Set unsigned char variable "Hum_H" to the third byte data of the buffer

      Temp_L = rx_tx_buf[3]; // Set unsigned char variable "Hum_H" to the fourth byte data of the buffer

      double humidity; //Declare the humidity variable with decimal point type variable (double ~ decimal)
      humidity = (((unsigned int) (Hum_H & 0x3f) << 8) | Hum_L) * 100 / (pow(2,14) - 1); //Math to relate byte data to actual Humidity %


      double temperature; //Declare the temperature variable with decimal point type variable (double ~ decimal)
      temperature = (((unsigned int) (Temp_H << 6) + (Temp_L >> 2)) / (pow(2, 14) - 1) * 165 - 40); //Math to relate byte data to actual Temperature


      outDataHum.open("hum.txt", fstream::in | fstream::out | fstream::app);
      outDataHum << humidity << endl; //Write the calculated humidity variable to the file, plus a line space
      outDataHum.close();

	  outDataTemp.open("temp.txt", std::ios::out |std::ios::app);
      outDataTemp << temperature << endl; //Write the calculated temperature variable to the file, plus a line space
      outDataTemp.close();



}
