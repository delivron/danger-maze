#pragma once

#include <map>
#include <string>
#include <vector>

#include "SDL.h"

#include "frame.h"

namespace media {

    class ResourceManager {
    public:
        void                        loadSpriteFromDescription(const std::string& path, SDL_Renderer* renderer);
        void                        loadAnimationFromDescription(const std::string& path, SDL_Renderer* renderer);
        SpritePtr                   getSprite(const std::string& name) const;
        //AnimationPtr                getAnimation(const std::string& name) const;
        void                        cleanup();
        
    private:
        using Textures              = std::vector<SDL_Texture*>;
        using SpriteByName          = std::map<std::string, SpritePtr>;
        using FramesByName          = std::map<std::string, std::vector<Frame>>;

        Textures                    _textures;
        SpriteByName                _nameToSprite;
        FramesByName                _nameToFrames;
    };
    
}
