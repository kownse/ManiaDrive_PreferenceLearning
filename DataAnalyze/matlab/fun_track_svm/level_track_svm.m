function level_track_svm
clear;
clc;
tic;

%��������ݽ���ѵ��
load 'G:\game\maniadrive\ManiaDrive_Custom\���ݷ���\matlab\fun_track_svm\fun_track_ramdom.mat';
level_track_mat = fun_track_statistic_mat;
[m,n] = size(level_track_mat);
train_level = [level_track_mat(1:200,2:n)];
train_level_label = [level_track_mat(1:200,1)];
test_level = [level_track_mat(201:236,2:n)];
test_level_label = [level_track_mat(201:236,1)];


%{
%�ù������ݽ���ѵ��
load 'G:\game\maniadrive\ManiaDrive_Custom\���ݷ���\matlab\fun_track_svm\fun_track_classed.mat';
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


% GA�Ĳ���ѡ���ʼ��
ga_option.maxgen = 100;
ga_option.sizepop = 50; 
ga_option.cbound = [0,100];
ga_option.gbound = [0,100];
ga_option.v = 5;
ga_option.ggap = 0.9;

%�Ŵ��㷨��c,g
[bestacc,bestc,bestg] = gaSVMcgForClass(train_level_label,train_level_scale,ga_option);
%����Ⱥ�㷨��c,g
%[bestacc,bestc,bestg] = psoSVMcgForClass(train_level_label,train_level_scale);
% ��ӡѡ����
disp('��ӡѡ����');
str = sprintf( 'Best Cross Validation Accuracy = %g%% Best c = %g Best g = %g',bestacc,bestc,bestg);
disp(str);

%% ������ѵĲ�������SVM����ѵ��
cmd = ['-c ',num2str(bestc),' -g ',num2str(bestg)];
model = svmtrain(train_level_label,train_level_scale,cmd);

%% SVM����Ԥ��
[predict_label,accuracy] = svmpredict(test_level_label,test_level_scale,model);

% ��ӡ���Լ�����׼ȷ��
total = length(test_level_label);
right = sum(predict_label == test_level_label);
disp('��ӡ���Լ�����׼ȷ��');
str = sprintf( 'Accuracy = %g%% (%d/%d)',accuracy(1),right,total);
disp(str);
disp(['�ܵ�����ʱ��:',num2str(toc)]);

%% �������

% ���Լ���ʵ�ʷ����Ԥ�����ͼ
figure;
hold on;
plot(test_level_label,'o');
plot(predict_label,'r*');
xlabel('���Լ�����','FontSize',12);
ylabel('����ǩ','FontSize',12);
legend('ʵ�ʲ��Լ�����','Ԥ����Լ�����');
title('���Լ���ʵ�ʷ����Ԥ�����ͼ','FontSize',12);
grid on;
snapnow;