#include "time.h"

double Time::currentTime = 0;
double Time::deltaTime = 0;


void Time::setTime(double time)
{
    Time::currentTime = time;
};

void Time::setDeltatime(double deltatime)
{
    Time::deltaTime = deltatime;
};
