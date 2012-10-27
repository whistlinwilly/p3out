/**
 * @file project.cpp
 * @brief GLSL project
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "glsl/project.hpp"
#include <fstream>

// A namespace declaration. All proejct files use this namespace.
// Add this declration (and its closing) to all source/headers you create.
// Note that all #includes should be BEFORE the namespace declaration.
namespace _462 {

// shader loading code

/**
 * Load a file as either a vertex shader or a fragment shader, and attach
 * it to a program.
 * @param file The filename to load
 * @param type Either GL_VERTEX_SHADER_ARB, or GL_FRAGMENT_SHADER_ARB
 * @param program The shader program to which to attach the loaded shader.
 * @return True on success.
 */
static bool load_shader( const char* file, GLint type, GLhandleARB program )
{
    std::ifstream infile;
    char* buffer;
    char error_msg[2048];
    GLhandleARB shader;

    infile.open( file );

    if( infile.fail() ) {
        std::cout << "ERROR: cannot open file: " << file << std::endl;
        infile.close();
        return false;
    }

    // calculate length
    infile.seekg( 0, std::ios::end );
    int length = infile.tellg();
    infile.seekg( 0, std::ios::beg );
    // allocate space for entire program
    buffer = (char *) malloc( (length + 1) * sizeof *buffer );
    if ( !buffer )
        return false;
    // copy entire program into memory
    infile.getline( buffer, length, '\0' );
    infile.close();

    // create shader object
    shader = glCreateShaderObjectARB( type );
    // link shader source
    const char* src = buffer; // workaround for const correctness
    glShaderSourceARB( shader, 1, &src, NULL );
    // compile shaders
    glCompileShaderARB( shader );
    // check success
    GLint result;
    glGetObjectParameterivARB( shader, GL_OBJECT_COMPILE_STATUS_ARB, &result );
    if ( result != GL_TRUE ) {
        glGetInfoLogARB( shader, sizeof error_msg, NULL, error_msg );
        std::cout << "GLSL COMPILE ERROR in " << file << ": " << error_msg << std::endl;
        return false;
    } else {
        std::cout << "Compiled shaders successfully" << std::endl;
    }

    // attach the shader object to the program object
    glAttachObjectARB( program, shader );

    free( buffer );
    return true;
}

/**
 * Loads a vertex and fragment shader from the given files and attaches them to the given
 * shader program object.
 * @param vert_file The filename of the vetex shader.
 * @param frag_file The filename of the fragment shader.
 * @return True on success.
 */
static bool create_shader( GLhandleARB program, const char* vert_file, const char* frag_file )
{
    bool rv = true;

    std::cout
        << "Loading vertex shader " << vert_file
        << "and fragment shader " << frag_file << std::endl;

    // Load vertex shader
    rv = rv && load_shader( vert_file, GL_VERTEX_SHADER_ARB, program );
    // Load fragment shader
    rv = rv && load_shader( frag_file, GL_FRAGMENT_SHADER_ARB, program );

    if ( !rv )
        return false;

    // link
    glLinkProgramARB( program );

    // check for success
    GLint result;
    glGetProgramiv( program, GL_LINK_STATUS, &result );
    if ( result == GL_TRUE ) {
        std::cout << "Successfully linked shader" << std::endl;
        return true;
    } else {
        std::cout << "FAILED to link shader" << std::endl;
        return false;
    }
}


// definitions of functions for the GlslProject class

// constructor, invoked when object is created
GlslProject::GlslProject()
{
    // TODO any basic construction or initialization of members
    // Warning: Although members' constructors are automatically called,
    // ints, floats, pointers, and classes with empty contructors all
    // will have uninitialized data!
}

// destructor, invoked when object is destroyed
GlslProject::~GlslProject()
{
    // Warning: Do not throw exceptions or call virtual functions from deconstructors!
    // They will cause undefined behavior (probably a crash, but perhaps worse).
}

/**
 * Initialize the project, loading the mesh from the given filename.
 * Also do any necessary opengl initialization.
 * @param renderer Object that renders the scene.
 * @param width The width in pixels of the framebuffer.
 * @param height The height in pixels of the framebuffer.
 * @return true on success, false on error.
 * @see scene/mesh.hpp
 */
bool GlslProject::initialize( const SceneRenderer* renderer, int width, int height )
{
    bool rv = true;

	this->camera = renderer->get_camera();

    // copy renderer for later use
    this->renderer = renderer;

	this->height = height;
	this->width = width;

	numFrames = 0;

    // create a test shader
    normalProgram  = glCreateProgramObjectARB();
   rv = rv && create_shader( normalProgram, "shaders/vert_normal.glsl", "shaders/frag_normal.glsl" );

	blurProgram = glCreateProgramObjectARB();
	rv = rv && create_shader( blurProgram, "shaders/vert_blur.glsl", "shaders/frag_blur.glsl");
 
	nothingProgram = glCreateProgramObjectARB();
	rv = rv && create_shader( nothingProgram, "shaders/vert_nothing.glsl", "shaders/frag_nothing.glsl");

	depthDataBuffer = (unsigned char*) malloc (this->width * this->height * 4);
	colorDataBuffer = (unsigned char* ) malloc (this->width * this->height * 4);
	normalDataBuffer = (unsigned char* ) malloc (this->width * this->height * 4);
	blurDataBuffer = (unsigned char* ) malloc (this->width * this->height * 4);

glActiveTexture(GL_TEXTURE0);
glGenTextures(1, &color);
glBindTexture(GL_TEXTURE_RECTANGLE_ARB, color);
glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorDataBuffer);
int color_location = glGetUniformLocationARB( nothingProgram, "color" );
glUniform1iARB(color_location, GL_TEXTURE0);

glActiveTexture(GL_TEXTURE1);
glGenTextures(1, &depth);
glBindTexture(GL_TEXTURE_RECTANGLE_ARB, depth);
glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthDataBuffer);
int depth_location = glGetUniformLocationARB( nothingProgram, "depth" );
glUniform1iARB(depth_location, GL_TEXTURE1);

glActiveTexture(GL_TEXTURE2);
glGenTextures(1, &normal);
glBindTexture(GL_TEXTURE_RECTANGLE_ARB, normal);
glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalDataBuffer);
int texture_location = glGetUniformLocationARB( nothingProgram, "normal" );
glUniform1iARB(texture_location, GL_TEXTURE2);

glActiveTexture(GL_TEXTURE3);
glGenTextures(1, &blur);
glBindTexture(GL_TEXTURE_RECTANGLE_ARB, blur);
glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, blurDataBuffer);
int blur_location = glGetUniformLocationARB( blurProgram, "blur" );
glUniform1iARB(blur_location, GL_TEXTURE3);
int frame_location = glGetUniformLocationARB( blurProgram, "frames" );
glUniform1fARB(frame_location, numFrames);





    return rv;
}

/**
 * Clean up the project. Free any memory, etc.
 */
void GlslProject::destroy()
{
    //glDeleteObjectARB( program );

    // TODO any cleanup code
}

/**
 * Render the scene with outlines using shaders.
 */
void GlslProject::render()
{

	int nearVar;
	int farVar;
    // TODO render code

    // example: render the scene with the test shader active
    // This doesn't actually do anything useful other than provide
    // some sample code for using shaders.

	glActiveTexture(GL_TEXTURE0);

    renderer->render_scene();

	glActiveTexture(GL_TEXTURE0);
	glReadPixels(0,0, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, colorDataBuffer);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorDataBuffer);

	glActiveTexture(GL_TEXTURE1);
	glReadPixels(0,0, this->width, this->height, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, depthDataBuffer);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthDataBuffer);

	glUseProgramObjectARB( normalProgram );
	renderer->render_scene();

	glActiveTexture(GL_TEXTURE2);
	glReadPixels(0,0, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, normalDataBuffer);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalDataBuffer);


/*		int cz_location = glGetUniformLocationARB( normalProgram, "cz" );
	glUniform1iARB(cz_location, cz);
	


	glUseProgramObjectARB( normalProgram );
	renderer->render_scene();

	glActiveTexture(GL_TEXTURE2);
	glReadPixels(0,0, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, colorDataBuffer);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, color);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorDataBuffer);

	int color_location = glGetUniformLocationARB( edgeProgram, "color" );
	glUniform1iARB(color_location, GL_TEXTURE2);

*/
//	renderer->render_scene();



/*
	glActiveTexture(GL_TEXTURE1);
	glReadPixels(0,0, this->width, this->height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthDataBuffer);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, depth);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthDataBuffer);

										

	glUseProgramObjectARB( program );
//	glActiveTexture(GL_TEXTURE0);
//    int texture_location = glGetUniformLocationARB( program, "texture" );
//    glUniform1iARB(texture_location, 0);
//	glActiveTexture(GL_TEXTURE1);
//    int depth_location = glGetUniformLocationARB( program, "nonamehere" );
//    glUniform1iARB(depth_location, 0);
//	nearVar = this->camera->get_near_clip();
//	farVar = this->camera->get_far_clip();

//	int near_location = glGetUniformLocationARB( program, "nearVar" );
 //   glUniform1iARB(near_location, 0);

//	int far_location = glGetUniformLocationARB( program, "farVar" );
//    glUniform1iARB(far_location, 0);

*/
	glActiveTexture(GL_TEXTURE0);
	glUseProgramObjectARB(nothingProgram);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	glBegin( GL_QUADS );
	glTexCoord2f( 0.0f, 0.0f );
	glVertex3f( -1.0f, -1.0f, -1.0f );
	glTexCoord2f( width, 0.0f );
	glVertex3f( 1.0f, -1.0f, -1.0f );
	glTexCoord2f( width, height );
	glVertex3f( 1.0f, 1.0f, -1.0f );
	glTexCoord2f( 0.0f, height );
	glVertex3f( -1.0f, 1.0f, -1.0f );
	glEnd();
/*
	if(numFrames % 2 == 0){
	glActiveTexture(GL_TEXTURE0);
	glReadPixels(0,0, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, colorDataBuffer);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorDataBuffer);
	}
	else{
	glActiveTexture(GL_TEXTURE1);
	glReadPixels(0,0, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, blurDataBuffer);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, blurDataBuffer);
	}

	numFrames++;
	glActiveTexture(GL_TEXTURE0);

	glUseProgramObjectARB( blurProgram );
	glUniform1f(frame_location, numFrames);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	glBegin( GL_QUADS );
	glTexCoord2f( 0.0f, 0.0f );
	glVertex3f( -1.0f, -1.0f, -1.0f );
	glTexCoord2f( width, 0.0f );
	glVertex3f( 1.0f, -1.0f, -1.0f );
	glTexCoord2f( width, height );
	glVertex3f( 1.0f, 1.0f, -1.0f );
	glTexCoord2f( 0.0f, height );
	glVertex3f( -1.0f, 1.0f, -1.0f );
	glEnd();
*/
    glUseProgramObjectARB( 0 );


}

} /* _462 */

