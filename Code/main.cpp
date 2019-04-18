///[DESCRIPTION]
//Midterm Project - Part C - Kostiantyn Makrasnov - 04/03/2019

///[REFERENCES]
//1) Base - Used SkyCube Example as Base for this Assignment
//2) GLUT Functions Description - https://www.opengl.org/resources/libraries/glut/spec3/node1.html

///[FUNCTION PROTOTYPES]
//Movement
void movePlayer();

///[INCLUDES]
//Standart
#include <iostream>			// cout
#include <iomanip>			// setprecision
#include <cmath>			// matrix operations
#include <math.h>			// matrix operations
#include <cassert>			// debuging shaders
#include <vector>			// creating maze and positions for collision checking
#include <stack>			// for proper path branching in maze generation
#include <algorithm>		// min, max
#include <random>			// to create a random maze (different apaths
//GLUT
#include "Angel.h";			// for several utility functions
#include <GL/glew.h>		// for OpenGL extensions
#include <GL/glut.h>		// for Glut utility kit
#include "texture.h"		// for the bitmap texture loader
//Projection matrices 
mat4 projection, modelview, translate;
//Graphic items
#include "GraphicsObject.h" // base class for below
#include "SkyBox.h"			// enviroment background
#include "Brick.h"			// wall objects
#include "Tree.h"			// center tree

using namespace std;


///[GLOBAL VARIABLES]
//Movement Aids (tell where to move when user presses WASD)
vec4 forwardVec(0.0, 0, 1.0, 0.0);
vec4 leftVec(1.0, 0, 0.0, 0.0);
//Projection matrix parameters
GLfloat  fovy = 60.0;			//the angle of the view/camera
GLfloat  aspect = 1.0;          //keeping this at 1 allows for a correctly scalled view (square)
GLfloat  zNear = 0.00000001;	//clipping plane (near) objects closer than this are not visible
GLfloat  zFar = 1000.0;			//clipping plane (far) objects farther than this are not visible
//Object types
SkyBox skybox;
Brick wall;
Tree tree;
//Map information
int mazeSize = 30;
vector<vector<bool>> wallPresent(mazeSize);
vector<vec3> wallPositions;
vector<vec3> treePositions;
//Camera positions
point4  eye(0.0, -248.0, -((double)(mazeSize * 4)) / 2.0 - mazeSize + 1.0, 1.0);
point4  at(0.0, -248.0, -((double)(mazeSize * 4)) / 2.0 - mazeSize - 1.0, 1.0);
vec4    up(0.0, 1.0, 0.0, 0.0);
//Mouse movement tracking
int lastX = 10000;
int lastY = 10000;
float dySum = 0;
bool mouseCenteredFlag = true;
//Keyboard movement tracking
bool isWPressed = false;
bool isAPressed = false;
bool isSPressed = false;
bool isDPressed = false;

///[VALIDATION]
/* checkForCollisions: allows to check a future position (where user intends to move for collision conflicts)
 * @param newAt - vec4 representing players position
 * @param newEye - vec4 representing player's camera point of interest (useful to protect from turning camera into an object)
 * @return - TRUE if proposed movement of player is allowable, FALSE if proposed movement will cause player to collide with an object */
bool checkForCollisions(point4 newAt, point4 newEye)
{
	bool moveAllowed = true;

	//Check for collision with any wall
	for (int i = 0; i < wallPositions.size(); i++)
	{
		//distance calculation
		double distanceAt = sqrt(pow(wallPositions[i].x - newAt.x, 2) + pow(wallPositions[i].y - newAt.y, 2) + pow(wallPositions[i].z - newAt.z, 2));
		double distanceEye = sqrt(pow(wallPositions[i].x - newEye.x, 2) + pow(wallPositions[i].y - newEye.y, 2) + pow(wallPositions[i].z - newEye.z, 2));

		if (distanceAt <= 3 || distanceEye <= 3)
		{
			//cout << "Collision with brick - distances at:" << distanceAt << " eye:" << distanceEye << endl;
			moveAllowed = false;
			break;
		}
	}
	//Check for collision with any tree
	for (int i = 0; i < treePositions.size(); i++)
	{
		//distance calculation
		double distanceAt = sqrt(pow(treePositions[i].x - newAt.x, 2) + pow(treePositions[i].y - newAt.y, 2) + pow(treePositions[i].z - newAt.z, 2));
		double distanceEye = sqrt(pow(treePositions[i].x - newEye.x, 2) + pow(treePositions[i].y - newEye.y, 2) + pow(treePositions[i].z - newEye.z, 2));

		if (distanceAt <= 2.1 || distanceEye <= 2.1)
		{
			//cout << "Collision with tree - distances at:" << distanceAt << " eye:" << distanceEye << endl;
			moveAllowed = false;
			break;
		}
	}

	return moveAllowed;
}


///[EVENTS]
// display: Paints the graphical objects onto the screen by calling their draw functions
void display()
{
	//Clear last frame frome the graphics window
	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//Define where camea is curently looking at
	modelview = Translate(0.0, 0.0, 1.0)*LookAt(eye, at, up);

	//Draw graphical objects
	skybox.draw();
	for (int i = 0; i < wallPositions.size(); i++)
		wall.draw(wallPositions[i]);
	tree.draw(vec3(0.0,0,0.0), vec3(-1.0,-240,0.0));

	//Handle player movement
	movePlayer();

	//Finalize current frame
	glutSwapBuffers();
	glutPostRedisplay();

}
/* mouse_move: called every frame the mouse moved in any direction
 * @param x - current mouse X position in window cordinates
 * @param y - current mouse Y position in window cordinates */
void mouse_move(int x, int y)
{
	//if mouse movement engaged rotate camera
	if (mouseCenteredFlag)
	{
		//Initial mouse cordinate setup (first frame only)
		if (lastX == 10000 || lastY == 10000)
		{
			lastX = x;
			lastY = y;
			return;
		}

		//Calculate amount to rotate camera
		int dx = lastX - x;
		int dy = lastY - y;
		dySum += (float)dy;					
		double dxScaled = (double)dx * .5;

		//Horizontal Rotation (w/ update of movement vectors)
		vec4 newForwardVec = RotateY(dxScaled) * forwardVec;
		vec4 newLeftVec = RotateY(dxScaled) * leftVec;
		point4 newAt = eye + forwardVec;

		//Vertical Rotation (dont allow to look up or down too much in verticle rotation)
		dySum = min(max((double)dySum, -550.0), 550.0);
		newAt.y = -248 + dySum * .005;

		//Rotate player camera if it is allowed by collision detector function
		if (checkForCollisions(newAt, eye))
		{
			at = newAt;
			forwardVec = newForwardVec;
			leftVec = newLeftVec;
		}
		else //reset settings if rotation not allowed
		{
			dySum -= (double)dy;
		}

		//Reset pointer to the center of the screen w/o rotating the camera back 
		// - allows for infinite rotation of camera in any direction
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		lastX = glutGet(GLUT_WINDOW_WIDTH) / 2;
		lastY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

		glutPostRedisplay();
	}
}

/* reshape: called every frame the graphics window gets resized
 * @param w - the width of the resized window
 * @param h - the height of the resized window */
void reshape(int w, int h)
{
	//only allow reshape of graphics view if aspect ratio is withing accepted
	float newAspect = (float)w / (float)h;
	if (newAspect < 1.85 && newAspect > .7)
	{
		glViewport(0, 0, w, h);
		aspect = GLfloat(w) / h;
	}

	//Correct camera perspective based on new window aspect ratio
	projection = Perspective(fovy, aspect, zNear, zFar);
}

/* key_up: called on any key press
 * @param key - the char value of the unpressed key
 * @param x - the window X coordinte of mouse when buttton was unpressed
 * @param y - the window Y coordinate of mosue when button was unpressed */
void key_up(unsigned char key, int x, int y)
{
	//Movement
	if (key == 'w')//move forward
		isWPressed = false;
	else if (key == 's')//move backward
		isSPressed = false;
	else if (key == 'a')//move left
		isAPressed = false;
	else if (key == 'd')//move right
		isDPressed = false;
}

/* key: called on any key press
 * @param key - the char value of the pressed key
 * @param x - the window X coordinte of mouse when buttton was pressed
 * @param y - the window Y coordinate of mosue when button was pressed */
void key(unsigned char key, int x, int y)
{
	//Movement
	if (key == 'w')//move forward
		isWPressed = true;
	else if (key == 's')//move backward
		isSPressed = true;
	else if (key == 'a')//move left
		isAPressed = true;
	else if (key == 'd')//move right
		isDPressed = true;

	//Special
	else if (key == 'q') //quit program
		exit(0);
	else if (key == 27) //uncenter/center mouse (esc key)
		mouseCenteredFlag = !mouseCenteredFlag;
	else //dont check new position if no valid button was pressed
		return;
}

//movePlayer: moves player based on the key that is pressed
void movePlayer()
{
	//Hold new movement proposition world locations
	point4 newAt;
	point4 newEye;
	bool keyPressed = false;

	//Find proposed player location
	if (isWPressed)//move forward
	{
		newEye = eye + 0.25*forwardVec;
		newAt = at + 0.25*forwardVec;
		keyPressed = true;
	}
	if (isSPressed)//move backward
	{
		newEye = eye - 0.25*forwardVec;
		newAt = at - 0.25*forwardVec;
		keyPressed = true;
	}
	if (isAPressed)//move left
	{
		newEye = eye + 0.25*leftVec;
		newAt = at + 0.25*leftVec;
		keyPressed = true;
	}
	if (isDPressed)//move right
	{
		newEye = eye - 0.25*leftVec;
		newAt = at - 0.25*leftVec;
		keyPressed = true;
	}

	//Move player if move is allowed
	if (keyPressed && checkForCollisions(newAt, newEye))
	{
		at = newAt;
		eye = newEye;
		glutPostRedisplay();
	}
}


///[MAZE GENERATION]
/* wallRemovable: tells wether it is allowable to remove a certain block
 * @param x - the x index of the wall to check
 * @param y - the y index of the wall to check
 * @param prevX - empty wall X index that is the closest connection point to the checked wall
 * @param prevY - empty wall Y index that is the closest connection point to the checked wall
 * @return - TRUE if it is allowable to remove the block, FALSE if not allowble to remove block (because removing would allow for second path/exit)*/
bool wallRemovable(int x, int y, int prevX, int prevY)
{
	//Check if given index is outside of maze
	if (y <= 0 || x >= (wallPresent.size() - 1) || y >= (wallPresent[x].size() - 1) || x <= 0)
		return false;

	//Check if given path point was already used (empty)
	if (!wallPresent[x][y])
		return false;

	//Check if the selected wall blocks another path and is not the previous path point
	if (!(((y + 1) == prevY) && (x == prevX)) && !wallPresent[x][y + 1]) //check top
		return false;
	else if (!(((y - 1) == prevY) && (x == prevX)) && !wallPresent[x][y - 1]) //check bottom
		return false;
	else if (!((y == prevY) && ((x + 1) == prevX)) && !wallPresent[x + 1][y]) //check right
		return false;
	else if (!((y == prevY) && ((x - 1) == prevX)) && !wallPresent[x - 1][y]) //check left
		return false;

	//If block is not the edge of existing path or the maze itself then it ok to remove the block
	return true;
}

//PathPoint: struct used to represent a single next step wall selection
struct PathPoint
{
	//Location of the wall in the storage 2D vector
	int x;
	int y;
	//Storage of avaliable directions to which to branch out
	vector<pair<int, int>> openConnection;

	//PathPoint: main constructor
	// @param x - the x index of the wall of this path point
	// @param y - the y index of the wall of this path point
	PathPoint(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	//updateOpenConnections: checks each of the four directions to check avaliable path directions
	void updateOpenConnections()
	{
		//Clear current list of current options
		openConnection.clear();

		//Add all blocks that are ok to remove
		if (wallRemovable(x, y + 1, x, y)) //check top
			openConnection.push_back(make_pair(x, y + 1));
		if (wallRemovable(x, y - 1, x, y)) //check bottom
			openConnection.push_back(make_pair(x, y - 1));
		if (wallRemovable(x + 1, y, x, y)) //check right 
			openConnection.push_back(make_pair(x + 1, y));
		if (wallRemovable(x - 1, y, x, y)) //check left 
			openConnection.push_back(make_pair(x - 1, y));
	}
};

/* generateMaze: called to populate the wall data structures and create a random maze formation on screen
 * @param mazeSize - the maze length and width in amount of cube walls
 * @param seed - seed for random number generator, DEFAULT uses current time as seed */
void generateMaze(int mazeSize, int seed = time(NULL))
{
	//Setup the random number generator
	srand(seed);

	//Setup used data structures
	// - walls present everywhere at start
	for (int i = 0; i < mazeSize; i++)
	{
		wallPresent[i] = vector<bool>(mazeSize);
		for(int g = 0; g < mazeSize; g++)
			wallPresent[i][g] = true;
	}
	// - stack that represent paths of points during generation
	stack<PathPoint*> curPath;

	// Pick an entrance
	int startCellY = (rand() % (mazeSize-2)) + 1;
	wallPresent[0][startCellY] = false;

	//Add Start point to path stack
	PathPoint * start = new PathPoint(0, startCellY);
	curPath.push(start);

	//Until there are no more options to form paths exist find next path point
	while (!curPath.empty())
	{
		//Update current path points options (directions to go in)
		(*curPath.top()).updateOpenConnections();

		//Decide on next path point if one is avaliable
		if ((*curPath.top()).openConnection.size() != 0) // Next path point exists select that as next point
		{
			//Chose random direction
			int dir = rand() % (*curPath.top()).openConnection.size();

			//Create a path point and destroy wall at that point
			PathPoint * nextPoint = new PathPoint((*curPath.top()).openConnection[dir].first, (*curPath.top()).openConnection[dir].second);
			wallPresent[(*nextPoint).x][(*nextPoint).y] = false;

			//Add point to current path stack
			curPath.push(nextPoint);
		}
		else											// No alloweble direction to go to
		{
			//Pop back a single path point from stack
			delete curPath.top();
			curPath.pop();
		}
	}

	//Pick an exit location
	// - find walls that if broken will form an exit on the right side of maze
	vector<int> pathPoints;
	for (int o = 1; o < wallPresent[wallPresent.size() - 2].size()-2; o++)
	{
		if (!wallPresent[wallPresent.size() - 2][o])
		{
			pathPoints.push_back(o);
		}
	}
	// - pick and destroy a wall for the chosen exit
	if (pathPoints.size() >= 1)
	{
		int exitPick = rand() % pathPoints.size();
		wallPresent[wallPresent.size() - 1][pathPoints[exitPick]] = false;
	}

	//Clear space in the middle to give room to admire the giant center tree
	if (wallPresent.size() > 7)
	{
		//Find how many blocks to clear to get everything symmetrical
		int removeAmount = wallPresent.size() % 2 ? 6 : 5;
		int start = ((wallPresent.size() / 2) - 1) - (removeAmount/2);
		//Destroy walls
		for (int i = start; i < start + removeAmount; i++)
		{
			for (int d = start; d < start + removeAmount; d++)
			{
				wallPresent[i][d] = false;
			}
		}
	}
	
	//Generate the blocks for the maze (store locations at which to display wall objects)
	for (int y = 0; y < wallPresent.size(); y++)
	{
		for (int x = 0; x < wallPresent[y].size(); x++)
		{
			if (wallPresent[y][x])
			{
				int offset = (wallPresent.size() * 4) / 2;
				wallPositions.push_back(vec3(-offset + x*4, -249, -offset + y*4));
			}
		}
	}
}


///[SETUP/QUIT]
//init: called to create the map and properly position player
void init()
{   
	//Glut setup
	glEnable(GL_DEPTH_TEST);


	//Object template setup
	skybox.init_data();	        // Setup the data for the skybox object
	skybox.init_VAO();          // Initialize the vertex array object for this object
	skybox.init_VBO();			// Initialize the data buffers for this object
	skybox.init_shader();		// Initialize the shader objects and textures for skybox
	skybox.init_texture_map();	// Initialize the texture map for this object
	
	wall.init_data();	        // Setup the data for the wall object
	wall.init_VAO();            // Initialize the vertex array object for this object
	wall.init_VBO();			// Initialize the data buffers for this object
	wall.init_shader();		    // Initialize the shader objects and textures for wall
	wall.init_texture_map();	// Initialize the texture map for this object
	
	tree.init_data();			// Setup the data for the tree object
	tree.init_VAO();			// Initialize the vertex array object for this object
	tree.init_VBO();			// Initialize the data buffers for this object
	tree.init_shader();			// Initialize the shader objects and textures for tree
	tree.init_texture_map();	// Initialize the texture map for this object


	//Map setup
	// - generate the maze (with preset seed - in this case 234)
	//generateMaze(mazeSize, 234);

	// - generate the maze (randomly - seeded based on current time)
	generateMaze(mazeSize);

	// - add the single center tree collider 
	treePositions.push_back(vec3(0, -248, 0));


	//Player setup
	// - rotate player so (s)he sees the maze at start
	forwardVec = RotateY(15) * forwardVec;
    at = eye + forwardVec;

	// - set cursor to crosshair
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);


	GL_CHECK_ERRORS
}

//OnShutdown: called when user quits application in any way
void OnShutdown()
{
	skybox.cleanup(); // release the textures on the graphics card
}

//checkGlew: checks if glew can be started on current hardware + displays version info 
void checkGlew()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)	{
		cerr<<"Error: " << glewGetErrorString(err)<<endl;
	} else {
		if (GLEW_VERSION_3_3)
		{
			cout<<"Driver supports OpenGL 3.3\nDetails:"<<endl;
		}
	}
	cout << "Using GLEW "<<glewGetString(GLEW_VERSION)<<endl;
	cout << "Vendor: "<<glGetString (GL_VENDOR)<<endl;
	cout << "Renderer: "<<glGetString (GL_RENDERER)<<endl;
	cout << "Version: "<<glGetString (GL_VERSION)<<endl;
	cout << "GLSL: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<endl;
}

//main: the entry point for the program
int main(int argc, char **argv)
{
	//Designate shutdown function at start
	atexit(OnShutdown);

	//Setup GLUT window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);	//Mode
	glutInitWindowSize(1280, 720);								//Size
	glutCreateWindow("Midterm Project - Part C");				//Title	

	//Perform graphics setup
	checkGlew();
    init();

	//Attach callbacks to various GLUT events
    glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(mouse_move);   // Called when mouse moves no button press
	glutMotionFunc(mouse_move);			 // Called when mouse moves and a button pressed
    glutKeyboardFunc(key);
	glutKeyboardUpFunc(key_up);

	//Display controls information to the console window
	cout << "*****************************************************" << endl;
	cout << "*   w moves forward" << endl;
	cout << "*   s moves backward" << endl;
	cout << "*   a moves left" << endl;
	cout << "*   d moves right" << endl;
	cout << "*   esc centers/uncenters the mouse and player" << endl;
	cout << "*   mouse rotates the camera" << endl;
	cout << "*****************************************************" << endl;

	//Enter the main glut loop (calls display and others until application exit)
    glutMainLoop();

    return 0;
}
