#ifndef COLLECTGUI_H_INCLUDED
#define COLLECTGUI_H_INCLUDED

///���ڴ��������ռ��ͻ��˵�GUI�ĺ���Щ
//����Ϸ���˳�������ʱ����������
void prelearn_to_gui_clean(void);
//��session��ʼ�Ľ���
void prelearn_build_gui_New_session(void);
//4-AFC����
void prelearn_build_gui_4AFC(void);
//������������
void prelearn_build_gui_single(void);
//�����û���Ϣ
void prelearn_build_gui_user_info(int handle, int top = 80, int button = 40);
//��������������ģ��
void prelearn_build_gui_Lan(void);
//���������ʾ�
void prelearn_build_gui_level_question(void);
//��������ģʽ
void prelearn_build_gui_set_local_model(void);
//ǰ������
void prelearn_btnForward(raydium_gui_Object *w);
//���
void prelearn_btnRandomPair(raydium_gui_Object *w);
//�Զ�
void prelearn_btnRandom(raydium_gui_Object *w);
//ѡ������ģʽ
void prelearn_btnChooseTrackModel(raydium_gui_Object *w);


//�µ�����session
void prelearn_btnNewGameSession(raydium_gui_Object *w);
//�������ռ�����
void prelearn_btnLocalCollect(raydium_gui_Object *w);
//���Ӿ�����
void prelearn_btnLanConnect(raydium_gui_Object *w);
//�������ռ���ʼ
void prelearn_btnLocalStart(raydium_gui_Object *w);
//�ؿ���Ϣ�ռ�
void prelearn_btnLevel_Result(raydium_gui_Object *w);

//��ʼ���������ռ��ĺ���ָ��
void Init_Lan(void);
//��ʼ�������ռ��ĺ���ָ��
void Init_Single(void);

extern void* newGameSessionFunc;

#endif // COLLECTGUI_H_INCLUDED
