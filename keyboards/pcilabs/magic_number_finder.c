#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

const int ftbl[33] = {0,    1,     1,     2,     2,     4,    5,    8,    11,
                      16,   22,    32,    45,    64,    90,   128,  181,  256,
                      362,  512,   724,   1024,  1448,  2048, 2896, 4096, 5792,
                      8192, 11585, 16384, 23170, 32768, 46340};
const int ftbl2[32] = {32768, 33276, 33776, 34269, 34755, 35235, 35708, 36174,
                       36635, 37090, 37540, 37984, 38423, 38858, 39287, 39712,
                       40132, 40548, 40960, 41367, 41771, 42170, 42566, 42959,
                       43347, 43733, 44115, 44493, 44869, 45241, 45611, 45977};

#define HALL_SENSOR_DISTANCE 160
#define UP_POS 1870
#define DOWN_POS 1110

const int16_t mc = 92, kc = -490, qc = -39;
int16_t m = mc, k = kc, q = qc;

FILE* fptr;

int fisqrt(int val) {
  int cnt = 0;
  int t = val;
  while (t) {
    cnt++;
    t >>= 1;
  }
  if (6 >= cnt)
    t = (val << (6 - cnt));
  else
    t = (val >> (cnt - 6));

  return (ftbl[cnt] * ftbl2[t & 31]) >> 15;
}

float inv_sqrt(float x) {
  union {
    float f;
    uint32_t i;
  } y = {x};
  y.i = 0x5F1FFFF9ul - (y.i >> 1);
  return 0.703952253f * y.f * (2.38924456f - x * y.f * y.f);
}

uint32_t formula_example(uint16_t reading) {
  double tmp = HALL_SENSOR_DISTANCE * (sqrt((UP_POS + mc) - (DOWN_POS + kc)) /
                                       sqrt((UP_POS + mc) - reading)) -
               HALL_SENSOR_DISTANCE + qc;
  uint32_t res = (uint32_t)tmp;
  return res;
}

uint32_t map_hall(uint16_t reading) {
  double tmp =
      HALL_SENSOR_DISTANCE * (fisqrt((UP_POS + m) - (DOWN_POS + k)) *
                              inv_sqrt((float)((UP_POS + m) - reading))) -
      HALL_SENSOR_DISTANCE + q;
  uint32_t res = (uint32_t)tmp;
  return res;
}
/*
return (HALL_SENSOR_DISTANCE * fast_sqrt(positions_difference[key_index]) *
        inv_sqrt(user_config_ptr->rest_positions[key_index] + REST_POS_MAGIC -
                 read)) -
       HALL_SENSOR_DISTANCE + MAP_MAGIC;
*/

int main() {
  time_t rawtime = time(NULL);
  fptr = fopen("output.txt", "w");
  fprintf(fptr, "%s", asctime(localtime(&rawtime)));
  fprintf(fptr, "value\texpected\tmapped\n");
  for (int i = UP_POS; i >= DOWN_POS; i--) {
    fprintf(fptr, "%d\t%u\t%u\n", i, formula_example(i), map_hall(i));
  }
}
