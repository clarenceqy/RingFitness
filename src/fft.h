#ifndef __fft_h__
#define __fft_h__

#include <math.h>
#include <cmath>
#include <vector>

using std::vector;

#define M_PI 3.141592654
#define duration 40
#define segpersec 10

void DFT_Cal_Coeff(float* realdata,int N);
float Find_Highest_Freq();
vector<float> Find_All_Pitches();
vector<float> Integral_Pos_DPS();
void testo();

#endif