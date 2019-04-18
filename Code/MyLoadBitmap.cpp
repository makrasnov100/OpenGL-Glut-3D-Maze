#include <iostream>
#include <cmath>
#include <iomanip>
#include <cassert>
#include <vector>
#include <cstdio>
using namespace std;

#include <Windows.h>
#include "Angel.h"
#include <GL/glew.h> // for OpenGL extensions
#include <GL/glut.h> // for Glut utility kit

#ifndef GL_CHECK_ERRORS
#define GL_CHECK_ERRORS \
{ \
	int err=glGetError(); \
	if (err!=0) \
	{   cout << "OpenGL Error: " << err << endl; \
		assert(err == GL_NO_ERROR); \
	} \
}
#endif

extern int texture_num;

/**********************************************************
 *
 * FUNCTION MyLoadBitmap(char *)
 *
 * This function loads a bitmap file and return the OpenGL reference ID to use that texture
 *
 *********************************************************/

int MyLoadBitmap(char *filename, GLenum  target, bool AlphaBlend = false ) 
{
    int i, j=0; //Index variables
    FILE *l_file; //File pointer
    unsigned char *l_texture; //The pointer to the memory zone in which we will load the texture
     
    // windows.h gives us these types to work with the Bitmap files
    BITMAPFILEHEADER fileheader; 
    BITMAPINFOHEADER infoheader;
    RGBTRIPLE rgb;
	
	texture_num++; // The counter of the current texture is increased
    
	if( (l_file = fopen(filename, "rb"))==NULL) return (-1); // Open the file for reading
    
    fread(&fileheader, sizeof(fileheader), 1, l_file); // Read the fileheader
    
    fseek(l_file, sizeof(fileheader), SEEK_SET); // Jump the fileheader
    fread(&infoheader, sizeof(infoheader), 1, l_file); // and read the infoheader

    // Now we need to allocate the memory for our image (width * height * color deep)
    l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
    // And fill it with zeros
    memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);
 
    // At this point we can read every pixel of the image
    for (i=0; i < infoheader.biWidth*infoheader.biHeight; i++)
    {            
            // We load an RGB value from the file
            fread(&rgb, sizeof(rgb), 1, l_file); 

            // And store it
            l_texture[j+0] = rgb.rgbtRed; // Red component
            l_texture[j+1] = rgb.rgbtGreen; // Green component
            l_texture[j+2] = rgb.rgbtBlue; // Blue component

			if ( AlphaBlend && rgb.rgbtRed == 0 && rgb.rgbtGreen == 0 && rgb.rgbtBlue == 0 )
				            l_texture[j+3] = 0; // Alpha value transparent
			else
				            l_texture[j+3] = 255; // Alpha value opaque
            j += 4; // Go to the next position
    }

    fclose(l_file); // Closes the file stream
     
	GL_CHECK_ERRORS

    //glBindTexture(target, num_texture); // Bind the ID texture specified by the 2nd parameter

	GL_CHECK_ERRORS

    // The next commands sets the texture parameters
    glTexParameterf(target, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	GL_CHECK_ERRORS

	glTexImage2D( target , 0, GL_RGBA, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

	GL_CHECK_ERRORS

	glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 4);

    // And create 2d mipmaps for the minifying function
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
	glGenerateMipmap(target);

	GL_CHECK_ERRORS

    free(l_texture); // Free the memory we used to load the texture

    return (texture_num); // Returns the current texture OpenGL ID
}