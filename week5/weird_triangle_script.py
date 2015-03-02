# An attempt at a triangle however does not work the way
# originally thought... now it gives hockey sticks (sometimes)

# Based on the right triangle code by Neil Gershenfeld 1/24/15
# (^ that one worked, my modifications messed it up)

import fab

title('weird triangle')

def right_triangle(x0,y0,w,h):
   from fab.types import Shape, Transform
   # max(max(x0-X,y0-Y),X-(x0*(Y-y0)+(x0+w)*(y0+h-Y))/h)

   xmin = x0
   xmax = x0+w
   ymin = y0
   ymax = y0+h   

   if(w < x0): 
      xmin = x0-w
      xmax = x0

   if(h < y0):
      ymin = y0-h
      ymax = y0

   s = 'a-X/f%(h)g+*f%(xmax)g-f%(ymax)gY*f%(x0)g-Yf%(y0)ga-f%(x0)gX-f%(y0)gY'

   print('xmin: %f ymin: %f' % (xmin, ymin))

   return Shape(s % locals(),
       xmin,ymin,xmax,ymax)


input('x0',float)
input('y0',float)
input('width',float)
input('height',float)

rt = right_triangle(x0,y0,width,height)

output('shape',rt)

fab.ui.point(x0,y0)
fab.ui.point(x0+width,y0)
fab.ui.point(x0,y0+height)
