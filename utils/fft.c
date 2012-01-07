/* fft.c
 *
 * Floating test
 */
#include <stdio.h>
#include <string.h>
#include <math.h>

void fft(double[], double[], const unsigned short, const unsigned char);

int main(void)
{
        int i;
        double x[16386], y[16386];

        for(i = 0; i <= 16385; i++){
            x[i] = (double)i;
            y[i] = 0;
        }
        printf("begin to calculate\n");
        fft(x, y, 16384, 14);
        printf("mission accomplished\n");
        return 0;
}

void fft(double x[], double y[], const unsigned short i_dianshu, const unsigned char i_dianshu_mi)
{
	int i, j, k, l, m, l1;
	double t1, t2, u1, u2, w1, w2, p2, z;

	j = 1; 
	memset(y, 0, (i_dianshu + 1) * sizeof(double));
	
	for (l = 1; l <= (i_dianshu - 1); l++) {
		if (l < j) {
			t1 = x[j];
			t2 = y[j];
			x[j] = x[l];
			y[j] = y[l];
			x[l] = t1;
			y[l] = t2;
		}

		k = (i_dianshu) >> 1;
		while (k < j) {
			j -= k;
			k = k >> 1;
		}
		j = j + k;
	}
	m = 1;

	for (i = 1; i <= i_dianshu_mi; i++) {
		u1 = 1;
		u2 = 0;
		k = m;
		m = m << 1;
		p2 =3.1415926 / k;
		w1 = cos(p2);
		w2 = -sin(p2);
		w2 = -w2;

		for (j = 1; j <= k; j++) {
			for (l = j; l <= i_dianshu; l += m) {
		        	l1 = l + k;
		        	t1 = x[l1] * u1 - y[l1] * u2;
		        	t2 = x[l1] * u2 + y[l1] * u1;
				x[l1] = x[l] - t1;
				y[l1] = y[l] - t2;
				x[l] += t1;
				y[l] += t2;
			}
			z = u1 * w1 - u2 * w2;
			u2 = u1 * w2 + u2 * w1;
			u1 = z;
		} 
	}
	for (i = 0; i <= i_dianshu_mi - 1; i++) {
		x[i] = (sqrt(x[i+1] * y[i+1])) / 4096;
		y[i] = atan(y[i+1] / x[i+1]);
	}
}

