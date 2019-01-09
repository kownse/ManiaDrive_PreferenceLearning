function ret = input_select(popu, gen, show)
    global S P_train T_train P_test T_test
    %% 遗传算法优化
    [x,s]=size(P_train);
    bounds=ones(s,1)*[0,1];
    % 产生初始种群
    
    %initPop=randint(popu,s,[0 1]);
    
    initPop=zeros(popu,s);
    for i=1:s
        %{
        for j=1:30
            if randint(1,1, [0 5]) > 3
                initPop(i,j)=1;
            end
        end
        %}
        initPop(i,i) = 1;
        for j=1:5
            rindex = randi([1 s]);
            initPop(i,rindex) = 1;
        end
    end
    
    % 计算初始种群适应度
    initFit=zeros(popu,1);
    for i=1:size(initPop,1)
        initFit(i)=de_code(initPop(i,:));
    end
    initPop=[initPop initFit];
    %gen=100; 
    % 优化计算

    [X,EndPop,BPop,Trace]=ga(bounds,'fitness',[],initPop,[1e-6 1 0],'maxGenTerm',...
        gen,'normGeomSelect',0.09,'twoPointXover',[2],'boundaryMutation',[2 gen 3]);
    
    [m,n]=find(X==1);
    ret = X;
    disp(['优化筛选后的输入自变量编号为:' num2str(n)]);
    [sm,sn] = size(n);
        disp(['占所有输入参数的:' num2str(sn/S)]);
    if nargin<3;
        show = 1;
    end
    if show==1;
        % 绘制适应度函数进化曲线
        figure
        plot(Trace(:,1),Trace(:,3),'r:')
        hold on
        plot(Trace(:,1),Trace(:,2),'b')
        hold on
        plot(Trace(:,1),Trace(:,4) * 10,'g')
        xlabel('进化代数')
        ylabel('适应度函数')
        title('适应度函数进化曲线')
        legend('平均适应度函数','最佳适应度函数')
        xlim([1 gen])
    end
end