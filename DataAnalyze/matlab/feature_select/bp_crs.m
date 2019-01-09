function Val=bp_crs(t,p)
    global K
    K = 5;
    [m,n] = size(t);
    indices = crossvalind('Kfold', n, K);
    col_total = 0;

    cor_mat = zeros(K,1);
    for i=1:K  
        index_test = (indices == i);
        index_train = ~index_test; 

        [mm,nn]=find(index_train==1);
        p_train=zeros(size(p,1),size(mm,1));
        t_train=zeros(size(t,1),size(mm,1));
        for j=1:length(mm)
            p_train(:,j)=p(:,mm(j));
            t_train(:,j)=t(:,mm(j));
        end

        [mm,nn]=find(index_test==1);
        p_test=zeros(size(p,1),size(mm,1));
        t_test=zeros(size(t,1),size(mm,1));
        for j=1:length(mm)
            p_test(:,j)=p(:,mm(j));
            t_test(:,j)=t(:,mm(j));
        end

        %net = newff(p_train, t_train, [],{},'traingdx');
        nhid = log2(size(p_train,1) + 5);
        net = newff(p_train, t_train, []);
        net.trainParam.epochs = 100;
        net.trainParam.lr = 0.01;
        net.trainParam.goal = 1e-5;
        net.trainParam.lr = 0.05;
        net.trainParam.lr_inc = 1.05;
        net.trainParam.mc = 0.9;
        net.trainParam.showWindow=0;

        net = train(net, p_train, t_train);
        an = sim(net, p_test);

        [m, n] = size(an);
        for ii=1:n
            tmp = max(an(:,ii));
            for jj=1:m
                if an(jj,ii)==tmp
                    an(jj,ii)=1;
                else
                    an(jj,ii)=0;
                end
            end
        end

        cor = 0;
        for ii=1:n
            if isequal(an(:,ii),t_train(:,ii))
                cor = cor + 1;
                %disp(num2str(T_test(i)));
            else
                %disp(num2str(T_train_a(i)));
            end
        end
        corratio = cor / n;
        col_total = col_total + corratio;
        cor_mat(i) = corratio;
    end
    col_cross = col_total / K;
    disp(['crossvalid = ' num2str(col_cross)]);
    Val = col_cross;
end