% 清空环境变量
clear all
clc
warning off
% 声明全局变量
global P_train T_train P_test T_test  mint maxt S s1
S=510;
s1=50;

%导入数据
%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\feel_best_P_train.mat';

%if loddata('classed_fun')==0;
%if loddata('random_fun')==0;
%if loddata('level_prefer')==0;
if loddata('wzm_prefer')==0;
    disp('数据读取失败');
    return;
end

%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\fun_best_feature.mat';
%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\feel_best_feature.mat';
%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\fun_prefer_feature.mat';
%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\fun_feature1.mat';
load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\best_prefer_linear_feature.mat';

[s1,s]=size(P_train);
bounds=ones(s,1)*[0,1];
% 数据提取
x=best(:,1:s);
[m,n]=find(x==1);
p_train=zeros(size(T_train,1),size(n,1));
for i=1:length(n)
    p_train(:,i)=P_train(:,n(i));
end
T_train;

p = p_train';

%用线性pca求解
model_pca = pca(p, 5);
out_pca = linproj(p,model_pca);

options.ker = 'rbf';
options.arg = 0.5;
options.new_dim = 5;

%用特征选择后的数据求解
model_kpca = kpca(p, options);
out_kpca = kernelproj(p, model_kpca );
%out_kpca = kpcarec( p, model_kpca );


%用klt做变换
model_klt = klt(T_train,p','sw');

figure; 
%ppatterns( out_pca, T_train ); 
ppatterns( out_kpca, T_train);

%ppatterns( out_pca); 
%ppatterns( out_kpca, 'ro' ); 
