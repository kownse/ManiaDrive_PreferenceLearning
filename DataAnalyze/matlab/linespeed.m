speedmat = cell2mat(speed);
[m,n] = size(speedmat);
linespd = zeros(1,m);
for i=1:m
    linespd(i) = sqrt(speedmat(i,1)*speedmat(i,1)+speedmat(i,2)*speedmat(i,2)+speedmat(i,3)*speedmat(i,3));
end

addspd = zeros(1,m);
for i=1:m
    addspd(i) = sqrt(speedmat(i,4)*speedmat(i,4)+speedmat(i,5)*speedmat(i,5)+speedmat(i,6)*speedmat(i,6));
end

subplot(2,1,1);
plot(linespd);
hold on;
plot(addspd, 'r');
xlabel('时间/s');ylabel('速度/加速度');
title('速度/加速度曲线');

NFFT = 2^nextpow2(m);
y1 = fft(linespd,NFFT)/m;
y2 = fft(addspd, NFFT)/m;
f = 20/2*linspace(0,1,NFFT/2+1);

subplot(2,1,2);
plot(f,2*abs(y1(1:NFFT/2+1)));
hold on;
plot(f,2*abs(y2(1:NFFT/2+1)), 'r');
xlim([0, 0.5]);
ylim([0, 2.0]);

xlabel('频率');ylabel('振幅');
title('FFT结果');

%
% N = m;
% Fs = 20;
% num = 0:N-1;
% t = num/Fs;
% Lag = 100;
% [c, lags] = xcorr(linespd, Lag, 'unbiased');

% subplot(4,1,3);
% plot(lags/Fs,c);
% xlabel('时间/s');ylabel('Rx(t)');
% title('自相关函数');

% Nfft = 256;
% %[P,f] = pmusic(linespd, [Inf, 1.1], Nfft, Fs, 32, 16);
% [P,f] = pmusic(linespd, [Inf, 1.1], [], 8000,7);
% %[P,f] = pmem(linespd, 14, Nfft, Fs);
% subplot(4,1,4);
% plot(P);
%