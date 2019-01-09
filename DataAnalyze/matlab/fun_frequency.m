funmat = cell2mat(fun);

%{变量定义}
funmap = zeros(5, 5);
funnum = zeros(5,5);
funavg = zeros(5,5);

%{计算绝对数和频数}
[m,n] = size(funmat);
for i = 1:m
    funmap(funmat(i,1)+1,funmat(i,2)+1) = funmap(funmat(i,1)+1,funmat(i,2)+1) + funmat(i,3);
    funnum(funmat(i,1)+1,funmat(i,2)+1) = funnum(funmat(i,1)+1,funmat(i,2)+1) + 1;
end

[M,N] = size(funavg);
for i=1:M
    for j=1:N
        funavg(i,j) = funmap(i,j) / funnum(i,j);
    end
end

x = 1:1:5;
y = 1:1:5;
[X,Y] = meshgrid(x,y);

subplot(2,1,1);
surface(X,Y,funmap);
xlabel('技能');
ylabel('挑战');
title('累积娱乐度');

subplot(2,1,2);
surface(X,Y,funavg);
xlabel('技能');
ylabel('挑战');
title('平均娱乐度');