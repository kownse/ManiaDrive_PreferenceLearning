funmat = cell2mat(fun);

%{��������}
funmap = zeros(5, 5);
funnum = zeros(5,5);
funavg = zeros(5,5);

%{�����������Ƶ��}
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
xlabel('����');
ylabel('��ս');
title('�ۻ����ֶ�');

subplot(2,1,2);
surface(X,Y,funavg);
xlabel('����');
ylabel('��ս');
title('ƽ�����ֶ�');