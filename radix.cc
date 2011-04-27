/*
Copyright 2011 Erik Gorset. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

      THIS SOFTWARE IS PROVIDED BY Erik Gorset ``AS IS'' AND ANY EXPRESS OR IMPLIED
      WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
      FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Erik Gorset OR
      CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
      CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
      SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
      ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
      NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
      ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

      The views and conclusions contained in the software and documentation are those of the
      authors and should not be interpreted as representing official policies, either expressed
      or implied, of Erik Gorset.
*/

#include <iostream>
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>

void insertion_sort(int *array, int offset, int end) {
    int x, y, temp;
    for (x=offset; x<end; ++x) {
        for (y=x; y>offset && array[y-1]>array[y]; y--) {
            temp = array[y];
            array[y] = array[y-1];
            array[y-1] = temp;
        }
    }
}

int radix_sort(int *array, int offset, int end, int shift) {
    int x, y, value, temp;
    int last[256], pointer[256];
    bzero(last, sizeof(int) * 256);

    for (x=offset; x<end; ++x) {
        ++last[(array[x] >> shift) & 0xFF];
    }

    last[0] += offset;
    pointer[0] = offset;
    for (x=1; x<256; ++x) {
        pointer[x] = last[x-1];
        last[x] += last[x-1];
    }

    for (x=0; x<256; ++x) {
        while (pointer[x] != last[x]) {
            value = array[pointer[x]];
            y = (value >> shift) & 0xFF;
            while (x != y) {
                temp = array[pointer[y]];
                array[pointer[y]++] = value;
                value = temp;
                y = (value >> shift) & 0xFF;
            }
            array[pointer[x]++] = value;
        }
    }

    if (shift > 0) {
        shift -= 8;
        for (x=0; x<256; ++x) {
            temp = x > 0 ? pointer[x] - pointer[x-1] : pointer[0] - offset;
            if (temp > 50) {
                radix_sort(array, pointer[x] - temp, pointer[x], shift);
            } else if (temp > 1) {
                // std::sort(array + (pointer[x] - temp), array + pointer[x]);
                insertion_sort(array, pointer[x] - temp, pointer[x]);
            }
        }
    }
}

int intcmp(const void *aa, const void *bb)
{
    const int *a = (int *)aa, *b = (int *)bb;
    return (*a < *b) ? -1 : (*a > *b);
}

int main(int argc, char **argv) {
    if (sizeof(int) != 4) {
        std::cerr << "Ooops. sizeof(int) != 4\n";
        return 1111;
    }

    int N;
    if (argc != 2 || sscanf(argv[1], "%d", &N) == -1) {
        std::cerr << "n missing\n";
        return 111;
    }
    int *array = (int *)malloc(sizeof(int) * N);

    for (int i=0; i<3; ++i) {
        int n = N;
        srand(1);
        while (n --> 0) {
            array[n] = rand();
        }

        struct timeval start, end;
        long long mtime, seconds, useconds;   
        gettimeofday(&start, NULL);
        switch(i) {
        case 0: std::sort(array, array+N); break;
        case 1: qsort(array, N, sizeof(int), intcmp); break;
        case 2: radix_sort(array, 0, N, 24); break;
        }
        gettimeofday(&end, NULL);

        n = N - 1;
        while (n --> 0) {
            if (array[n] > array[n+1]) {
                std::cerr << "sorting failed\n";
                return -1;
            }
        }

        seconds  = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        mtime = seconds * 1000000LL + useconds;
        switch(i) {
        case 0: std::cout << "std::sort " << mtime << "\n"; break;
        case 1: std::cout << "qsort " << mtime << "\n"; break;
        case 2: std::cout << "radix_sort " << mtime << "\n"; break;
        }
    }

    return 0;
}
