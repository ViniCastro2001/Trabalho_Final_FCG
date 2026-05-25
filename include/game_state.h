#ifndef GAME_STATE_H
#define GAME_STATE_H

enum class GameStatus
{
    MainMenu,
    UpgradeShop,
    ConfirmReset,
    Playing,
    Won,
    Lost
};

struct GameState
{
    GameStatus status = GameStatus::MainMenu;
};

#endif
