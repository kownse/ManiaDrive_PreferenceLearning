function level_track_svm
clear;
clc;
tic;

%level_track_mat = cell2mat(fun_track);
load 'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\fun_track.mat';
level_track_mat = fun_track;
[m,n] = size(level_track_mat);
train_level = [level_track_mat(7:47,2:n); level_track_mat(61:101,2:n);level_track_mat(175:215,2:n)];
train_level_label = [level_track_mat(7:47,1); level_track_mat(61:101,1);level_track_mat(175:215,1)];
test_level = [level_track_mat(48:58,2:n); level_track_mat(102:112,2:n);level_track_mat(216:226,2:n)];
test_level_label = [level_track_mat(48:58,1); level_track_mat(102:112,1);level_track_mat(216:226,1)];

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

%model = svmtrain(train_level_label,train_level_scale, ' -v 5 -c 2 -g 1 -t 0');
%svmpredict(test_level_label, test_level_scale, model);

% GA的参数选项初始化
ga_option.maxgen = 100;
ga_option.sizepop = 20; 
ga_option.cbound = [0,100];
ga_option.gbound = [0,100];
ga_option.v = 5;
ga_option.ggap = 0.9;

[bestacc,bestc,bestg] = gaSVMcgForClass(train_level_label,train_level_scale,ga_option);
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

%% 子函数 gaSVMcgForClass.m
function [BestCVaccuracy,Bestc,Bestg,ga_option] = gaSVMcgForClass(train_label,train_data,ga_option)
% gaSVMcgForClass

%
% by faruto
%Email:patrick.lee@foxmail.com QQ:516667408 http://blog.sina.com.cn/faruto BNU
%last modified 2010.01.17
%Super Moderator @ www.ilovematlab.cn

% 若转载请注明：
% faruto and liyang , LIBSVM-farutoUltimateVersion 
% a toolbox with implements for support vector machines based on libsvm, 2009. 
% Software available at http://www.ilovematlab.cn
% 
% Chih-Chung Chang and Chih-Jen Lin, LIBSVM : a library for
% support vector machines, 2001. Software available at
% http://www.csie.ntu.edu.tw/~cjlin/libsvm

% 参数初始化
if nargin == 2
    ga_option = struct('maxgen',200,'sizepop',20,'ggap',0.9,...
        'cbound',[0,100],'gbound',[0,1000],'v',5);
end
% maxgen:最大的进化代数,默认为200,一般取值范围为[100,500]
% sizepop:种群最大数量,默认为20,一般取值范围为[20,100]
% cbound = [cmin,cmax],参数c的变化范围,默认为(0,100]
% gbound = [gmin,gmax],参数g的变化范围,默认为[0,1000]
% v:SVM Cross Validation参数,默认为5

%
MAXGEN = ga_option.maxgen;
NIND = ga_option.sizepop;
NVAR = 2;
PRECI = 20;
GGAP = ga_option.ggap;
trace = zeros(MAXGEN,2);

FieldID = ...
[rep([PRECI],[1,NVAR]);[ga_option.cbound(1),ga_option.gbound(1);ga_option.cbound(2),ga_option.gbound(2)]; ...
 [1,1;0,0;0,1;1,1]];

Chrom = crtbp(NIND,NVAR*PRECI);

gen = 1;
v = ga_option.v;
BestCVaccuracy = 0;
Bestc = 0;
Bestg = 0;
%
cg = bs2rv(Chrom,FieldID);

for nind = 1:NIND
    cmd = ['-v ',num2str(v),' -c ',num2str(cg(nind,1)),' -g ',num2str(cg(nind,2))];
    ObjV(nind,1) = svmtrain(train_label,train_data,cmd);
end
[BestCVaccuracy,I] = max(ObjV);
Bestc = cg(I,1);
Bestg = cg(I,2);

for gen = 1:MAXGEN
    FitnV = ranking(-ObjV);
    
    SelCh = select('sus',Chrom,FitnV,GGAP);
    SelCh = recombin('xovsp',SelCh,0.7);
    SelCh = mut(SelCh);
    
    cg = bs2rv(SelCh,FieldID);
    for nind = 1:size(SelCh,1)
        cmd = ['-v ',num2str(v),' -c ',num2str(cg(nind,1)),' -g ',num2str(cg(nind,2))];
        ObjVSel(nind,1) = svmtrain(train_label,train_data,cmd);
    end
    
    [Chrom,ObjV] = reins(Chrom,SelCh,1,1,ObjV,ObjVSel);
    
    if max(ObjV) <= 40
        continue;
    end
    
    [NewBestCVaccuracy,I] = max(ObjV);
    cg_temp = bs2rv(Chrom,FieldID);
    temp_NewBestCVaccuracy = NewBestCVaccuracy;
    
    if NewBestCVaccuracy > BestCVaccuracy
       BestCVaccuracy = NewBestCVaccuracy;
       Bestc = cg_temp(I,1);
       Bestg = cg_temp(I,2);
    end
    
    if abs( NewBestCVaccuracy-BestCVaccuracy ) <= 10^(-2) && ...
        cg_temp(I,1) < Bestc
       BestCVaccuracy = NewBestCVaccuracy;
       Bestc = cg_temp(I,1);
       Bestg = cg_temp(I,2);
    end    
    
    trace(gen,1) = max(ObjV);
    trace(gen,2) = sum(ObjV)/length(ObjV);
    str = sprintf( 'Gen = %d',gen);
    disp(str);
end
%
figure;
hold on;
trace = round(trace*10000)/10000;
plot(trace(1:gen,1),'r*-','LineWidth',1.5);
plot(trace(1:gen,2),'o-','LineWidth',1.5);
legend('最佳适应度','平均适应度',3);
xlabel('进化代数','FontSize',12);
ylabel('适应度','FontSize',12);
axis([0 gen 0 100]);
grid on;
axis auto;

line1 = '适应度曲线Accuracy[GAmethod]';
line2 = ['(终止代数=', ...
    num2str(gen),',种群数量pop=', ...
    num2str(NIND),')'];
line3 = ['Best c=',num2str(Bestc),' g=',num2str(Bestg), ...
    ' CVAccuracy=',num2str(BestCVaccuracy),'%'];
title({line1;line2;line3},'FontSize',12);

%%%
% 
% <html>
% <table align="center" >	<tr>		<td align="center"><font size="2">版权所有：</font><a
% href="http://www.ilovematlab.cn/">Matlab中文论坛</a>&nbsp;&nbsp; <script
% src="http://s3.cnzz.com/stat.php?id=971931&web_id=971931&show=pic" language="JavaScript" ></script>&nbsp;</td>	</tr></table>
% </html>
% 