#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <include/systems/resolve_movment.h>
#include <include/systems/drawEntety.h>
#include <include/collections/setUpPlayer.h>
#include <include/systems/attack.h>
#include <include/systems/mouse_clicked.h>

#include "states/state_game.h"
#include "gameEngine/resource_holder.h"
#include "map/map.h"
#include "util/utility.h"



StateGame::StateGame(StateStack &stack, StateBase::Context context)
        : StateBase(stack, context)
        , playerCam()
{
    isMovingUp = false;
    isMovingDown = false;
    isMovingLeft = false;
    isMovingRight = false;


    playerCam.setSize(1920, 1080);
    playerCam.zoom(0.3f);


    // Load map information from JSON into object list
    if (!Map::load("assets/map/map.json", objects))
        std::runtime_error("StateGame::StateGame - Failed to load map data.");

    sf::Texture& Herobody = context.textures->get(Textures::Hero);
    sf::Texture& GoblinTexture = context.textures->get(Textures::Goblin);

    anax::World& world = *getContext().world;
    sf::RenderWindow& window = *getContext().window;
    DrawEntetys drawEntetys;
    drawEntetys.draw(window,world, "Game");
    player = world.createEntity();
    anax::Entity goblin = world.createEntity();
    anax::Entity goblin2 = world.createEntity();
    anax::Entity goblin3 = world.createEntity();


    SetUpPlayer creatureSetup;
    creatureSetup.setUpPlayer(player, Herobody, *getContext().window);
    creatureSetup.setUpEnemie(goblin, GoblinTexture, *getContext().window, 200, 200, "Hard");
    creatureSetup.setUpEnemie(goblin2, GoblinTexture, *getContext().window ,100 ,100, "Medium");
    creatureSetup.setUpEnemie(goblin3, GoblinTexture, *getContext().window ,0 ,100, "Easy");

    playerGold.setPosition(window.getPosition().x, window.getPosition().y);
    sf::Font& font = context.fonts->get(Fonts::Main);
    playerGold.setFont(font);
    Looteble looteble = player.getComponent<Looteble>();
    playerGold.setString(std::to_string(looteble.gold));
    playerGold.setScale(0.3,0.3);
    sf::Color gold(255,215,0);
    playerGold.setColor(gold);

    context.music->play(Music::Test);
}

void StateGame::draw()
{
    sf::RenderWindow& window = *getContext().window;

    window.setView(playerCam);
    //Sorting objects based on priority (y coordinate), from low to high.
    objects.sort([](Object *f, const Object *s) { return f->priority < s->priority; });

    anax::World& world = *getContext().world;
    DrawEntetys drawEntetys;

    for (Object* object : objects)
    {
        object->process(1.f/60.f);
        object->draw(window);
        if (object->priority < mPlayer.getPosition().y)
            window.draw(mPlayer);
    }
    drawEntetys.draw(window,world, "Game");
    Looteble looteble = player.getComponent<Looteble>();
    sf::Vector2f viewCenter = window.getView().getCenter();
    sf::Vector2f halfExtents = window.getView().getSize() / 2.0f;
    sf::Vector2f translation = viewCenter - halfExtents;

    int mX = static_cast<int>(translation.x);
    int mY = static_cast<int>(translation.y);

    playerGold.setPosition(mX,mY);
    playerGold.setString(std::to_string(looteble.gold));
    window.draw(playerGold);

}

bool StateGame::update(sf::Time dt)
{
    sf::Vector2f movement(0.f, 0.f);
    if (isMovingUp)
        movement.y -= 2.f;
    if (isMovingDown)
        movement.y += 2.f;
    if (isMovingLeft)
        movement.x -= 2.f;
    if (isMovingRight)
        movement.x += 2.f;

    PositionComponent& positionComponent = player.getComponent<PositionComponent>();
    playerCam.setCenter(positionComponent.XPos, positionComponent.YPos);

    //std::cout<< "XPOS:  " << positionComponent.XPos << " | YPOS:  " << positionComponent.YPos << std::endl;

    return true;
}

bool StateGame::handleEvent(const sf::Event &event)
{
    if(event.type == sf::Event::MouseButtonPressed)
    {
        anax::World& world = *getContext().world;
        sf::RenderWindow& window = *getContext().window;
        MouseClicked mouseClicked;
        mouseClicked.Clicked(world,sf::Mouse::getPosition(window).x,sf::Mouse::getPosition(window).y, player);

    }

    switch (event.type)
    {
        case sf::Event::KeyPressed:
            handleUserInput(event.key.code, true);
            break;
        case sf::Event::KeyReleased:
            handleUserInput(event.key.code, false);
            break;
    }

    return true;
}

void StateGame::handleUserInput(sf::Keyboard::Key key, bool isPressed)
{
    AnimationComponent& animationComponent = player.getComponent<AnimationComponent>();
    float deltaTime = animationComponent.animationClock.getElapsedTime().asSeconds();
    animationComponent.animationClock.restart().asSeconds();


    if (key == sf::Keyboard::W|| key == sf::Keyboard::Up)
    {
        if(animationComponent.direction != "Up" )animationComponent.changedDirection = true;
        animationComponent.direction = "Up";
        animationComponent.movementDirection.y -= animationComponent.movementSpeed*animationComponent.deltaTime;
        animationComponent.row = 2;
        ResolveMovment resolve;
        resolve.resolveMovment(player, "Walk", deltaTime);


        PositionComponent& positionComponent = player.getComponent<PositionComponent>();
        Moveble moveble = player.getComponent<Moveble>();
        positionComponent.YPos -= moveble.speed;
        isMovingUp = isPressed;
    }

    else if (key == sf::Keyboard::S|| key == sf::Keyboard::Down)
    {
        if(animationComponent.direction != "Down" )animationComponent.changedDirection = true;
        animationComponent.direction = "Down";
        animationComponent.movementDirection.y += animationComponent.movementSpeed*animationComponent.deltaTime;
        animationComponent.row = 6;
        ResolveMovment resolve;
        resolve.resolveMovment(player, "Walk", deltaTime);

        PositionComponent& positionComponent = player.getComponent<PositionComponent>();
        Moveble moveble = player.getComponent<Moveble>();
        positionComponent.YPos += moveble.speed;
        isMovingDown = isPressed;
    }

    else if (key == sf::Keyboard::A || key == sf::Keyboard::Left)
    {
        if(animationComponent.direction != "Left" )animationComponent.changedDirection = true;
        animationComponent.direction = "Left";
        animationComponent.movementDirection.x -= animationComponent.movementSpeed*animationComponent.deltaTime;
        animationComponent.row = 0;
        ResolveMovment resolve;
        resolve.resolveMovment(player, "Walk", deltaTime);

        PositionComponent& positionComponent = player.getComponent<PositionComponent>();
        Moveble moveble = player.getComponent<Moveble>();
        positionComponent.XPos -= moveble.speed;

        isMovingDown = isPressed;
    }

    else if (key == sf::Keyboard::D || key == sf::Keyboard::Right)
    {
        if(animationComponent.direction != "Right" )animationComponent.changedDirection = true;
        animationComponent.direction = "Right";
        animationComponent.movementDirection.x += animationComponent.movementSpeed*animationComponent.deltaTime;
        animationComponent.row = 4;
        ResolveMovment resolve;
        resolve.resolveMovment(player, "Walk", deltaTime);

        PositionComponent& positionComponent = player.getComponent<PositionComponent>();
        Moveble moveble = player.getComponent<Moveble>();
        positionComponent.XPos += moveble.speed;

        isMovingRight = isPressed;
    }
    else if (key == sf::Keyboard::Space)
    {
        Attack attack;
        anax::World& world = *getContext().world;
        attack.resolveAttack(world,player);
    }

    else if (key == sf::Keyboard::Escape && isPressed)
        requestStackPush(States::Pause);
    else if (key == sf::Keyboard::F5 && isPressed)
    {
        objects.clear();
        std::cout << "Loading map data ..." << std::endl;
        if (!Map::load("assets/map/map.json", objects))
        {
            std::cout << "Failed to reload map data." << std::endl;
        }
    }
}