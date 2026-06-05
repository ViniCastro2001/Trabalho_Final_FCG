#ifndef DEBUG_FLAGS_H
#define DEBUG_FLAGS_H

// Compile-time flags for debug and cheat features. These modules are not wired
// into gameplay yet; keeping the switches central makes later phases smaller.

#define MAP_VIEW_ENABLED 1
#define BIGFOOT_INVISIBILITY_CHEAT_ENABLED 1
#define INFINITE_ADRENALINE_CHEAT_ENABLED 1
#define INFINITE_COINS_CHEAT_ENABLED 1

// Permite congelar o Pé Grande (parar de perseguir o jogador) com a tecla B.
// Defina como 0 para desabilitar completamente o atalho de debug.
#define BIGFOOT_FREEZE_DEBUG_ENABLED 1

// Permite mostrar as coordenadas do jogador no canto superior direito (tecla F3).
// Defina como 0 para desabilitar completamente o atalho de debug.
#define SHOW_COORDS_DEBUG_ENABLED 1

// Permite alternar um "modo dia" de debug (céu claro + iluminação solar) com a
// tecla F2. Defina como 0 para remover completamente o atalho.
#define DAY_MODE_DEBUG_ENABLED 1

#endif
