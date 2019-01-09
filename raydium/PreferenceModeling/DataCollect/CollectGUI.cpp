#include "../raydium/raydium/index.h"
#include "PreferenceModeling/DataCollect/DataCollect.h"
#include "../raydium/PreferenceModeling/DataCollect/DataCollect.h"
#include "../raydium/PreferenceModeling/DataCollect/CollectGUI.h"
#include "../raydium/PreferenceModeling/DataCollect/CollectNet.h"
#include "../raydium/PreferenceModeling/DataCollect/CollectSingle.h"
#include "libintl.h"
#include "PreferenceModeling/utility.h"
#include "../raydium/mania_drive.h"

void* newGameSessionFunc = (void*)prelearn_send_new_session;

void prelearn_btnPreference_Result_A(raydium_gui_Object *w)
{
    preference_result = PREFERENCE_RESULT_A;
    //prelearn_gameinfo_prefer_send();
    //((void (*)(void))afterPreferResultFunc)();
    assert(func_prefer_collect && "偏好数据处理函数为空");
    func_prefer_collect();
}

void prelearn_btnPreference_Result_B(raydium_gui_Object *w)
{
    preference_result = PREFERENCE_RESULT_B;
    //prelearn_gameinfo_prefer_send();
    //prelearn_send_prefer_result();
    //((void (*)(void))afterPreferResultFunc)();
    assert(func_prefer_collect && "偏好数据处理函数为空");
    func_prefer_collect();
}

void prelearn_btnPreference_Result_Both(raydium_gui_Object *w)
{
    preference_result = PREFERENCE_RESULT_BOTH;
    //prelearn_gameinfo_prefer_send();
    //prelearn_send_prefer_result();
    //((void (*)(void))afterPreferResultFunc)();
    assert(func_prefer_collect && "偏好数据处理函数为空");
    func_prefer_collect();
}

void prelearn_btnPreference_Result_None(raydium_gui_Object *w)
{
    preference_result = PREFERENCE_RESULT_NONE;
    //prelearn_gameinfo_prefer_send();
    //prelearn_send_prefer_result();
    //((void (*)(void))afterPreferResultFunc)();
    assert(func_prefer_collect && "偏好数据处理函数为空");
    func_prefer_collect();
}

void prelearn_btnLevel_Result(raydium_gui_Object *w)
{
    assert(func_level_result_collect && "关卡数据处理函数为空");
    func_level_result_collect();
}

//从游戏到GUI的清理
void prelearn_to_gui_clean(void)
{
    mni_current[0]=0;
    message[0]=0;
    //raydium_network_client_disconnect();
    raydium_ode_time_change(0);
    mode=MODE_NONE;

    raydium_clear_frame();
    raydium_camera_look_at(0.1,0.1,0,0,1,0);
    raydium_osd_draw_name("mania_logo2.tga",0,0,100,100);

    raydium_gui_window_delete_name("menu");
    raydium_rendering_finish();
}

void prelearn_build_gui_4AFC(void)
{
    prelearn_to_gui_clean();

    //preference_index = raydium_random_i(0, PREFERENCE_MAX);
    preference_index = prelearn_get_session_emotion();
    //preference_index = PREFERENCE_FUN;

    int handle;
    handle=raydium_gui_window_create("menu",48,10,50,40);
    raydium_gui_widget_sizes(0,0,18);
    //raydium_gui_label_create("lblInfo",handle,50,90,gettext("Which Track make you feel more"),0,0,0);
    raydium_gui_label_create("lblInfo",handle,50,90,gettext("\237 \241 \242 \243 \244 \245 \246 \247 \250"),0,0,0);

    char szEmotion[20];
    getPreferStr(preference_index, szEmotion);
    raydium_gui_widget_sizes(0,0,24);
    raydium_gui_label_create("lblPreference", handle, 50, 75, gettext(szEmotion), 255,0,0);

    raydium_gui_widget_sizes(18,5,18);
    raydium_gui_button_create("btnA",handle,30,55,gettext("\301 \302 \303"), (void*)prelearn_btnPreference_Result_A);
    raydium_gui_button_create("btnB",handle,30,40,gettext("\301 \304 \303"), (void*)prelearn_btnPreference_Result_B);
    raydium_gui_button_create("btnBoth",handle,30,25,gettext("\305 \306"), (void*)prelearn_btnPreference_Result_Both);
    raydium_gui_button_create("btnNone",handle,30,10,gettext("\305 \307"), (void*)prelearn_btnPreference_Result_None);

    //raydium_gui_widget_sizes(8,3,14);
    //raydium_gui_button_create("btnAgain",handle,4,5,gettext("Again"),btnTest4AFC);
    gui_start();
}

void prelearn_build_gui_New_session(void)
{
    int handle;
    raydium_gui_window_delete_name("menu");
    handle=raydium_gui_window_create("menu",48,10,50,40);

    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblMode",handle,50,90,gettext("- New Game Session - "),0,0,0);
    raydium_gui_label_create("lblInfo",handle,50,70,gettext("You will play 2 tracks"),0,0,0);

    raydium_gui_widget_sizes(18,5,18);
    raydium_gui_button_create("btnMulti",handle,25,35,gettext("Begin"),(void*)prelearn_btnNewGameSession);

    mode = MODE_MULTI;
    gui_start();
}

//设置赛道模式
void prelearn_build_gui_set_local_model(void)
{
    int handle;
    raydium_gui_window_delete_name("menu");
    handle=raydium_gui_window_create("menu",48,10,50,40);

    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblLocalTrackMode",handle,50,90,gettext("- Track Model - "),0,0,0);

    raydium_gui_widget_sizes(27,4,18);
    raydium_gui_button_create("btnTrackModelOrdered",handle,25,55,gettext("\347 \350 \345 \346"),(void*)prelearn_btnForward);
    raydium_gui_button_create("btnTrackModelRandomPaire",handle,25,40,gettext("\351 \352 \353 \345 \346"),(void*)prelearn_btnRandomPair);
    raydium_gui_button_create("btnTrackModelRandom",handle,25,25,gettext("\351 \352 \345 \346"),(void*)prelearn_btnRandom);

    raydium_gui_widget_sizes(6,3,14);
    raydium_gui_button_create("btnBackToMain",handle,5,5,gettext("<"),(void*)btnBackToMainMenu);

    gui_start();
}

void prelearn_btnChooseTrackModel(raydium_gui_Object *w)
{
    prelearn_handle_user_info();
    prelearn_build_gui_set_local_model();
}

//前向赛道
void prelearn_btnForward(raydium_gui_Object *w)
{
    local_track_model = TRACK_ORDERED;

    int next_model = (local_track_model + 1)%3;
    char buf[1];
    itoa(next_model, buf, 10);
    raydium_parser_db_set("local_track_model", buf);

    prelearn_build_gui_New_session();
}

//随机
void prelearn_btnRandomPair(raydium_gui_Object *w)
{
    local_track_model = TRACK_RANDOM_PAIRE;

    int next_model = (local_track_model + 1)%3;
    char buf[1];
    itoa(next_model, buf, 10);
    raydium_parser_db_set("local_track_model", buf);

    prelearn_build_gui_New_session();
}

//自动
void prelearn_btnRandom(raydium_gui_Object *w)
{
    /*
    //设置赛道脚本
    char lagLocalModel[RAYDIUM_MAX_NAME_LEN];
    raydium_parser_db_get("local_track_model",lagLocalModel,"0");
    sscanf(lagLocalModel,"%d",&local_track_model);
    */

    local_track_model = TRACK_RANDOM;

    int next_model = (local_track_model + 1)%3;
    char buf[1];
    itoa(next_model, buf, 10);
    raydium_parser_db_set("local_track_model", buf);

    prelearn_build_gui_New_session();
}

void prelearn_btnNewGameSession(raydium_gui_Object *w)
{
    GLfloat from[4]= {0,0,0,0};
    GLfloat to[4]= {0,0,0,1};

    raydium_sound_source_fade(0,1);
    raydium_osd_fade_from(from,to,1,newGameSessionFunc);
    raydium_gui_window_delete_name("menu");
    raydium_gui_window_delete_name("besttime");

    assert(func_userinfo_collect && "用户信息处理函数为空");
    func_userinfo_collect();

    //清理收集数据结构
    prelearn_gameinfo_clean_local();
}

void Init_Lan(void)
{
    newGameSessionFunc = (void*)prelearn_send_new_session;
    //afterPreferResultFunc = (void*)prelearn_send_prefer_result;

    func_prefer_collect = prelearn_gameinfo_prefer_send;
    func_attempt_collect = prelearn_gameinfo_attempt_send;
    func_level_collect = prelearn_gameinfo_level_send;
    func_original_collect = prelearn_gameinfo_original_send;
    func_userinfo_collect = prelearn_gameinfo_userinfo_send;
    func_level_result_collect = prelearn_gameinfo_level_result_send;
}

void Init_Single(void)
{
    newGameSessionFunc = (void*)switch_local_track;
    func_prefer_collect = prelearn_gameinfo_prefer_store;
    func_attempt_collect = prelearn_gameinfo_attempt_store;
    func_level_collect = prelearn_gameinfo_level_store;
    func_original_collect = prelearn_gameinfo_original_store;
    func_userinfo_collect = prelearn_gameinfo_userinfo_store;
    func_level_result_collect = prelearn_gameinfo_level_result_store;
    //afterPreferResultFunc = (void*)prelearn_store_prefer_result_local;
}

//单机收集界面
void prelearn_btnLocalCollect(raydium_gui_Object *w)
{
    prelearn_build_gui_single();
}

//单机收集游戏开始
void prelearn_btnLocalStart(raydium_gui_Object *w)
{
    //设置赛道脚本
    char lagLocalModel[RAYDIUM_MAX_NAME_LEN];
    raydium_parser_db_get("local_track_model",lagLocalModel,"0");
    sscanf(lagLocalModel,"%d",&local_track_model);

    int next_model = (local_track_model + 1)%3;
    char buf[1];
    itoa(next_model, buf, 10);
    raydium_parser_db_set("local_track_model", buf);

    prelearn_handle_user_info();
    prelearn_build_gui_New_session();
}

void prelearn_build_gui_single(void)
{
    raydium_gui_window_delete_name("menu");
    raydium_gui_window_delete_name("besttime");

    int handle;
    handle=raydium_gui_window_create("menu",48,10,50,50);
    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblInfo",handle,50,90,gettext("- User Info -"),0,0,0);

    //建立用户信息界面
    prelearn_build_gui_user_info(handle, 80, 30);

    raydium_gui_widget_sizes(18,5,18);
    raydium_gui_button_create("OK",handle,30,5,gettext("OK"), (void*)prelearn_btnChooseTrackModel);

    raydium_gui_widget_sizes(6,3,14);
    raydium_gui_button_create("btnBackToMain",handle,5,5,gettext("<"),(void*)btnBackToMainMenu);

    Init_Single();
    //raydium_gui_widget_sizes(8,3,14);
    //raydium_gui_button_create("btnAgain",handle,4,5,gettext("Again"),btnTest4AFC);
    gui_start();
}

//建立用户信息窗口
void prelearn_build_gui_user_info(int handle, int top, int button)
{
    float gap = (float)(top - button) / 4.0f;
    float theta = 4.5f;

    raydium_gui_label_create("lblPlayerName",handle,25,top,gettext("ID :"),0,0,0);
    raydium_gui_widget_sizes(25,4,18);
    raydium_gui_edit_create("edtPlayerName",handle,37,top - theta,raydium_network_name_local);

    raydium_gui_label_create("lblPlayerSex",handle,25,top - gap,gettext("\310 \311 :"),0,0,0);
    raydium_gui_widget_sizes(17,4,18);
    //raydium_gui_combo_create("cboPlayerSex",handle,37,top - gap - theta,PLAYER_SEX_LIST,raydium_gui_list_id(player_sex,PLAYER_SEX_LIST));
    raydium_gui_combo_create("cboPlayerSex",handle,37,top - gap - theta,PLAYER_SEX_LIST,userinfo.sex);

    raydium_gui_label_create("lblPlayerAge",handle,25, top - gap*2,gettext("\312 \313 :"),0,0,0);
    raydium_gui_widget_sizes(25,4,18);
    raydium_gui_edit_create("edtPlayerAge",handle,37, top - gap*2 - theta,player_age_buf);

    raydium_gui_label_create("lblPlayerType",handle,25,top - gap*3,gettext("\354 \355 \356 \357:"),0,0,0);
    raydium_gui_widget_sizes(17,4,18);
    //raydium_gui_combo_create("cboPlayerType",handle,37,top - gap*3 - theta,PLAYER_TYPE_LIST,raydium_gui_list_id(player_type,PLAYER_TYPE_LIST));
    raydium_gui_combo_create("cboPlayerType",handle,37,top - gap*3 - theta,PLAYER_TYPE_LIST,userinfo.type);

    raydium_gui_label_create("lblPlayerFrequency",handle,25,top - gap*4,gettext("\354 \355 \316 \317:"),0,0,0);
    raydium_gui_widget_sizes(30,4,18);
    //raydium_gui_combo_create("cboPlayerFrequency",handle,37,top - gap*4 - theta,PLAYER_FREQUENCY_LIST,raydium_gui_list_id(player_game_frequency,PLAYER_FREQUENCY_LIST));
    raydium_gui_combo_create("cboPlayerFrequency",handle,37,top - gap*4 - theta,PLAYER_FREQUENCY_LIST,userinfo.frequency);
}

void prelearn_build_gui_Lan(void)
{
    int handle;
    char server[RAYDIUM_MAX_NAME_LEN];

    raydium_parser_db_get("Generic-PlayerName",raydium_network_name_local,NULL);
    raydium_parser_db_get("ManiaDrive-Server",server,"192.168.0.1");

    raydium_network_client_discover("ManiaDrive",10);

    //handle=raydium_gui_window_create("menu",4,10,92,40);
    handle=raydium_gui_window_create("menu",48,10,50,60);

    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblMode",handle,50,90,gettext("- Network Game - "),0,0,0);

    prelearn_build_gui_user_info(handle, 80, 40);

    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblServer",handle,25,25,gettext("Server :"),0,0,0);
    raydium_gui_widget_sizes(25,4,18);
    raydium_gui_edit_create("edtServer",handle,37,21.5,server);

    raydium_gui_widget_sizes(18,5,18);
    raydium_gui_button_create("btnMulti",handle,30,5,gettext("Connect"),(void*)prelearn_btnLanConnect);

    raydium_gui_widget_sizes(6,3,14);
    raydium_gui_button_create("btnBackToMain",handle,5,5,gettext("<"),(void*)btnBackToMainMenu);

    gui_start();
}

//建立关卡问卷
void prelearn_build_gui_level_question(void)
{
    prelearn_to_gui_clean();

    int handle;
    handle=raydium_gui_window_create("menu",48,10,50,50);

    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblInfo1",handle,50,90,gettext("- Evaluate this Level -"),0,0,0);

    raydium_gui_label_create("lblLevelDifficult",handle,30,75,gettext("\230 \231 \36 \37 :"),0,0,0);
    raydium_gui_widget_sizes(23,4,18);
    raydium_gui_combo_create("cboLevelDifficult",handle,50,70,LEVEL_DIFFICULT_LIST,-1);

    raydium_gui_label_create("lblLevelSkill",handle,30,65,gettext("\232 \233 \234 \235 :"),0,0,0);
    raydium_gui_widget_sizes(23,4,18);
    raydium_gui_combo_create("cboLevelSkill",handle,50,60,LEVEL_SKILL_LIST,-1);

    raydium_gui_label_create("lblLevelFun",handle,30,55,gettext("\224 \225 \30 \31 :"),0,0,0);
    raydium_gui_widget_sizes(23,4,18);
    raydium_gui_combo_create("cboLevelFun",handle,50,50,LEVEL_FUN_LIST,-1);


    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblInfo1",handle,50,90,gettext("- Evaluate Your Emotion -"),0,0,0);

    raydium_gui_widget_sizes(23,4,18);
    raydium_gui_label_create("lblLevelFeel",handle,30,35,gettext("\226 \227 \265 \266 :"),0,0,0);
    raydium_gui_widget_sizes(23,4,18);
    raydium_gui_combo_create("cboLevelFeel",handle,50,30,LEVEL_FEEL_LIST,-1);

    raydium_gui_label_create("lblEmotionDegree",handle,30,25,gettext("\265 \266 \267 \270 :"),0,0,0);
    raydium_gui_widget_sizes(23,4,18);
    raydium_gui_combo_create("cboEmotionDegree",handle,50,20,DEGREE_LIST,-1);

    raydium_gui_widget_sizes(18,5,18);
    raydium_gui_button_create("btnLevelQuestionOver",handle,30,5,gettext("OK"),(void*)prelearn_btnLevel_Result);
    gui_start();
}

void prelearn_btnLanConnect(raydium_gui_Object *w)
{
    char server[512];
    int handle;
//    char str[RAYDIUM_MAX_NAME_LEN];

    //设置用户属性
    raydium_gui_read_name("menu","edtServer",server);
    raydium_parser_db_set("ManiaDrive-Server",server);
    prelearn_handle_user_info();

    strcpy(server_ip, server);
    if(!raydium_network_client_connect_to(server))
    {
        handle=raydium_gui_window_create("error",25,45,50,15);
        raydium_gui_widget_sizes(0,0,18);
        raydium_gui_label_create("lblError",handle,50,80,gettext("Cannot connect to server ..."),0,0,0);
        raydium_gui_widget_sizes(15,5,18);
        raydium_gui_button_create("btnErrorOk",handle,35,15,gettext("OK"),(void*)btnErrorOkClick);
        return;
    }

    mode=MODE_MULTI;
    raydium_gui_window_delete_name("menu");
    handle=raydium_gui_window_create("menu",20,45,55,15);
    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblInfo",handle,150,50,gettext("This is a Gameplay Information Collection\n     by 411 Lab of Sichuan University\n\n     Please Wait..."),0,0,0);

    Init_Lan();
#ifdef DIRECT_DEBUG
    prelearn_build_gui_New_session();
#endif
    //raydium_gui_label_create("lblInfo",handle,50,100,gettext("by 411 Lab of Sichuan University"),0,0,0);
    //raydium_gui_label_create("lblInfo",handle,50,150,gettext("Please Wait..."),0,0,0);
}
