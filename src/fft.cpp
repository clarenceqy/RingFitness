#include "fft.h"
#include <stdio.h>

using std::vector;

float Ykr[duration/2];
float Yki[duration/2];
float Sxx[duration/2];

//Fourier Transformation: Calculate coeffciences and put them into YKr and YKi
void DFT_Cal_Coeff(float* realdata, int N){
    for(int k = 0; k < N/2;k++){
        Ykr[k] = 0;
        Yki[k] = 0;
        for(int j = 0; j < N; j++){
            Ykr[k] = Ykr[k] + realdata[j] * cos(2*M_PI*k*j/N);
            Yki[k] = Yki[k] + realdata[j] * sin(2*M_PI*k*j/N);
        }
        Ykr[k] = Ykr[k]/N;
        Yki[k] = Yki[k]/N;
    }
     for(int i = 0; i < duration / 2;i++){
         Ykr[i] = 2*Ykr[i];
         Yki[i] = 2*Yki[i];
     }
     Ykr[0] /= 2;
}

//Function that calcualtes Sxx and return the highest frenquency according sxx
float Find_Highest_Freq(){
    float max_freq = 0.0;
    float freq_table[duration/2];
    for(int i = 0; i < duration/2;i++){
        freq_table[i] = i * (1.0/(duration/2));
        Sxx[i]= Ykr[i]*Ykr[i] + Yki[i]*Yki[i];
    }
    for(int i = 1; i < duration/2 - 1;i++){
         if(Sxx[i] > Sxx[i -1] && Sxx[i] > Sxx[i+1])
             max_freq = freq_table[i];
    }
    return max_freq;
}

vector<float> Find_All_Pitches(){
    float interval = 1.0 / Find_Highest_Freq();
    float start = 0.0;
    float end = (float)(duration/2);
    float dps = 0.0;
    vector<float> pitches;

    while(start < end){
        dps = 0.0;
        for(int k = 0; k < duration/2;k++){
            dps += Ykr[k]* cos((2*M_PI*k*(start+interval/2.0))/((float)(duration/2))) + Yki[k] * sin((2*M_PI*k*(start+interval/2.0))/((float)(duration/2)));
        }
        pitches.push_back(dps);
        start += interval;
    }
    return pitches;
    
}

vector<float> Integral_Pos_DPS(){
    int maxbound = segpersec*duration/2;
    float seglen = 1.0/segpersec;
    float tmp = 0.0;
    float total = 0.0;
    float point = 0.0;
    vector<float> degrees;
    
    for(int i = 0; i < maxbound;i++){
        tmp = 0.0;
        for(int k = 0; k < duration/2;k++){
            tmp += Ykr[k]* cos((2*M_PI*k*(i*seglen))/((float)(duration/2))) + Yki[k] * sin((2*M_PI*k*(i*seglen))/((float)(duration/2)));;   
        }
        total = tmp < 0.0 ? total + abs((seglen * tmp)) : total;
        if(tmp > 0.0 && total != 0.0){
            degrees.push_back(abs(total));
            total = 0.0;
        }
    }
    return degrees;
}
