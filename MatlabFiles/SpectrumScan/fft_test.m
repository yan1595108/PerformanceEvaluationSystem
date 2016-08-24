clear
clc

nfftpoints = 32*1024;    %fft点数
ntimesfft = 1000000;    %fft进行的次数

% d_real = rand(1,nfftpoints);
% d_imag = rand(1,nfftpoints);
% d = d_real + j*d_imag;
% 
% file_path = 'E:\MainProject\5-4\多通道转发性能评估系统\频谱分析\FFTCode\FFT\Data\';
% file_dir_real = sprintf('%sData_%dK_real.txt',file_path,nfftpoints/1024);
% file_dir_imag = sprintf('%sData_%dK_imag.txt',file_path,nfftpoints/1024);
% fid_real = fopen(file_dir_real,'w');
% fid_imag = fopen(file_dir_imag,'w');
% format long e;
% for m = 1:nfftpoints
%     fprintf(fid_real,'%.17f\r\n',d_real(m));
%     fprintf(fid_imag,'%.17f\r\n',d_imag(m));
% end
% fclose(fid_real);
% fclose(fid_imag);
%return;



file_path = 'E:\MainProject\5-4\多通道转发性能评估系统\频谱分析\FFTCode\FFT\Data\';
file_dir_real = sprintf('%sData_%dK_real.txt',file_path,nfftpoints/1024);
file_dir_imag = sprintf('%sData_%dK_imag.txt',file_path,nfftpoints/1024);
fid_real = fopen(file_dir_real,'r');
fid_imag = fopen(file_dir_imag,'r');
d_real = fscanf(fid_real,'%f');
d_imag = fscanf(fid_imag,'%f');
fclose(fid_real);
fclose(fid_imag);


d = d_real + j*d_imag;

fft_d = fft(d);
fft_dtest = fft(fft_d);
test = abs(fft_d);
test1 = fftshift(test);



timeused = zeros(1,11);%最后一个值表示平均耗时

for j = 1:10
    t1 = clock;
    for i=1:ntimesfft
        d = fft(d);
    end
    t2 = clock;
    timeused(j) = etime(t2,t1);
end
timeused = timeused';
timeused(11) = mean(timeused(1:10));
msgbox('计算完成','提示');
% sig_base1 = sig_base*ones(1,100);
% sig_base2 = reshape(sig_base1.',1,N);