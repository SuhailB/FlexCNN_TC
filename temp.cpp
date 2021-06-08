void cin_load(
		bus_t0                         *global_cin,
		uint                           config[CONFIG_PARAMS],
		hls::stream<CinLoadData0Type>  &fifo_cin,
		hls::stream<ConfigInst>        &fifo_config_out
){
#pragma HLS INLINE off 
	// on-chip buffer for cin data
	static bus_t0 cin_burst_buf_ping[IN_NUM_T * (IN_H_T + K_T - 1) * (IN_W_T + K_T - 1) / BUS_PACK_FACTOR0];
	static bus_t0 cin_burst_buf_pong[IN_NUM_T * (IN_H_T + K_T - 1) * (IN_W_T + K_T - 1) / BUS_PACK_FACTOR0];
#pragma HLS RESOURCE variable=cin_burst_buf_ping core=XPM_MEMORY uram
#pragma HLS RESOURCE variable=cin_burst_buf_pong core=XPM_MEMORY uram  



	// layer batch
	ap_uint<32> LAYER_BATCH = config[25];

	// tiling iterators
	uint in_num_iter = 0;
	uint out_num_iter = 0;
	uint in_h_iter = 0;
	uint in_w_iter = 0;
	uint layer_iter = 0;

	uint in_num_iter_prev = 0;
	uint out_num_iter_prev = 0;
	uint in_h_iter_prev = 0;
	uint in_w_iter_prev = 0;
	uint layer_iter_prev = 0;

	ConfigInst inst0 = fifo_config_in.read();
	fifo_config_out.write(inst0);
	ConfigInst inst1 = fifo_config_in.read();
	fifo_config_out.write(inst1);
	ConfigInst inst2 = fifo_config_in.read();
	fifo_config_out.write(inst2);
	ConfigInst inst3 = fifo_config_in.read();
	fifo_config_out.write(inst3);
	ConfigInst inst4 = fifo_config_in.read();
	fifo_config_out.write(inst4);
  ConfigInst inst5 = fifo_config_in.read();
	fifo_config_out.write(inst5);
  
	// parameters
	// inst0
	ap_uint<32> LAYER_IN_NUM_HW;
	ap_uint<32> LAYER_OUT_NUM_HW;
	ap_uint<32> LAYER_IN_H_HW;
	ap_uint<32> LAYER_IN_W_HW;
	ap_uint<32> LAYER_OUT_H_HW;
	ap_uint<32> LAYER_OUT_W_HW;
	// inst1
	ap_uint<32> LAYER_IN_NUM;
	ap_uint<32> LAYER_OUT_NUM;
	ap_uint<32> LAYER_IN_H;
	ap_uint<32> LAYER_IN_W;
	ap_uint<32> LAYER_OUT_H;
	ap_uint<32> LAYER_OUT_W;
	// inst2
	ap_uint<32> CIN_OFFSET;
	ap_uint<32> WEIGHT_OFFSET;
	ap_uint<32> BIAS_OFFSET;
	ap_uint<32> COUT_OFFSET;
	ap_uint<16> FILTER_S1;
	ap_uint<16> FILTER_S2;
	ap_uint<32> STRIDE;
	// inst3
	ap_uint<32> LAYER_EN;
	ap_uint<16> LAYER_IN_NUM_T;
	ap_uint<16> LAYER_OUT_NUM_T;
	ap_uint<32> LAYER_IN_H_T;
	ap_uint<32> LAYER_IN_W_T;
	ap_uint<32> PREV_CIN_OFFSET;
	// inst4
	ap_uint<32> LAYER_TASK_NUM1;
	ap_uint<32> LAYER_TASK_NUM2;
	ap_uint<32> LAYER_LOCAL_ACCUM_NUM;
	ap_uint<32> LAYER_LOCAL_REG_NUM;
	ap_uint<32> LAYER_ROW_IL_FACTOR;
	ap_uint<32> LAYER_COL_IL_FACTOR;
	//inst5
	ap_uint<32> LAYER_CONV_TYPE;
	ap_uint<16> FILTER_D0;
	ap_uint<16> FILTER_D1;
	ap_uint<32> LAYER_DILATION_RATE;
	ap_uint<32> LAYER_TCONV_STRIDE;
	ap_uint<32> K_NUM;
	ap_uint<32> KH_KW;

	ap_uint<4>  FILTER;

	ap_uint<1>  CONV_1ST_EN;
	ap_uint<1>  LOAD_PREV_CIN;

	uint LAYER_IN_NUM_T_prev;
	uint LAYER_OUT_NUM_T_prev;
	uint LAYER_IN_H_T_prev;
	uint LAYER_IN_W_T_prev;
	uint FILTER_prev;
	

	uint task_cnt = 0;
	bool layer_start = 1;
	bool layer_start_prev = 0;
	bool done = 0;
	// We assum that cin has been pre-padded with zeros
	uint prev = 0;
	uint init = 1;
	uint num_tile = 0;
	bool write_last_cin = 0;
	bool write_last_prev_cin = 0;
	bool start_prev = 0;
	bool done_prev = 0;
	bool change_layout = 0;
	uint inter_tile = 0;
	uint channel_iter = 0;
  int count_fifo = 0;
  int count_dram = 0;
  int count = 0;
	while(!done){
		LAYER_IN_NUM_HW  = inst0(32*0+31, 32*0);
    LAYER_OUT_NUM_HW = inst0(32*1+31, 32*1);
    LAYER_IN_H_HW    = inst0(32*2+31, 32*2);
    LAYER_IN_W_HW    = inst0(32*3+31, 32*3);
    LAYER_OUT_H_HW   = inst0(32*4+31, 32*4);
    LAYER_OUT_W_HW   = inst0(32*5+31, 32*5);
    LAYER_IN_NUM     = inst1(32*0+31, 32*0);
    LAYER_OUT_NUM    = inst1(32*1+31, 32*1);
    LAYER_IN_H       = inst1(32*2+31, 32*2);
    LAYER_IN_W       = inst1(32*3+31, 32*3);
    LAYER_OUT_H      = inst1(32*4+31, 32*4);
    LAYER_OUT_W      = inst1(32*5+31, 32*5);
    CIN_OFFSET       = inst2(32*0+31, 32*0);
    WEIGHT_OFFSET    = inst2(32*1+31, 32*1);
    BIAS_OFFSET      = inst2(32*2+31, 32*2);
    COUT_OFFSET      = inst2(32*3+31, 32*3);
    FILTER_S1        = inst2(32*4+15, 32*4);
    FILTER_S2        = inst2(32*4+31, 32*4+16);
    STRIDE           = inst2(32*5+31, 32*5);
    LAYER_EN         = inst3(32*0+31, 32*0);
    PREV_CIN_OFFSET  = inst3(32*1+31, 32*1);
    LAYER_IN_NUM_T   = inst3(32*2+15, 32*2);
    LAYER_OUT_NUM_T  = inst3(32*2+31, 32*2+16);
    LAYER_IN_H_T     = inst3(32*3+31, 32*3);
    LAYER_IN_W_T     = inst3(32*4+31, 32*4);
    KH_KW 			    = inst5(32*0+31, 32*0);
    LAYER_CONV_TYPE = inst5(32*3+31, 32*3);
    CONV_1ST_EN    = LAYER_EN[0];
    DEPTH_CONV_EN  = LAYER_EN[1];
    CONV_EN        = LAYER_EN[2];
    RELU_EN        = LAYER_EN[3];
    RELU6_EN       = LAYER_EN[4];
    POOL_EN        = LAYER_EN[5];
    UP_SAMPLE_EN   = LAYER_EN[6];  // reserved
    BIAS_EN        = LAYER_EN[7];
    INTER_LOAD_EN  = LAYER_EN[8];
    INTER_WRITE_EN = LAYER_EN[9];
    BATCH_NORM_EN  = LAYER_EN[10];
    LOAD_PREV_CIN  = LAYER_EN[11];

#ifdef DEBUG_config_cin
		cout << LAYER_IN_NUM_HW << " " << LAYER_OUT_NUM_HW << " " << LAYER_IN_H_HW << " " << LAYER_IN_W_HW << " " << LAYER_OUT_H_HW << " " << LAYER_OUT_W_HW << endl;
		cout << LAYER_IN_NUM << " " << LAYER_OUT_NUM << " " << LAYER_IN_H << " " << LAYER_IN_W << " " << LAYER_OUT_H << " " << LAYER_OUT_W << endl;
		cout << CIN_OFFSET << " " << WEIGHT_OFFSET << " " << BIAS_OFFSET << " " << COUT_OFFSET << " " << FILTER_S1 << " " << FILTER_S2 << " " << STRIDE << endl;
		cout << LAYER_EN << " " << PREV_CIN_OFFSET << " " << LAYER_IN_NUM_T << " " << LAYER_OUT_NUM_T << " " << LAYER_IN_H_T << " " << LAYER_IN_W_T << endl;
#endif
		
		// Pass the config/instructions
		if (layer_start){
			fifo_config_out.write(inst0);
			fifo_config_out.write(inst1);
			fifo_config_out.write(inst2);
			fifo_config_out.write(inst3);
			fifo_config_out.write(inst4);
			fifo_config_out.write(inst5);
			layer_start = 0;
		}

		// offsets
		uint cin_offset = CIN_OFFSET;
		uint prev_cin_offset = PREV_CIN_OFFSET;
		FILTER = (LAYER_CONV_TYPE == 1)? (ap_uint<4>)(KH_KW>>28) : (ap_uint<4>) FILTER_S2;

		if (prev == 1) start_prev = 1;

		// set up some configuration signals
		uint FILTER_S = (DEPTH_CONV_EN == 1)? (uint)FILTER_S1: ((CONV_EN == 1)? (uint)FILTER_S2: 1);
		bool separable_conv = (DEPTH_CONV_EN == 1) && (CONV_EN == 1);
		bool conv2d = (DEPTH_CONV_EN == 0) && (CONV_EN == 1);
		bool max_pool = (DEPTH_CONV_EN == 0) && (CONV_EN == 0);
		change_layout = (((LAYER_IN_W_HW == LAYER_IN_W) || (LAYER_IN_W_HW == LAYER_IN_W_T)) && ((LAYER_IN_H_HW == LAYER_IN_H) || (LAYER_IN_H_HW == LAYER_IN_H_T))); // if next filter = 1 : change the layout to num_tile, Th, Tw, Tn

		// If it has to read from DRAM and not the stored data in on-chip storage
		if (INTER_LOAD_EN == 0){
			if ((max_pool && out_num_iter == 0) || separable_conv || conv2d || (UP_SAMPLE_EN && out_num_iter == 0)){
				if (task_cnt == 0){
					// first load cin
					cin_load_ddr_read(global_cin, cin_burst_buf_ping, LAYER_IN_H_HW, LAYER_IN_W_HW, LAYER_IN_NUM_T, LAYER_IN_H_T, LAYER_IN_W_T, FILTER, cin_offset, in_num_iter, in_h_iter, in_w_iter, num_tile, change_layout, max_pool, 0);
          count_dram++;
        } else {
					// Apply double buffering for reading the data and filling the FIFO
					if (task_cnt % 2 == 1){
						cin_load_ddr_read(global_cin, cin_burst_buf_pong, LAYER_IN_H_HW, LAYER_IN_W_HW, LAYER_IN_NUM_T, LAYER_IN_H_T, LAYER_IN_W_T, FILTER, cin_offset, in_num_iter, in_h_iter, in_w_iter, num_tile, change_layout, max_pool, 0);
						cin_load_fifo_write(cin_burst_buf_ping, fifo_cin, LAYER_IN_NUM_T_prev, LAYER_IN_H_T_prev, LAYER_IN_W_T_prev, FILTER_prev);
            count_fifo++;
            count_dram++;
					} else {
						cin_load_ddr_read(global_cin, cin_burst_buf_ping, LAYER_IN_H_HW, LAYER_IN_W_HW, LAYER_IN_NUM_T, LAYER_IN_H_T, LAYER_IN_W_T, FILTER, cin_offset, in_num_iter, in_h_iter, in_w_iter, num_tile, change_layout, max_pool, 0);
						cin_load_fifo_write(cin_burst_buf_pong, fifo_cin, LAYER_IN_NUM_T_prev, LAYER_IN_H_T_prev, LAYER_IN_W_T_prev, FILTER_prev);
            count_fifo++;
            count_dram++;
					}
				}

				task_cnt++;
				LAYER_IN_NUM_T_prev = LAYER_IN_NUM_T;
				LAYER_OUT_NUM_T_prev = LAYER_OUT_NUM_T;
				LAYER_IN_H_T_prev = LAYER_IN_H_T;
				LAYER_IN_W_T_prev = LAYER_IN_W_T;
				FILTER_prev = FILTER;
			}
		}

		// Continue until all the tiles are read
		// Since each layer produces LAYER_OUT_NUM feature maps, 
		// repeat reading the tiles LAYER_OUT_NUM times
		in_num_iter += LAYER_IN_NUM_T;
		if (in_num_iter < LAYER_IN_NUM){
			channel_iter += ((LAYER_IN_W / LAYER_IN_W_T) * (LAYER_IN_H / LAYER_IN_H_T));
		} else {
			channel_iter = 0;
			inter_tile++;
		}
		num_tile = channel_iter + inter_tile;
    cout<<in_num_iter<<endl;
		if (in_num_iter >= LAYER_IN_NUM){
			in_num_iter = 0;
			channel_iter = 0;
			in_h_iter += LAYER_IN_H_T;
			if (in_h_iter >= LAYER_IN_H){
				in_h_iter = 0;
				in_w_iter += LAYER_IN_W_T;
				if (in_w_iter >= LAYER_IN_W){
					in_w_iter = 0;
					out_num_iter += LAYER_OUT_NUM_T;
					num_tile = 0;
					inter_tile = 0;
					channel_iter = 0;
					if (out_num_iter >= LAYER_OUT_NUM){
						out_num_iter = 0;
						layer_iter += 1;
						prev = 0;
						layer_start = 1;
						if (layer_iter == LAYER_BATCH){
							layer_iter = 0;
							out_num_iter = 0;
							in_h_iter = 0;
							in_w_iter = 0;
							done = 1;
						}
					}
				}
			}
		}
	}


	// Fill the FIFOs with the data for the last tile
	if (task_cnt % 2 == 1){
		cin_load_fifo_write(cin_burst_buf_ping, fifo_cin, LAYER_IN_NUM_T_prev, LAYER_IN_H_T_prev, LAYER_IN_W_T_prev, FILTER_prev);
    count_fifo++;
	} else {
		cin_load_fifo_write(cin_burst_buf_pong, fifo_cin, LAYER_IN_NUM_T_prev, LAYER_IN_H_T_prev, LAYER_IN_W_T_prev, FILTER_prev);
    count_fifo++;
	}
  // cout<<"count_dram: "<<count_dram<<endl;
  // cout<<"count_fifo: "<<count_fifo<<endl;
  // exit(0);
}