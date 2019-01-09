#ifndef IRECORDER_H
#define IRECORDER_H


class IRecorder
{
    public:
        IRecorder();
        virtual ~IRecorder();

        virtual void DoRecord(float value) = 0;
    protected:
    private:
};

#endif // IRECORDER_H
