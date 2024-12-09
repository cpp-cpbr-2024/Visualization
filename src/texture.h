#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <string>
#include <memory>

class Texture
{
public:
    Texture(SDL_Renderer* renderer, const std::string& filepath, float width, float height) 
     :texture_(nullptr), texture_width_(0), texture_height_(0)
    {
        SDL_Log("Loading texture: %s", filepath.c_str());

        SDL_Surface *surface = NULL;
        char *bmp_path = NULL;
        /* Textures are pixel data that we upload to the video hardware for fast drawing. Lots of 2D
        engines refer to these as "sprites." We'll do a static texture (upload once, draw many
        times) with data from a bitmap file. */

        /* SDL_Surface is pixel data the CPU can access. SDL_Texture is pixel data the GPU can access.
        Load a .bmp into a surface, move it to a texture from there. */
        SDL_asprintf(&bmp_path, filepath.c_str(), SDL_GetBasePath());  /* allocate a string of the full file path */
       // surface = SDL_LoadBMP(filepath.c_str());
       surface = IMG_Load(filepath.c_str());
        if (!surface)
            SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
    

        SDL_free(bmp_path);  /* done with this, the file is loaded. */

        texture_ = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture_)
            SDL_Log("Couldn't create static texture: %s", SDL_GetError());

        SDL_DestroySurface(surface); 

        dst_rect_.x = 0.0f;
        dst_rect_.y = 0.0f;
        dst_rect_.w = width;
        dst_rect_.h = height;
    }
    ~Texture() 
    {
        SDL_DestroyTexture(texture_);
    } 


    bool DrawTexture(SDL_Renderer* renderer)
    {
        return SDL_RenderTexture(renderer, texture_, NULL, &dst_rect_);
    }

    bool DrawTexture(SDL_Renderer* renderer, float x, float y)
    {
        dst_rect_.x = x;
        dst_rect_.y = y;
        return SDL_RenderTexture(renderer, texture_, NULL, &dst_rect_);
    }

private:
    SDL_Texture *texture_;
    float texture_width_;
    float texture_height_;

    SDL_FRect dst_rect_;
};
