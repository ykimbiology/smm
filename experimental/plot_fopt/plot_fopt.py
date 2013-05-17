#!/usr/bin/env python
"""
Illustrate simple contour plotting, contours on an image with
a colorbar for the contours, and labelled contours.

See also contour_image.py.
"""
import matplotlib
import numpy as np
import matplotlib.cm as cm
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt

import random

matplotlib.rcParams['xtick.direction'] = 'out'
matplotlib.rcParams['ytick.direction'] = 'out'

def get_model(p = 2):
    """
    """
    x = [random.random()-0.5 for i in range(p)]
    x = np.array(x)
    return x

def get_binary_vector(p=2):
    v = get_model(p=p)
    vbinary = []
    for i in range(len(v)):
        value = 0.0
        if v[i] > 0.0:
            value = 1.0
        vbinary.append(value)
    vbinary = np.array(vbinary)
    return vbinary
    
def get_design_matrix(n = 100, p = 2):
    A = []
    for i in range(n):
        v = get_binary_vector(p=p)
        A.append(v)
    A = np.array(A)
    return A

def get_pair_terms(A, x):
    n = len(A)
    p = len(A[0])
    g = np.array([0.0 for i in range(n)])  # [n x 1]
    
    for j in range(p): 
        for k in range(p):
            #g = g + (A.col(j) % A.col(k)) * x(j)*x(k);
            g = g + (A.T[j] * A.T[k]) * x[j]*x[k];
    g = -1.0 * g;
    return g


def get_measured(A, x):
    """
    A: [nxp]
    x: [px1]
    """
    g = get_pair_terms(A,x)
    #y = A.dot(x)
    y = A.dot(x) + g
    return y

def fopt_ols(A, y, x):
    """
    fopt = (Ax - y)^2
    OUTPUT:
        Z
    """
    ypred = A.dot(x)
    diff = ypred - y
    diff = diff*diff
    xvalue = sum(diff)
    return xvalue


def fopt_pair(A, y, x):
    """
    fopt = (Ax + g - y)^2
    OUTPUT:
        Z
    """
    g = get_pair_terms(A,x)
    ypred = A.dot(x) + g
    diff = ypred - y
    diff = diff*diff
    xvalue = sum(diff)
    return xvalue
    
p = 2
n = 100
model = get_model(p=p)
A = get_design_matrix(n=n, p=p)
yobs = get_measured(A, model)
print 'model ', model.shape, model
print 'A ', A.shape
print 'yobs ', yobs.shape

x = np.array([0.5, 0.5])
#xvalue = fopt_ols(A, yobs, x)
xvalue = fopt_pair(A, yobs, x)
print 'xvalue ', xvalue


#xvalue = fopt_ols(A, yobs, model)
xvalue = fopt_pair(A, yobs, model)
print 'xvalue ', xvalue






delta = 0.025
#x = np.arange(-3.0, 3.0, delta)
#y = np.arange(-3.0, 3.0, delta)
x = np.arange(-1.0, 1.5, delta)
y = np.arange(-1.0, 1.0, delta)
X, Y = np.meshgrid(x, y)
(nrows, ncols) = X.shape
Z = []
for i in range(nrows):
    zrow = []
    for j in range(ncols):
        x=[X[i][j], Y[i][j]]
        x = np.array(x)
        #zi = fopt_ols(A, yobs, x)
        zi = fopt_pair(A, yobs, x)
        zrow.append(zi)
    Z.append(zrow)
Z = np.array(Z)




# Create a simple contour plot with labels using default colors.  The
# inline argument to clabel will control whether the labels are draw
# over the line segments of the contour, removing the lines beneath
# the label
plt.figure()
#CS = plt.contour(X, Y, Z)
#CS = plt.contour(X, Y, Z, 20)
V = [i*0.1 for i in range(200)]
CS = plt.contour(X, Y, Z, V)
plt.clabel(CS, inline=1, fontsize=10)
plt.title('Simplest default with labels')


## contour labels can be placed manually by providing list of positions
## (in data coordinate). See ginput_manual_clabel.py for interactive
## placement.
#plt.figure()
#CS = plt.contour(X, Y, Z)
#manual_locations = [(-1, -1.4), (-0.62, -0.7), (-2, 0.5), (1.7, 1.2), (2.0, 1.4), (2.4, 1.7)]
#plt.clabel(CS, inline=1, fontsize=10, manual=manual_locations)
#plt.title('labels at selected locations')
#
#
## You can force all the contours to be the same color.
#plt.figure()
#CS = plt.contour(X, Y, Z, 6,
#                 colors='k', # negative contours will be dashed by default
#                 )
#plt.clabel(CS, fontsize=9, inline=1)
#plt.title('Single color - negative contours dashed')
#
## You can set negative contours to be solid instead of dashed:
#matplotlib.rcParams['contour.negative_linestyle'] = 'solid'
#plt.figure()
#CS = plt.contour(X, Y, Z, 6,
#                 colors='k', # negative contours will be dashed by default
#                 )
#plt.clabel(CS, fontsize=9, inline=1)
#plt.title('Single color - negative contours solid')
#
#
## And you can manually specify the colors of the contour
#plt.figure()
#CS = plt.contour(X, Y, Z, 6,
#                 linewidths=np.arange(.5, 4, .5),
#                 colors=('r', 'green', 'blue', (1,1,0), '#afeeee', '0.5')
#                 )
#plt.clabel(CS, fontsize=9, inline=1)
#plt.title('Crazy lines')
#
#
## Or you can use a colormap to specify the colors; the default
## colormap will be used for the contour lines
#plt.figure()
#im = plt.imshow(Z, interpolation='bilinear', origin='lower',
#                cmap=cm.gray, extent=(-1,1.5,-1,1))
##levels = np.arange(0.0, 10, 0.11)
#levels = np.arange(0.0, 10, 0.2)
#CS = plt.contour(Z, levels,
#                 origin='lower',
#                 linewidths=2,
#                 extent=(-1,1.5,-1,1))
#
##Thicken the zero contour.
#zc = CS.collections[6]
#plt.setp(zc, linewidth=4)
#
##plt.clabel(CS, levels[1::2],  # label every second level
#plt.clabel(CS, levels[1::4],  # label every second level
#           inline=1,
#           fmt='%1.1f',
#           fontsize=14)
#
## make a colorbar for the contour lines
#CB = plt.colorbar(CS, shrink=0.8, extend='both')
#
#plt.title('Lines with colorbar')
##plt.hot()  # Now change the colormap for the contour lines and colorbar
#plt.flag()
#
## We can still add a colorbar for the image, too.
#CBI = plt.colorbar(im, orientation='horizontal', shrink=0.8)
#
## This makes the original colorbar look a bit out of place,
## so let's improve its position.
#
#l,b,w,h = plt.gca().get_position().bounds
#ll,bb,ww,hh = CB.ax.get_position().bounds
#CB.ax.set_position([ll, b+0.1*h, ww, h*0.8])

plt.show()