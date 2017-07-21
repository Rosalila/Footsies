#ifndef FRAME_H
#define FRAME_H

#include "../Rosalila/Rosalila.h"
#include "Character.h"
#include "Move.h"

class Character;
class Move;

class Frame
{
public:

    Character* character;
    Move* move;
    Image* image;
    int frame;
    int duration;
    bool finished;
    vector<Hitbox*> hitboxes;
    vector<Hitbox*> hurtboxes;

    int velocity_x;
    int velocity_y;

    Frame(Character*character, Move* move, Node* frame_node);
    void draw(int x, int y, bool fipped);
    void logic();
    void restart();
    bool isFinished();
};

#endif
