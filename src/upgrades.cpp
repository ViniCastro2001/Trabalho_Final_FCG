#include "upgrades.h"

#include <cmath>
#include <limits>

namespace
{
    struct UpgradeDef
    {
        const char* name;
        const char* description;
        int base_cost;
        float cost_growth;
        float base_value;
        float per_level;
    };

    const UpgradeDef kUpgradeDefs[(int)UpgradeId::COUNT] = {
        { "Energia",           "+0.5s de boost por lata.",        3, 1.5f, 3.0f, 0.5f  },
        { "Velocidade",        "+10% de velocidade.",             4, 1.7f, 1.0f, 0.10f },
        { "Alcance de coleta", "+0.3 de raio para coleta.",       5, 1.8f, 0.0f, 0.30f },
        { "Adrenalina",        "+0.25x de velocidade no boost.",  8, 2.0f, 2.0f, 0.25f },
        { "Recarga",           "+35% de velocidade de recarga.",  5, 1.6f, 1.0f, 0.35f },
    };

    int g_Coins = 0;
    int g_Levels[(int)UpgradeId::COUNT] = { 0, 0, 0, 0, 0 };
    bool g_InfiniteCoinsCheat = false;

    const UpgradeDef& Def(UpgradeId id)
    {
        return kUpgradeDefs[(int)id];
    }
}

void ResetUpgradesState()
{
    g_Coins = 0;

    for (int i = 0; i < (int)UpgradeId::COUNT; ++i)
        g_Levels[i] = 0;
}

int GetCoins()
{
    if (g_InfiniteCoinsCheat)
        return 999999;

    return g_Coins;
}

int GetRawCoins()
{
    return g_Coins;
}

void SetRawCoins(int n)
{
    if (n < 0)
        n = 0;

    g_Coins = n;
}

void AddCoins(int n)
{
    if (n <= 0)
        return;

    g_Coins += n;
}

int GetUpgradeLevel(UpgradeId id)
{
    return g_Levels[(int)id];
}

void SetUpgradeLevel(UpgradeId id, int level)
{
    if (level < 0)
        level = 0;

    g_Levels[(int)id] = level;
}

int GetUpgradeMaxLevel(UpgradeId id)
{
    (void)id;
    return 0;
}

int GetUpgradeCost(UpgradeId id)
{
    const UpgradeDef& def = Def(id);
    int level = g_Levels[(int)id];

    double cost = (double)def.base_cost * std::pow((double)def.cost_growth, (double)level);

    if (cost > (double)std::numeric_limits<int>::max())
        return std::numeric_limits<int>::max();

    return (int)std::ceil(cost);
}

float GetUpgradeValue(UpgradeId id)
{
    const UpgradeDef& def = Def(id);
    return def.base_value + def.per_level * (float)g_Levels[(int)id];
}

const char* GetUpgradeName(UpgradeId id)
{
    return Def(id).name;
}

const char* GetUpgradeDescription(UpgradeId id)
{
    return Def(id).description;
}

void SetInfiniteCoinsCheat(bool enabled)
{
    g_InfiniteCoinsCheat = enabled;
}

bool IsInfiniteCoinsCheatActive()
{
    return g_InfiniteCoinsCheat;
}

bool TryPurchaseUpgrade(UpgradeId id)
{
    int level = g_Levels[(int)id];

    int cost = GetUpgradeCost(id);

    if (g_InfiniteCoinsCheat)
    {
        g_Levels[(int)id] = level + 1;
        return true;
    }

    if (g_Coins < cost)
        return false;

    g_Coins -= cost;
    g_Levels[(int)id] = level + 1;
    return true;
}
