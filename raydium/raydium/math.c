/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/math.h"
#include "headers/shadow.h"
#endif

GLfloat raydium_math_cos(GLfloat i)
{
return( (GLfloat)cos(i*PI/180) );
}

GLfloat raydium_math_sin(GLfloat i)
{
return( (GLfloat)sin(i*PI/180) );
}

GLfloat raydium_math_cos_inv(GLfloat i)
{
return(acos(i)*180/PI);
}

GLfloat raydium_math_sin_inv(GLfloat i)
{
return(asin(i)*180/PI);
}

void raydium_math_rotate(GLfloat *p, GLfloat rx, GLfloat ry, GLfloat rz, GLfloat *res)
{
res[0]= (p[0]*raydium_math_cos(ry)+(p[2]*raydium_math_cos(rx)+p[1]*raydium_math_sin(rx))*raydium_math_sin(ry))*raydium_math_cos(rz) + (p[1]*raydium_math_cos(rx)-p[2]*raydium_math_sin(rx))*raydium_math_sin(rz);
res[1]=-(p[0]*raydium_math_cos(ry)+(p[2]*raydium_math_cos(rx)+p[1]*raydium_math_sin(rx))*raydium_math_sin(ry))*raydium_math_sin(rz) + (p[1]*raydium_math_cos(rx)-p[2]*raydium_math_sin(rx))*raydium_math_cos(rz);
res[2]= (p[2]*raydium_math_cos(rx)+ p[1]*raydium_math_sin(rx))*raydium_math_cos(ry)-p[0]*raydium_math_sin(ry);
}


// pos: GLfloat[3], m: GLfloat[16]
void raydium_math_pos_to_matrix(GLfloat *pos, GLfloat *m)
{
m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = pos[0];
m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = pos[1];
m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = pos[2];
m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

// res: GLfloat[3]
void raydium_math_pos_get_modelview(GLfloat *res)
{
GLfloat tmp[16];
glGetFloatv(GL_MODELVIEW_MATRIX,tmp);
res[0]=tmp[12];
res[1]=tmp[13];
res[2]=tmp[14];
}

/* Unfinished !
// pos == res is safe
void raydium_trigo_carth_to_sphere(GLfloat *pos, GLfloat *res)
{
GLfloat r,G,T;

r=sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
G=atan(pos[1]/pos[0]);
T=acos(pos[2]/r);

//printf("%f\n",r);
r=1;

if(pos[0]<0)
    res[0]=-r*cos(G);
else
    res[0]=+r*cos(G);

//res[1]=r*sin(G);
if(pos[0]<0)
    res[0]=-r*sin(T)*cos(G);
else
    res[0]=r*sin(T)*cos(G);
res[1]=r*sin(G)*cos(G);
res[2]=r*cos(T);

if(isnan(res[0])) res[0]=0;
if(isnan(res[1])) res[1]=0;
if(isnan(res[2])) res[2]=0;

}
*/

int raydium_math_pow2_next(int value)
{
int pows[]={0,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536};
int pows_count=17; // 16 + first (0)
int i;

if(value>65536 || value<0)
    {
    raydium_log("trigo: value is outside of limits of this ugly function :/");
    }

for(i=0;i<pows_count;i++)
    if(pows[i]>=value)
        return pows[i];


// should never hit this point
raydium_log("trigo: raydium_math_pow2_next: ?!!");
return -1;
}

float raydium_math_angle_from_projections(float px, float py)
{
float realangle;
//using the arccos we get the "base angle"
realangle = acos ( px );
//check quadrants
//if the Y projection is negative, the angle has to be adjusted
if ( py < 0 )
    realangle = ( float ) 2 * PI - realangle;

return realangle;
}

#ifndef RAYDIUM_NETWORK_ONLY
signed char raydium_math_point_unproject_3D(GLfloat x, GLfloat y, GLfloat z, float* resx, float* resy)
{
GLdouble sx,sy,sz;
GLdouble modelMatrix[16];
GLdouble projectionMatrix[16];
GLint   viewport[4];

raydium_camera_replace();
glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
glGetIntegerv(GL_VIEWPORT, viewport);
gluProject(x,y,z,modelMatrix,projectionMatrix,viewport,&sx,&sy,&sz);

sx=sx/raydium_window_tx*100;
sy=sy/raydium_window_ty*100;
(*resx)=sx;
(*resy)=sy;

if(sz<=1.0f)
    return 1; // above the camera

return 0; // behind the camera
}
#endif

// Our matrix_inverse seems broken.
// This code works, thanks to Alexander Zaprjagaev (frustum@public.tsu.ru)
int _raydium_math_MatrixInverse(const float *m,float *out) {
    float   det;
    det = m[0] * m[5] * m[10];
    det += m[4] * m[9] * m[2];
    det += m[8] * m[1] * m[6];
    det -= m[8] * m[5] * m[2];
    det -= m[4] * m[1] * m[10];
    det -= m[0] * m[9] * m[6];
    if(det * det < 1e-25) return 0;
    det = 1.0 / det;
    out[0] =    (m[5] * m[10] - m[9] * m[6]) * det;
    out[1] =  - (m[1] * m[10] - m[9] * m[2]) * det;
    out[2] =    (m[1] * m[6] -  m[5] * m[2]) * det;
    out[3] = 0.0;
    out[4] =  - (m[4] * m[10] - m[8] * m[6]) * det;
    out[5] =    (m[0] * m[10] - m[8] * m[2]) * det;
    out[6] =  - (m[0] * m[6] -  m[4] * m[2]) * det;
    out[7] = 0.0;
    out[8] =    (m[4] * m[9] -  m[8] * m[5]) * det;
    out[9] =  - (m[0] * m[9] -  m[8] * m[1]) * det;
    out[10] =   (m[0] * m[5] -  m[4] * m[1]) * det;
    out[11] = 0.0;
    out[12] = - (m[12] * out[0] + m[13] * out[4] + m[14] * out[8]);
    out[13] = - (m[12] * out[1] + m[13] * out[5] + m[14] * out[9]);
    out[14] = - (m[12] * out[2] + m[13] * out[6] + m[14] * out[10]);
    out[15] = 1.0;
    return 1;
}

void raydium_math_quaternion_normalize(float *quat)
{
float magnitude;

magnitude = sqrt((quat[0] * quat[0]) + (quat[1] * quat[1]) + (quat[2] * quat[2]) + (quat[3] * quat[3]));
quat[0] /= magnitude;
quat[1] /= magnitude;
quat[2] /= magnitude;
quat[3] /= magnitude;
}

void raydium_math_quaternion_multiply(float *q1, float *q2, float *res)
{
float A, B, C, D, E, F, G, H;

A = (q1[3] + q1[0])*(q2[3] + q2[0]);
B = (q1[2] - q1[1])*(q2[1] - q2[2]);
C = (q1[3] - q1[0])*(q2[1] + q2[2]);
D = (q1[1] + q1[2])*(q2[3] - q2[0]);
E = (q1[0] + q1[2])*(q2[0] + q2[1]);
F = (q1[0] - q1[2])*(q2[0] - q2[1]);
G = (q1[3] + q1[1])*(q2[3] - q2[2]);
H = (q1[3] - q1[1])*(q2[3] + q2[2]);

res[3] = B + (-E - F + G + H)/2.0f;//w
res[0] = A - ( E + F + G + H)/2.0f;//x
res[1] = C + ( E - F + G - H)/2.0f;//y
res[2] = D + ( E - F - G + H)/2.0f;//z
}

#define SLERP_TO_LERP_SWITCH_THRESHOLD 0.01f
void raydium_math_quaternion_slerp(float *start, float *end, float alpha,float *result)
{
float startWeight, endWeight, difference;

difference = ((start[0] * end[0]) + (start[1] * end[1]) + (start[2] * end[2]) + (start[3] * end[3]));

if ((1.0f - fabs(difference)) > SLERP_TO_LERP_SWITCH_THRESHOLD)
    {
    float theta, oneOverSinTheta;

    theta = acos(fabs(difference));
    oneOverSinTheta = (1.0f / sin(theta));
    startWeight = (sin(theta * (1.0f - alpha)) * oneOverSinTheta);
    endWeight = (sin(theta * alpha) * oneOverSinTheta);

    if (difference < 0.0f)
        {
        startWeight = -startWeight;
        }
    }
else
    {
    startWeight = (1.0f - alpha);
    endWeight = alpha;
    }

result[0] = ((start[0] * startWeight) + (end[0] * endWeight));
result[1] = ((start[1] * startWeight) + (end[1] * endWeight));
result[2] = ((start[2] * startWeight) + (end[2] * endWeight));
result[3] = ((start[3] * startWeight) + (end[3] * endWeight));
raydium_math_quaternion_normalize(result);
}

void raydium_math_hms(double t, int *h, int *m, int *s, int *ms)
{
(*h)=t/3600;
t-=(*h)*3600;

(*m)=t/60;
t-=(*m)*60;

(*s)=t;
t-=(*s)*1;

(*ms)=t*1000;
}

