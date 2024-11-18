#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_main.h>
#include <cmath>

#include <string>

#include <memory>

class Texture;

const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
std::unique_ptr<Texture> bg_texture_;

struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AppResult app_quit = SDL_APP_CONTINUE;
};

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
        surface = SDL_LoadBMP(filepath.c_str());
        if (!surface)
            SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
    

        SDL_free(bmp_path);  /* done with this, the file is loaded. */

        texture_width_ = surface->w;
        texture_height_ = surface->h;

        texture_ = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture_)
            SDL_Log("Couldn't create static texture: %s", SDL_GetError());

        SDL_DestroySurface(surface); 

        dst_rect_.x = 0.0f;
        dst_rect_.y = 0.0f;
        dst_rect_.w = WINDOW_WIDTH;
        dst_rect_.h = WINDOW_HEIGHT;
    }
    ~Texture() 
    {
        SDL_DestroyTexture(texture_);
    } 

    bool DrawTexture(SDL_Renderer* renderer)
    {
        return SDL_RenderTexture(renderer, texture_, NULL, &dst_rect_);
    }

private:
    SDL_Texture *texture_;
    float texture_width_;
    float texture_height_;

    SDL_FRect dst_rect_;
};


SDL_AppResult SDL_Fail(){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    // init the library, here we make a window so we only need the Video capabilities.
    if (not SDL_Init(SDL_INIT_VIDEO)){
        return SDL_Fail();
    }
    
    // create a window
    SDL_Window* window = SDL_CreateWindow("Window", 800, 600, 0);
    if (not window){
        return SDL_Fail();
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (not renderer){
        return SDL_Fail();
    }
    
    // print some information about the window
    SDL_ShowWindow(window);
    {
        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth){
            SDL_Log("This is a highdpi environment.");
        }
    }

    // set up the application data
    *appstate = new AppContext{
       window,
       renderer,
    };

    SDL_Log("Application started successfully!");

    //load textures here
    bg_texture_ = std::make_unique<Texture>(renderer, "cs_bg.bmp", WINDOW_WIDTH, WINDOW_HEIGHT);


    //==================//

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event) {
    auto* app = (AppContext*)appstate;
    
    if (event->type == SDL_EVENT_QUIT) {
        app->app_quit = SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto* app = (AppContext*)appstate;

    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  /* grey, full alpha */
    SDL_RenderClear(app->renderer);  /* start with a blank canvas. */

    bg_texture_->DrawTexture(app->renderer);

    SDL_RenderPresent(app->renderer);

    return app->app_quit;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto* app = (AppContext*)appstate;
    if (app) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        delete app;
    }

    SDL_Log("Application quit successfully!");
}
