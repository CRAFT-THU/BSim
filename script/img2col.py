#!/usr/bin/python

def img2col(img = [], k_w = 0, k_h = 0):
    i_h = len(img)
    i_w = len(img[0])
    i_c = len(img[0][0])

    matrix = []
    m_w = i_c * k_w * k_h
    m_h = i_w * i_h

    for i in range(0, i_h - k_h + 1):
        line = []
        for j in  range(0, i_w - k_w + 1):
            data = []
            for k in range(0, i_c):
                for l in range(0, k_h):
                    for m in range(0, k_w):
                        data.append(img[i+l][j+m][k])
            line.append(data)
        matrix.append(line)

    return matrix

def flat_kernel(kernel = []):
    k_h = len(kernel)
    k_w = len(kernel[0])
    i_c = len(kernel[0][0])
    o_c = len(kernel[0][0][0])

    flat = []
    for i in range(0, o_c):
        line = []
        for l in range(0, i_c):
            for j in range(0, k_h):
                for k in range(0, k_w):
                    line.append(kernel[j][k][l][i])

    return flat


#def test_data:
i_w = 8
i_h = 8
i_c = 1

kernel = [[[[1]], [[2]], [[1]]], [[[2]], [[3]], [[1]]], [[[3]], [[2]], [[2]]]]

img = []
for i in range(0, i_h):
    line = []
    for j in range(0, i_w):
        channel = []
        for i in range(0, i_c):
            channel.append(j*i)
        line.append(channel)
    img.append(line)

matrix = img2col(img = img)
flat = flat_kernel(kernel=kernel)



if __name__ == "__main__":
    test()
