function result(model,t,p)
    [predict_label,accuracy] = svmpredict(t,p,model);
    
    result_mat = zeros(max(t)+1,2);
    for i=0:max(t)
        total = size(find(predict_label==i),1);
        need = size(find(t==i),1);
        correct = size(find(predict_label==t & t==i),1);
        accuracy = correct / total;
        recall = correct / need;  
        result(i+1,1) = accuracy;
        result(i+1,2) = recall;
        str = sprintf( 'label %d\t\taccuracy=%f(%d/%d)\t\trecall=%f(%d/%d)',i, accuracy, correct, total, recall, correct, need);
        disp(str);
    end
end