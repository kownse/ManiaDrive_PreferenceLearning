fT = T_train;
[m,n] = size(fT);
for i=1:m
    if fT(i)==-1;
        fT(i)=2;
    end
end

data.y = fT;
data.X = p;