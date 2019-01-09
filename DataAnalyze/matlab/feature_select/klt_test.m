clear;
clc;

global P_train T_train P_test T_test  mint maxt S s1
S=510;
s1=50;

%loddata('random_fun');
%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\best_fun_linear_feature.mat';

loddata('level_prefer');
%loddata('wzm_prefer');
%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\best_prefer_linear_feature.mat';
load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\best_prefer_std.mat';

%loddata('difficult_random');
%load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\best_dif_linear_feature.mat';

[m,n]=find(best==1);
p_train=zeros(size(T_train,1),size(n,1));
p_test=zeros(size(T_test,1),size(n,1));
for i=1:length(n)
    p_train(:,i)=P_train(:,n(i));
    p_test(:,i)=P_test(:,n(i));
end
t_train=T_train;
p=p_train;
t=t_train;
km = klt(t,p,'var');

fT = T_train;
[m,n] = size(fT);
for i=1:m
    if fT(i)==-1;
        fT(i)=2;
    end
end
data.y = fT;
data.X = km.out';


km2 = klt(t,p,'mvzip');
nf = size(p,2)
kmw = zeros(nf,3);
kmw(:,1) = km2.W(:,1);
kmw(:,2) = km.W(:,2);
kmw(:,3) = km2.W(:,2);
km_out = p * kmw;
data.X = km_out';

ppatterns(data);