#ifndef PLAYER_MODEL_H
#define PLAYER_MODEL_H

#include <glm/vec3.hpp>

// Corpo procedural do jogador (caçador segurando a espingarda), montado a
// partir de cubos/esferas. É exibido quando o jogador observa a cena pela
// câmera do Pé Grande (Modo Monstro), já que aí ele se vê de fora.
//
// position        - pés do jogador no mundo
// yaw             - direção que o jogador encara (radianos)
// time_seconds    - tempo corrente, para respiração e ciclo de caminhada
// movement_intensity - 0 parado, ~1 caminhando (amplitude do passo)
// recoil_timer / recoil_duration - reservados para recuo da arma
void DrawPlayerModel(glm::vec3 position, float yaw, float time_seconds,
                     float movement_intensity, float recoil_timer, float recoil_duration);

#endif
