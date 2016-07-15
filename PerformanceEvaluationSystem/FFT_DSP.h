#ifndef __FFT_H__
#define __FFT_H__

#include <stdio.h>
//#include <stdlib.h>
#include <math.h>

#define pi 3.1415926535897932384626433832795028841971

void DSPF_sp_cfftr2_dit(double* x, double* w, int n);
void gen_w_r2(double* w, int n);
void bit_rev(double* x, int n);
void complex2real(double*src,double*dst,int N);
void fftshift(double *p,int N);

#endif