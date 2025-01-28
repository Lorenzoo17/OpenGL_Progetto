#ifndef TIME_H
#define TIME_H

class Time{
public:
    static double currentTime;
    static double deltaTime;
    
    Time();
    
    static void setTime(double time);
    static void setDeltatime(double deltatime);
};

#endif
