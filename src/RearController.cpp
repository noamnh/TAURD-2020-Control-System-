////////////////////////////////////////
//      Controlling Thorttle          //
//                                    //
////////////////////////////////////////

// Made by Noam Nahum & Dror Balbul Tel-Aviv University (Engineering Faculty) //

/*
    Pingout:
    Nucleo-L432KC
    A0 - PA_0 ---> Analogin ---> Read Potentiometer 1
    A1 - PA_1 ---> Analogin ---> Read Potentiometer 2
    A2 - PA_3 ---> Analogin ---> Volt Dvider (Optional)
    A3 - PA_4 ---> Analogin ---> Volt Dvider (Optional)
    A4 - PA_5 ---> Digitalout ---> Motor/in A = FWD direction
    A5 - PA_6 ---> Digitalout ---> Motor/in B = REV direction
    A6 - PA_7 ---> Digitalout ---> NPN Transistor for shutdown circuit
    A7 - PA_2 ---> Digitalout ---> To Brake MOSFET
    D10 - PA_11 ---> CANBUS TX
    D2 - PA_12 ----> CANBUS RX
    D9 - PA_8  ---> PWMOUT ---> to H bridge motor Driver
    D0 - PA_10 ---> SerialRX ---> Xbee DOUT 
    D1 - PA_9 ---> SerialTX ---> Xbee Din 
    
*/

///////////////
// Libraries //
///////////////

#include "PID.h" // pid library
#include "Motor.h" // Motor library
#include <Map.hpp> // Mapping library
#include "mbed.h" // Mbed library
#include "Sensor.h" // Sensor Reading and filtering library
#include "BufferedSerial.h"




///////////////
// #defines  //
//////////////

/////////////
// Objects //
/////////////

// ShutDown Fuel Transistor // 

DigitalOut ShutDown(PA_7);

/////Thorttle Potentiometers////

AnalogIn Sensor_1(PA_0);
AnalogIn Sensor_2(PA_1);

/////Motor Control////

Motor Thorttle(PA_8, PA_5, PA_6); // (pwm, inA, inB)

/////Serial - XBEE, PC////
Serial pc(SERIAL_TX, SERIAL_RX);
BufferedSerial XBEE(PA_9, PA_10);

//// CAN BUS communication ////
CAN CANBUS(PA_11, PA_12);

/////PID////

float Thorttledegree, Pedaldegree, Angle = 0;
//pid constants

const float kp = 1;
const float ki = 3;
const float kd = 0.0;
const float Ts = 0.001; // Time sample

//pid range

const float output_lower_limit = -255;
const float output_upper_limit = 255;

PID pid(&Pedaldegree, &Thorttledegree, &Angle, output_lower_limit, output_upper_limit, kp, ki, kd, Ts);

/////Sensor reading and filtering////

float Thorttle_1, Thorttle_2; // Thorttle Sensor 1 and 2 Initalizing
float weight = 0.05; // Expontetial filter weight
int Thorttle1, Thorttle2, Pedal1, Pedal2 = 0;
int countermsgdata = 0;


Sensor Thorttle_Sensors(&Thorttle_1, &Thorttle_2, &Thorttledegree,  weight);

/////Mapping////

Map Degree_to_speed = Map(-255,255,-1,1);

// Brake // 
//DigitalOut mybrake(PA_2);


//Tickers//
Ticker MainTicker;
Ticker SystemErrorTicker;

//Timers//
Timer CANTimer;

//Error Flags//
int  BrakeError, SystemError, BSPD = 0;
float ThorttleError, PedalError, CANError = 0;

//Data Packets//


// Functions//

void commandThorttle(){
    float Thorttlecommand = Degree_to_speed.Calculate(Angle);
    Thorttle.speed(Thorttlecommand);
    }
/*void BrakeOn(){
    mybrake = 1;
    Brake = 1;
    }
void BrakeOff(){
    mybrake = 0;
    Brake = 0;
    }*/

void SendError() {
    if (SystemError == 1){
        MainTicker.detach();
        pid.stop();
        SystemErrorTicker.attach(SendError, 0.0005);
        ShutDown = 0;
        }
    Thorttle.speed(-1);
}
void MainFunction() {
    Thorttle_1 = Sensor_1.read();
    Thorttle_2 = Sensor_2.read();
    if (Thorttle_Sensors.ErrorCheck()){
        ThorttleError = 1;
        SystemError = 1;
        SendError();
        }
    Thorttle_Sensors.ApplyFilter(Thorttle_Sensors.Sensor_Value_1());
    Thorttle1 = Thorttle_Sensors.Sensor_Value_1();
    Thorttle2 = Thorttle_Sensors.Sensor_Value_2();
    
    CANMessage msg;
    
        if(CANBUS.read(msg)) {
            if (CANTimer.read() > 0) {
                CANTimer.stop();
                CANTimer.reset();
                }
            if (msg.data[3]==1){
                //pc.printf("%d\n",1);
                PedalError = 1;
                SystemError = 1;
                SendError();
                }
            if (msg.data[3]==0){
                PedalError = 0;
                }
            if (msg.data[4]==1){
                //BrakeOn();
                }
            if (msg.data[4]==0){
                //BrakeOff();
                }
            Pedaldegree = msg.data[0];
            Pedal1 = msg.data[0];
            Pedal2 = msg.data[2];
            }
        if (!CANBUS.read(msg)) {
            CANTimer.start();
            }
        if (CANTimer.read() > 0.1) { // if message not recieved more then 0.1 sec send error
            CANTimer.stop();
            CANError = 1;
            SystemError = 1;
            SendError();
            }
        pid.sample();
        commandThorttle();
        countermsgdata++;
        }
        


int main(){
    XBEE.baud(115200);
    ShutDown = 1;
    pid.start();
    MainTicker.attach(MainFunction, 0.0005);
    while(1){
        /*if(Thorttle_Sensors.Sensor_Sub()>0.1){
            pc.printf("T1::%.4f\r\n", Thorttle_1);
            pc.printf("T2::%.4f\r\n", Thorttle_2);
            pc.printf("Sum::%.4f\r\n", Thorttle_Sensors.Sensor_Sum());
            pc.printf("Sub::%.4f\r\n", Thorttle_Sensors.Sensor_Sub());
            pc.printf("Timer::%.4f\r\n", Thorttle_Sensors.Sensor_timer());
            }*/
        /*pc.printf("P%.4f\n", Pedaldegree);
        pc.printf("M%.4f\n", Thorttledegree);
        pc.printf("C%.4f\n", CANError);
        pc.printf("T%.4f\n", ThorttleError);
        pc.printf("p%.4f\n", PedalError);*/
        XBEE.printf("P%.4f\n", Pedaldegree);
        XBEE.printf("M%.4f\n", Thorttledegree);
        XBEE.printf("C%.4f\n", CANError);
        XBEE.printf("T%.4f\n", ThorttleError);
        XBEE.printf("p%.4f\n", PedalError);
        
        //printf("0%d\n",Thorttle1);
            
        }
}





