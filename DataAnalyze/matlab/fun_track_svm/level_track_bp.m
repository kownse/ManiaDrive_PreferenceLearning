function level_track_svm
clear;
clc;
tic;

%用随机数据进行训练
load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\fun_track_svm\fun_track_ramdom.mat';
level_track_mat = fun_track;
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

t = cputime;
net_bp = newff(minmax(train_level_scale),[50,1],{'tansig','purelin'},'trainlm');
net_bp.trainParam.epochs = 1000;
net_bp.trainParam.show = 10;
net_bp.trainParam.goal = 0.1;
net_bp.trainParam.lr = 0.1;

net_bp = train(net_bp,train_level_scale,train_level_label);

tn_bp_sim = sim(net_bp, test_level);

T_bp_sim = postmnmx(tn_bp_sim,mint,maxt);
e = cputime - t;
