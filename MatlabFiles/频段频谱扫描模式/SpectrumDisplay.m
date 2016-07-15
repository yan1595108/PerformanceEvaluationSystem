clc
clear

fid = fopen('E:\MainProject\5-4\C����\Ƶ����ʾ\��ͼ�ؼ�����\TEST\Orign_dat_Sam.txt','rt');
input = fscanf(fid,'%f');
fclose(fid);

size(input)

nSamplePoints = length(input)-1;
fs = 32*1024*1024;
%x_freq = 0:fs/nSamplePoints:fs-fs/nSamplePoints;
x_freq = -fs/2:fs/nSamplePoints:fs/2-fs/nSamplePoints;
%ampl = 20*log10(abs(input(1:nSamplePoints)));
freqs = fft(input(1:nSamplePoints),nSamplePoints);
freqs = abs(freqs);
y_ampl = 20*log10(freqs/max(freqs));
%y_ampl = freqs/max(freqs);
%plot(x_freq,y_ampl);
plot(x_freq(1:nSamplePoints/2),y_ampl(1:nSamplePoints/2));

%%����-���Ƶ�����ݵ��ļ�
%FileOut = fopen('E:\MainProject\5-4\C����\Ƶ����ʾ\��ͼ�ؼ�����\TEST\Spectrum.txt','wt');
%for i=nSamplePoints/2:nSamplePoints;
%fprintf(FileOut,'%f\n',y_ampl(i));
%end
%fclose(FileOut);