#include <systems/generatePath.h>
#include <components/Comp_animation.h>
#include <components/Comp_size.h>


void GeneratePath::generate(anax::World &world, anax::Entity player) {
    auto enteties = world.getEntities();

    for (auto i : enteties) {
        if (i.hasComponent<Movable>() && i.getId() != player.getId()) {
            PositionComponent positionComponent = i.getComponent<PositionComponent>();
            Movable &moveble = i.getComponent<Movable>();
            moveble.path = "";
            moveble.current = 0;
            const int mapsize = 64;
            int pixelsPerBlock = 10;

            if (i.getComponent<Movable>().agro) {
                PositionComponent &playerPos = player.getComponent<PositionComponent>();

                auto heroSize = player.getComponent<SizeComponent>();
                auto enemySize = i.getComponent<SizeComponent>();

                // Calculating the tile position of the hero
                sf::Vector2i playerTilePos = sf::Vector2i(playerPos.XPos + heroSize.width / 2, playerPos.SpriteTop);
                playerTilePos = sf::Vector2i((2 * playerTilePos.y + playerTilePos.x) / 64,
                                             (2 * playerTilePos.y - playerTilePos.x) / 64);

                sf::Vector2i enemyTilePos = sf::Vector2i(positionComponent.SpriteLeft + 17,
                                                         positionComponent.SpriteTop + 25);
                enemyTilePos = sf::Vector2i((2 * enemyTilePos.y + enemyTilePos.x) / 64,
                                            (2 * enemyTilePos.y - enemyTilePos.x) / 64);

                if (enemyTilePos == playerTilePos)
                    continue;

                if (0 < playerTilePos.x < mapsize && 0 < playerTilePos.y < mapsize) {
                    std::string path = pathFind(enemyTilePos.x, enemyTilePos.y, playerTilePos.x, playerTilePos.y);

                    moveble.path = path;

                    for (int i = 0; i < path.length(); i++) {
                        for (int n = 0; n < pixelsPerBlock; n++) {
                            char instruction = moveble.path.at(i);
                            moveble.path.push_back(instruction);
                            int index = i * pixelsPerBlock + n;

                            // Rotated direction clockwise because the pathfinder logic uses an orthogonal representation of the map
                            if (instruction == '7') {
                                moveble.moveX[index] = 1;
                                moveble.moveY[index] = 0;
                            }
                            if (instruction == '0') {
                                moveble.moveX[index] = 0.66;
                                moveble.moveY[index] = 0.33;
                            }
                            if (instruction == '1') {
                                moveble.moveX[index] = 0;
                                moveble.moveY[index] = 1;
                            }
                            if (instruction == '2') {
                                moveble.moveX[index] = -0.66;
                                moveble.moveY[index] = 0.33;
                            }
                            if (instruction == '3') {
                                moveble.moveX[index] = -1;
                                moveble.moveY[index] = 0;
                            }
                            if (instruction == '4') {
                                moveble.moveX[index] = -0.66;
                                moveble.moveY[index] = -0.33;
                            }
                            if (instruction == '5') {
                                moveble.moveX[index] = 0;
                                moveble.moveY[index] = -1;
                            }
                            if (instruction == '6') {
                                moveble.moveX[index] = 0.66;
                                moveble.moveY[index] = -0.33;
                            }
                        }
                    }
                    i.getComponent<AnimationComponent>().action = "Walk";
                } else {
                    moveble.agro = false;

                    std::string path = pathFind(enemyTilePos.x, enemyTilePos.y, playerTilePos.x, playerTilePos.y);

                    if (0 < playerTilePos.x < mapsize && 0 < playerTilePos.y < mapsize) {
                        std::string path = pathFind(enemyTilePos.x, enemyTilePos.y, playerTilePos.x, playerTilePos.y);
                        moveble.path = path;
                        i.getComponent<AnimationComponent>().action = "Walk";
                    } else {
                        i.kill();
                        world.refresh();
                    }
                }
            }
        }
    }
}