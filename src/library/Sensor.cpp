#include "mbed.h"
#include "Sensor.h"
 
Sensor::Sensor(float* Sensor_1, float* Sensor_2, float* lastscope, float weight){
    _Sensor_1 = Sensor_1; 
    _Sensor_2 = Sensor_2;
    _Sensor_1_max = 0.93;
    _Sensor_2_max = 0.93;
    _Sensor_1_min = 0.095;
    _Sensor_2_min = 0.095;
    _Error = 0;
    Sensor_sub = -1;
    Sensor_sum = -1;
    
    _lastscope = lastscope;
    _weight = weight;
    
}

    
int Sensor::ErrorCheck() { // if there is error for more than 100ms Sensor_Value_1 is -1
    Sensor_sum = *_Sensor_1 + *_Sensor_2;
    Sensor_sub = abs(Sensor_sum-1);
    if (Sensor_sub<0.1){
        _Error = 0;
        timer.stop();
        timer.reset();
        }
    if (Sensor_sub>0.1){
        if (_Error == 0){
            timer.start();
            }
        if (timer.read() > 0.1){
            timer.stop();
            _Error = 1;
            }  
        }
        return _Error;
    }
    
void Sensor::ApplyFilter(float newscope)
{
    newscope = (_weight*newscope) + ((1-_weight)*(*_lastscope));
    *_lastscope = newscope; 
}

void Sensor::ChangeWeight(float new_weight)
{
    _weight= new_weight;
} 
float Sensor::Sensor_Value_1() {
    /*if (*_Sensor_1 > _Sensor_1_max){
        _Sensor_1_max = *_Sensor_1;
        }
    if (*_Sensor_1 < _Sensor_1_min){
        _Sensor_1_min = *_Sensor_1;
        }*/
    _Sensor_Value_1 = ((*_Sensor_1 - _Sensor_1_min)*(255 - 0))/((_Sensor_1_max - _Sensor_1_min) + 0 );
    if (_Sensor_Value_1<0){
        return 0;
        }
    if (_Sensor_Value_1>255){
        return 255;
        }
    else{
        return _Sensor_Value_1;
        }
    }
float Sensor::Sensor_Value_2() {
    /*if (*_Sensor_2 > _Sensor_2_max){
        _Sensor_2_max = *_Sensor_2;
        }
    if (*_Sensor_2 < _Sensor_2_min){
        _Sensor_2_min = *_Sensor_2;
        }*/
    _Sensor_Value_2 = ((*_Sensor_2 - _Sensor_2_min)*(255 - 0))/((_Sensor_2_max - _Sensor_2_min) + 0 );
    if (_Sensor_Value_2<0){
        return 0;
        }
    if (_Sensor_Value_2>255){
        return 255;
        }
    else{
        return _Sensor_Value_2;
        }
    } 
float Sensor::Sensor_Sum(){ // return the sum of the value of sensor
    return Sensor_sum;
    }
float Sensor::Sensor_Sub(){ return (1-Sum of the sensor)
    return Sensor_sub;
    }
float Sensor::Sensor_timer(){ // return sensor timer
    return timer.read();
    }

