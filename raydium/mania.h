#ifndef _MANIA_H_
#define _MANIA_H_

#define TYPE_CHECKPOINT         100

#define TYPE_LOOP               30

#define TYPE_START_E            10
#define TYPE_START_W            11
#define TYPE_START_N            12
#define TYPE_START_S            13

#define TYPE_END                20

#define TYPE_TURBO_E            2
#define TYPE_TURBO_W            3
#define TYPE_TURBO_N            4
#define TYPE_TURBO_S            5

#define TYPE_CAR                1
#define TYPE_CAR_BODY           7
#define TYPE_CAR_BODY_SP        8
#define TYPE_BALANCIER          6

#define TRACK_TYPE_STRAIGHT     200
#define TRACK_TYPE_CROOK        201
#define TRACK_TYPE_BUMP         202
#define TRACK_TYPE_UPWARD       203
#define TRACK_TYPE_DOWNWARD     204
#define TRACK_TYPE_SPRING       205

#define TURBO_POWA              0.5

#define IS_CAR_BODY(a)          (((a)==TYPE_CAR_BODY) || ((a)==TYPE_CAR_BODY_SP))

typedef struct Grid
{
    char state;
    float x;
    float y;
    float z;
    int rot;
    int flags;
    int obj;
} Grid;

typedef struct Box
{
    char state;
    float x;
    float y;
    float z;
    float tx;
    float ty;
    float tz;
    int type;
    int id;
} Box;

#define MAX_ELEMS       4096
#define FACT            6
#define MOVE_X          -5
#define MOVE_Y          -3
#define MOVE_Z          -1

extern Grid grid[MAX_ELEMS];
extern Box box[MAX_ELEMS];
extern char tdata[4096];

void extract_data(char *from, char *to_name, char *to_author, char *to_gold, char *to_author_time, char *message_file, char *ent_file);

void dump_data_to(char *filename);

// read mni file and generate .tri, .box and .dat
signed char mni_generate(char *filename);

#endif
