#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> oneLock(mtx);
    _cond.wait(oneLock); //,[this] () {return !_queue.empty();}
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> oneLock(mtx);
  	_queue.clear();
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen() //void
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {   
        if ( LightPhaseQ.receive() == TrafficLightPhase::green){
            return;
        }
    }
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::TogglePhase() //toggling the phase btwn red and green
{
    _currentPhase = (_currentPhase == TrafficLightPhase::red)? TrafficLightPhase::green : TrafficLightPhase::red;;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    TrafficObject::threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

int TrafficLight::RandomTime() //generating random time between 4 to 6 seconds
{   
    std::random_device device;
    std::uniform_int_distribution<int> distribution(4000,6000); //creating a dis between 1 and 6 seconds
    std::mt19937 generator(device());
    return distribution(generator);
}

// virtual function which is executed in a thread

void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point t2;
    double timeBetweenLights = TrafficLight::RandomTime(); //selectin random number between 4 and 6 seconds

    while(true)
    {   
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); //sleeping for 1ms to reduce cpu load
        //starting the time clock!
        //collecting the time between first measurement and now
        t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(); //measuring time difference in seconds
        if(duration >= timeBetweenLights){
            timeBetweenLights = TrafficLight::RandomTime(); //resetting timebetweenlights to new random valu
            TrafficLight::TogglePhase(); //toggling the current phase
            LightPhaseQ.send(std::move(TrafficLight::getCurrentPhase())); //sending a message to queue for _currentPhase update
            t1 = std::chrono::high_resolution_clock::now(); //resetting the time here to current time value
        }

    }
}

