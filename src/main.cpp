
#include <SDL3/SDL_timer.h>

#include "texture.h"
#include "plane.h"

#include <cmath>
#include <queue>
#include <mutex>

int x_plane_pos=0;
int y_plane_pos=0;
int id_plane = 0;

const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 800;

std::unique_ptr<Texture> bg_texture_;
std::shared_ptr<Texture> plane_texture_;

std::mutex mutex_;
std::vector<Plane> base_plane_list;

struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AppResult app_quit = SDL_APP_CONTINUE;
};

void get_new_plane() {

    std::lock_guard<std::mutex> lock(mutex_);

    //tmp
    x_plane_pos = x_plane_pos + 10;
    y_plane_pos = y_plane_pos + 10;
    base_plane_list.push_back(Plane(id_plane, x_plane_pos, y_plane_pos, 5.0f, 10.0f, plane_texture_));
    id_plane++;
    base_plane_list.push_back(Plane(id_plane, x_plane_pos+100, y_plane_pos+30, 5.0f, 10.0f, plane_texture_));
    id_plane++;
    // base_plane_list.push_back(Plane(1, x_plane_pos+10.0f, 100.0f, -5.0f, 10.0f, plane_texture_));
    // base_plane_list.push_back(Plane(2, x_plane_pos+20.0f, 200.0f, 5.0f, -10.0f, plane_texture_));
    // base_plane_list.push_back(Plane(3, x_plane_pos+30.0f, 300.0f, -5.0f, -10.0f, plane_texture_));
    // base_plane_list.push_back(Plane(4, x_plane_pos+40.0f, 400.0f, 5.0f, 10.0f, plane_texture_));
    // base_plane_list.push_back(Plane(5, x_plane_pos+50.0f, 500.0f, -5.0f, 10.0f, plane_texture_));
    // base_plane_list.push_back(Plane(6, x_plane_pos+60.0f, 600.0f, 5.0f, -10.0f, plane_texture_));
    // base_plane_list.push_back(Plane(7, x_plane_pos+70.0f, 700.0f, -5.0f, -10.0f, plane_texture_));
}

Uint32 timerCallback(void* userdata, unsigned int timer_id, unsigned int interval)
{
    std::vector<Plane> new_planes;
    get_new_plane();
    for(auto& plane : base_plane_list)
    {
        plane.update();
        if(plane.get_counter() > 0)
            new_planes.push_back(plane);
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        base_plane_list = new_planes;
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
    SDL_Window* window = SDL_CreateWindow("Window", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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
    bg_texture_ = std::make_unique<Texture>(renderer, "../..//data//cs_bg.png", WINDOW_WIDTH, WINDOW_HEIGHT);
    plane_texture_ = std::make_shared<Texture>(renderer, "../..//data//plane1.png", 20, 20);
    //bg_texture_ = std::make_unique<Texture>(renderer, "../../../../../data/cs_bg.bmp", WINDOW_WIDTH, WINDOW_HEIGHT);
    //plane_texture_ = std::make_shared<Texture>(renderer, "../../../../../data/plane1.png", 40, 40);


    //==================//


    //tmp data

    get_new_plane();

    //==============

    //update callback
    SDL_AddTimer(100, timerCallback, (void*)0);
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
    for(auto& plane : base_plane_list)
        plane.draw(app->renderer);

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
