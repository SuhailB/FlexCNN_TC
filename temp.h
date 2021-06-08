#include "util.h"
<<<<<<< HEAD
#define LOAD_PROGRESS 1
#define SAVE_PROGRESS 0
#define PRJ_PATH "D:/Spring2021/recut/recut/fpga"
#define STRIDE 1
#define FILTER_S2 3
#define CIN_OFFSET 0
#define LAYER 21
#define OUTFILE "/data/L21_outputs.dat"
#define OUT_OFFSET1 262144
#define OUT_OFFSET2 262144
#define CHANGE_LAYOUT 1
#define IN_NUM_HW 8
#define OUT_NUM_HW 8
#define IN_H_HW 17
#define IN_W_HW 17
#define OUT_H_HW 32
#define OUT_W_HW 32
#define IN_NUM 8
#define OUT_NUM 8
#define IN_H 16
#define IN_W 16
#define OUT_H 32
#define OUT_W 32
#define IN_NUM_T 8
#define OUT_NUM_T 8
=======
#define LOAD_PROGRESS 0
#define SAVE_PROGRESS 1
#define PRJ_PATH "/curr/eddie/recut/fpga"
#define FILTER_S2 1
#define CIN_OFFSET 0
#define LAYER 1
#define OUTFILE "/data/L1_outputs.dat"
#define OUT_OFFSET1 524288
#define OUT_OFFSET2 524288
#define CHANGE_LAYOUT 1
#define IN_NUM_HW 8
#define OUT_NUM_HW 32
#define IN_H_HW 256
#define IN_W_HW 256
#define OUT_H_HW 256
#define OUT_W_HW 256
#define IN_NUM 3
#define OUT_NUM 32
#define IN_H 256
#define IN_W 256
#define OUT_H 256
#define OUT_W 256
#define IN_NUM_T 8
#define OUT_NUM_T 32
>>>>>>> 9fab470a8b6cb4f6d8747560de27b39f92cbeb91
#define IN_H_T 8
#define IN_W_T 8
void instInit(uint* config);
void preprocess(
  data_t0* cin_hw,
  data_t1* weight_hw,
  data_t2* bias_hw,
  data_t0  outputs_sw[OUT_NUM][OUT_H][OUT_W]
);
void postprocess(
  data_t0* cin_hw,
  data_t0  outputs_hw[OUT_NUM][OUT_H][OUT_W],
  data_t0  outputs_py[OUT_NUM][OUT_H][OUT_W]
);
void compareResults(data_t0  outputs_hw[OUT_NUM][OUT_H][OUT_W], data_t0  outputs_sw[OUT_NUM][OUT_H][OUT_W]);
void save_progress(data_t0* cin_hw, uint data_offset);
void load_progress(data_t0* cin_hw);