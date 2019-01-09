function level_track_svm
clear;
clc;
tic;

%用随机数据进行训练
load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\fun_track_svm\fun_track_ramdom.mat';
level_track_mat = fun_track_statistic_mat;
[m,n] = size(level_track_mat);
train_level = [level_track_mat(1:200,2:n)];
train_level_label = [level_track_mat(1:200,1)];
test_level = [level_track_mat(201:236,2:n)];
test_level_label = [level_track_mat(201:236,1)];


%{
%用归类数据进行训练
load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\fun_track_svm\fun_track_classed.mat';
level_track_mat = fun_track;
[m,n] = size(level_track_mat);
train_level = [level_track_mat(7:47,2:n); level_track_mat(61:101,2:n);level_track_mat(175:215,2:n)];
train_level_label = [level_track_mat(7:47,1); level_track_mat(61:101,1);level_track_mat(175:215,1)];
test_level = [level_track_mat(48:58,2:n); level_track_mat(102:112,2:n);level_track_mat(216:226,2:n)];
test_level_label = [level_track_mat(48:58,1); level_track_mat(102:112,1);level_track_mat(216:226,1)];
%}

[train_level_scale,ps] = mapminmax(train_level');
ps.ymin = 0;
ps.ymax = 1;
[train_level_scale,ps] = mapminmax(train_level', ps);
train_level_scale = train_level_scale';

[test_level_scale, ps] = mapminmax(test_level');
ps.ymin = 0;
ps.ymax = 1;
[test_level_scale,ps] = mapminmax(test_level', ps);
test_level_scale = test_level_scale';

%{
model = svmtrain(train_level_label,train_level_scale, ' -v 5 -c 2 -g 1 -t 2');
svmpredict(test_level_label, test_level_scale, model);
%}


% GA的参数选项初始化
ga_option.maxgen = 100;
ga_option.sizepop = 50; 
ga_option.cbound = [0,100];
ga_option.gbound = [0,100];
ga_option.v = 5;
ga_option.ggap = 0.9;

%遗传算法求c,g
[bestacc,bestc,bestg] = gaSVMcgForClass(train_level_label,train_level_scale,ga_option);
%粒子群算法求c,g
%[bestacc,bestc,bestg] = psoSVMcgForClass(train_level_label,train_level_scale);
% 打印选择结果
disp('打印选择结果');
str = sprintf( 'Best Cross Validation Accuracy = %g%% Best c = %g Best g = %g',bestacc,bestc,bestg);
disp(str);

%% 利用最佳的参数进行SVM网络训练
cmd = ['-c ',num2str(bestc),' -g ',num2str(bestg)];
model = svmtrain(train_level_label,train_level_scale,cmd);

%% SVM网络预测
[predict_label,accuracy] = svmpredict(test_level_label,test_level_scale,model);

% 打印测试集分类准确率
total = length(test_level_label);
right = sum(predict_label == test_level_label);
disp('打印测试集分类准确率');
str = sprintf( 'Accuracy = %g%% (%d/%d)',accuracy(1),right,total);
disp(str);
disp(['总的运行时间:',num2str(toc)]);

%% 结果分析

% 测试集的实际分类和预测分类图
figure;
hold on;
plot(test_level_label,'o');
plot(predict_label,'r*');
xlabel('测试集样本','FontSize',12);
ylabel('类别标签','FontSize',12);
legend('实际测试集分类','预测测试集分类');
title('测试集的实际分类和预测分类图','FontSize',12);
grid on;
snapnow;