
#include <stdio.h>

int main(void)
{
    int n = 64; // n = 64
    int n_samples;
    int numins = 1;
    int numouts = 1;

    for (int i = 0; i < n; i += n) {
    	printf("i: %d\n", i);

        n_samples = n - i;
        printf("n_samples: %d\n", n_samples);

        for (int s_in = i; s_in < n; s_in++) {
        	// printf("s_in: %d\n", s_in);
            for (int chan = 0; chan < numins; chan++) {
                printf("chan: %d s_in: %d\n", chan, s_in);
            }
        }

        for (int s_out = 0; s_out < n_samples; s_out++) {
            for (int chan = 0; chan < numouts; chan++) {
                printf("chan: %d s_out: %d\n", chan, s_out);
            }
        }
    }
}