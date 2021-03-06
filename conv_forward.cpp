// ConsoleApplication3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <assert.h>
//typedef char int8;
//typedef int int32;
typedef float int8;
typedef float int32;

void conv_layer(int8 *input, int32 *output, int8 *kernel, int8 *bias, int *input_dim, int *kernel_dim, int stride,int pad_w, int pad_h){
	/*
	input: input image with size input_w*input_h*channel_n*batch_size
	output: output image with size output_w * output_h * kernel_num * batch_size
	kernel: filters with size kernel_w*kernel_h*channel_n*kernel_num
	bias: size: kernel_num
	input_dim: array [input_w, input_h, channel_n, batch_size]
	kernel_dim: array [kernel_w, kernel_h, channel, kernel_num]
	stride: stride length
	pad_w: pad in width (symmetric)
	pad_h: pad in height (symmetric)
	*/
	assert(*(kernel_dim + 2) == *(input_dim + 2));
	int input_w = *input_dim;
	int input_h = *(input_dim + 1);
	int output_w = (*input_dim + 2 * pad_w - *kernel_dim) / stride + 1;
	int output_h = (*(input_dim + 1) + 2 * pad_h - *(kernel_dim + 1)) / stride + 1;
	int kernal_w = *kernel_dim;
	int kernal_h = *(kernel_dim + 1);
	int channel_n = *(kernel_dim + 2);
	int kernel_num = *(kernel_dim + 3);
	int batch_size = *(input_dim + 3);
	int kernel_size = 1;
	for (int j = 0; j != 3; ++j){
		kernel_size *= *(kernel_dim + j);
	}
	int input_size0 = input_w * input_h;
	int input_size1 = input_size0 * channel_n;
	int output_size0 = output_h * output_w;
	int output_size1 = output_size0 * kernel_num;
	/*const int kernel_size = (*kernel_dim) * (*(kernel_dim + 1)) * (*(kernel_dim + 2));
	int8 tmp[kernel_size];*/
	for (int ind_batch = 0; ind_batch != batch_size; ++ind_batch){
		for (int ind_h = 0; ind_h != output_h; ++ind_h){
			for (int ind_w = 0; ind_w != output_w; ++ind_w){
				for (int ind_kernel = 0; ind_kernel != kernel_num; ++ind_kernel){
					// compute output pixel
					int32 cur_res = *(bias+ind_kernel);
					int count_h = 0;
					int count_w = 0;
					int ind_channel = 0;
					for (int i = 0; i != kernel_size; ++i){
						// current input pixel index
						int cur_w = ind_w * stride - pad_w + count_w;
						int cur_h = ind_h * stride - pad_h + count_h;
						
						if (cur_h >= 0 && cur_w >= 0 && cur_h<input_h && cur_w<input_w){
							int input_offset = ind_batch * input_size1 + ind_channel * input_size0 + cur_h * input_w + cur_w;
							cur_res += (int32)((*(input + input_offset)) * (*(kernel + ind_kernel * kernel_size + i)));
						}
						count_w++;
						if (count_w == kernal_w){
							count_w = 0;
							count_h++;
						}
						if (count_h == kernal_h){
							count_h = 0;
							ind_channel++;
						}
					}
					int output_offset = ind_batch * output_size1 + ind_kernel * output_size0 + ind_h * output_w + ind_w;
					*(output + output_offset) = cur_res;
				}
			}
		}
	}
}
int _tmain(int argc, _TCHAR* argv[])
{

	int8 input[64] = { 1, 3, 3, 0, 3, 1, 4, 1, 3, 1, 3, 3, 3, 4, 0, 2, 1, 3, 3, 0, 3, 1, 4, 1, 3, 1, 3, 3, 3, 4, 0, 2, 1, 3, 3, 0, 3, 1, 4, 1, 3, 1, 3, 3, 3, 4, 0, 2, 1, 3, 3, 0, 3, 1, 4, 1, 3, 1, 3, 3, 3, 4, 0, 2 };
	int8 kernel[18] = { -1, 0, 1, 1, 1, 1, 1, 0, -1, -1, 0, 1, 1, 1, 1, 1, 0, -1 };
	/*int8 input[32] = { 1, 3, 3, 0, 3, 1, 4, 1, 3, 1, 3, 3, 3, 4, 0, 2, 1, 3, 3, 0, 3, 1, 4, 1, 3, 1, 3, 3, 3, 4, 0, 2 };
	int8 kernel[18] = { -1, 0, 1, 1, 1, 1, 1, 0, -1, -1, 0, 1, 1, 1, 1, 1, 0, -1 };*/
	int input_dim[4] = { 4, 4, 2, 2 };
	int kernel_dim[4] = { 3, 3, 2, 2 };
	int8 bias[2] = { -2, 1 };
	int pad_w = 0;
	int pad_h = 0;
	int stride = 1;
	int output_w = (input_dim[0] + 2 * pad_w - kernel_dim[0]) / stride + 1;
	int output_h = (input_dim[1] + 2 * pad_h - kernel_dim[1]) / stride + 1;
	int output_batchsize = input_dim[3];
	int output_channel = kernel_dim[3];
	int output_size = output_w * output_h * output_batchsize * output_channel;
	int32 *output = (int32*)malloc(sizeof(int32)* output_size);
	conv_layer(input, output, kernel, bias, input_dim, kernel_dim, stride, pad_w, pad_h);
	/*for (int i = 0; i != output_size; ++i){
		printf("%d\t", *(output+i));
	}*/
	for (int i = 0; i != output_size; ++i){
		printf("%f\t", *(output + i));
	}
	return 0;
}

