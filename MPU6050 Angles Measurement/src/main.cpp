#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <wire.h>
#include <wifi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

MPU6050 mpu;

// Clave y nombre de la red Wifi
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// URL a la que se le va a hacer el POST
const char* serverUrl = "http://example.com/api/data";

uint8_t devStatus;

uint16_t fifoCount;     
uint8_t fifoBuffer[64]; 

Quaternion q;           
VectorInt16 aa;         
VectorInt16 aaReal;     
VectorInt16 aaWorld;    
VectorFloat gravity; 
   
float ypr[3];
double insVel[3], accVel[3], insPos[3], accPos[3];
int _cont = 0;

void setup() {
    Wire.begin();
    Wire.setClock(400000); 
    Serial.begin(115200);

    /*// Conectarse a la red WiFi
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED){
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");*/

    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    mpu.setXGyroOffset(93.00000);
    mpu.setYGyroOffset(-81.00000);
    mpu.setZGyroOffset(32.00000);
    mpu.setZAccelOffset(1836.00000); 
    
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
        _cont++;
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetAccel(&aa, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
        mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
        
        insVel[0] = ((double) aaWorld.x*9.81/16384.0)*20/1000.0;
        insVel[1] = ((double) aaWorld.y*9.81/16384.0)*20/1000.0;
        insVel[2] = ((double) aaWorld.z*9.81/16384.0)*20/1000.0;

        DynamicJsonDocument JSON(128);
        
        JSON["Id"] = _cont;
        JSON["posx"] = (double) aaWorld.x*9.81/16384.0;
        JSON["posy"] = (double) aaWorld.y*9.81/16384.0;
        JSON["posz"] = (double) aaWorld.z*9.81/16384.0;

        String S_JSON;
        
        serializeJson(JSON, S_JSON);

        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(serverUrl);
            http.addHeader("Content-Type", "application/json");


            // Envío de POST al servidor y verifica el estatus de la respuesta
            int httpResponseCode = http.POST(S_JSON);

            if (httpResponseCode > 0) {
                String response = http.getString();
                Serial.println("Respuesta del servidor:");
                Serial.println(response);
            } else {
                Serial.print("Error en el envío de POST (Codigo): ");
                Serial.println(httpResponseCode);
            }

            http.end();      
        }          
        delay(20);
    }
}