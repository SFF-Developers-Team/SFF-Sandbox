#include <player.hpp>

void Player::update() {
    if (IsKeyDown(KEY_D)) {
        hitbox.x += 2.0f;
    }   
    if (IsKeyDown(KEY_A)) {
        hitbox.x -= 2.0f;
    }  
    if (IsKeyDown(KEY_W)) {
        hitbox.y += 2.0f;
    }   
    if (IsKeyDown(KEY_S)) {
        hitbox.y -= 2.0f;
    }   
}
Player::Player() {
    this->player = LoadTexture("assets/player.png");
    this->hitbox = Rectangle((float)position.x, (float)position.y, (float)playersize.x,(float)playersize.y);
}