% ��ջ�������
clear all
clc
warning off
% ����ȫ�ֱ���
global P_train T_train P_test T_test  mint maxt S s1 K T_train_a
S=510;
s1=50;
K = 5;

%��������
if loddata('random_fun')==0;
    disp('���ݶ�ȡʧ��');
    return;
end

%����BP����
T_train_a = zeros(size(T_train), max(T_train)+1);
for i=1:size(T_train)
    switch T_train(i)
        case 0
            T_train_a(i,:) = [1 0 0 0 0];
        case 1
            T_train_a(i,:) = [0 1 0 0 0];
        case 2
            T_train_a(i,:) = [0 0 1 0 0];
        case 3
            T_train_a(i,:) = [0 0 0 1 0];
        case 4
            T_train_a(i,:) = [0 0 0 0 1];
    end
end


T_test_a = zeros(size(T_test), max(T_train)+1);
for i=1:size(T_test)
    switch T_test(i)
        case 0
            T_test_a(i,:) = [1 0 0 0 0];
        case 1
            T_test_a(i,:) = [0 1 0 0 0];
        case 2
            T_test_a(i,:) = [0 0 1 0 0];
        case 3
            T_test_a(i,:) = [0 0 0 1 0];
        case 4
            T_test_a(i,:) = [0 0 0 0 1];
    end
end


P_train = P_train';
T_train_a = T_train_a';
P_test = P_test';
T_test_a = T_test_a';

%ret = bp_crs(T_train_a, P_train);

tic;
best_acat = 0;
test_time = 10;
ret_record = zeros(test_time,S+1);

for z=1:test_time
    ret = input_select_mlp(510, 4000, 0);
    ret_record(z,:) = ret;
end
save  'G:\game\maniadrive\ManiaDrive_Custom\���ݷ���\matlab\feature_select\slp_fun_features.mat' ret_record;

%% �����Ż�BP����
toc