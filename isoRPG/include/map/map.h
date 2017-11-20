#ifndef ISORPG_MAP_H
#define ISORPG_MAP_H

#include <string>
#include <list>

#include "object.h"

namespace Json
{
    class Value;
}

// Class with a single public static function that loads a map into an object list
class Map
{
public:
    // Load map from Tiled JSON file
    static bool load(std::string filename, std::list<Object*>& objects, StateBase::Context context);


private:
    // Handles regular layers
    static void loadLayer(Json::Value& layer, std::list<Object*>& objects, TileSize tileSize, StateBase::Context context);

    // Handles object layers
    static void loadObjects(Json::Value& root, Json::Value& layer, std::list<Object*>& objects, TileSize tileSize, StateBase::Context context);

    // Handles collsion layer
    static void loadCollision(Json::Value& root, Json::Value& layer, StateBase::Context context);
};


#endif //ISORPG_MAP_H
