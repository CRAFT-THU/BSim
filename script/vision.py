import os

import sys

import numpy as np

os.environ['TF_CPP_MIN_LOG_LEVEL']='3'

import tensorflow as tf

#tf.logging.set_verbosity(tf.logging.ERROR)

import TJ

from mnist import MNIST


def conv(input_data, kernel, bias, name='conv'):
    conv = tf.nn.conv2d(input_data, kernel, strides=[1, 1, 1, 1], padding='SAME', name=name+'_conv')
    u = tf.add(conv, bias, name=name+'_u')

    ret = TJ.TJ(u, name=name)
    return ret;

def fc(input_data, weight, bias, name='fc'):
    matmul = tf.matmul(input_data, weight)
    u = tf.add(matmul, bias, name=name+'_u')
    ret = TJ.TJ(u, name=name)
    return ret
			
def fc2(input_data, weight, bias, name='fc'):
    matmul = tf.matmul(input_data, weight)
    u = tf.add(matmul, bias, name=name+'_u')
    return u 
			

def load_mnist(location='/home/yhx/data/mnist'):
    mndata = MNIST(location)
    return mndata.load_testing()


#tf_image = tf.constant(images[2])

image_data = tf.placeholder(tf.float32, shape = [784])

def build_network():
    tf_image = tf.reshape(image_data, [-1, 28, 28, 1])

    conv1_weight = np.load('/home/yhx/BSim/script/conv1_kernel:0.data.npy')
    conv1_bias = np.load('/home/yhx/BSim/script/conv1_bias:0.data.npy')
    tf_conv1_w = tf.constant(conv1_weight)
    tf_conv1_b = tf.constant(conv1_bias)

    conv2_weight = np.load('/home/yhx/BSim/script/conv2_kernel:0.data.npy')
    conv2_bias = np.load('/home/yhx/BSim/script/conv2_bias:0.data.npy')
    tf_conv2_w = tf.constant(conv2_weight)
    tf_conv2_b = tf.constant(conv2_bias)

    fc1_weight = np.load('/home/yhx/BSim/script/fc1_weight:0.data.npy')
    fc1_bias = np.load('/home/yhx/BSim/script/fc1_bias:0.data.npy')
    tf_fc1_w = tf.constant(fc1_weight)
    tf_fc1_b = tf.constant(fc1_bias)

    fc2_weight = np.load('/home/yhx/BSim/script/fc2_weight:0.data.npy')
    fc2_bias = np.load('/home/yhx/BSim/script/fc2_bias:0.data.npy')
    tf_fc2_w = tf.constant(fc2_weight)
    tf_fc2_b = tf.constant(fc2_bias)

    conv1 = conv(tf_image, tf_conv1_w, tf_conv1_b, name = 'conv1')
    pool1 = tf.nn.max_pool(conv1, ksize=[1,2,2,1], strides=[1, 2, 2, 1], padding='SAME', name='max1')

    conv2 = conv(pool1, tf_conv2_w, tf_conv2_b, name = 'conv2')

    pool2 = tf.nn.max_pool(conv2, ksize=[1,2,2,1], strides=[1, 2, 2, 1], padding='SAME', name='max2')

    pool2_flat = tf.reshape(pool2, [-1, 7*7*64])

    fc1 = fc(pool2_flat, tf_fc1_w, tf_fc1_b, name='fc1')
    res = fc2(fc1, tf_fc2_w, tf_fc2_b, name='fc1')

    return res

def vision(image):

    sess = tf.Session()
    network = build_network()
    sess.run(tf.global_variables_initializer())
    res = sess.run(network, feed_dict={image_data:image})

    return res

def test():

    idx = 0

    images, labels = load_mnist()
    label = labels[idx]
    #tf_image = tf.constant(images[2])
    value = vision(images[idx])
    #print value
    min_value = np.amin(value)
    value = value - min_value
    #print value
    max_value = np.amax(value)
    res = value/max_value

    print "Label: " + str(label)

    return list(res[0])



if __name__ == "__main__":
    res = test()
    print res


