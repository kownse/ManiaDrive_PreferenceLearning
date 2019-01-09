#ifndef MANIA_DRIVE_H_INCLUDED
#define MANIA_DRIVE_H_INCLUDED

extern char *version;

#include "raydium/index.h"

#include <libintl.h>
#include <locale.h>

#include "mania.h"
#include "mania_creds.h"

#include "PreferenceModeling/DataCollect/DataCollect.h"

#define NO_NETWORK_COLLISIONS

#ifdef WIN32
#define SERVER_BINARY "mania_server.exe"
#else
#define SERVER_BINARY "./mania_server.static"
#endif

#define STORY_FILE_BEG          "mania_drive.story.beg"
#define STORY_FILE_PRO          "mania_drive.story.pro"
#define HISTORY_STATE_FILE      raydium_file_home_path("mania_drive.state")

#define MUSIC_MENU      "mania_music/i_got_it_bad_-_The_Napoleon_Blown_Aparts.ogg"
#ifdef APPLE
// Offer some common display resolutions for Apple Macintosh.
// 4:3 VGA, SVGA, XGA, UGA / 5:4 SXGA / 16:10 WXGA, WUXGA
#ifdef MACOSX
#define RESOLUTION_LIST "640x480\n800x600\n1024x768\n1280x1024\n1440x900\n1600x1200\n1920x1200"
#endif
// There is only one possible resolution available for the iPhone OS.
#ifdef IPHONEOS
#define RESOLUTION_LIST "480x320"
#endif
#else
#define RESOLUTION_LIST "320x240\n640x480\n800x600\n1024x768\n1152x864"
#endif

#include <tchar.h>

#define NET_SCORE_TRACK (RAYDIUM_NETWORK_PACKET_BASE+1)
#define NET_RESTART     (RAYDIUM_NETWORK_PACKET_BASE+2)
#define NET_CHANGE_MAP  (RAYDIUM_NETWORK_PACKET_BASE+3)
#define NET_REMAINING   (RAYDIUM_NETWORK_PACKET_BASE+4)
#define NET_SCORE_BASE  (RAYDIUM_NETWORK_PACKET_BASE+20)

#define GAME_COUNTDOWN  1
#define GAME_GAME       2
#define GAME_END        3
#define GAME_SCORE      4
#define GAME_MENU       5

#define MODE_NONE       0
#define MODE_SOLO       1
#define MODE_NET        2
#define MODE_MULTI      3
#define MODE_OTHERS     4

#define SHADOW_OFFSET   0.3

#define DIRECT_DEBUG

extern char mni_next[RAYDIUM_MAX_NAME_LEN];
extern signed char mode;
extern char mni_current[RAYDIUM_MAX_NAME_LEN];
extern char message[RAYDIUM_MAX_NAME_LEN];
extern bool bPassed;

void change_game_state(int type);
void create_car(void);
void load_boxes(char *filename);
void dat_load(char *filename);
void hms(GLfloat t,int *h, int *m, int *s, int *ms);
int mni_load(char *mni);
signed char mni_network_get(char *mni);
void build_gui_Main(void);
void music_random(void);
void leave(void);
void build_gui_ErrorDownload(void);
void build_gui_Lan(void);
void btnBackToMainMenu(raydium_gui_Object *w);
void build_gui_NeedRestart(void);
void build_gui_Story(void);
void showMessage(char *file, int id);
void build_gui_Solo(void);
void build_gui_single(void);

typedef struct Score
{
    float time;
    char player[RAYDIUM_MAX_NAME_LEN];
} Score;

typedef struct TrackData
{
    char name[RAYDIUM_MAX_NAME_LEN];
    char author[RAYDIUM_MAX_NAME_LEN];
    float gold_time;
    float author_time;
    char message_file[RAYDIUM_MAX_NAME_LEN];
    char ent_file[RAYDIUM_MAX_NAME_LEN];
} TrackData;

void delete_props(void);
void create_props(char *filename);
void change_music_volume(float vol);
void change_engine_volume(float vol);
float get_score(char *track,char *player);
int post_score(char *track, char *name, char *score);
void post_score_local(char *track, float time);
signed char get_tracklist(char *list);
void restart_asked_from_network(int type,char *buff);
void netcall_mni_change(int type, char *buff);
void netcall_remaining_time(int type, char *buff);
void session_end(int type, char *buff);
signed char mni_network_get(char *mni);
//int mni_load(char *mni);
void btnBestTimeOk(raydium_gui_Object *w);
void btnCantDriveOk(raydium_gui_Object *w);
void btnKeyHelpOk(raydium_gui_Object *w);
void AfterFade(void);
void drive(char *track);
void gui_start(void);
void credits_start(void);
void credits_stop(void);
void after_gui(void);
void video_beg_delete(void);
void btnDriveNet(raydium_gui_Object *w);
void btnDriveSolo(raydium_gui_Object *w);
void btnOtherTracksSolo(raydium_gui_Object *w);
void btnBackToMainMenu(raydium_gui_Object *w);
void btnCantDrive(raydium_gui_Object *w);
void btnKeyHelp(raydium_gui_Object *w);
void btnErrorOkClick(raydium_gui_Object *w);
void btnStoryCompletedOk(raydium_gui_Object *w);
void btnRestartClick(raydium_gui_Object *w);
void btnNetworkConnect(raydium_gui_Object *w);
void net_new_session(raydium_gui_Object *w);
void btnServerClick(raydium_gui_Object *w);
void btnSearchLAN(raydium_gui_Object *w);
void btnCreateServer(raydium_gui_Object *w);
void btnApplyOptions(raydium_gui_Object *w);
void btnEngineVolTest(raydium_gui_Object *w);


int run(int argc, char **argv);
#endif // MANIA_DRIVE_H_INCLUDED
