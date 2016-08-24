clear
clc

N = 32768;
X1n_real = sin(8*atan(1)*1000*(0:1:N-1)/N);
X1n_imag = zeros(1,N);

X1n = X1n_real + j*X1n_imag;
fft_X1n = fft(X1n)';

abs_fft_X1n = abs(fft_X1n);
final_fft_X1n = fftshift(abs_fft_X1n);
max(final_fft_X1n)

