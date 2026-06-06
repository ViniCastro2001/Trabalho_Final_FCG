#include "player_model.h"

#include <cmath>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrices.h"
#include "object_ids.h"
#include "gpu_render.h"

void DrawPlayerModel(glm::vec3 position, float yaw, float time_seconds, float movement_intensity, float recoil_timer, float recoil_duration)
{
    (void)recoil_timer;
    (void)recoil_duration;

    // Respiração/idle: oscilação leve do tronco e dos ombros.
    float breath = sin(time_seconds * 1.8f);
    float body_bob = breath * 0.012f;
    float shoulder_roll = breath * 0.03f;

    // Balanço de caminhada (fica ~0 enquanto o jogador está parado observando).
    float walk_cycle = time_seconds * 7.2f;
    float leg_swing = sin(walk_cycle) * 0.42f * movement_intensity;

    glm::mat4 base_model = Matrix_Translate(position.x, position.y, position.z)
        * Matrix_Rotate_Y(yaw);

    auto DrawPart = [](const char* object_name, int object_id, glm::mat4 model)
    {
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, object_id);
        DrawVirtualObject(object_name);
    };

    // Pernas (calça jeans): cada perna pivota no QUADRIL (topo da perna), e não
    // no seu centro. Pivotar no centro fazia o pé girar para trás enquanto a coxa
    // ia para frente (movimento "invertido"). Com o pivô no quadril, todo o
    // segmento descreve o arco de passada correto.
    const float hip_y    = 0.85f;          // altura da articulação do quadril
    const float leg_len  = 0.86f;          // comprimento do segmento da perna
    const float leg_half = leg_len * 0.5f; // metade, para pendurar a perna sob o quadril

    // Os quadris balançam em oposição: quando uma perna avança, a outra recua.
    glm::mat4 left_hip = base_model
        * Matrix_Translate(-0.12f, hip_y, 0.0f)
        * Matrix_Rotate_X(leg_swing);
    glm::mat4 right_hip = base_model
        * Matrix_Translate(0.12f, hip_y, 0.0f)
        * Matrix_Rotate_X(-leg_swing);

    // Coxa/canela: pende a partir do quadril.
    DrawPart("the_cube", OBJECT_PLAYER_PANTS,
        left_hip * Matrix_Translate(0.0f, -leg_half, 0.0f) * Matrix_Scale(0.16f, leg_len, 0.20f));
    DrawPart("the_cube", OBJECT_PLAYER_PANTS,
        right_hip * Matrix_Translate(0.0f, -leg_half, 0.0f) * Matrix_Scale(0.16f, leg_len, 0.20f));

    // Botas: presas à ponta de cada perna, acompanhando o balanço (antes ficavam
    // cravadas no chão, descoladas das pernas).
    DrawPart("the_cube", OBJECT_WEAPON_WOOD,
        left_hip * Matrix_Translate(0.0f, -0.80f, 0.04f) * Matrix_Scale(0.17f, 0.10f, 0.28f));
    DrawPart("the_cube", OBJECT_WEAPON_WOOD,
        right_hip * Matrix_Translate(0.0f, -0.80f, 0.04f) * Matrix_Scale(0.17f, 0.10f, 0.28f));

    // Quadril/cinto.
    DrawPart("the_cube", OBJECT_WEAPON_WOOD,
        base_model * Matrix_Translate(0.0f, 0.86f + body_bob, 0.0f) * Matrix_Scale(0.40f, 0.10f, 0.24f));

    // Tronco (jaqueta de caçador).
    DrawPart("the_cube", OBJECT_PLAYER_CLOTH,
        base_model
            * Matrix_Translate(0.0f, 1.18f + body_bob, 0.0f)
            * Matrix_Rotate_Z(shoulder_roll * 0.4f)
            * Matrix_Scale(0.44f, 0.64f, 0.26f));

    // Ombros.
    DrawPart("the_cube", OBJECT_PLAYER_CLOTH,
        base_model
            * Matrix_Translate(0.0f, 1.46f + body_bob, 0.0f)
            * Matrix_Rotate_Z(shoulder_roll)
            * Matrix_Scale(0.56f, 0.16f, 0.28f));

    // Pescoço.
    DrawPart("the_sphere", OBJECT_HANDS,
        base_model * Matrix_Translate(0.0f, 1.55f + body_bob, 0.0f) * Matrix_Scale(0.08f, 0.08f, 0.08f));

    // Cabeça.
    DrawPart("the_sphere", OBJECT_HANDS,
        base_model * Matrix_Translate(0.0f, 1.66f + body_bob, 0.0f) * Matrix_Scale(0.135f, 0.155f, 0.135f));

    // Boné/aba (mesma cor da jaqueta).
    DrawPart("the_cube", OBJECT_PLAYER_CLOTH,
        base_model * Matrix_Translate(0.0f, 1.75f + body_bob, 0.0f) * Matrix_Scale(0.145f, 0.05f, 0.145f));
    DrawPart("the_cube", OBJECT_PLAYER_CLOTH,
        base_model * Matrix_Translate(0.0f, 1.73f + body_bob, 0.14f) * Matrix_Scale(0.135f, 0.025f, 0.10f));

    // --- Braços segurando a espingarda à frente do peito ---
    // Braço direito (na coronha): desce do ombro e vem à frente.
    DrawPart("the_cube", OBJECT_PLAYER_CLOTH,
        base_model
            * Matrix_Translate(0.24f, 1.28f + body_bob, 0.10f)
            * Matrix_Rotate_X(-0.85f)
            * Matrix_Scale(0.12f, 0.46f, 0.13f));

    // Braço esquerdo (apoia o cano), mais à frente.
    DrawPart("the_cube", OBJECT_PLAYER_CLOTH,
        base_model
            * Matrix_Translate(-0.20f, 1.30f + body_bob, 0.24f)
            * Matrix_Rotate_X(-1.15f)
            * Matrix_Scale(0.11f, 0.44f, 0.12f));

    // Mãos (pele) na arma.
    DrawPart("the_sphere", OBJECT_HANDS,
        base_model * Matrix_Translate(0.16f, 1.16f + body_bob, 0.34f) * Matrix_Scale(0.075f, 0.065f, 0.075f));
    DrawPart("the_sphere", OBJECT_HANDS,
        base_model * Matrix_Translate(-0.14f, 1.16f + body_bob, 0.50f) * Matrix_Scale(0.075f, 0.065f, 0.075f));

    // --- Espingarda na horizontal, à frente do peito, apontando +Z ---
    float gun_y = 1.20f + body_bob;
    // Coronha (madeira), perto do corpo.
    DrawPart("the_cube", OBJECT_WEAPON_WOOD,
        base_model * Matrix_Translate(0.10f, gun_y, 0.18f) * Matrix_Scale(0.05f, 0.08f, 0.20f));
    // Mecanismo (metal).
    DrawPart("the_cube", OBJECT_WEAPON_METAL,
        base_model * Matrix_Translate(0.04f, gun_y + 0.01f, 0.36f) * Matrix_Scale(0.05f, 0.06f, 0.18f));
    // Cano (metal), longo, à frente.
    DrawPart("the_cube", OBJECT_WEAPON_METAL,
        base_model * Matrix_Translate(-0.02f, gun_y + 0.02f, 0.66f) * Matrix_Scale(0.028f, 0.028f, 0.40f));
    // Guarda-mato (madeira) sob o cano.
    DrawPart("the_cube", OBJECT_WEAPON_WOOD,
        base_model * Matrix_Translate(-0.02f, gun_y - 0.03f, 0.52f) * Matrix_Scale(0.04f, 0.04f, 0.20f));
}
