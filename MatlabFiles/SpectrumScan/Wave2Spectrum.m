clear
clear
clc
close all

A1 = 1;         %幅值
freq = 70*10E6; %中频
fs = 160*10E6;  %采样频率
N = 16000;        %采样点数
sig_in=A1*cos(freq/fs*2*pi*(0:N-1));%输入信号
% figure(1)
% plot(0:fs/N:fs-fs/N,abs(fft(sig_in)));
% figure(2)
% plot(-fs/2:fs/N:fs/2-fs/N,fftshift(abs(fft(sig_in))));

%freq = 70*10E6;
%fs = 180*10E6;
freq = 512;
fs = 1024;
N = 32*1024;
A = 1;
signal = A*cos(2*pi*freq/fs*(0:1:N-1))';
signal1 = signal*ones(1,64);
signal2 = reshape(signal1.',1,fs/16*N);
fs1 = fs*64;

figure(1)
plot(signal2);
ylim([-1.2 1.2]);

figure(2)
plot(-fs/2:fs/N:fs/2-fs/N, fftshift(abs(fft(signal'))));

figure(3)
spectrum = abs(fft(signal2));
df = fs1/(64*N);
plot(0:df:fs1-df,spectrum);
%xlim([-fs/2 fs/2-fs/N]);

figure(4)
spectrum1 = abs(fft(signal));
df = fs/N;
plot(0:df:fs-df,spectrum1);