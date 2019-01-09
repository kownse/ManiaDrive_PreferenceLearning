#ifndef _FASTMATH_WIN32_
#define _FASTMATH_WIN32_

#define MATH_PI 3.141592653f

#include <math.h>
#include <float.h>


float DegreeToRadian(float r)
{
    float radian = r * (MATH_PI / 180.0f);
    return radian;
}

float DegToRad(float r)
{
    return DegreeToRadian(r);
}

float RadianToDegree(float r)
{
    float degree = r * (180.0f / MATH_PI);
    return degree;
}

float RadToDeg(float r)
{
    return RadianToDegree(r);
}

float Abs(float v)
{
    float result;
    __asm
    {
        fld [v]
        fabs
        fstp [result]
    }
    return result;
}

float Sqrt(float v)
{
    float result;
    __asm
    {
        fld [v]
        fsqrt
        fstp [result]
    }
    return result;
}

float ReciprocalSqrt(float v)
{
    float result;
    __asm
    {
        fld [v]
        fsqrt
        fld1
        fdivrp st(1),st
        fstp [result]
    }
    return result;
}

float Sin(float radian)
{
    float result;
    __asm
    {
        fld [radian]
        fsin
        fstp [result]
    }
    return result;
}

// -pi < p_Angle < pi
float FastSin(float p_Angle)
{
    const float Factor1 = 4.0f / MATH_PI;
    const float Factor2 = 4.0f / (MATH_PI * MATH_PI);
    return (Factor1 * p_Angle) - (Factor2 * p_Angle * fabs(p_Angle));
}

float ASin(float v)
{
    float result = (float) asin(v);
    return result;
}

float Cos(float radian)
{
    float result;
    __asm
    {
        fld [radian]
        fcos
        fstp [result]
    }
    return result;
}

float ACos(float v)
{
    float result = (float) acos(v);
    return result;
}

void SinCos(float radian, float &sinvalue, float &cosvalue)
{
    unsigned short cwd_flag = 0;

    __asm
    {
        fstsw [cwd_flag]
    }

    int stacktop = (cwd_flag & 0x3800)>>11;

    if ( stacktop < 7 )
    {
        __asm
        {
            mov eax, dword ptr[cosvalue]
            mov ebx, dword ptr[sinvalue]
            fld [radian]
            fsincos
            fstp dword ptr [eax]
            fstp dword ptr [ebx]
        }
    }
    else
    {
        float s0, s1;
        __asm
        {
            fstp [s0]
            fstp [s1]
            mov eax, dword ptr[cosvalue]
            mov ebx, dword ptr[sinvalue]
            fld [radian]
            fsincos
            fstp dword ptr [eax]
            fstp dword ptr [ebx]
            fld [s1]
            fld [s0]
        }
    }
}

float Tan(float radian)
{
    float result;

    unsigned short cwd_flag = 0;

    __asm
    {
        fstsw [cwd_flag]
    }

    int stacktop = (cwd_flag & 0x3800)>>11;
    if ( stacktop < 7 )
    {
        __asm
        {
            fld [radian]
            fptan
            fstp [result]
            fstp [result]
        }
    }
    else
    {
        float s0,s1;
        __asm
        {
            fstp [s0]
            fstp [s1]
            fld [radian]
            fptan
            fstp [result]
            fstp [result]
            fld [s1]
            fld [s0]
        }
    }

    return result;
}

float Cot(float radian)
{
    float result;

    unsigned short cwd_flag = 0;
    __asm
    {
        fstsw [cwd_flag]
    }
    int stacktop = (cwd_flag & 0x3800)>>11;
    if ( stacktop < 7 )
    {
        __asm
        {
            fld [radian]
            fptan
            fdivrp st(1), st(0)
            fstp [result]
        }
    }
    else
    {
        float s0,s1;
        __asm
        {
            fstp [s0]
            fstp [s1]
            fld [radian]
            fptan
            fdivrp st(1), st(0)
            fstp [result]
            fld [s1]
            fld [s0]
        }
    }

    return result;
}

float ATan(float radian)
{
    float result;
    __asm
    {
        fld [radian]
        fpatan
        fstp [result]
    }
    return result;
}

float Lerp(float a, float b, float t)
{
    return a + (b-a) * t;
}

float frac(float a)
{
    return a - (int)a;
}
#endif // _FASTMATH_WIN32_
