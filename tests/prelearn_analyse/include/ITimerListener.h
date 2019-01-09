#ifndef ITIMERLISTENER_H
#define ITIMERLISTENER_H


class ITimerListener
{
    public:
        ITimerListener();
        virtual ~ITimerListener();

        virtual void TimerDone(float fTime, int flag) = 0;
    protected:
    private:
};

#endif // ITIMERLISTENER_H
