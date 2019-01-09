function level_track_statistic_lvq
clear;
clc;
tic;

%用随机的数据训练
load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\fun_track_statistic_svm\fun_t_s_random.mat';
level_track_mat = fun_track_statistic_mat;
[m,n] = size(level_track_mat);
train_level = [level_track_mat(1:200,2:30)]';
train_level_label = [level_track_mat(1:200,1)]';
for i=1:200
    train_level_label(i) = train_level_label(i)+1;
end
T_train = ind2vec(train_level_label);

test_level = [level_track_mat(201:236,2:30)]';
test_level_label = [level_track_mat(201:236,1)]';

count_A = length(find(train_level_label==1));
count_B = length(find(train_level_label==2));
count_C = length(find(train_level_label==3));
count_D = length(find(train_level_label==4));
count_E = length(find(train_level_label==5));

rate_A = count_A / 200;
rate_B = count_B / 200;
rate_C = count_C / 200;
rate_D = count_D / 200;
rate_E = count_E / 200;

net = newlvq(minmax(train_level), 20, [rate_A rate_B rate_C rate_D rate_E]);

net.trainParam.epochs = 1000;
net.trainParam.show = 10;
net.trainParam.lr = 0.1;
net.trainParam.goal = 0.1;

net = train(net, train_level, T_train);

T_sim = sim(net, test_level);
Tc_sim = vec2ind(T_sim);
result = [Tc_sim;test_level_label];


