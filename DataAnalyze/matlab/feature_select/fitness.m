function [sol,Val]=fitness(sol,options)
global S P_train
[s1,s]=size(P_train);
for i=1:s
    x(i)=sol(i);
end
Val=de_code(x);
%Val=de_code_svm_reg(x);
%Val=de_code_mlp(x);
end