clear
clc;
tic;

%% 声明全局变量
global P_train T_train P_test T_test  mint maxt S s1
S=510;
s1=50;

%%读取数据
if loddata('random_feel')==0;
    return;
end
load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\fun_best_feature.mat';
%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\feel_best_feature.mat';

[s1,s]=size(P_train);
bounds=ones(s,1)*[0,1];
% 数据提取
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

% GA的参数选项初始化
ga_option.maxgen = 10;
ga_option.sizepop = 50; 
ga_option.cbound = [0,5];
ga_option.gbound = [0,5];
ga_option.v = 10;
ga_option.ggap = 0.9;
ga_option.t = 0;

%遗传算法求c,g
[bestacc,bestc,bestg] = gaSVMcgForClass(t,p,ga_option);
%粒子群算法求c,g
%[bestacc,bestc,bestg] = psoSVMcgForClass(t,p);
% 打印选择结果
disp('打印选择结果');
str = sprintf( 'Best Cross Validation Accuracy = %g%% Best c = %g Best g = %g',bestacc,bestc,bestg);
disp(str);

%% 利用最佳的参数进行SVM网络训练
cmd = ['-c ',num2str(bestc),' -g ',num2str(bestg)];
model = svmtrain(t,p,cmd);

%% SVM网络预测
[predict_label,accuracy] = svmpredict(T_test,p_test,model);

% 打印测试集分类准确率
total = length(T_test);
right = sum(predict_label == T_test);
disp('打印测试集分类准确率');
str = sprintf( 'Accuracy = %g%% (%d/%d)',accuracy(1),right,total);
disp(str);
disp(['总的运行时间:',num2str(toc)]);

result(model,t,p);
