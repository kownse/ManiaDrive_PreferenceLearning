#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define C2S_DRIVEINFO_PERFRAME 512

int bCollision;

typedef struct c2s_driveninfo_perframe
{
    int nid;
    int bCollision;
} _PACKED c2s_driveninfo_perframe;

#endif
