#ifndef MONSTER_CAM_H
#define MONSTER_CAM_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

// "Modo Monstro": câmera que mostra o jogo pelos olhos do Pé Grande.
//
// O recurso que alimenta esse modo são os "segundos de visão": um valor
// cumulativo (igual às moedas, é salvo em disco e persiste entre partidas).
// Cada lata coletada adiciona segundos; eles são consumidos enquanto o jogador
// usa a câmera do Pé Grande.

float GetVisionSeconds();
void SetRawVisionSeconds(float seconds);
void AddVisionSeconds(float seconds);
void ConsumeVisionSeconds(float seconds);

// Zera os segundos de visão (usado no reset de progresso).
void ResetVisionSeconds();

// Estado da câmera do Modo Monstro: fica ativa enquanto o jogador segura ALT,
// há segundos de visão e existe um Pé Grande vivo por perto.
bool IsBigfootCamActive();
void SetBigfootCamActive(bool active);

// Atualiza a câmera do Modo Monstro a cada frame. O chamador informa apenas o
// contexto que é dele (se o jogador está pedindo a câmera — ALT + estado de
// jogo — e se há um Pé Grande alvo por perto); o módulo aplica a política do
// recurso: só ativa se ainda houver segundos de visão, e os consome enquanto
// ativa. Retorna o novo estado de ativação.
bool UpdateBigfootCam(bool wants_to_activate, bool bigfoot_target_available, float delta_t);

// Monta a matriz View "pelos olhos" do Pé Grande, a partir da posição (base) e
// do yaw (direção que ele encara, em radianos) do monstro observado.
glm::mat4 ComputeBigfootCamView(glm::vec3 bigfoot_position, float bigfoot_yaw);

#endif
