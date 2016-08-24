N = 40;
n = 0:1:N-1;
x(n+1) = cos(2*pi*n/N);
fftx = fft(x) / N;
sx = fftshift(fftx);
stem(-N/2:1:N/2-1, sx);