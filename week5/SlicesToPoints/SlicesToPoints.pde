/* SlicesToPoints
 * Looks at a series of slice images <file_name><slice_num>.<file_ext>
 * If the red value of the pixel colour is < thresh, then it marks it
 * as a point.
 * The points are exported to .txt files in the format <x>,<y>\n
 * by Erin RobotGrrl, March 3 2015
 */

int max_files = 3; // number of slices
String file_name = "fablab_logo_slice";
String file_ext = "jpg";
int thresh = 200; // red < thresh
int skip = 3; // record every this many points
int border = 1; // if there is a pixel border around the slice images

PImage img;
ArrayList<Point> points = new ArrayList<Point>();
PrintWriter output;
int pcount = 0;
int slice_num = 1;


void setup() {
  img = loadImage(file_name+slice_num+"."+file_ext);
  output = createWriter("points"+slice_num+".txt");
  
  size(img.width,img.height);
  noStroke();
  noLoop();
}

void draw() {
  
  background(128);
  image(img,0,0);
  loadPixels();
  img.loadPixels();
  
  
  // get the pixels
  
  for(int j=border; j<img.height-border; j++) {
    
    for(int i=border; i<img.width-border; i++) {
  
      int loc = i + j*img.width;
  
      float r = red(img.pixels[loc]);
      float g = green(img.pixels[loc]);
      float b = blue(img.pixels[loc]);
      
      if(r <= 200) {
        print("point: (" + i + ", " + j + ")");
        println(" colour: ("+r+","+g+","+b+")");
        
        if(pcount%skip == 0) {
          points.add(new Point(i, j));  
          pcount = 0;
        }
        pcount++;
                
      }
    
    }

  }
  
  
  // find the centorid and write the file
  
  int centroid_x = 0;
  int centroid_y = 0;
  
  for(int i=0; i<points.size(); i++) {
    Point p = points.get(i);
    centroid_x += p.x;
    centroid_y += p.y;
    fill(0, 255, 0);
    rect(p.x-5, p.y-5, 10, 10);
    
    output.println(""+p.x+","+p.y+"");
  }

  centroid_x/=points.size();
  centroid_y/=points.size();

  output.flush();
  output.close();
  
  fill(128, 0, 128);
  rect(centroid_x-5, centroid_y-5, 10, 10);


  // go to the next slice

  next();

}


void next() {
 
  slice_num++;
  
  if(slice_num > max_files) exit();
  
  img = loadImage(file_name+slice_num+"."+file_ext);
  output = createWriter("points"+slice_num+".txt");
  
  for(int i=0; i<points.size(); i++) {
    points.remove(0);
  }
  
  draw(); 
 
}


