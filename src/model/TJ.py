#!/usr/bin/python

import tensorflow as tf
import math

def TJ(input_data, name=''):
    input_shape = input_data.get_shape()

    one_plus = tf.fill(input_shape, 1.0+1e-5)
    zero_plus = tf.fill(input_shape, 1e-5)
    ones = tf.ones(input_shape)
    small_values = tf.fill(input_shape, 1e-7)
    zeros = tf.zeros(input_shape)

    t_ref_B = tf.fill(input_shape, 0.004)
    c_m_B = tf.fill(input_shape, 0.5)
    v_th_B = ones

    #t_ref_B = zeros
    #c_m_B = ones

    input_data_useful = tf.where(tf.greater(input_data, zeros), input_data, zero_plus)

    #real_rate = tf.div(ones, tf.add(tf.mul((tf.log(tf.add(tf.div(v_th_B, input_data_useful - v_th_B), ones))), t_rc_B), t_ref_B))
    real_rate = input_data_useful/(input_data_useful * t_ref_B + c_m_B * v_th_B);

    rate = tf.where(tf.greater(input_data, zeros), real_rate, zeros, name=name+'_rate')
    #rate_P = tf.Print(rate, [input_data, rate], message=name+' is')

    return rate
