function Val=Jd(t,p)    
    nf = size(p,2);
    Sb = zeros(nf,nf);
    Sw = zeros(nf,nf);
    all_mean = mean(p,1);
    all_t = unique(t);
    p_mean = zeros(length(all_t), size(p,2));
    for j=1:length(all_t);
        tj = find(t==all_t(j));
        ppj = length(tj) / size(p,1);
        pj = zeros(size(tj,1), size(p,2));
        for i=1:length(tj);
            pj(i,:)=p(tj(i),:);
        end
        
        pj_mean = mean(pj,1);
        p_mean(j,:) = pj_mean;
        
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
    Val = trace(Sb)/trace(Sw);
end