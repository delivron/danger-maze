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
        DisplaySettings& disp = settings.display;
        WorldSettings& world = settings.world;

        xml_node settingsNode = doc.child("Settings");

        xml_node displayNode = settingsNode.child("Display");
        xml_node displaySizeNode = displayNode.child("Size");
        xml_node fullscreenNode = displayNode.child("Fullscreen");
        tryReadXmlAttribute(disp.width, displaySizeNode.attribute("width"));
        tryReadXmlAttribute(disp.height, displaySizeNode.attribute("height"));
        tryReadXmlAttribute(disp.fullscreen, fullscreenNode.attribute("value"));

        xml_node worldNode = settingsNode.child("World");
        xml_node worldSizeNode = worldNode.child("Size");
        xml_node enemiesNode = worldNode.child("Enemies");
        xml_node cannonsNode = worldNode.child("Cannons");
        tryReadXmlAttribute(world.width, worldSizeNode.attribute("width"));
        tryReadXmlAttribute(world.height, worldSizeNode.attribute("height"));
        tryReadXmlAttribute(world.enemiesCount, enemiesNode.attribute("count"));
        tryReadXmlAttribute(world.cannonsCount, cannonsNode.attribute("count"));
    }

    return settings;
}
