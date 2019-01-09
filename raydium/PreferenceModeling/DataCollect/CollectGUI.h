#ifndef COLLECTGUI_H_INCLUDED
#define COLLECTGUI_H_INCLUDED

///用于创建数据收集客户端的GUI的函数些
//从游戏中退出到界面时所做的清理
void prelearn_to_gui_clean(void);
//新session开始的界面
void prelearn_build_gui_New_session(void);
//4-AFC界面
void prelearn_build_gui_4AFC(void);
//建立单机界面
void prelearn_build_gui_single(void);
//建立用户信息
void prelearn_build_gui_user_info(int handle, int top = 80, int button = 40);
//建立局域网连接模型
void prelearn_build_gui_Lan(void);
//建立单局问卷
void prelearn_build_gui_level_question(void);
//单机赛道模式
void prelearn_build_gui_set_local_model(void);
//前向赛道
void prelearn_btnForward(raydium_gui_Object *w);
//随机
void prelearn_btnRandomPair(raydium_gui_Object *w);
//自动
void prelearn_btnRandom(raydium_gui_Object *w);
//选择赛道模式
void prelearn_btnChooseTrackModel(raydium_gui_Object *w);


//新的网络session
void prelearn_btnNewGameSession(raydium_gui_Object *w);
//局域网收集界面
void prelearn_btnLocalCollect(raydium_gui_Object *w);
//连接局域网
void prelearn_btnLanConnect(raydium_gui_Object *w);
//局域网收集开始
void prelearn_btnLocalStart(raydium_gui_Object *w);
//关卡信息收集
void prelearn_btnLevel_Result(raydium_gui_Object *w);

//初始化局域网收集的函数指针
void Init_Lan(void);
//初始化单机收集的函数指针
void Init_Single(void);

extern void* newGameSessionFunc;

#endif // COLLECTGUI_H_INCLUDED
