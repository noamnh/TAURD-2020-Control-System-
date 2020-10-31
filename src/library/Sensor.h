#ifndef SENSOR_H
#define SENSOR_H
#include "mbed.h"
/* 
    Noam Nahum and Dror Balbul 05/10/2020
     Sensor reading and filtering for formula student vehicle
    The library takes 2 diffrential sensor and calculate to the diffrence between them.
    this library is for the FSAE regulation For reading Pedal Sensors and Thorttle Sensors.
    
    
    
    Note: Sensor objects use given pointers, ie Sensor_1 
            Sensor_2, lastscope. When reading/ modifying
            these vars make sure we don't have possible read/write 
            conflicts if the interrupt fires. */
            
    
class Sensor {
    public:
        Sensor(float* Sensor_1, float* Sensor_2, float* lastscope, float weight);  // Sensor_1, (Analogin value of sensor 1)
        float Sensor_Value_1(); // Sensor_1 Value between 0-255
        float Sensor_Value_2(); // Sensor_1 Value between 0-255
        //int Get_Error();
        void ApplyFilter(float newscope);
        void ChangeWeight(float new_weight);
        int ErrorCheck();
        float Sensor_Sum();
        float Sensor_Sub();
        float Sensor_timer();
     
    private:
        Timer timer;
        int _Error;
        float* _Sensor_1;
        float* _Sensor_2;
        float _Sensor_Value_1;
        float _Sensor_Value_2;
        float Sensor_sum, Sensor_sub;
        float _weight;
        float* _lastscope;
        float _Sensor_1_max;
        float _Sensor_1_min;
        float _Sensor_2_max;
        float _Sensor_2_min;
        
};
    

#endif     
     
