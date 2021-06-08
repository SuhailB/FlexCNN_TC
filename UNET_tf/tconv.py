## Imports
from ast import walk
import os
import sys
import random
import math

import numpy as np
import matplotlib.pyplot as plt

import tensorflow as tf
from tensorflow import keras
import tensorflow as tf
from tensorflow.keras.activations import relu
from tensorflow.keras.initializers import GlorotUniform, GlorotNormal
from tensorflow.keras.layers import Layer, Conv2D, Softmax, BatchNormalization, \
    LayerNormalization, ReLU, Lambda, Conv2DTranspose, MaxPooling2D, concatenate, add
# from tensorflow_addons.layers import InstanceNormalization
from tensorflow.keras import Model

from keras.models import Sequential
from keras.layers import Dense
from keras.utils.vis_utils import plot_model

contract_filters0 = 32
contract_filters1 = contract_filters0 * 2
contract_filters2 = contract_filters1 * 2
contract_filters3 = contract_filters2 * 2
expand_filters3 = contract_filters3
expand_filters2 = expand_filters3 // 2

def TCONV():

  inputs = keras.layers.Input((16, 16, 8))
  tconv = Conv2DTranspose(8, 3, strides=2, use_bias=True,
                                      dtype=tf.float32, padding='same', data_format='channels_last',
                                      kernel_initializer=GlorotNormal(), bias_initializer=tf.initializers.Constant(0))(inputs)
  model = keras.models.Model(inputs, tconv)
  return model

def tconv(inputs, weights, outputs_sw):
  STRIDE = 2
  OUT_NUM = 8
  IN_H = 16
  IN_W = 16
  IN_NUM = 8
  for o in range(OUT_NUM):
    for h in range(IN_H):
      for w in range(IN_W):
        for p in range(3):
          for q in range(3):
            for i in range(IN_NUM):
              outputs_sw[o][h+p+h*(STRIDE-1)][w+q+w*(STRIDE-1)] += inputs[i][h][w]*weights[o][i][p][q]
            

model = TCONV()
model.compile(optimizer="adam", loss="binary_crossentropy", metrics=["acc"])
model.summary()

layer = model.get_layer('conv2d_transpose')
w = layer.get_weights()
w[0].fill(1)
layer.set_weights(w)
inputs = np.ones((1,16,16,8))
ouptuts = model.predict(inputs)
for o in range(8):
  print('---------------------channel '+str(o)+' ---------------------')
  for h in range(32):
    for w in range(32):
      print(str('{0:8}'.format(ouptuts[0][h][w][o])), end = '\t')
    print()

inputs = np.ones((8,16,16))
weights = np.ones((8,8,3,3))
outputs_sw = np.zeros((8,33,33))
tconv(inputs, weights, outputs_sw)
print('output_sw')
for o in range(8):
  print('---------------------channel '+str(o)+' ---------------------')
  for h in range(33):
    for w in range(33):
      print(str('{0:8}'.format(outputs_sw[o][h][w])), end = '\t')
    print()

