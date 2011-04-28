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

import java.util.Arrays;
import java.util.Random;

public class Radix {
    public static void sort(int[] array, int offset, int end, int shift) {
        int[] last = new int[256];
        int[] pointer = new int[256];

        for (int x=offset; x<end; ++x) {
            ++last[(array[x] >> shift) & 0xFF];
        }

        last[0] += offset;
        pointer[0] = offset;
        for (int x=1; x<256; ++x) {
            pointer[x] = last[x-1];
            last[x] += last[x-1];
        }

        for (int x=0; x<256; ++x) {
            while (pointer[x] != last[x]) {
                int value = array[pointer[x]];
                int y = (value >> shift) & 0xff;
                while (x != y) {
                    int temp = array[pointer[y]];
                    array[pointer[y]++] = value;
                    value = temp;
                    y = (value >> shift) & 0xff;
                }
                array[pointer[x]++] = value;
            }
        }
        if (shift > 0) {
            shift -= 8;
            for (int x=0; x<256; ++x) {
                int size = x > 0 ? pointer[x] - pointer[x-1] : pointer[0] - offset;
                if (size > 64) {
                    sort(array, pointer[x] - size, pointer[x], shift);
                } else if (size > 1) {
                    insertionSort(array, pointer[x] - size, pointer[x]);
                    // Arrays.sort(array, pointer[x] - size, pointer[x]);
                }
            }
        }
    }

    private static void insertionSort(int array[], int offset, int end) {
        for (int x=offset; x<end; ++x) {
            for (int y=x; y>offset && array[y-1]>array[y]; y--) {
                int temp = array[y];
                array[y] = array[y-1];
                array[y-1] = temp;
            }
        }
    }

    public static void benchmark(int algorithm, int x) {
        int y = (int) (Math.ceil(100000000d / x) + 0.5001d);
        int[] array = new int[x*y];

        Random r = new Random(1);
        for (int i=0; i<array.length; ++i) {
            array[i] = r.nextInt() & Integer.MAX_VALUE;
        }

        long a = System.nanoTime();
        for (int i=0; i<y; ++i) {
            if (algorithm == 0) sort(array, i * x, (i+1) * x, 24); else Arrays.sort(array, i * x, (i+1) * x);
        }
        long b = System.nanoTime();

        for (int i=0; i<y; ++i) {
            int n = (i+1) * x - 1;
            while (n --> i * x) {
                if (array[n] > array[n+1]) {
                    throw new RuntimeException("oops");
                }
            }
        }

        long micro = ((b - a) / y) / 1000L;

        System.out.println((algorithm == 0 ? "Radix.sort " : "Arrays.sort ") + micro);
    }

    public static void main(String[] args) {
        benchmark(Integer.parseInt(args[0]), Integer.parseInt(args[1]));
    }
}
