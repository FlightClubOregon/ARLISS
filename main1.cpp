
#include "hum.h"
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<mraa.h>
#include"mraa.hpp"
#include"timer.h"
#include <fstream>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include "SparkFunADS1015.h"
#include <string>
#include <time.h>
#include "SFE_LSM9DS0.h"


//mraa::I2c* adc_i2c;

using namespace std;

//static sig_atomic_t volatile isrunning = 1;


mraa::I2c* adc_i2c;

int UVOUT = 0; //Output from the UV sensor
int REF_3V3 = 1; //3.3V power from board

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) // mapping function for UV sensor, which uses floats for greater accuracy
{
  return ((x - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min;
}

/*
void save_pgm_file()
{
	int i;
	int j;
	unsigned int maxval = 0;
	unsigned int minval = 100000;
	FILE *f = fopen("image.pgm", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	printf("\nCalculating min/max values for proper scaling...\n");
	for(i=0;i<60;i++)
	{
		for(j=0;j<80;j++)
		{
			if (image[i * 80 + j] > maxval) {
				maxval = image[i * 80 + j];
			}
			if (image[i * 80 + j] < minval) {
				minval = image[i * 80 + j];
			}
		}
	}
	printf("maxval = %u\n",maxval);
	printf("minval = %u\n",minval);
	fprintf(f,"P2\n80 60\n%u\n",maxval-minval);
	for(i=0;i<60;i++)
	{
		for(j=0;j<80;j++)
		{
			fprintf(f,"%d ", image[i * 80 + j] - minval);
		}
		fprintf(f,"\n");
	}
	fprintf(f,"\n\n");
	fclose(f);
}
*/

/*
void sig_handler(int signum)
{
	if(signum == SIGINT)
		isrunning = 0;
}
*/

int main()
{

// ---- Timestamp BEGIN ------------------------------------
    time_t result;

    result = time(NULL);
    asctime(localtime(&result)),
            (uintmax_t)result;
// ---- Time stamp END ---------------------------------------

	Hum aHum("aHum");
//	Dof aDof("aDof");
	Timer aTimer("aTimer");

	ofstream loop, gps, uv, hum, temp, timer, mag, gyro, accel, test;

	/*ofstream FileNames[9] = {gps, uv, hum, temp, timer, mag, gyro, accel, test};

	string FileArray[10] = {"gps.txt", "uv.txt", "hum.txt", "temp.txt", "timer.txt", "mag.txt", "gyro.txt", "accel.txt", "test.txt"};

	for(int k = 0; k < 10; ++k)
	{
		FileNames[k].open(FileArray[k].c_str());
		loop << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
		loop.close();
	}*/

	// Indicates when the Edison starts up. Rather than deleting the file contents. This tracks Edison restarts in case of power failure
	cout << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;

	// Opens the file, setting it as an Output type (which prevents the file opening from erasing data) and as Append, which sets the pointer to the end of the file (appends new data at the end)
	gps.open("gps.txt");
		gps << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
	uv.open("uv.txt");
		uv << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
	hum.open("hum.txt");
		hum << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
	temp.open("temp.txt");
		temp << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
	timer.open("time.txt");
		timer << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
	mag.open("mag.txt");
		mag << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
	gyro.open("gyro.txt");
		gyro << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
	accel.open("accel.txt");
		accel << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
	test.open("test.txt");
		test << endl << "Edison Startup at: " << endl << (asctime(localtime(&result))) << endl;
	//aGps.intGps();


	//Begin GPS UART Setup
	    mraa::Uart* dev;
	    try {
	        dev = new mraa::Uart(0); //UART Constructor, takes a pin number which will map directly to the Linux UART number (check the Types.h file), this 'enables' the UART, nothing more
	        // dev = new mraa::Uart(26); for Mini Breakout Board UART RX-1 (physical pin J18-13)
	    } catch (std::exception& e) { // Catch any exceptions. If there are, output an error message
	        cout << e.what() << "Failed to start UART. Likely invalid platform config" << endl;
	    }

	    try {
	        dev = new mraa::Uart("/dev/ttyMFD1"); // UART Constructor, takes a string to the path of the serial interface needed. /dev/ttyMFD1 is the Linux path to Pin 0 (Rx) on the Arduino Dev Board
	        // dev = new mraa::Uart("/dev/ttyACM0"); //for Mini Breakout Board UART RX-1 (physical pin J18-13)
	    } catch (std::exception& e) { // Catch any exceptions. If there are, output an error message
	        cout << "Error while setting up UART on MFD1 (Pin 0). Using Android Dev Board? Check connections" << endl;
	        terminate();											// Do the setBaudRate function found in uart.hpp, and if it doesn't return a success, print the error message
	    }
	    	cout << endl << "UART initialized" << endl;				// Otherwise, if it returns a success, print the success message

	    if (dev->setBaudRate(4800) != MRAA_SUCCESS) {
	        std::cout << "Error setting Baud Rate on UART" << endl; // Do the setBaudRate function found in uart.hpp, and if it doesn't return a success, print the error message
	    }
	    	else cout << "Baud Rate set" << endl;					// Otherwise, if it returns a success, print the success message

	    if (dev->setMode(8, MRAA_UART_PARITY_NONE, 1) != MRAA_SUCCESS) {
	        std::cout << "Error setting parity on UART" << endl;	// Do the setMode function found in uart.hpp, and if it doesn't return a success, print the error message
	    }
	    	else cout << "Parity set" << endl;						// Otherwise, if it returns a success, print the success message

	    if (dev->setFlowcontrol(false, false) != MRAA_SUCCESS) {
	        std::cout << "Error setting flow control UART" << endl; // Do the setFlowControl function found in uart.hpp, and if it doesn't return a success, print the error message
	    }
	    	else cout << "Flow Control set" << endl;				// Otherwise, if it returns a success, print the success message

    string UARTString;


/*    End GPS UART Setup*/

 //   ---------------- Begin 9DoF Setup -----------------
    LSM9DS0 *imu;
      imu = new LSM9DS0(0x6B, 0x1D);

      ofstream outDataGyro, outDataAccel, outDataMag;

      // The begin() function sets up some basic parameters and turns the device
      //  on; you may not need to do more than call it. It also returns the "whoami"
      //  registers from the chip. If all is good, the return value here should be
      //  0x49d4. Here are the initial settings from this function:
      //  Gyro scale:        245 deg/sec max
      //  Xl scale:          4g max
      //  Mag scale:         2 Gauss max
      //  Gyro sample rate:  95Hz
      //  Xl sample rate:    100Hz
      //  Mag sample rate:   100Hz
      // These can be changed either by calling appropriate functions or by
      //  pasing parameters to the begin() function. There are named constants in
      //  the .h file for all scales and data rates; I won't reproduce them here.
      //  Here's the list of fuctions to set the rates/scale:
      //  setMagScale(mag_scale mScl)      setMagODR(mag_odr mRate)
      //  setGyroScale(gyro_scale gScl)    setGyroODR(gyro_odr gRate)
      //  setAccelScale(accel_scale aScl)  setGyroODR(accel_odr aRate)
      // If you want to make these changes at the point of calling begin, here's
      //  the prototype for that function showing the order to pass things:
      //  begin(gyro_scale gScl, accel_scale aScl, mag_scale mScl,
      //                gyro_odr gODR, accel_odr aODR, mag_odr mODR)
      uint16_t imuResult = imu->begin();
      //cout<<hex<<"Chip ID: 0x"<<imuResult<<dec<<" (should be 0x49d4)"<<endl;

      bool newAccelData = false;
      bool newMagData = false;
      bool newGyroData = false;
      bool overflow = false;
     // cout << "9dof initialized" << endl;
      // Loop and report data

        // First, let's make sure we're collecting up-to-date information. The
        //  sensors are sampling at 100Hz (for the accelerometer, magnetometer, and
        //  temp) and 95Hz (for the gyro), and we could easily do a bunch of
        //  crap within that ~10ms sampling period.
        while ((newGyroData & newAccelData & newMagData) != true)
        {
          if (newAccelData != true)
          {
            newAccelData = imu->newXData();
          }
          if (newGyroData != true)
          {
            newGyroData = imu->newGData();
          }
          if (newMagData != true)
          {
            newMagData = imu->newMData(); // Temp data is collected at the same
                                          //  rate as magnetometer data.
          }
        }
     //   cout << "9dof New Data reported" << endl;

        newAccelData = false;
        newMagData = false;
        newGyroData = false;

      //  cout << "9dof Overflow begin" << endl;
        // Of course, we may care if an overflow occurred; we can check that
        //  easily enough from an internal register on the part. There are functions
        //  to check for overflow per device.
        overflow = imu->xDataOverflow() |
                   imu->gDataOverflow() |
                   imu->mDataOverflow();
     //   cout << "9dof Overflow End" << endl;
    /*    if (overflow)
        {
          cout<<"WARNING: DATA OVERFLOW!!!"<<endl;
        }*/

        // Calling these functions causes the data to be read from the IMU into
        //  10 16-bit signed integer public variables, as seen below. There is no
        //  automated check on whether the data is new; you need to do that
        //  manually as above. Also, there's no check on overflow, so you may miss
        //  a sample and not know it.

 //   ---------------- End 9DoF Setup -----------------


/*------------ Main Program Loop -----------------------------------------------------------------------------------------------------------------------*/
for (;;)
{
	for(int i = 1; i>= 1; i++ )
	{

	cout << i <<endl;
	aTimer.getTime();

    ofstream outData;

/*    -------------- GPS Start -----------------------*/
	 UARTString = dev->readStr(73); // Read the UART string the GPS is sending to variable, using MRAA readStr function (73 = length of string)

	 unsigned int j = 12; // This is the minimum length for string exclusion. Any strings with length less than j are ignored (assumed blank lines sent from Trimble GPS module)
	 unsigned int counter = 0;

	  while(j > UARTString.size()) // If the string length is less than j, keep looping until the string length is greater than j - this removes all blank (junk) GPS lines
	  {
		  UARTString = dev->readStr(73);
		   // keep looping and checking the length of the string length until it "passes" the minimum length j
		  counter++;
		  //usleep(50000); //Sleep for 50,000 microseconds (aka 50 milliseconds)
	  }

/*	  cout << "Wrote GPS Data: " << UARTString << endl;
	  cout << "UART String Length: " << UARTString.size() << endl;
	  cout<< "Looped (while) " << counter <<" times" << endl;*/

	  outData.open("gps.txt", fstream::in | fstream::out | fstream::app); //Open the file in a way which appends upcoming data to existing file data (no overwriting)
	  outData << UARTString << endl; // Write the string data to the file
	  outData.close(); // Close the file

/*	  ------------------- GPS End ---------------------------*/
// cout << "GPS End" << endl;
usleep(10000);

/*----- Humidity + Temperature Read -------------*/

// cout << "Humidity Begin" << endl;
	aHum.humRead();
// cout << "Humidity End" << endl;
usleep(10000);

/*----- 9 Degrees Read ------------------*/

//cout << "9 Degrees Begin" << endl;
	//aDof.nineDOF();
//cout << "9 Degrees End" << endl;

// ---- UV Sensor BEGIN ------------------------------------------------------------------------------------------------------------------ //

adc_i2c = new mraa::I2c(1);
    ads1015 adc(adc_i2c, 0x48);
    adc.setRange(_4_096V);

	float uvLevel = adc.getResult(UVOUT); //Get the analog voltage from the UV sensor (pin 0 on ADC Block)
	float refLevel = adc.getResult(REF_3V3); //Get Analog voltage from 3.3v reference (pin 1 on ADC Block, connected to power rail)

	float outputCalcVoltage = 3.3 / refLevel * uvLevel;  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor

	float uvCalcIntensity = mapfloat(outputCalcVoltage, 0.99, 2.9, 0.0, 15.0); //Maps the voltage output to the corresponding UV light level (refer to mapfloat function)

	cout << "Calculated UV Intensity (mW/cm^2): " << uvCalcIntensity << endl << endl;

	outData.open("uv.txt", std::ios::out |std::ios::app);
	outData << "Calculated UV Intensity (mW/cm^2): " << uvCalcIntensity << endl;
	outData.close();
	usleep(10000);
// ---- UV Sensor END ----- //

	 //   ---------------- Begin 9DoF Code -----------------
   // cout << "9dof Batch read data begin" << endl;
    imu->readAccel();
    imu->readMag();
    imu->readGyro();
    imu->readTemp();
   // cout << "9dof Batch read data end" << endl;

   // cout << "9dof print data begin" << endl;
    // Print the "real" values in more human comprehensible units.
	outDataGyro.open("gyro.txt", std::ios::out |std::ios::app);
    outDataGyro <<imu->calcGyro(imu->gx)<< ',' <<imu->calcGyro(imu->gy)<< ',' << imu->calcGyro(imu->gz)<< endl;
    outDataGyro.close();

  	outDataAccel.open("accel.txt", std::ios::out |std::ios::app);
  	outDataAccel <<imu->calcAccel(imu->ax)<< ',' << imu->calcAccel(imu->ay) << ',' << imu->calcAccel(imu->az) << endl;
  	outDataMag.close();

	outDataMag.open("mag.txt", std::ios::out |std::ios::app);
  	outDataMag <<imu->calcMag(imu->mx)<< ',' << imu->calcMag(imu->my) << ',' << imu->calcMag(imu->mz) << endl;
  	outDataAccel.close();

  	 //   ---------------- End 9DoF Code -----------------

	cout << "." << endl;
	usleep(200000);

	/*End Main Program Loop*/




	//string line;
	//string line1;

	/*if(i == 10)
	{
		ifstream inFile("temp.txt");
		ofstream outFile("test.txt");

		if(inFile.is_open())
		{
			//cout << "if" << endl;
			while(getline(inFile,line))
			{
				//cout << "while" << endl;
				//cout << line << '\n';
				outFile <<line << '\n';
			}
			inFile.close();
			outFile.close();
		}

	} // If bracket

	if(i == 10)
		{
			ifstream inFile1("temp.txt");
			ofstream outFile1("test1.txt", fstream::in | fstream::out | fstream::app);

			if(inFile1.is_open())
			{
				//cout << "if" << endl;
				while(getline(inFile1,line1))
				{
					//cout << "while" << endl;
					//cout << line << '\n';
					outFile1 << line1 << '\n';
				}
				inFile1.close();
				outFile1.close();

				i=0;
			}


			//i = 0;
		} */// If bracket
	} // Counting For loop
} // Main For loop




	//flir code
	return 0;
}
