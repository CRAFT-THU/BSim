#!/usr/bin/python

import numpy as np


def conv2d(img, kernel):
    i_shape = img.shape
    k_shape = kernel.shape

    i_h = i_shape[1]
    i_w = i_shape[2]

    k_h = k_shape[0]
    k_w = k_shape[1]

    d_up = (k_h - 1) / 2
    d_down = k_h - 1 - d_up

    d_left = (k_w - 1)/ 2
    d_right = k_w - 1 - d_left 

    res = np.zeros((i_h, i_w), dtype='float32')

    for i in range(0, i_h):
        for j in range(0, i_w):
            for k in range(0, k_h):
                for l in range(0, k_w):
                    idx_h = i + k - d_up 
                    idx_w = j + l - d_left

                    if (idx_h < 0 or idx_h > i_h -1 or idx_w < 0 or idx_w > i_w - 1):
                        i_c = 0.0
                    else:
                        i_c = img[0][idx_h][idx_w][0]

                    res[i][j] = res[i][j] + i_c * kernel[k][l][0][0]

    return res




