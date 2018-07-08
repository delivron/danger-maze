#include <sstream>

#include "settings.h"

#include "../util/xml_attribute.h"

using namespace std;
using namespace app;
using namespace util;
using namespace pugi;

Settings Settings::initializeFromFile(const std::string& path) {
    Settings settings;
    xml_document doc;

    auto loadSuccess = doc.load_file(path.c_str());
    if (loadSuccess) {
        xml_node displaySettings = doc.child("Settings").child("Display");
        
        xml_node sizeSettings = displaySettings.child("Size");
        xml_attribute widthAttribute = sizeSettings.attribute("width");
        xml_attribute heightAttribute = sizeSettings.attribute("height");
        tryReadXmlAttribute(settings.windowWidth, widthAttribute);
        tryReadXmlAttribute(settings.windowHeight, heightAttribute);

        xml_attribute fullscreenAttribute =
            displaySettings.child("Fullscreen").attribute("value");
        tryReadXmlAttribute<bool>(settings.fullscreen, fullscreenAttribute);
    }

    return settings;
}
