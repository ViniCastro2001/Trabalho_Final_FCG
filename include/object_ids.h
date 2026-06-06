#ifndef OBJECT_IDS_H
#define OBJECT_IDS_H

// Identificadores de "objeto" enviados ao fragment shader (uniform object_id),
// que seleciona a cor/material de cada parte desenhada. Estes valores são um
// contrato compartilhado com data/shaders/shader_fragment.glsl: ao alterar um
// número aqui, ajuste o switch correspondente no shader.

const int OBJECT_SPHERE = 0;
const int OBJECT_PLANE = 2;
const int OBJECT_SAFE_ZONE = 3;
const int OBJECT_BIGFOOT = 4;
const int OBJECT_WALL = 5;
const int OBJECT_BIGFOOT_EYES = 6;
const int OBJECT_HUD_BAR_BACK = 7;
const int OBJECT_HUD_BAR_FILL = 8;
const int OBJECT_SHOTGUN = 9;
const int OBJECT_HANDS = 10;
const int OBJECT_ROAD = 11;
const int OBJECT_SIDEWALK = 12;
const int OBJECT_CONCRETE = 13;
const int OBJECT_METAL_ROOF = 14;
const int OBJECT_WINDOW = 15;
const int OBJECT_TREE_TRUNK = 16;
const int OBJECT_TREE_LEAVES = 17;
const int OBJECT_CAR_BODY = 18;
const int OBJECT_CAR_GLASS = 19;
const int OBJECT_LAMP_LIGHT = 20;
const int OBJECT_MONSTER_DRINK = 21;
const int OBJECT_WEAPON_METAL = 25;
const int OBJECT_WEAPON_WOOD = 26;
const int OBJECT_WEAPON_ACCENT = 27;
const int OBJECT_ROCKY_FLOOR = 28;
const int OBJECT_CAR = 29; // Modelo .obj de carro (multi-material via u_material_diffuse).
const int OBJECT_BENCH = 30; // Modelo .obj de banco de madeira (wooden-bench).
const int OBJECT_PLAYER_CLOTH = 31; // Jaqueta do caçador (corpo do jogador no Modo Monstro).
const int OBJECT_PLAYER_PANTS = 32; // Calça do caçador.

#endif
