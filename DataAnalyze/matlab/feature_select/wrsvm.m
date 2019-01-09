function Val=wrsvm(t,p,model)
    w = model.SVs' * model.sv_coef;
    
    all_t = unique(t);
    p_mean = zeros(length(all_t),size(p,2));
    for j=1:length(all_t);
        tj = find(t==all_t(j));
        ppj = length(tj) / size(p,1);
        pj = zeros(size(tj,1), size(p,2));
        for i=1:length(tj);
            pj(i,:)=p(tj(i),:);
        end
        p_mean(j,:) = mean(pj,1);
    end
    
    m_diff = p_mean(1,:) - p_mean(2,:);
    Val = w .* m_diff';
end