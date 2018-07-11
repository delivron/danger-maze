#include <iostream>

#include "SDL_image.h"
#include "pugixml.hpp"

#include "resource_manager.h"

#include "../util/xml_attribute.h"

using namespace std;
using namespace pugi;
using namespace util;
using namespace media;

SDL_Rect generateRectangle(const xml_node& node) {
    SDL_Rect rectangle = { 0, 0, 0, 0 };
    tryReadXmlAttribute(rectangle.x, node.attribute("x"));
    tryReadXmlAttribute(rectangle.y, node.attribute("y"));
    tryReadXmlAttribute(rectangle.w, node.attribute("w"));
    tryReadXmlAttribute(rectangle.h, node.attribute("h"));
    return rectangle;
}

void ResourceManager::loadSpriteFromDescription(const std::string& path, SDL_Renderer* renderer) {
    xml_document doc;

    auto loadSuccess = doc.load_file(path.c_str());
    if (!loadSuccess) {
        return;
    }

    xml_node textureAtlasNode = doc.child("TextureAtlas");
    string imagePath = textureAtlasNode.attribute("imagePath").value();
    SDL_Surface* pngImage = IMG_Load(imagePath.c_str());
    if (pngImage == nullptr) {
        cout << "Error [IMG_Load]: " + path << endl;
        return;
    }

    _textures.push_back( SDL_CreateTextureFromSurface(renderer, pngImage) );
    SDL_FreeSurface(pngImage);

    SDL_Texture* texture = _textures.back();
    auto spriteNodes = textureAtlasNode.children();

    for (const xml_node& node : spriteNodes) {
        SDL_Rect rect = generateRectangle(node);
        string name = node.attribute("name").value();

        if (!name.empty()) {
            _nameToSprite[name] = make_shared<Sprite>(texture, rect);
        }
    }
}

void ResourceManager::loadAnimationFromDescription(const std::string& path, SDL_Renderer* renderer) {

}

SpritePtr ResourceManager::getSprite(const std::string& name) const {
    return (_nameToSprite.count(name) == 0) ? nullptr : _nameToSprite.at(name);
}

//AnimationPtr ResourceManager::getAnimation(const std::string& name) const

void ResourceManager::cleanup() {
    for (SDL_Texture* texture : _textures) {
        SDL_DestroyTexture(texture);
    }
}