#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

MPU6050 mpu;

//#define OUTPUT_REALACC
#define OUTPUT_YAWPITCHROLL
#define OUTPUT_GLOBALACC

uint8_t devStatus;

uint16_t fifoCount;     
uint8_t fifoBuffer[64]; 

Quaternion q;           
VectorInt16 aa;         
VectorInt16 aaReal;     
VectorInt16 aaWorld;    
VectorFloat gravity;    
float ypr[3];         

void setup() {
    Wire.begin();
    Wire.setClock(400000); 
    Serial.begin(115200);

    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    mpu.setXGyroOffset(101.00000);
    mpu.setYGyroOffset(-70.00000);
    mpu.setZGyroOffset(27.00000);
    mpu.setZAccelOffset(1870.00000); 
    
    if (devStatus == 0) {
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        Serial.println();
        mpu.setDMPEnabled(true);
    } else{
        Serial.println(F("Error"));
    }
}

void loop() {
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        #ifdef OUTPUT_REALACC
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            Serial.print("areal\t");
            Serial.print(aaReal.x);
            Serial.print("\t");
            Serial.print(aaReal.y);
            Serial.print("\t");
            Serial.println(aaReal.z);
        #endif

        #ifdef OUTPUT_GLOBALACC
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
            Serial.print(aaWorld.x);
            Serial.print(",");
            Serial.print(aaWorld.y);
            Serial.print(",");
            Serial.print(aaWorld.z);
            Serial.print(",");
        #endif

        #ifdef OUTPUT_YAWPITCHROLL
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print(",");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print(",");
            Serial.println(ypr[2] * 180/M_PI);
            delay(20);
        #endif
    }
}