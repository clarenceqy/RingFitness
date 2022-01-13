#include <mbed.h>
#include <stdio.h>
#include <complex.h>
#include <iostream>
#include <string>
#include <math.h>
#include "gyrodrv.h"

#include <vector>
#include "fft.h"

#define leglen 77.0 //The height from the device to the ground, in cm

float res[3] = {0.0};
float X[duration];
float Y[duration];
float Z[duration];
Ticker ticker;
volatile int shouldSample = 0;
int counter = 0;

void IRQ_Handler(){
  shouldSample = 1;
}

void startticker(){
  ticker.attach(&IRQ_Handler,0.5);  //0.5s sample rate
}

//Calculate the distance changes from delta degree
//More detail in lab report
float gait_dist(float degree){
  float smolHypo = leglen / cos(degree*M_PI/180) - leglen;
   //printf("Degree:%f\n",degree);
   return leglen * tan(degree*M_PI/180) - smolHypo * cos(((90.0-degree)*M_PI/180));
}

int main() {

  Gyro_Drv gyro_drv;
  gyro_drv.Gyro_Init();
  startticker();

  while(1) {

    if(shouldSample){
      gyro_drv.readXYZinDPS(res);
      X[counter] = res[0];
      Y[counter] = res[1];
      Z[counter++] = res[2];
      shouldSample = 0;
      if(counter == duration){
        ticker.detach();  //finished sampling,stops interrupt
      }
    }

    if (counter == duration)
    {
      //Fourier transformation
      DFT_Cal_Coeff(Z,duration);

      //Integral over fourier function over the time lapse.
      vector<float> degrees = Integral_Pos_DPS();
      float total = 0.0;
      for(auto i = degrees.begin();i != degrees.end();i++){
        float dist = gait_dist(*i);
        //printf("dist: %f\n",dist);
        total += dist;
      }
      total = total*2; // total = left + right ~= (left|right) * 2
       printf("total:: %f\n",total);
       float speed = total / (duration / 2.0);
       printf("average speed in cm/s:: %f\n",speed);
      
    }
  }
}