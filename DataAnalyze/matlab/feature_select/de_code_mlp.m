function Val=de_code_mlp(x)
    % 全局变量声明
    global S P_train T_train P_test T_test mint maxt T_train_a
    global p t
    
    [s1,s]=size(P_train);
    bounds=ones(s,1)*[0,1];
    % 数据提取
    x=x(:,1:s);
    [m,n]=find(x==1);
    p_train=zeros(size(n,2),size(T_train,1));
    p_test=zeros(size(n,2),size(T_test,1));
    for i=1:length(n)
        p_train(i,:)=P_train(n(i),:);
        p_test(i,:)=P_test(n(i),:);
    end
    t_train=T_train_a;
    p=p_train;
    t=t_train;
    
    % 计算适应度函数值
    accurate = bp_crs(t, p) * 100;
    [sm,sn] = size(n);
    del_affix = (s - sn)/s * 15;
    size_ratio = sn / s;
    disp(['size ratio: ' num2str(size_ratio)]);
    Val = accurate + del_affix;
end