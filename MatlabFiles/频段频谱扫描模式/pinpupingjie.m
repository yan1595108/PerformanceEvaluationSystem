clear;clc;
%close all

%fs=300;        %������
fs=81.92;       %������
fin1=70.125;    %����Ƶ��1-��Ƶ
fin2=70.2;      %����Ƶ��2-��Ƶ
fin3=70.3;      %����Ƶ��3-��Ƶ
A1=1;           %����Ƶ��1-��ֵ
A2=0;           %����Ƶ��2-��ֵ
A3=0;           %����Ƶ��3-��ֵ
N=1700000;      %��������
channelnum=10;  %ͨ����
f_carry=zeros(1,channelnum);
f_carry(1)=68;
for i=2:1:channelnum
    f_carry(i)=f_carry(i-1)+0.5;
end

%�����źţ�sig_in = A1*cos(f1)+A2*cos(f2)+A1*cos(f1)
%���Ҳ�A*cos(2*pi*f*t) ������8192��2^13����Ŀ����ģ��AD�������ݣ�14���أ�
%sig_in=A1*cos(fin1/fs*2*pi*(0:N-1))*8191+A2*cos(fin2/fs*2*pi*(0:N-1))*8191+A3*cos(fin3/fs*2*pi*(0:N-1))*8191;%�����ź�

sig_base = randint(17000,1,[-3,3],123);
sig_base1 = sig_base*ones(1,100);
sig_base2 = reshape(sig_base1.',1,N);
sig_in = A1*cos(fin1/fs*2*pi*(0:N-1))*8191/3.*sig_base2;

figure(1)
plot(-fs/2:fs/N:fs/2-fs/N,fftshift(log10(abs(fft(sig_in)))));
% figure(1)
% plot(-fs/2:fs/N:fs/2-fs/N,fftshift(abs(fft(sig_in))));
% title('�����ź�sig_inƵ��');

abs_fft_quan=zeros(1,10000*10);
ycos_down=zeros(1,N/10);
ysin_down=zeros(1,N/10);
ycos_firin=zeros(1,N/20);
ysin_firin=zeros(1,N/20);
ycos_fftin=zeros(1,N/100);
ysin_fftin=zeros(1,N/100);
for i=1:1:channelnum
    %�ز�
    cos_carry=fix(8191*cos(f_carry(i)/fs*2*pi*(0:N-1))+pi/10*i);
    sin_carry=fix(8191*sin(f_carry(i)/fs*2*pi*(0:N-1))+pi/10*i);
%     figure(2)
%     plot(-fs/2:fs/N:fs/2-fs/N,fftshift(abs(fft(cos_carry))));
%     title('�ز��ź�cos_carryƵ��');
    
    %ycos_mix=floor(sig_in.*cos_carry/4096);
    %ysin_mix=floor(sig_in.*sin_carry/4096);  
    ycos_mix=floor(sig_in.*cos_carry/4096);
    ysin_mix=floor(sig_in.*sin_carry/4096);
%     figure(3)
%     plot(-fs/2:fs/N:fs/2-fs/N,fftshift(abs(fft(ycos_mix))));
%     title('��Ƶ���±�Ƶ���ź�cos_mixƵ��');
 
    ycos_down=cic_filter(ycos_mix,10);
    ysin_down=cic_filter(ysin_mix,10);
    %Myfft(ycos_down,16384,3.2);
%     figure(4)
%     fs_real = fs/10;%CIC�����³飨ÿ10����ȡһ���㣩ʵ�ʲ�������Ϊԭ�����ʵ�1/10
%     plot(-fs_real/2:fs_real/(N/10):fs_real/2-fs_real/(N/10),fftshift(abs(fft(ycos_down))));
%     title('�±�Ƶ�ź�Ƶ�ף�10��');
    
    b_down_hb=fir1(31,1/4*2);
    ycos_hb=filter(b_down_hb,1,ycos_down);
    ysin_hb=filter(b_down_hb,1,ysin_down);
    
    for t=1:1:length(ycos_hb)/2
        ycos_firin(t)=ycos_hb(t*2);
        ysin_firin(t)=ysin_hb(t*2);
    end
    
%     figure(5)
%     fs_real = fs/20;
%     plot(-fs_real/2:fs_real/(N/20):fs_real/2-fs_real/(N/20),fftshift(abs(fft(ycos_firin))));
%     title('�±�Ƶ�ź�Ƶ�ף�20��');
    
    b_down_hb=fir1(50,1/10*2);
   % freqz(b_down_hb)
    
    ycos_firout=filter(b_down_hb,1,ycos_firin);
    ysin_firout=filter(b_down_hb,1,ysin_firin);
    
    for t=1:1:length(ycos_firout)/5
        ycos_fftin(t)=ycos_firout(t*5);
        ysin_fftin(t)=ysin_firout(t*5);
    end
    
%     figure(i)
%     fs_real = fs/100;
%     plot(-fs_real/2:fs_real/(N/100):fs_real/2-fs_real/(N/100),fftshift(abs(fft(ycos_fftin))));
    
    %��
    y=ycos_fftin-j*ysin_fftin;
    fft_y=fft(y(617:17000));%ȡ��16K��
    abs_fft=abs(fft_y)/16384;
    power_fft=abs_fft.^2;
    
    figure(i)
    fs_real = fs/100;
    n_power_fft = 16*1024;
    plot(-fs_real/2:fs_real/n_power_fft:fs_real/2-fs_real/n_power_fft,fftshift(power_fft));
    
    %ȡǰ��5000�� ����5000��
    abs_fft_quan((i-1)*10000+1:10000*i-5000)=power_fft(11385:16384);
    abs_fft_quan(10000*i-5000+1:10000*i)=power_fft(1:5000);
    disp(i);
end
figure(11)
plot(67.75+50/1000/1000*(0:1:(length(abs_fft_quan)-1)),10*log10((abs_fft_quan+0.1)/(6.6581*10^7))); 
grid on



