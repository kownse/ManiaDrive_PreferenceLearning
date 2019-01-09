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
if loddata('random_fun')==0;
    disp('数据读取失败');
    return;
end
%}

%创建BP网络
%{
P_train = P_train';
T_train = T_train';
%T_train_a = T_train_a';
P_test = P_test';
%T_test_a = T_test_a';

%net = newff(P_train, T_train_a, [10]);
net = newff(P_train, T_train, []);
net.trainParam.epochs = 100;
net.trainParam.lr = 0.01;
net.trainParam.goal = 0.00004;
%net.trainParam.showWindow=0;

%net = train(net, P_train, T_train_a);
net = train(net, P_train, T_train);
an = sim(net, P_test);
%an = sim(net, P_train);

[m, n] = size(an);
wrong =0;
for i=1:n
    target = T_test(i);
    ret = an(i);
    diff = target - ret;
    disp([num2str(target) '   ' num2str(ret) '   ' num2str(diff)]);
    if(abs(diff)>0.5)
        wrong = wrong+1;
    end
end
corratio = (n - wrong) / n


[m, n] = size(an);
for i=1:n
    tmp = max(an(:,i));
    for j=1:m
        if an(j,i)==tmp
            an(j,i)=1;
        else
            an(j,i)=0;
        end
    end
end

cor = 0;
for i=1:n
     if isequal(an(:,i),T_train_a(:,i))
%    if isequal(an(:,i),T_test_a(:,i))
        cor = cor + 1;
%     disp(num2str(T_train(i)));
        %disp(num2str(T_test(i)));
%    else
%        disp(num2str(T_test(i)));
    end
end
corratio = cor / n
%}

%% 创建svm网络
acurrate = svmtrain(T_train,P_train, '-v 10 -c 2 -g 1 -t 0');
%model = svmtrain(T_train,P_train, '-v 5 -c 2 -g 1 -s 3 -t 2 -p 0.1');
%[predict,mse] = svmpredict(T_train,P_train,model);


best_acat = 0;
test_time = 100;
ret_record = zeros(test_time,S+1);

for z=1:test_time
    ret = input_select(510, 8000, 0);
    ret_record(z,:) = ret;
end
save  'G:\game\maniadrive\ManiaDrive_Custom\数据分析\matlab\feature_select\fun_features.mat' ret_record;

%% 创建优化BP网络
