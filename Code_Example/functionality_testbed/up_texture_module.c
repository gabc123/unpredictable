#include "up_texture_module.h"
#include "up_sdl_redirect.h"
#include "up_opengl_redirect.h"
#include "up_error.h"
#define UP_MAX_TEXTURE 50


struct internal_texture_handler
{
    struct up_texture_data *texture;
    int count;
    int size;
};

static struct internal_texture_handler internal_texture;

int up_texture_start_setup()
{
    // set internal_texture to start value
    internal_texture.texture = malloc(sizeof(struct up_texture_data)* UP_MAX_TEXTURE);
    if (internal_texture.texture == NULL) {
        fprintf(stderr, "malloc fail: internal_texture ");
        return 0;
    }
    internal_texture.count = 0;
    internal_texture.size = UP_MAX_TEXTURE;

    return 1; //true
}

void up_texture_shutdown_deinit()
{
    int i=0;
    for (i = 0; i < internal_texture.count; i++) {
        glDeleteTextures(1, &(internal_texture.texture[i].textureId));
    }
    internal_texture.size = 0;
    internal_texture.count = 0;
    free(internal_texture.texture);
}

struct up_imageformat
{
    unsigned char *pixelData;
    int width;
    int height;
    int totalByteSize;
    GLenum format;
};

static struct up_texture_data *up_loadImage_withAlpha(const char  * filename)
{
    char rgb_imageFile[30];
    char alphaChannelFile[30];
    strcpy(rgb_imageFile,filename);
    strcpy(alphaChannelFile,filename);

    if (internal_texture.count >= internal_texture.size) {
        UP_ERROR_MSG("Error , tried to load too many textures");
        return NULL;
    }

    struct up_texture_data *tex_data = &(internal_texture.texture[internal_texture.count]);

    strncat(rgb_imageFile,".png",5);
    strncat(alphaChannelFile,"Alpha.png",10);

    struct up_imageformat image = {0};

    SDL_Surface *tex = IMG_Load(rgb_imageFile);
    if (tex == NULL) {
        UP_ERROR_MSG_STR("failed to load texture: %s \n",rgb_imageFile);
        return NULL;
    }else
    {
        fprintf(stderr, "loaded texture: %s \n",rgb_imageFile);
    }

    int bytesPerPixel = tex->format->BytesPerPixel;

    int number_pixels = tex->w * tex->h;
    int total_size = number_pixels * bytesPerPixel; //rgba

    printf("Loading rgb part %s with bytes per pixels %d\n",rgb_imageFile,bytesPerPixel);

    image.totalByteSize = number_pixels * 4; //Always make room for the alpha channel
    image.width = tex->w;
    image.height = tex->h;

    image.pixelData = malloc(sizeof(unsigned char) * image.totalByteSize);
    if(image.pixelData == NULL)
    {
        UP_ERROR_MSG("malloc failure");
        return NULL;
    }

    // we want to load the image data into our own buffer
    // to do that we transfer it like this
    unsigned char *tmp_imageData = (unsigned char *)tex->pixels;
    int i = 0;
    int j = 0;

    for(i = 0; i < total_size; i++ )
    {
        // they both have been given the same amount off bytes per pixel
        image.pixelData[i] = tmp_imageData[i];
    }

    SDL_FreeSurface(tex);
    // TODO: load the alpha image and merge them
    int alphaChannel = 3;
#ifdef __linux
    int redChannel = 0;
#else
    int redChannel = 2;
#endif
    
    SDL_Surface *texAlpha = IMG_Load(alphaChannelFile);
    if (texAlpha == NULL) {
        fprintf(stderr, "no alpha channel: %s \n",alphaChannelFile);
        for(i = 0; i < image.totalByteSize; i =i + 4)
        {
            // fills in the alpha part of the texture to 1
            image.pixelData[i + alphaChannel] = (unsigned char)255;
        }
    }else
    {
        fprintf(stderr, "loading alpha channel: %s \n",alphaChannelFile);
        if (texAlpha->format->BytesPerPixel != bytesPerPixel) {
            UP_ERROR_MSG_INT("Bytes per pixel mismatch ",texAlpha->format->BytesPerPixel);
            SDL_FreeSurface(texAlpha);
            return NULL;
        }

        // we want to load the image data into our own buffer
        // to do that we transfer it like this
        unsigned char *tmp_alphaData = (unsigned char *)texAlpha->pixels;

        for(i = 0; i < image.totalByteSize; i =i + 4, j =j + 3 )
        {
            // they both have been given the same amount off bytes per pixel
            image.pixelData[i + alphaChannel] = tmp_alphaData[i + redChannel];
        }

        SDL_FreeSurface(texAlpha);
    }





    glGenTextures(1, &(tex_data->textureId));
    glBindTexture(GL_TEXTURE_2D, tex_data->textureId);


    // So that the texture wraps around, so if we where to try to acces
    // a tex coord outside the texture it wraps around
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // this tells opengle what too do when a texture is miniturized
    // if we for example zoom out it will use linear interpolation to determin
    // what color a pixel should be, same for magnifikation,
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#ifdef __linux
    image.format = GL_RGBA;
#else
    image.format = GL_BGRA;

#endif // __linux


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, image.format, GL_UNSIGNED_INT_8_8_8_8_REV, image.pixelData);

    //SDL_FreeSurface(tex);
    internal_texture.count++;
    free(image.pixelData);

    return tex_data;
}


static struct up_texture_data *up_load_special_error_texture(const char  * filename)
{
    // we try to load lala.png with the normal loading functions first
    struct up_texture_data *texture = up_loadImage_withAlpha(filename);
    if ( texture != NULL) {
        return texture;
    }
    
    // if we fail to load the standard error texture we first check if there is a other way to acces it
    if (internal_texture.count >= internal_texture.size) {
        UP_ERROR_MSG("Error , tried to load too many textures");
        // the program is setup in a way that texture 0 is always the error texture
        // so if the internal buffer is full we can always return texture 0
        return &(internal_texture.texture[0]);
    }
    
    // if we fail to load the standard error texture we generate a img programaticlly
    struct up_imageformat image = {0};

    texture = &(internal_texture.texture[internal_texture.count]);
    
    int bytesPerPixel = 4;
    int width = 10;
    int hight = 10;
    int number_pixels = width * hight;
    int total_size = number_pixels * bytesPerPixel; //rgba
    
    image.totalByteSize = number_pixels * bytesPerPixel; //Always make room for the alpha channel
    image.width = width;
    image.height = hight;
    // create room for the textuer
    image.pixelData = malloc(sizeof(unsigned char) * image.totalByteSize);
    if(image.pixelData == NULL)
    {
        UP_ERROR_MSG("malloc failure");
    }
    
    int i = 0;
    
    for(i = 0; i < total_size; i++ )
    {
        // we want some color in the image,
        image.pixelData[i] = (unsigned char)(100 + i%100);
    }
    
    
    int alphaChannel = 3;
    
    //set the alpha channel
    for(i = 0; i < image.totalByteSize; i =i + 4)
    {
        // fills in the alpha part of the texture to 1
        image.pixelData[i + alphaChannel] = (unsigned char)255;
    }
    
    glGenTextures(1, &(texture->textureId));
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    
    
    // So that the texture wraps around, so if we where to try to acces
    // a tex coord outside the texture it wraps around
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // this tells opengle what too do when a texture is miniturized
    // if we for example zoom out it will use linear interpolation to determin
    // what color a pixel should be, same for magnifikation,
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
#ifdef __linux
    image.format = GL_RGBA;
#else
    image.format = GL_BGRA;
    
#endif // __linux
    
    // load our programaticlly generated image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, image.format, GL_UNSIGNED_INT_8_8_8_8_REV, image.pixelData);
    
    internal_texture.count++;
    // clean up
    free(image.pixelData);
    
    return texture;
}

struct up_texture_data *up_load_texture(const char  * filename)
{
    char newName[30];
    strcpy(newName,filename);
    char *postfix = strstr(newName, ".png");
    if(postfix != NULL)
    {
        postfix[0] = '\0';
    }
    
    // the lala.png is a backup img incase off missing file
    // it is vital that it sucseed in loading it
    // this checks if this special loading thing need to be used
    if (strcmp(newName, "lala") == 0) {
        // special loading sequance, that can not fail execpt incase of malloc failure
        return up_load_special_error_texture(newName);
        
    }
    
    

    return up_loadImage_withAlpha(newName);
}

void up_texture_bind(struct up_texture_data *texture, unsigned int texUnit)
{
    //glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
}


//  Created by Waleed Hassan on 2015-05-11.

struct up_texture_data *up_cubeMapTexture_load(){

    struct up_texture_data *textureId;

    if (internal_texture.count >= internal_texture.size) {
        UP_ERROR_MSG("Error , tried to load too many textures");
        return NULL;
    }

    textureId = &(internal_texture.texture[internal_texture.count]);


    glGenTextures(1, &textureId->textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId->textureId);

    // the resson to load the errtexture regardless is to make sure that error checking is operational
    // and to re use the same texture in case of error
    int errflag = 0;
    SDL_Surface *errTex = IMG_Load("lala.png");
    if (errTex == NULL) {
        UP_ERROR_MSG("err texture failed to load, aborting sktbox");
        return NULL;
    }
    
    SDL_Surface *texF = IMG_Load("front.png");
    SDL_Surface *texT = IMG_Load("top.png");
    SDL_Surface *texG = IMG_Load("bottom.png");
    SDL_Surface *texR = IMG_Load("right.png");
    SDL_Surface *texL = IMG_Load("left.png");
    SDL_Surface *texB = IMG_Load("back.png");
    
    
    if(texF == NULL) {
        texF = errTex;
        UP_ERROR_MSG("could not load skybox images");
        errflag = 1;
    }
    if (texT == NULL){
        texT = errTex;
        UP_ERROR_MSG("could not load skybox images");
        errflag = 1;
    }
    
    if (texG == NULL){
        texG = errTex;
        UP_ERROR_MSG("could not load skybox images");
        errflag = 1;
    }
    
    if (texR == NULL){
        texR = errTex;
        UP_ERROR_MSG("could not load skybox images");
        errflag = 1;
    }
    
    if (texL == NULL){
        texL = errTex;
        UP_ERROR_MSG("could not load skybox images");
        errflag = 1;
    }
    
    if (texB == NULL){
        texB = errTex;
        UP_ERROR_MSG("could not load skybox images");
        errflag = 1;
    }
    
    if (texF->format->BytesPerPixel != 4) {

        UP_ERROR_MSG("Error: picture is in wrong pixel format");
        errflag = 1;

    }

    GLuint format_rgb = GL_RGBA;
    #ifdef __linux
    format_rgb = GL_RGBA;
    #else
    format_rgb = GL_BGRA;
    #endif // __linux

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, texF->w, texF->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texF->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, texT->w, texT->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texT->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, texG->w, texG->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texG->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, texR->w, texR->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texR->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, texL->w, texL->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texL->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, texB->w, texB->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texB->pixels);



    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    if (errflag == 0) {
        SDL_FreeSurface(texF);
        SDL_FreeSurface(texT);
        SDL_FreeSurface(texG);
        SDL_FreeSurface(texR);
        SDL_FreeSurface(texL);
        SDL_FreeSurface(texB);
    }
    
    SDL_FreeSurface(errTex);

    return textureId;
}
