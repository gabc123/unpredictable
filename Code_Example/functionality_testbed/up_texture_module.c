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


struct up_texture_data *up_load_texture(const char  * filename)
{
    if (internal_texture.count >= internal_texture.size) {
        UP_ERROR_MSG("Error , tried to load too many textures");
        return NULL;
    }

    SDL_Surface *tex = IMG_Load(filename);
    if (tex == NULL) {
        fprintf(stderr, "failed to load texture: %s \n",filename);
        return NULL;
    }else
    {
        fprintf(stderr, "loaded texture: %s \n",filename);
    }

    struct up_texture_data *tex_data = &(internal_texture.texture[internal_texture.count]);

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


    // set the RGB format to use in the glTexImage2d load function
    GLenum format_rgb = GL_RGB;
    if (tex->format->BytesPerPixel == 4) {
        
        format_rgb = GL_RGBA;
        printf("\nImg : %s have\n",filename);
        printf("SDL imgFormat: %s",SDL_GetPixelFormatName(tex->format->format));
        format_rgb = GL_BGRA;
    }


    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->w, tex->h, 0, format_rgb, GL_UNSIGNED_BYTE, tex->pixels);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex->w, tex->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, tex->pixels);
    
    SDL_FreeSurface(tex);
    internal_texture.count++;
    return tex_data;
}

void up_texture_bind(struct up_texture_data *texture, unsigned int texUnit)
{
    //glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
}
