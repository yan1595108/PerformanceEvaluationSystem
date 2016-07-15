clear;clc;
fs=81.92; %采样率
fin1=70.125; 
fin2=70.2;
fin3=70.3;
A1=1;
A2=0;
A3=0;
N=1700000;
%通道数
channelnum=10;
f_carry=zeros(1,channelnum);
f_carry(1)=68;
for i=2:1:channelnum
    f_carry(i)=f_carry(i-1)+0.5;
end 
sig_in=A1*cos(fin1/fs*2*pi*(0:N-1))*8191+A2*cos(fin2/fs*2*pi*(0:N-1))*8191+A3*cos(fin3/fs*2*pi*(0:N-1))*8191;
%输入信号
abs_fft_quan=zeros(1,10000*10);
ycos_down=zeros(1,N/10);
ysin_down=zeros(1,N/10);
ycos_firin=zeros(1,N/20);
ysin_firin=zeros(1,N/20);
ycos_fftin=zeros(1,N/100);
ysin_fftin=zeros(1,N/100);
for i=1:1:channelnum
    cos_carry=fix(8191*cos(f_carry(i)/fs*2*pi*(0:N-1))+pi/10*i);
    sin_carry=fix(8191*sin(f_carry(i)/fs*2*pi*(0:N-1))+pi/10+i);
    %ycos_mix=floor(sig_in.*cos_carry/4096);
    %ysin_mix=floor(sig_in.*sin_carry/4096);  
    ycos_mix=floor(sig_in.*cos_carry/4096);
    ysin_mix=floor(sig_in.*sin_carry/4096);
 
     ycos_down=cic_filter(ycos_mix,10);
     ysin_down=cic_filter(ysin_mix,10);
     %Myfft(ycos_down,16384,3.2);      
    
    b_down_hb=fir1(31,1/4*2);
    
    ycos_hb=filter(b_down_hb,1,ycos_down);
    ysin_hb=filter(b_down_hb,1,ysin_down);
    
    for t=1:1:length(ycos_hb)/2
        ycos_firin(t)=ycos_hb(t*2);
        ysin_firin(t)=ysin_hb(t*2);
    end
    
    
    
    b_down_hb=fir1(50,1/10*2);
   % freqz(b_down_hb)
    
    ycos_firout=filter(b_down_hb,1,ycos_firin);
    ysin_firout=filter(b_down_hb,1,ysin_firin);
    
    for t=1:1:length(ycos_firout)/5
        ycos_fftin(t)=ycos_firout(t*5);
        ysin_fftin(t)=ysin_firout(t*5);
    end
    
    y=ycos_fftin-j*ysin_fftin;
    fft_y=fft(y(617:17000));
    abs_fft=abs(fft_y)/16384;
    power_fft=abs_fft.^2;
    abs_fft_quan((i-1)*10000+1:10000*i-5000)=power_fft(11385:16384);
    abs_fft_quan(10000*i-5000+1:10000*i)=power_fft(1:5000);
    disp(i);
end
figure(2)
plot(67.75+50/1000/1000*(0:1:(length(abs_fft_quan)-1)),10*log10((abs_fft_quan+0.1)/(6.6581*10^7))); 
grid on



