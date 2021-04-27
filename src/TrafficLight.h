#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"
#include <random>
#include <bits/stdc++.h>

// forward declarations to avoid include cycle

class Vehicle;


// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:

void send(T &&LightPhaseState);
T receive();


private:
std::deque<T> _queue;
std::mutex mtx;
std::condition_variable _cond;
    
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

class TrafficLight : public TrafficObject
{
public:
    enum TrafficLightPhase {red, green};
    // constructor / desctructor
    TrafficLight(); //constructor
    // getters / setters
    TrafficLightPhase getCurrentPhase(); //get lightphase
    void TogglePhase(); //toggle current lightphase
    // typical behaviour methods
    int RandomTime(); //generating random time for traffic light
    
    void waitForGreen();
    void simulate();
    

private:
    
    // typical behaviour methods
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.
    
    MessageQueue<TrafficLight::TrafficLightPhase> LightPhaseQ; //FP.4b
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif
