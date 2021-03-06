#include "Character.h"

Character::Character(Footsies* footsies, int player, int x, string name)
{
    this->footsies = footsies;
    this->name = name;
    this->current_state = "idle";
    this->frame = 0;
    this->animation_frame = 0;
    this->x = x;
    this->y = 0;
    this->velocity_x = 0;
    this->velocity_y = 0;
    this->acceleration_x = 0;
    this->acceleration_y = 0;

    opponent = NULL;
    this->player = player;
    game_started = false;

    for(int i=0;i<20;i++)
      input_buffer.push_back("5");

    Node* config_node = rosalila()->parser->getNodes(assets_directory+"character/" + this->name + "/config.xml");
    vector<Node*> files_nodes = config_node->getNodeByName("MoveFiles")->getNodesByName("File");
    for(int i=0; i< (int)files_nodes.size(); i++)
    {
        Node* moves_node = rosalila()->parser->getNodes(assets_directory+"character/" + this->name + "/" + files_nodes[i]->attributes["path"]);
        vector<Node*> move_nodes = moves_node->getNodesByName("Move");
        for(int i=0; i< (int)move_nodes.size(); i++)
        {
            moves[move_nodes[i]->attributes["name"]] = new Move(this,move_nodes[i]);
        }
    }
}

void Character::draw()
{
    Move* current_move = moves[this->current_state];
    current_move->draw(this->x, this->y, this->isFlipped());
}

void Character::updateBuffer()
{
  bool left_pressed = rosalila()->receiver->isJoyDown(-4,player);
  bool right_pressed = rosalila()->receiver->isJoyDown(-6,player);
  bool up_pressed = rosalila()->receiver->isJoyDown(-8,player);
  bool down_pressed = rosalila()->receiver->isJoyDown(-2,player);

  if(this->isFlipped())
  {
      bool temp = left_pressed;
      left_pressed = right_pressed;
      right_pressed = temp;
  }

  string input_pressed = "5";

  if(left_pressed && up_pressed)
    input_pressed = "7";
  else if(left_pressed && down_pressed)
    input_pressed = "1";
  else if(right_pressed && up_pressed)
    input_pressed = "9";
  else if(right_pressed && down_pressed)
    input_pressed = "3";
  else if(left_pressed)
    input_pressed = "4";
  else if(right_pressed)
    input_pressed = "6";
  else if(up_pressed)
    input_pressed = "8";
  else if(down_pressed)
    input_pressed = "2";

  if(rosalila()->receiver->isJoyPressed(1,player))
    input_pressed += "a";
  if(rosalila()->receiver->isJoyPressed(2,player))
    input_pressed += "b";
  if(rosalila()->receiver->isJoyPressed(3,player))
    input_pressed += "c";

  input_buffer.pop_back();
  input_buffer.push_front(input_pressed);

}

void Character::logic()
{
      Move* previous_move = moves[this->current_state];
      if(previous_move->isFinished())
          cancel("idle");

      updateBuffer();

      for(map<string,Move*>::iterator move_iterator = moves.begin(); move_iterator != moves.end(); move_iterator++)
      {
        string move_name = (*move_iterator).first;
        Move* move = (*move_iterator).second;
        if(move->inputIsInBuffer() && move->canCancel(this->current_state))
          if(game_started)
            cancel(move_name);
      }

      velocity_x += acceleration_x;
      velocity_y += acceleration_y;

      int last_y = y;

      if(this->isFlipped())
          this->x -= velocity_x;
      else
          this->x += velocity_x;
      this->y += velocity_y;

      if(y<=0 && isInBounds())
      {
        y = 0;
        acceleration_y = 0;
      }else
      {
        acceleration_y-=0.25;
      }

      if(last_y > 0 && y == 0)
      {
        cancel("idle");
      }

    Move* current_move = moves[this->current_state];
    current_move->logic();
}

void Character::cancel(string new_state)
{
    this->velocity_x = 0;
    this->velocity_y = 0;
    this->current_state=new_state;
    Move* new_move = moves[current_state];
    new_move->restart();
}

bool Character::isFlipped()
{
    return opponent->x < this->x;
}

Move* Character::getCurrentMove()
{
    return moves[this->current_state];
}

bool Character::isInBounds()
{
    return this->x > footsies->stage_boundaries && this->x < rosalila()->graphics->screen_width - footsies->stage_boundaries;
}
