#!/usr/bin/python

import tensorflow as tf
import math

def LIF(input_data, name=''):
    input_shape = input_data.get_shape()

    one_plus = tf.fill(input_shape, 1.0+1e-5)
    zero_plus = tf.fill(input_shape, 1e-5)
    ones = tf.ones(input_shape)
    small_values = tf.fill(input_shape, 1e-7)
    zeros = tf.zeros(input_shape)

    t_ref_B = tf.fill(input_shape, 0.004)
    t_rc_B = tf.fill(input_shape, 0.1)
    v_th_B = ones

    input_data_useful = tf.where(tf.greater(input_data, zeros), input_data, zero_plus)

    #real_rate = tf.div(ones, tf.add(tf.mul((tf.log(tf.add(tf.div(v_th_B, input_data_useful - v_th_B), ones))), t_rc_B), t_ref_B))
    #real_rate = ones/(t_ref_B + t_rc_B * tf.log1p(v_th_B/(input_data_useful)))
    real_rate = ones/(t_ref_B + t_rc_B * tf.log1p(v_th_B/(input_data_useful)))

    rate = tf.where(tf.greater(input_data, zeros), real_rate, zeros, name=name+'_rate')
    #rate_P = tf.Print(rate, [input_data, rate], message=name+' is')

    return rate

def soft_LIF(input_data, name=''):
    input_shape = input_data.get_shape()

    t_ref_B = tf.fill(input_shape, 0.004)
    t_rc_B = tf.fill(input_shape, 0.02)
    v_th_B = tf.ones(input_shape)

    rate = tf.div(1.0, t_ref_B + t_rc_B * tf.log1p(v_th_B/(tf.log1p(tf.exp(input_data-v_th_B)))), name=name+'rate')

    return rate

