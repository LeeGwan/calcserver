#include <chrono>
class Timer
{
public:

	Timer();
	void Update();
	float GetDeltaTime()const;
private:
	std::chrono::high_resolution_clock::time_point lastTime;
	float deltaTime;
	float Time;
};