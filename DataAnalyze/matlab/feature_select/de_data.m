function [p,t]=de_data(x)
    % 全局变量声明
    global S P_train T_train P_test T_test mint maxt
    global p t
    [s1,s]=size(P_train);
    bounds=ones(s,1)*[0,1];
    % 数据提取
    x=x(:,1:s);
    [m,n]=find(x==1);
    p_train=zeros(size(T_train,1),size(n,1));
    p_test=zeros(size(T_test,1),size(n,1));
    for i=1:length(n)
        p_train(:,i)=P_train(:,n(i));
        p_test(:,i)=P_test(:,n(i));
    end
    t_train=T_train;
    p=p_train;
    t=t_train;
end