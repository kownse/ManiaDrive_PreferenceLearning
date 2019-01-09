function model=klt(t,p, method)   
    nf = size(p,2);
    Sb = zeros(nf,nf);
    Sw = zeros(nf,nf);
    all_t = unique(t);
    nt = length(all_t);
    all_mean = mean(p,1);
    p_mean = zeros(nt, nf);
    p_var = zeros(nt, nf);
    p_pro = zeros(nt,1);
    for j=1:length(all_t);
        tj = find(t==all_t(j));
        ppj = length(tj) / size(p,1);
        p_pro(j) = ppj;
        pj = zeros(size(tj,1), size(p,2));
        for i=1:length(tj);
            pj(i,:)=p(tj(i),:);
        end
        
        pj_mean = mean(pj,1);
        p_mean(j,:) = pj_mean;
        p_var(j,:) = var(pj,1);
        
        dis_x_all = zeros(nf,nf);
        for i=1:length(tj);
            disx = pj(i,:) - pj_mean;
            tmp = disx' * disx;
            dis_x_all = dis_x_all + tmp;
        end
        dis_x_all = dis_x_all / length(tj);
        Sw = Sw + ppj * dis_x_all;
        
        dis = (pj_mean - all_mean);
        Sb = Sb + ppj * dis' * dis;
    end
    
    e = zeros(nf, 1);
    V = zeros(nf,nf);
    D = zeros(nf,nf);
    J = zeros(nf, 1);
    if strcmp(method,'sw')==1;
        [V,D] = eigs(Sw, nf);
        e = diag(D);
    elseif strcmp(method,'sb')==1;
        [V,D] = eigs(Sb, nf);
        e = diag(D);
        J = e;
    elseif strcmp(method, 'sb/sw')==1;
        if(det(Sw)==0) 
            disp('this critia does not work,sw is singular') 
            return; 
        end 
        [V,D]=eigs(Sb/Sw, nf);
        e = diag(D);
        J = e;
    elseif strcmp(method, 'mvzip')==1;
       [V,D]=eigs(Sw, nf);
       %[V,D]=eig(Sw);
       e = diag(D);
       baihua=zeros(nf,nf); 
       for feas=1:nf 
          baihua(:,feas)=V(:,feas)/(e(feas)^(1/2)); 
       end 
       sbb=baihua'*Sb*baihua; 
       [W,D]=eigs(sbb, nf);
       %[W,D]=eig(sbb);
       e = diag(D);
       V=baihua*W;  
       J = e;
    elseif strcmp(method, 'mean')==1;
        [V,D]=eigs(Sw, nf);
        e = diag(D);
        for feas=1:nf 
          J(feas)=V(:,feas)'*Sb*V(:,feas); 
          J(feas)=J(feas)/D(feas,feas); 
        end 
    elseif strcmp(method, 'var')==1;
        [V,D]=eigs(Sw, nf);
        e = diag(D);
        r = zeros(nt, nf);
        
        for j=1:nf;
            J(j) = 1;
        end
        for i=1:nt;
            for j=1:nf;
                r(i,j) = p_pro(i) * p_var(i,j) / e(j);
                %J(j) = J(j) + r(i,j) * log(r(i,j));
                J(j) = J(j) * r(i,j);
            end
        end
        
        %{
        for j=1:nf;
            J(j) = -J(j);
        end
        %}
    end
    
    [Jd, index] = sort(J,'descend');
    
    Vec = zeros(size(V,1), size(V,2));
    for i=1:length(index)
        Vec(:,i) = V(:,index(i));
    end
    
    model.eig = e;
    model.J = Jd;
    model.W = Vec;
    model.out = p * model.W;
end