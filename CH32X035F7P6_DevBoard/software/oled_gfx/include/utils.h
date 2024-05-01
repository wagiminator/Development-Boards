// ===================================================================================
// Some Useful Functions                                                      * v0.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// abs(n)                   returns positive value if n
// min(x,y)                 returns minimum of x and y
// max(x,y)                 returns maximum of x and y
// swap(x,y)                swaps values of x and y
//
// random(max)              returns random value between 0 and max-1
// random_seed(val)         sets start value of random function
//
// map(x, imin, imax, omin, omax)   mapping function
//
// ... to be continued
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#define abs(n)        (((n)>=0)?(n):(-(n)))
#define min(x,y)      (((x)>(y))?(y):(x))
#define max(x,y)      (((x)>(y))?(x):(y))

#define swap(x,y) do {   \
  typeof(x) _t = x;      \
  x = y;                 \
  y = _t;                \
} while(0)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

static uint32_t rnval = 0xACE1DFEE;

static void random_seed(uint32_t val) {
  rnval = val;
}

static uint32_t random(uint32_t max) {
  rnval = rnval << 16 | (rnval << 1 ^ rnval << 2) >> 16;
  return(rnval % max);
}

static int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
  return(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#pragma GCC diagnostic pop
