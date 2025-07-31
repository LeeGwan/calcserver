#include "Timer.h"
#include <cmath> 
Timer::Timer()
{
    lastTime = std::chrono::high_resolution_clock::now();
    deltaTime = 0.0f;
    Time = 0.f;
}

void Timer::Update()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = currentTime - lastTime;
    deltaTime =elapsed.count();
    
    
    lastTime = currentTime;
}

float Timer::GetDeltaTime() const
{
    return deltaTime;
}
