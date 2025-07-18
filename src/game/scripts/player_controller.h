#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

#include "../game.h"

class TransformComponent;
class Object;

class PlayerController : public ScriptBase {
public:
    PlayerController();

    void OnCreate() override;
    void OnUpdate(float deltaTime) override;
    void OnDestroy() override;

    TransformComponent* transform;
};

#endif // PLAYER_CONTROLLER_H