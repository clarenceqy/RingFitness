#include "gyrodrv.h"
#include <mbed.h>

SPI spi(PF_9, PF_8, PF_7); // mosi, miso, sclk
DigitalOut cs(PC_1);


//Enable gyro by setting the forth LSB to high
void Gyro_Drv::Gyro_Init(){
  cs = 1; //Default Deselected
  spi.format(8,3);
  spi.frequency(1000000);
  Write_Byte(0x0f,I3G4250D_CTRL_REG1_ADDR);
}

void Gyro_Drv::Write_Byte(uint8_t data,uint8_t reg_Addr){
  cs = 0;
  spi.write(reg_Addr);
  spi.write(data);
  cs = 1;
}

uint8_t Gyro_Drv::Read_Byte(uint8_t reg_Addr){
  cs = 0;
  spi.write(0x80 | reg_Addr);
  uint8_t res = spi.write(0x00);
  cs = 1;
  return res;
}

//Reading 3 axis's raw data and do a preprocessing(times sensitivity to make it real dps)
void Gyro_Drv::readXYZinDPS(float* buffer){
  uint8_t tmpbuffer[6] = {0};
  int16_t RawData[3] = {0};
  uint8_t tmpreg = 0;
  float sensitivity = 0;

  tmpreg = Read_Byte(I3G4250D_CTRL_REG4_ADDR);
  
  tmpbuffer[0] = Read_Byte(I3G4250D_OUT_X_L_ADDR);
  tmpbuffer[1] = Read_Byte(I3G4250D_OUT_X_H_ADDR);
  tmpbuffer[2] = Read_Byte(I3G4250D_OUT_Y_L_ADDR);
  tmpbuffer[3] = Read_Byte(I3G4250D_OUT_Y_H_ADDR);
  tmpbuffer[4] = Read_Byte(I3G4250D_OUT_Z_L_ADDR);
  tmpbuffer[5] = Read_Byte(I3G4250D_OUT_Z_H_ADDR);

  for(int i = 0; i < 3;i++){
     RawData[i] = (int16_t)(((uint16_t)tmpbuffer[2 * i + 1] << 8) + tmpbuffer[2 * i]);
  }

  switch (tmpreg & I3G4250D_FULLSCALE_SELECTION)
  {
    case I3G4250D_FULLSCALE_245:
      sensitivity = I3G4250D_SENSITIVITY_245DPS;
      break;

    case I3G4250D_FULLSCALE_500:
      sensitivity = I3G4250D_SENSITIVITY_500DPS;
      break;

    case I3G4250D_FULLSCALE_2000:
      sensitivity = I3G4250D_SENSITIVITY_2000DPS;
      break;
  }

  for (int i = 0; i < 3; i++)
  {
    buffer[i] = (float)(RawData[i] * sensitivity);
  }

}

