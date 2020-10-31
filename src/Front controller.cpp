////////////////////////////////////////
//      Controlling Thorttle          //
//                                    //
////////////////////////////////////////

// Made by Noam Nahum & Dror Balbul Tel-Aviv University (Engineering Faculty) //

/*
    Pingout:
    Nucleo-L432KC
    A0 - PA_0 ---> Analogin ---> Read Pedal 1 Sensor
    A1 - PA_1 ---> Analogin ---> Read Pedal 2 Sensor 
    A2 - PA_3 ---> Analogin ---> Volt Dvider - Brake Reading
    D10 - PA_11 ---> CANBUS TX
    D2 - PA_12 ----> CANBUS RX
    
*/

#include "mbed.h" // Mbed library
#include "Sensor.h" // Sensor Reading and filtering library


AnalogIn Sensor_1(PA_0); // Pedal 1 sensor reading
AnalogIn Sensor_2(PA_1); // Pedal 2 sensor reading
AnalogIn Sensor_3(PA_3); // Brake Sensor reading
CAN CANBUS(PA_11, PA_12);
Serial pc(USBTX,USBRX);

/////Sensor reading and filtering////

float Pedal_1, Pedal_2; // Pedal Sensor 1 and 2 Initalizing
float weight = 0.1; // Expontetial filter weight
float Pedaltodegree = 0;
int Pedal_data = 0;
int Pedal1, Pedal2 = 0;
Sensor Pedal_Sensors(&Pedal_1, &Pedal_2, &Pedaltodegree, weight);

//Brake//
int mybrake = 0;
int BrakePressure = 0;

//Counters//
float sendcounter = 0;
float Nonesendcounter = 0;
//Tickers// 
Ticker MainTicker;
//Errors//
int PedalError, BrakeError = 0;

char message_data[6] = {0,0,0,0,0,0};


void MainFunction() { 
   Pedal_1 = Sensor_1.read();
    Pedal_2 = Sensor_2.read();
    if (Pedal_Sensors.ErrorCheck()){
        PedalError = 1;
        }
    Pedal_Sensors.ApplyFilter(Pedal_Sensors.Sensor_Value_1());
    Pedal_data = Pedaltodegree;
    Pedal1 = Pedal_Sensors.Sensor_Value_1();
    Pedal2 = Pedal_Sensors.Sensor_Value_2();
    message_data[0] = Pedal_data;
    message_data[1] = Pedal1;
    message_data[2] = Pedal2;
    message_data[3] = PedalError;
    message_data[4] = mybrake;
    message_data[5] = BrakePressure;
    if(CANBUS.write(CANMessage(1337, message_data, 6))) {
        sendcounter++;
        }
    else{
        Nonesendcounter++;
        }
    }

int main() {
    MainTicker.attach(MainFunction, 0.0005); // This function runs every 0.0005 seconds
    while(1) {
        }

}
