#include "../raydium/raydium/index.h"
#include "../raydium/PreferenceModeling/DataCollect/CollectSingle.h"
#include "../raydium/PreferenceModeling/DataCollect/DataCollect.h"
#include "../raydium/mania_drive.h"
#include <stdio.h>

int local_track_model;

///在本地选择赛道进行游戏
void switch_local_track(void)
{
    static int last=-1;
    FILE *fp;

    raydium_register_variable(mni_next,RAYDIUM_REGISTER_STR,"track");
    raydium_register_variable(&last,RAYDIUM_REGISTER_INT,"last");

    switch(local_track_model)
    {
        case TRACK_ORDERED:
            raydium_php_exec("local_tracks_forward.php");
            break;
        case TRACK_RANDOM:
            raydium_php_exec("local_tracks_random.php");
            break;
        case TRACK_RANDOM_PAIRE:
            raydium_php_exec("local_tracks_random_pair.php");
            break;
    }

    raydium_register_variable_unregister_last();
    raydium_register_variable_unregister_last();

    if(! (fp=raydium_file_fopen(mni_next,"rt")) )
    {
        raydium_log("ERROR ! cannot switch to track '%s' !",mni_next);
        return;
    }
    fclose(fp);
    raydium_log("Switching to track '%s'",mni_next);

    GLfloat from[4]= {0,0,0,0};
    GLfloat to[4]= {0,0,0,1};
    //strcpy(mni_next,track);
    raydium_sound_source_fade(0,1);
    //raydium_osd_fade_from(from,to,1,(void*)AfterFade);
    raydium_gui_window_delete_name("menu");
    raydium_gui_window_delete_name("besttime");

    mode=MODE_SOLO;

    AfterFade();
    //mni_load(track);
}
