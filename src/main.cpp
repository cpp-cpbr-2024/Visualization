
#include <SDL3/SDL_timer.h>

#include "texture.h"
#include "plane.h"

#include <cmath>
#include <queue>
#include <mutex>

const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;

std::unique_ptr<Texture> bg_texture_;
std::shared_ptr<Texture> plane_texture_;

std::mutex mutex_;
std::vector<Plane> planes_list;
std::vector<Plane> base_plane_list;

struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AppResult app_quit = SDL_APP_CONTINUE;
};


Uint32 timerCallback(void* userdata, unsigned int timer_id, unsigned int interval)
{
    {
    std::lock_guard<std::mutex> lock(mutex_);
    planes_list.clear();
    std::vector<std::vector<Plane>::iterator> to_erase;

        for(auto plane = base_plane_list.begin(); plane != base_plane_list.end(); plane++)
        {
            plane->update();
            
            if(plane->get_counter() > 0)
                planes_list.push_back(*plane);
            else
                to_erase.push_back(plane);
        }

        for(auto& idx : to_erase)
            base_plane_list.erase(idx);
    }
    
    return interval;
}

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
    bg_texture_ = std::make_unique<Texture>(renderer, "../..//data//cs_bg.bmp", WINDOW_WIDTH, WINDOW_HEIGHT);
    plane_texture_ = std::make_shared<Texture>(renderer, "../..//data//plane1.bmp", 40, 40);
    //bg_texture_ = std::make_unique<Texture>(renderer, "../../../../../data/cs_bg.bmp", WINDOW_WIDTH, WINDOW_HEIGHT);
    //plane_texture_ = std::make_shared<Texture>(renderer, "../../../../../data/plane1.bmp", 40, 40);


    //==================//


    //tmp data

    base_plane_list.push_back(Plane(0, 0.3f, 0.3f, plane_texture_));
    base_plane_list.push_back(Plane(1, 6.0f, 300.0f, plane_texture_));
    base_plane_list.push_back(Plane(2, 20.0f, 60.0f, plane_texture_));
    base_plane_list.push_back(Plane(3, 40.0f, 100.0f, plane_texture_));
    
    planes_list.push_back(Plane(0, 0.3f, 0.3f, plane_texture_));
    planes_list.push_back(Plane(1, 6.0f, 300.0f, plane_texture_));
    planes_list.push_back(Plane(2, 20.0f, 60.0f, plane_texture_));
    planes_list.push_back(Plane(3, 40.0f, 100.0f, plane_texture_));

    //==============

    //update callback
    SDL_AddTimer(1000, timerCallback, (void*)0);
    //===============

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


    //rysuj samoloty
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto& plane : planes_list)
        {
            //mutex_.lock();
            plane.draw(app->renderer);
        }
    }
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
