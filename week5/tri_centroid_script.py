import fab

input('highlight', int)

coords = [ [1,1],
           [3,0],
           [8,1],
           [7,6],
           [2,5] ]

centroid = [0,0]
total_x = 0
total_y = 0


# calculating the centroid

for i in range(0, len(coords)):
  total_x = coords[i][0] + total_x
  total_y = coords[i][1] + total_y

centroid[0] = total_x / len(coords)
centroid[1] = total_y / len(coords)

print(centroid)

fab.ui.point(centroid[0], centroid[1], 0, color=fab.color.orange)


# making our triangles

t0 = fab.shapes.triangle( coords[0][0], coords[0][1], centroid[0], centroid[1], coords[1][0], coords[1][1] )

for i in range(1, len(coords)-1):
  t = fab.shapes.triangle( coords[i][0], coords[i][1], centroid[0], centroid[1], coords[i+1][0], coords[i+1][1] )
  t0 |= t

num = len(coords)

tn = fab.shapes.triangle( coords[num-1][0], coords[num-1][1], centroid[0], centroid[1], coords[0][0], coords[0][1] )

tn |= t0


# use the highlight input to show the triangle wireframe colours

meep = highlight

if meep < 0:
  meep = 0

if meep < num-1:

  fab.ui.wireframe([(coords[meep][0], coords[meep][1], 0), (centroid[0], centroid[1], 0), (coords[meep+1][0], coords[meep+1][1], 0)], close=True, color=(128, 0, 128))

else:
  
  fab.ui.wireframe([(coords[meep][0], coords[meep][1], 0), (centroid[0], centroid[1], 0), (coords[0][0], coords[0][1], 0)], close=True, color=(128, 0, 128))



output('shape', tn)
