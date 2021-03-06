////////////////////////////////////////////////////////////////////////////////
#include "core.h"
////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////
Keyboard	keyboard;
Mouse		mouse;
Screen		screen;
////////////////////////////////////////////////////////////////////////////////
// includes, system
#include "glsl.h"
////////////////////////////////////////////////////////////////////////////////
static void get_error()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) 
	{
		printf("GL Error: %s\n",gluErrorString(err));
		printf("Programm Stopped!\n");
		while(1)Sleep(1000);;
	}
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#pragma warning(disable:4996)
#endif
////////////////////////////////////////////////////////////////////////////////
// includes, GL
// includes, GL
#include <GL/glew.h>
#include <GL/wglew.h>
#ifdef UNIX
    #if defined(__APPLE__) || defined(MACOSX)
       #include <OpenGL/OpenGL.h>
       #include <GLUT/glut.h>
    #else
       #include <GL/freeglut.h>
       #include <GL/glx.h>
    #endif
#else
    #include <GL/freeglut.h>
#endif
enum TexLoadFlags {
  TEX_ADDALPHA=1,
  TEX_NORMALMAP=2,
  TEX_HORIZONMAP=4,
  TEX_HORIZONLOOKUP=8,
  TEX_NORMALIZE=16,
  TEX_16BIT=32,
};

bool core_fullscreen=false;

////////////////////////////////////////////////////////////////////////////////
void Core_KeyPressed(int key, int x, int y,bool pressed) 
{
    if(key==27)  exit(0);
    keyboard.key[ key&255 ] = pressed;
}
void Core_keyDown1Static(int key, int x, int y)           { Core_KeyPressed(key, x, y,true); }
void Core_keyDown2Static(unsigned char key, int x, int y) { Core_KeyPressed(key, x, y,true); }
void Core_keyUp1Static(int key, int x, int y)             { Core_KeyPressed(key, x, y,false); }
void Core_keyUp2Static(unsigned char key, int x, int y)   { Core_KeyPressed(key, x, y,false); }
////////////////////////////////////////////////////////////////////////////////
void Core_MouseMotionStatic (int x,int y)
{
	mouse.mouseX = float(x)/float(screen.window_width);
	mouse.mouseY = float(y)/float(screen.window_height);
}
////////////////////////////////////////////////////////////////////////////////
//! Display callback
////////////////////////////////////////////////////////////////////////////////
void Core_reshape_static(int w, int h)
{
    screen.window_width = w;
    screen.window_height = h;
}
////////////////////////////////////////////////////////////////////////////////
//! Display callback
////////////////////////////////////////////////////////////////////////////////
void Core_idle_static()
{
	wglSwapIntervalEXT(0);
    glutPostRedisplay();
}
////////////////////////////////////////////////////////////////////////////////
void Core_MouseButtonStatic(int button_index, int state, int x, int y)
{
	mouse.button[button_index] =  ( state == GLUT_DOWN ) ? true : false;
}
////////////////////////////////////////////////////////////////////////////////
void Core_ToggleFullscreen()
{
	static int win_width=screen.window_width;
	static int win_height=screen.window_height;

	if(core_fullscreen)
	{
		glutReshapeWindow(win_width,win_height);

		screen.window_width = win_width;
		screen.window_height = win_height;
	}
	else
	{
		win_width=screen.window_width;
		win_height=screen.window_height;

		glutFullScreen() ;
	}
	core_fullscreen = (core_fullscreen) ? false : true;
}

////////////////////////////////////////////////////////////////////////////////
void Core_Init(int window_width, int window_height, bool fullscreen,void (*display_func)(void))
{  
	screen.window_width = window_width;
	screen.window_height = window_height;

	// Create GL context

	char* headline="CUDA Voxel Demo";int nop=1;
	glutInit( &nop, &headline );
	glutInitDisplayMode( GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize( window_width, window_height);
	glutCreateWindow( headline );

	core_fullscreen = fullscreen;

	if(fullscreen)
	{
		glutFullScreen() ;
	}

	glewInit();

	// default initialization
	glClearColor( 0.5, 0.5, 0.5, 1.0);
	glDisable( GL_DEPTH_TEST);

	// viewport
	glViewport( 0, 0, window_width, window_height);

	// projection
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)window_width / (GLfloat) window_height, 0.1, 10.0);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_LIGHT0);
	float red[] = { 1.0, 0.1, 0.1, 1.0 };
	float white[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);

	// register callbacks
	glutDisplayFunc( display_func);
	glutReshapeFunc( &Core_reshape_static);
	glutIdleFunc( &Core_idle_static );

	glutSpecialFunc(&Core_keyDown1Static);
	glutSpecialUpFunc(&Core_keyUp1Static);
	glutKeyboardFunc(&Core_keyDown2Static);
	glutKeyboardUpFunc(&Core_keyUp2Static);
	glutMotionFunc(&Core_MouseMotionStatic);
	glutPassiveMotionFunc(&Core_MouseMotionStatic);
	glutMouseFunc (&Core_MouseButtonStatic);
	get_error();
}
////////////////////////////////////////////////////////////////////////////////
void Core_deleteTexture( GLuint* tex) {

    glDeleteTextures( 1, tex);
    get_error();

    *tex = 0;
}
////////////////////////////////////////////////////////////////////////////////
void Core_createTexture( uint* tex_name, unsigned int size_x, unsigned int size_y,int bpp) {

    // create a tex as attachment
    glGenTextures( 1, tex_name);
    glBindTexture( GL_TEXTURE_2D, *tex_name);

    // set basic parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR);//NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR);//GL_NEAREST);

    // buffer data
	if(bpp==16)		
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, NULL);
	else
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    get_error();
}
////////////////////////////////////////////////////////////////////////////////
void Core_createFloatTexture( GLuint* tex_name, unsigned int size_x, unsigned int size_y) {

    // create a tex as attachment
    glGenTextures( 1, tex_name);
    glBindTexture( GL_TEXTURE_2D, *tex_name);

    // set basic parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR);//NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_LINEAR);//GL_NEAREST);

    // buffer data
    glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, size_x, size_y, 0, GL_LUMINANCE, GL_FLOAT, NULL);

    get_error();
}

////////////////////////////////////////////////////////////////////////////////
//! Create PBO
////////////////////////////////////////////////////////////////////////////////
void Core_createPBO( uint* pbo,int image_width , int image_height,int bpp) {

    // set up vertex data parameter
    void *data = malloc(image_width * image_height * (bpp/8));

    // create buffer object
    glGenBuffers( 1, pbo);
    glBindBuffer( GL_ARRAY_BUFFER, *pbo);

    // buffer data
    glBufferData( GL_ARRAY_BUFFER, image_width * image_height * (bpp/8), data, GL_DYNAMIC_COPY);
    free(data);

    glBindBuffer( GL_ARRAY_BUFFER, 0);

    get_error();
}

////////////////////////////////////////////////////////////////////////////////
//! Delete PBO
////////////////////////////////////////////////////////////////////////////////
void Core_deletePBO( GLuint* pbo) {

    glBindBuffer( GL_ARRAY_BUFFER, *pbo);
    glDeleteBuffers( 1, pbo);

	get_error();

    *pbo = 0;
}
////////////////////////////////////////////////////////////////////////////////
int Core_LoadTexBmp(Bmp &bmp,int flags)
{
  get_error();
	int gl_handle;
	glGenTextures(1,(GLuint*)(&gl_handle));

	glBindTexture  (GL_TEXTURE_2D, gl_handle);
	glPixelStorei  (GL_UNPACK_ALIGNMENT, 4);

	//if(flags==0)						bmp.addalpha(255,0,255);
	if((flags & TEX_NORMALIZE)!=0)		bmp.normalize();
	if((flags & TEX_NORMALMAP)!=0)		bmp.normalMap();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );// _MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );// _MIPMAP_LINEAR);
	glTexEnvf      (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,   GL_MODULATE);
  get_error();

	int format1 = (bmp.bpp==32)	? GL_RGBA : GL_RGB;
	int format2 = (bmp.bpp==32)	? GL_BGRA_EXT : GL_BGR_EXT;

	glTexImage2D (
		GL_TEXTURE_2D,
		0,
		format1,
		bmp.width, 
		bmp.height, 
		0, 
		format2 , 
		GL_UNSIGNED_BYTE, 
		bmp.data);
/*
	glTexImage2D (
		GLenum target, 
		GLint level, 
		GLint internalformat, 
		GLsizei width, 
		GLsizei height, 
		GLint border, 
		GLenum format, 
		GLenum type, 
		const GLvoid *pixels);
*/
	//if(bmp.bpp==32)	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA, bmp.width, bmp.height,/*GL_RGBA*/GL_BGRA_EXT, GL_UNSIGNED_BYTE, bmp.data );
	//else			gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB, bmp.width, bmp.height,/*GL_RGB*/ GL_BGR_EXT , GL_UNSIGNED_BYTE, bmp.data );

  get_error();

  return gl_handle;
}
////////////////////////////////////////////////////////////////////////////////
int Core_LoadTex(const char *name,int flags)
{
	Bmp bmp;
	if(!bmp.load(name))
		return -1;

	return Core_LoadTexBmp(bmp,flags);
}
////////////////////////////////////////////////////////////////////////////////
void Core_UpdateTexBmp(int handle, Bmp &bmp,int flags)
{
	glBindTexture ( GL_TEXTURE_2D , handle );

	int format1 = (bmp.bpp==32)	? GL_RGBA : GL_RGB;
	int format2 = (bmp.bpp==32)	? GL_BGRA_EXT : GL_BGR_EXT;

	glTexSubImage2D (
		GL_TEXTURE_2D,//GLenum target, 
		0,//GLint level, 
		0,//GLint xoffset, 
		0,//GLint yoffset, 
		bmp.width,//GLsizei width, 
		bmp.height,//GLsizei height, 
		format2,//GLenum format, 
		GL_UNSIGNED_BYTE,//GLenum type, 
		bmp.data);//const GLvoid *pixels

	get_error();
}
////////////////////////////////////////////////////////////////////////////////