#ifndef GAME_STATE_H
#define GAME_STATE_H

enum class GameStatus
{
    Playing,
    Won,
    Lost
};

struct GameState
{
    GameStatus status = GameStatus::Playing;
};

#endif