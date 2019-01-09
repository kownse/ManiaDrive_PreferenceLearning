clear all
clc
warning off

global P_train T_train P_test T_test  mint maxt S s1
S = 510;

load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\结果\features_fun.mat';
[m,n] = size(ret_record);

ret = zeros(n-1,1);
scale_acr = ret_record(:,511);
max_acr = max(scale_acr);
min_acr = min(scale_acr);
dis_acr = max_acr - min_acr;

for i=1:m
    scale_acr(i) = (scale_acr(i) - min_acr) / dis_acr;
end

for i=1:n-1
    for j=1:m
        if ret_record(j,i)==1
            ret(i) = ret(i) + scale_acr(j);
        end
    end
end

if loddata('classed_fun')==0;
    disp('数据读取失败');
    return;
end

max_rat = max(ret);
step = 0.005;
sparam = 1 / step;
fc_acr = zeros(max_rat*sparam, 3);
for i = 1:max_rat*sparam
    [findex, fvar] = find(ret > (i*step));
    p_train=zeros(size(T_train,1),size(findex,1));
    fs = length(findex);
    for j=1:fs
        p_train(:,j)=P_train(:,findex(j));
    end
    p=p_train;
    t=T_train;
    
    accurate = svmtrain(t,p, '-v 5 -c 2 -g 1 -t 2');
    fc_acr(i,1) = i*step;
    fc_acr(i,2) = fs;
    fc_acr(i,3) = accurate; 
end