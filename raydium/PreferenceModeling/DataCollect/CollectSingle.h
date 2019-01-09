#ifndef COLLECTSINGLE_H_INCLUDED
#define COLLECTSINGLE_H_INCLUDED

#define TRACK_ORDERED       0
#define TRACK_RANDOM_PAIRE  1
#define TRACK_RANDOM        2

extern int local_track_model;

///在本地选择赛道进行游戏
void switch_local_track(void);

#endif // COLLECTSINGLE_H_INCLUDED
