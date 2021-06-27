
#include <GL/glut.h>

// other include files...
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <vector>
 #include <cstring>

// the following make it so we can use string and vector objects
// by just declaring like "string s;" rather than having to
// do "std::string s;".  Just a shortcut.
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::stringstream;

// Global variables...
int score;
GLfloat xT, yT, zT, xRot, yRot; // keep track of where we are in the world.
GLuint texture; // holds a picture for displaying
int windowWidth, windowHeight; // # pixels in the main window
int beginY, beginX; // keeping track of mouse drags
float minX, maxX, minY, maxY, minZ, maxZ; // ranges where bubbles are allowed
int success;
bool endgame = false;


//Return:   random floating point value between min and max.
GLfloat randFloat(GLfloat min, GLfloat max) {
  return ((float) rand() / (float) RAND_MAX) * (max-min) + min;
}

/* Function: distanceSq
                                                                                                                    
   Return:   the distance squared between points 1 and 2.  Distance
             is returned squared so don't have to compute sqrt,
             which is a time-expensive operation.
*/
GLfloat distanceSq(GLfloat x1, GLfloat y1, GLfloat z1,
                                 GLfloat x2, GLfloat y2, GLfloat z2) {
  return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2);
}

/*
  Class: myBubble
  Description: The bubble stores the position of a bubble and
  the direction and speed it is "floating".  The class has a
  function to draw the bubble on the screen.  When the bubble
  is created, it has random location and direction.
 */
class myBubble {
public:
  GLfloat x, y, z,
          dx, dy, dz, r;

  // put random values into the position and direction/speed.
  void randomValues() {
    x = randFloat(minX, maxX);
    y = randFloat(minY, maxY);
    z = randFloat(minZ, maxZ);
        dx = randFloat(-0.01f,0.01f);
        dy = randFloat(-0.01f,0.01f);
        dz = randFloat(-0.01f,0.01f);
    r = randFloat(0.1f, 1.0f);
  }

  // constructor - called when a bubble is created.
  myBubble() {
        randomValues();
  }

  // draw the bubble on the screen.
  void draw() {
    glPushMatrix(); // push the current matrix onto the stack
    glTranslatef(x, y, z); // move to where the bubble should be
    glutSolidSphere(r, 20, 20); // draw a sphere of radius r out of "20x20" rectangles
    glPopMatrix();  // pop off the matrix we saved before - so move back to where
                    // we were before drawing the bubble

    // change position based off of speed for next time we draw.
    x += dx;
    y += dy;
    z += dz;

    // if have gone too far, re-initialize somewhere random.
    if (x < minX || x > maxX || y < minY || y > maxY || z < minZ || z > maxZ) {
      randomValues();
    }
  }
};


/*
  Class: arrow
  Description: used for shooting into the bubbles.  Right now it is just
               a tetrahedron that flies through the screen.
 */
class arrow {
public:
  GLfloat x, y, z;

  // constructor - save where the arrow should start.
  arrow(GLfloat initX, GLfloat initY, GLfloat initZ) {
    x = initX; y = initY; z = initZ;
  }

                                                                                                         



  // draw the arrow - similar to the draw function for the bubbles.
  void draw() {
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(1.0f,5,5);
    glPopMatrix();
    z -= 0.5f;
  }

  // hits function - test if the arrow has hit the given bubble.
  int hits(myBubble &b) {
    // if squared distances is less than one, we're close enough to
    // call it a "hit"
    if (distanceSq(x,y,z,b.x,b.y,b.z) < 1.0f)
      return 1;
    else
      return 0;
  }
};

// list of bubbles and arrows.  keep them in a vector because
// that is an automatically growing arrow.  they are initially
// empty - no bubbles or arrows.
vector<myBubble> bubbles;
vector<arrow> arrows;

// create bubbles.  Intially we will create 7, but the numBubblesInit
// can be changed to create move.
int numBubblesInit = 16;                        //NUMBER OF BUBBLES!
void createBubbles() {
  srand(time(NULL));
  for(int i=0;i<numBubblesInit;i++) {
    myBubble b;
    bubbles.push_back(b);
  }
}

//Display Score
void D_score(int success)
{
     if(success==1)
    glColor3f(1,0,0);
    glRasterPos3f(0,5,-5);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'S');
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'C');
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'O');
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'R');
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'E');
    glColor3f(0,0,1);
        glRasterPos3f(0,5,-7);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '0' + score);
}
/* Function: init
   Purpose: initialize the program - setting up bubbles and arrows,
   setting up openGL stuff in the window.
 */
void init() {
  windowWidth = 640; windowHeight = 480;
  beginY = beginX = 0;
  xT = yT = zT = xRot = yRot = 0.0f;    //Initial window coordinates.
  minX = minY = -5.0;
  maxX = maxY = 5.0;
  maxZ = 0.0;
  minZ = -10.0;

  glClearColor(0.0,0.0,0.0,0.0);
  //window specification
  glutInitWindowSize(windowWidth,windowHeight);
  glutInitWindowPosition(0,0);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutCreateWindow("Bubbles Game");

  // random bubbles.
  createBubbles();
                                                                                                                        

  // set up lighting that will be used for having objects that look "shaded".
  glShadeModel(GL_SMOOTH);
  GLfloat light_position[] = {0.0, 0.0, maxZ*2, 1.0},
    white_light[] = {1.0, 5.0, 1.0, 1.0},
    diffColors[] = {1.0, 0.0, 0.0, 1.0},
    lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};

  glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffColors);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

  // set up a texture that will have an image on it.  It will
  // be used to draw a "background" behind the scene.
  //
  // first read in the data from the picture file.  The picture
  // was created in GIMP and saved as a raw image file.
  const int width=640, height=840;
  unsigned char *data;
  FILE * file;
  //file = fopen("./BS11_logo.bmp", "rb"); //loading RAW file for texture Mapping in read mode
    //if(file == NULL) return 0;
  data = (unsigned char *)malloc( width * height * 3 );
  fread(data,width*height*3, 1, file);
  fclose(file);

  for(int i = 0; i < width * height ; ++i)
    {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];

        data[index] = R;
        data[index+2] = B;
                                                                                                                        
}

  // now that we have the data loaded from the file, load it
  // into the texture object that can later be used to put it
  // onto the screen.

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);    //Extracting Pixels
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  // enable the following OpenGL features/computations...
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
//  glEnable(GL_MULTISAMPLE);

  // blending is for making the bubbles seem "transparent-ish".
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void Timer(int i)
{
        glutPostRedisplay();
        glutTimerFunc(60,Timer,0);
}

void displayScore()
{
        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        glTranslatef(0,1,0);

        glClearColor(0.1,0.0,0.5,0.9);
        glColor3f(    0.412, 0.412, 0.412);
        glBegin(GL_QUADS);
                glVertex2f(0.0,0.0);
                glVertex2f(0.0,400.0);
                glVertex2f(400.0,400.0);
                glVertex2f(400.0,0.0);
        glEnd();

        glColor3f(0.0,0.0,0.0);
        char string[4] = "   ";
        sprintf(string,"%d",score);
        int x = 0;
        int y = 0;
        int z = 0;
        //set the position of the text in the window using the x and y coordinates
        glRasterPos3f(x,y,z);
        char txt[100] = "Your score is ";
        strcat(txt, string);
        //get the length of the string to display
        int len = (int) strlen(txt);

        //loop to display character by character
        for (int i = 0; i < len; i++)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,txt[i]);

    glPopMatrix();
}

/*
  Function: resize
  Purpose: called by glut whenever the window is resized, including when
           the window is created.  So take care of setting up the perspective,
           etc. of OpenGL.
*/
                                                                                                                          
void resize(int width, int height)
{
  // save the width and height to global variables.
  windowWidth = width;
  windowHeight = height;

  // tell OpenGL about the size of the window.
  glViewport(0, 0, width, height);

  // set the viewing mode based on the width and height.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0f, (GLfloat)width/(GLfloat)height, 1.0f, 1000.0f);

  // set OpenGL so that any transformations will happen to the
  // "modelview matrix" - meaning it will put objects in different
  // places in the world.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_CULL_FACE);
}

/*
   Function: display
   Purpose: called by glut whenever the window needs to be redisplayed.
   This is where we draw the bubbles and so on.
*/
void display()
{
    if(endgame == false){
  // start off with clearing out the screen and resetting our
  // "modelview" position to be at 0,0,0.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                                                                                                                       
  glLoadIdentity();


  // draw the isu background.  This is by taking the image that
  // was loaded as a texture and "texture mapping" it to a
  // rectangle.
  //
  // first set up the lighting/texture-mapping stuff...
  GLfloat diffColorsW[] = {1.0, 1.0, 1.0, 1.0};
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffColorsW);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, texture);

  // now draw the rectangle that will have the image on it.
  glBegin(GL_QUADS);

  glNormal3f(0.0f, 0.0f, 1.0f);
  GLfloat d = 500.0f;
  glTexCoord2f(0.0f, 0.0f); glVertex3f(-d, d/1.6, -d);
  glTexCoord2f(0.0f, 1.0f); glVertex3f(-d, -d/1.6, -d);
  glTexCoord2f(1.0f, 1.0f); glVertex3f(d, -d/1.6, -d);
  glTexCoord2f(1.0f, 0.0f); glVertex3f(d, d/1.6, -d);

  glEnd(); // done drawing the rectangle.

  glDisable(GL_TEXTURE_2D);

  // now move the camera/scene based off where we should be.
  glTranslatef(xT,yT,zT);
  glRotatef(xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    const GLfloat light_ambient[]  = { 2.0f, 2.0f, 2.0f, 2.0f };
    const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
                                                                                                                        
    const GLfloat light_position[] = { 10.0f, 5.0f, 5.0f, 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // draw bubbles
  //
  // first set up the reflective/color/light properties of the bubbles.
  GLfloat diffColorsR[] = {0.9, 4.9, 0.9, 0.3};
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffColorsR);
  GLfloat spec[] = {0.9, 4.9, 0.9, 0.3};
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
  int shin= 250;
  glMaterialiv(GL_FRONT, GL_SHININESS, &shin);

  // second, draw them.
  for(int i=0;i<bubbles.size();i++) {
    bubbles.at(i).draw();
  }

  // draw arrows - same game as with drawing bubbles.
  GLfloat diffColorsRw[] = {0.0, 1.0, 0.0, 0.9};
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffColorsRw);
  GLfloat specw[] = {0.9, 0.9, 0.9, 0.9};
  glMaterialfv(GL_FRONT, GL_SPECULAR, specw);

  for(int i=0;i<arrows.size();i++) {
    arrows.at(i).draw();

    // if arrow has gone too far away, then kill it.
    if (arrows.at(i).z < minZ) {
      arrows.erase(arrows.begin()+i);
      i--;
      continue;
    }
                                                                                                                        
    int j;
    // if arrow hits any bubbles, kill the bubble
    for(j=0;j<bubbles.size();j++) {
      if (arrows.at(i).hits(bubbles.at(j))) {
        bubbles.erase(bubbles.begin()+j);
        j--;
        score = score+1;
        success = 1;
        D_score(success);
      }
    }
    if(j==0){
            xT, yT, zT, xRot, yRot = 0.0f;
        endgame = true;
    }
  }
        glFlush();
  }
  if(endgame == true)
    displayScore();

   //if out of bubbles, then let's do more!
   if (bubbles.size() <= 0) {
        numBubblesInit *= 2;
          createBubbles();
  }
    glFlush();
  // this call makes the changes we have computed get put on the
  // screen.  since we used GLUT_DOUBLE earlier, there are two
  // images always - the one drawn on the screen, and the one we
  // are making changes to before we're ready to put it on the screen.
  glutSwapBuffers();
}

/* Function: key
   Purpose: called by glut whenever user presses a key.
*/
                                                                                                                         
void key(unsigned char key, int x, int y)
{
  switch (key)
    {
        case '2':
          numBubblesInit *= 2;
          createBubbles();
          break;
    case 'r': case 'R':
      zT = xT = yT = xRot = yRot = 0.0f;
      break;
    case 27 :
      exit(0);
      break;
    case '+':
      zT += .5f;
      break;
    case '-':
      zT -= .5f;
      break;
    case 'p':
      glutIdleFunc(NULL);
      break;
    case 's':
        glutIdleFunc(display);
        break;
    default:
      break;
    }

  // redraw the screen sinc something could have changed.
  glutPostRedisplay();
}

/* Function: specialKey
   Purpose: called by glut when user types a non-printable key.
*/
                                                                                                                        
void specialKey(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_LEFT:
    xT += 1.0f;
    break;
  case GLUT_KEY_RIGHT:
    xT -= 1.0f;
    break;
  case GLUT_KEY_UP:
    yT -= 1.0f;
    break;
  case GLUT_KEY_DOWN:
    yT += 1.0f;
    break;
  }
  glutPostRedisplay();
}

/* Function: idle
   Purpose: called by glut when nothing else is happening.
*/
void idle()
{

  glutPostRedisplay();
}

/* Function: mouseFunc
   Purpose: called by glut when user clicks or releases a mouse button.
*/
void mouseFunc(int button, int state, int x, int y) {
  // when the right click and drag, figure out how
  // many pixels they dragged through, and use that to
  // rotate the screen by a certain amount.
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
      beginY = y; beginX = x;
    }
                                                                                                                          
    else if (state == GLUT_UP) {
      xRot += (float)(y-beginY) / 10.0;
      yRot += (float)(x-beginX) / 10.0;
      glutPostRedisplay();
    }


  // when the left click, fire an arrow.
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLfloat xx = (float)(x) / (float) windowWidth * (maxX-minX) + minX;
        GLfloat yy = (float)(windowHeight-y) / (float) windowHeight * (maxY-minY) + minY;
    arrow a(xx,yy,0.0);
    arrows.push_back(a);
  }
}

// not used right now.
/*int checkError(int line) {
  GLenum e = glGetError();
  if (e == GL_NO_ERROR) return 0;

  const GLubyte *errString = gluErrorString(e);
  cout << "OpenGL error:" <<  errString << endl;
  exit(0);
  return 1;
}*/

/* Function: main
   Purpose: entry point of the program, this is where things start.
*/
int main(int argc, char *argv[])
{
  // initialize glut, based off any command line arguments.
  glutInit(&argc, argv);
    //glClearColor(0.0,0.0,0.0,0.0);
  // initialize the window
  init();
                                                                                                                            
  // print version information.
  cout << "Press - to zoom out" << endl;
  cout << "Press + to zoom in" << endl;
  cout << "Click Mouse:Left button to hit the bubbles (position the mouse correctly)" << endl;
  cout << "Click Mouse:Right button to drag the view to certain amount" << endl;
  cout << "Press R/r to reset the view to (0,0) origin" << endl;
  cout << "Press Arrow keys:Left,Right,Up and Down to change the view of the window" << endl;
  cout << "Press Esc to exit from the game" << endl;
  cout << "The time in the console will decide who the winner is" << endl;
  //cout << "Score" << score << endl;

  // register callback functions.  If we didn't do this, they
  // wouldn't get called.
  glutReshapeFunc(resize);
  glutDisplayFunc(display);
  glEnable(GL_LIGHTING);
  glutKeyboardFunc(key);
  glutSpecialFunc(specialKey);
  glutMouseFunc(mouseFunc);
  glutIdleFunc(idle);
  Timer(0);
  //glutFullScreen();

  // start the main loop.  This will infinitely loop until the
  // window is called, and glut will call our callback functions
  // when needed.
  glutMainLoop();
  return 0;
}


                                                                                                                          
