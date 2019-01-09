% 清空环境变量
clear all
clc
warning off
% 声明全局变量
global P_train T_train P_test T_test  mint maxt S s1
S=510;
s1=50;

pop = S;
gen = 8000;
test_time = 50;

%导入数据
%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\feel_best_P_train.mat';

if loddata('level_prefer')==0;
    disp('数据读取失败');
    return;
end

ret_record_prefer = zeros(test_time,S+1);

for z=1:test_time
    ret = input_select(pop, gen, 0);
    ret_record_prefer(z,:) = ret;
    clc;
end
save  'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\fun_prefer_nor_pca_linear_features1.mat' ret_record_prefer;

%{
%if loddata('classed_fun')==0;
if loddata('random_fun')==0;
%if loddata('level_prefer')==0;
    disp('数据读取失败');
    return;
end

best_acat = 0;
ret_record_fun = zeros(test_time,S+1);

for z=1:test_time
    ret = input_select(pop, gen, 0);
    ret_record_fun(z,:) = ret;
end
save  'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\fun_std_linear_features.mat' ret_record_fun;

if loddata('difficult_random')==0;
    disp('数据读取失败');
    return;
end

ret_record_dif = zeros(test_time,S+1);

for z=1:test_time
    ret = input_select(pop, gen, 0);
    ret_record_dif(z,:) = ret;
end
save  'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\dif_std_features.mat' ret_record_dif;



if loddata('skill_random')==0;
    disp('数据读取失败');
    return;
end

ret_record_skill = zeros(test_time,S+1);

for z=1:test_time
    ret = input_select(pop, gen, 0);
    ret_record_skill(z,:) = ret;
end
save  'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\skill_std_features.mat' ret_record_skill;
%}