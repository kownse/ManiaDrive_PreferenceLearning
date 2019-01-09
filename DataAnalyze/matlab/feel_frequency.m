feelmat = cell2mat(feel);

%{变量定义}
feeladd = zeros(5,5,6);
feelnum = zeros(5,5,6);
feelavg = zeros(5,5,6);

[m,n] = size(feelmat);
for i=1:m
    feeladd(feelmat(i,1)+1,feelmat(i,2)+1,feelmat(i,3)+1) = feeladd(feelmat(i,1)+1,feelmat(i,2)+1,feelmat(i,3)+1) + feelmat(i,4);
    feelnum(feelmat(i,1)+1,feelmat(i,2)+1,feelmat(i,3)+1) = feelnum(feelmat(i,1)+1,feelmat(i,2)+1,feelmat(i,3)+1) + 1;
end

[L,M,N]=size(feelavg);
for i=1:L
    for j=1:M
        for k=1:N
            feelavg(i,j,k) = feeladd(i,j,k) / feelnum(i,j,k);
        end
    end
end

anxiety = 1;
satisfy = 2;
boring = 3;
excit = 4;
frustration = 5;
chanllenge = 6;

x = 1:1:5;
y = 1:1:5;
[X,Y] = meshgrid(x,y);

subplot(3,2,1);
%{feelavg(:,:,chanllenge)}
feelnum(:,:,chanllenge);
surface(X,Y,feelnum(:,:,chanllenge));
xlabel('技能');ylabel('难度');title('挑战感受分布');

subplot(3,2,2);
%{feelavg(:,:,chanllenge)}
feelnum(:,:,chanllenge);
surface(X,Y,feelnum(:,:,anxiety));
xlabel('技能');ylabel('难度');title('焦虑感受分布');

subplot(3,2,3);
%{feelavg(:,:,chanllenge)}
feelnum(:,:,chanllenge);
surface(X,Y,feelnum(:,:,satisfy));
xlabel('技能');ylabel('难度');title('满足感受分布');

subplot(3,2,4);
%{feelavg(:,:,chanllenge)}
feelnum(:,:,chanllenge);
surface(X,Y,feelnum(:,:,boring));
xlabel('技能');ylabel('难度');title('无聊感受分布');

subplot(3,2,5);
%{feelavg(:,:,chanllenge)}
feelnum(:,:,chanllenge);
surface(X,Y,feelnum(:,:,excit));
xlabel('技能');ylabel('难度');title('激动感受分布');

subplot(3,2,6);
%{feelavg(:,:,chanllenge)}
feelnum(:,:,chanllenge);
surface(X,Y,feelnum(:,:,frustration));
xlabel('技能');ylabel('难度');title('挫败感受分布');