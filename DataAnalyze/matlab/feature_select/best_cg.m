clear
clc;
tic;

%% ����ȫ�ֱ���
global P_train T_train P_test T_test  mint maxt S s1
S=510;
s1=50;

%%��ȡ����
if loddata('random_feel')==0;
    return;
end
load 'G:\game\maniadrive\ManiaDrive_Custom\���ݷ���\matlab\feature_select\���\fun_best_feature.mat';
%load 'G:\game\maniadrive\ManiaDrive_Custom\���ݷ���\matlab\feature_select\���\feel_best_feature.mat';

[s1,s]=size(P_train);
bounds=ones(s,1)*[0,1];
% ������ȡ
x=best(:,1:s);
[m,n]=find(x==1);
p_train=zeros(size(T_train,1),size(n,1));
p_test=zeros(size(T_test,1),size(n,1));
for i=1:length(n)
    p_train(:,i)=P_train(:,n(i));
    p_test(:,i)=P_test(:,n(i));
end
t_train=T_train;
p=p_train;
t=t_train;

% GA�Ĳ���ѡ���ʼ��
ga_option.maxgen = 10;
ga_option.sizepop = 50; 
ga_option.cbound = [0,5];
ga_option.gbound = [0,5];
ga_option.v = 10;
ga_option.ggap = 0.9;
ga_option.t = 0;

%�Ŵ��㷨��c,g
[bestacc,bestc,bestg] = gaSVMcgForClass(t,p,ga_option);
%����Ⱥ�㷨��c,g
%[bestacc,bestc,bestg] = psoSVMcgForClass(t,p);
% ��ӡѡ����
disp('��ӡѡ����');
str = sprintf( 'Best Cross Validation Accuracy = %g%% Best c = %g Best g = %g',bestacc,bestc,bestg);
disp(str);

%% ������ѵĲ�������SVM����ѵ��
cmd = ['-c ',num2str(bestc),' -g ',num2str(bestg)];
model = svmtrain(t,p,cmd);

%% SVM����Ԥ��
[predict_label,accuracy] = svmpredict(T_test,p_test,model);

% ��ӡ���Լ�����׼ȷ��
total = length(T_test);
right = sum(predict_label == T_test);
disp('��ӡ���Լ�����׼ȷ��');
str = sprintf( 'Accuracy = %g%% (%d/%d)',accuracy(1),right,total);
disp(str);
disp(['�ܵ�����ʱ��:',num2str(toc)]);

result(model,t,p);
