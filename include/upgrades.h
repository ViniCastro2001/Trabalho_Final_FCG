#ifndef UPGRADES_H
#define UPGRADES_H

enum class UpgradeId
{
    BoostDuration = 0,
    MoveSpeed,
    CollectRadius,
    BoostMultiplier,
    ReloadSpeed,
    COUNT
};

void ResetUpgradesState();

int GetCoins();
int GetRawCoins();
void SetRawCoins(int n);
void AddCoins(int n);

int GetUpgradeLevel(UpgradeId id);
void SetUpgradeLevel(UpgradeId id, int level);
int GetUpgradeMaxLevel(UpgradeId id);
int GetUpgradeCost(UpgradeId id);
float GetUpgradeValue(UpgradeId id);
const char* GetUpgradeName(UpgradeId id);
const char* GetUpgradeDescription(UpgradeId id);

void SetInfiniteCoinsCheat(bool enabled);
bool IsInfiniteCoinsCheatActive();

bool TryPurchaseUpgrade(UpgradeId id);

#endif
