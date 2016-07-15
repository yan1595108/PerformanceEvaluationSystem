/********************************************************************/
/*  Copyright 2004 by 5idsp Lab.							*/
/*  All rights reserved. Property of 5idsp Lab.				*/
/*  Restricted rights to use, duplicate or disclose this code are	*/
/*  granted through contract.									    */
/*  															    */
/********************************************************************/
#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#include "FFT_DSP.h"

void DSPF_sp_cfftr2_dit(double* x, double* w, int n)
{
	int n2, ie, ia, i, j, k, m;
	double rtemp, itemp, c, s;
	n2 = n;
	ie = 1;
	for(k=n; k > 1; k >>= 1)
	{
		n2 >>= 1;
		ia = 0;
		for(j=0; j < ie; j++)
		{
			c = w[2*j];
			s = w[2*j+1];
			for(i=0; i < n2; i++)
			{
				m = ia + n2;
				rtemp = c * x[2*m] + s * x[2*m+1];
				itemp = c * x[2*m+1] - s * x[2*m];
				x[2*m] = x[2*ia] - rtemp;
				x[2*m+1] = x[2*ia+1] - itemp;
				x[2*ia] = x[2*ia] + rtemp;
				x[2*ia+1] = x[2*ia+1] + itemp;
				ia++;
			}
			ia += n2;
		}
		ie <<= 1;
	}
} 




void gen_w_r2(double* w, int n)
{
	int i;
	//double pi = 4.0*atan(1.0);
	double e = pi*2.0/n;
	for(i=0; i < ( n>>1 ); i++)
	{
		w[2*i] = cos(i*e);
		w[2*i+1] = sin(i*e);
	}
}


void bit_rev(double* x, int n)
{
	int i, j, k;
	double rtemp, itemp;
	j = 0;
	for(i=1; i < (n-1); i++)
	{
		k = n >> 1;
		while(k <= j)
		{
			j -= k;
			k >>= 1;
		}
		j += k;
		if(i < j)
		{
			rtemp = x[j*2];
			x[j*2] = x[i*2];
			x[i*2] = rtemp;
			itemp = x[j*2+1];
			x[j*2+1] = x[i*2+1];
			x[i*2+1] = itemp;
		}

	}

}

void complex2real(double*src,double*dst,int N)
{
	int i;
	for(i=0;i<N;i++)
	{
		dst[i]=sqrt(pow(src[2*i],2)+pow(src[2*i+1],2));
	}
}

void fftshift(double *p,int N)
{
	int i;
	double tmp;
	for(i=0;i<N/2;i++)
	{
		tmp=p[i];
		p[i]=p[i+N/2];
		p[i+N/2]=tmp;
	}
}