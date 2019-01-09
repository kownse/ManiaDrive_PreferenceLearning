function ret = input_select_mlp(popu, gen, show)
    global S P_train T_train P_test T_test
    %% �Ŵ��㷨�Ż�
    [s,x]=size(P_train);
    bounds=ones(s,1)*[0,1];
    % ������ʼ��Ⱥ
    % initPop=crtbp(popu,S);
    %initPop=randint(popu,s,[0 1]);
    
    initPop=zeros(popu,s);
    for i=1:s-2
        initPop(i,i) = 1;
        initPop(i,i+1) = 1;
        initPop(i,i+2) = 1;
    end
    
    % �����ʼ��Ⱥ��Ӧ��
    initFit=zeros(popu,1);
    for i=1:size(initPop,1)
        initFit(i)=de_code_mlp(initPop(i,:));
    end
    initPop=[initPop initFit];
    %gen=100; 
    % �Ż�����

    [X,EndPop,BPop,Trace]=ga(bounds,'fitness',[],initPop,[1e-6 1 0],'maxGenTerm',...
        gen,'normGeomSelect',0.09,'twoPointXover',[2],'boundaryMutation',[2 gen 3]);
    
    [m,n]=find(X==1);
    ret = X;
    disp(['�Ż�ɸѡ��������Ա������Ϊ:' num2str(n)]);
    [sm,sn] = size(n);
        disp(['ռ�������������:' num2str(sn/S)]);
    if nargin<3;
        show = 1;
    end
    if show==1;
        % ������Ӧ�Ⱥ�����������
        figure
        plot(Trace(:,1),Trace(:,3),'r:')
        hold on
        plot(Trace(:,1),Trace(:,2),'b')
        hold on
        plot(Trace(:,1),Trace(:,4) * 10,'g')
        xlabel('��������')
        ylabel('��Ӧ�Ⱥ���')
        title('��Ӧ�Ⱥ�����������')
        legend('ƽ����Ӧ�Ⱥ���','�����Ӧ�Ⱥ���')
        xlim([1 gen])
    end
end