
/**********************************************************
 *
 * VARIABLES DECLARATION
 *
 *********************************************************/

// Counter to keep track of the last loaded texture
extern int num_texture;



/**********************************************************
 *
 * FUNCTION MyLoadBitmap(char *)
 *
 * This function loads a CUBE bitmap file and return the OpenGL reference ID to use that texture
 *
 *********************************************************/

extern int MyLoadCubeMapBitmap(char *filename, GLenum  target );


/**********************************************************
 *
 * FUNCTION MyLoadBitmap(char *)
 *
 * This function loads a regular bitmap file for alphablending and return the OpenGL reference ID to use that texture
 *
 *********************************************************/


extern int MyLoadBitmap(char *filename, GLenum  target, bool AlphaBlend ) ;
