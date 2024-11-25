#pragma once

#include "texture.h"
#include <chrono>

//TODO: Załaduj texture.
//TODO: Zmniejszaj counter_ po każdym okresie czasu.
//TODO: Modyfikuj texturę, bazując na zmiennej counter.

class Plane
{
public:
    Plane(size_t idx, float x, float y, const std::shared_ptr<Texture>& plane_texture)
        :idx_(idx), x_(x), y_(y), counter_(10), plane_texture_(plane_texture)
    {
    }

    ~Plane()
    {}

    void update()
    {
        counter_--;
        x_ += 10;
        SDL_Log("Plane: idx: %d pos: (%.2f, %2.f) counter: %d", idx_, x_, y_, counter_);
    }

    void draw(SDL_Renderer* renderer)
    {
        if(counter_ < 1)
            return;

        plane_texture_->DrawTexture(renderer, x_, y_);
    }

    inline const size_t get_counter() const { return counter_; }

private:
    size_t idx_;
    float x_;
    float y_;

    //counter jest w sekundach
    size_t counter_;

    std::shared_ptr<Texture> plane_texture_;
};