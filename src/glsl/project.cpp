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
    fboProgram  = glCreateProgramObjectARB();
   rv = rv && create_shader( fboProgram, "shaders/vert_fbo.glsl", "shaders/frag_fbo.glsl" );

	blurProgram = glCreateProgramObjectARB();
	rv = rv && create_shader( blurProgram, "shaders/vert_blur.glsl", "shaders/frag_blur.glsl");
 
	nothingProgram = glCreateProgramObjectARB();
	rv = rv && create_shader( nothingProgram, "shaders/vert_nothing.glsl", "shaders/frag_nothing.glsl");

	depthDataBuffer = (unsigned char*) malloc (this->width * this->height * 4);
	colorDataBuffer = (unsigned char* ) malloc (this->width * this->height * 4);
	normalDataBuffer = (unsigned char* ) malloc (this->width * this->height * 4);
	blurDataBuffer = (unsigned char* ) malloc (this->width * this->height * 4);

glActiveTexture(GL_TEXTURE1);
glGenTextures(1, &color);
glBindTexture(GL_TEXTURE_RECTANGLE_ARB, color);
glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

glActiveTexture(GL_TEXTURE2);
glGenTextures(1, &depth);
glBindTexture(GL_TEXTURE_RECTANGLE_ARB, depth);
glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

glGenFramebuffers(1, &fbo);
glBindFramebuffer(GL_FRAMEBUFFER, fbo);

glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_RECTANGLE_ARB, color, 0);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_RECTANGLE_ARB, depth, 0);

//Go back to regular frame buffer rendering
glBindFramebuffer(GL_FRAMEBUFFER, 0);
glActiveTexture(GL_TEXTURE0);


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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	renderer->render_scene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glUseProgramObjectARB( fboProgram );
	int color_location = glGetUniformLocationARB( fboProgram, "color" );
	glUniform1iARB(color_location, GL_TEXTURE1);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	glMatrixMode( GL_PROJECTION );
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
	glUseProgram( 0 );
}

} /* _462 */

