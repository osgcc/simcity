#include "Texture.h"

#include <SDL2/SDL_image.h>
#include <iostream>

Texture loadTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* temp = IMG_Load(filename.c_str());
    SDL_Texture* out = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);

    if (!out)
    {
        std::cout << SDL_GetError() << std::endl;
        throw std::runtime_error(std::string("loadTexture(): ") + SDL_GetError());
    }

    int width = 0, height = 0;
    SDL_QueryTexture(out, nullptr, nullptr, &width, &height);

    return Texture{ out, SDL_Rect{ 0, 0, width, height }, { width, height } };
}
