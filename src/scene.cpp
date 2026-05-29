#include "scene.h"

// Retorna os blocos retangulares estaticos do cenario.
const std::vector<BoxObstacle>& GetSceneObstacles()
{
    static std::vector<BoxObstacle> obstacles = {
        // Fileiras principais de predios do campus.
        { glm::vec3(-11.0f, 3.20f, -18.0f), glm::vec3(17.0f, 6.4f, 12.0f) },
        { glm::vec3(12.0f, 3.20f, -18.0f), glm::vec3(17.0f, 6.4f, 12.0f) },
        { glm::vec3(-11.0f, 3.20f, -39.0f), glm::vec3(17.0f, 6.4f, 12.0f) },
        { glm::vec3(12.0f, 3.20f, -39.0f), glm::vec3(17.0f, 6.4f, 12.0f) },
        { glm::vec3(-11.0f, 3.20f, -60.0f), glm::vec3(17.0f, 6.4f, 12.0f) },
        { glm::vec3(12.0f, 3.20f, -60.0f), glm::vec3(17.0f, 6.4f, 12.0f) },
        { glm::vec3(-11.0f, 3.20f, -81.0f), glm::vec3(17.0f, 6.4f, 12.0f) },
        { glm::vec3(12.0f, 3.20f, -81.0f), glm::vec3(17.0f, 6.4f, 12.0f) },

        // Predios laterais e blocos de entrada.
        // Os 3 predios compridos da direita sao ocos: corredor caminhavel ao longo de Z.
        // Cada um vira 2 paredes laterais + 4 ombreiras de porta (topo e base), deixando
        // o miolo (x:[29.5,34.5]) e o vao central (x:[30.75,33.25]) livres.
        // Predio A (z -9..-31)
        { glm::vec3(28.75f, 3.0f, -20.0f),  glm::vec3(1.5f,  6.0f, 22.0f) },
        { glm::vec3(35.25f, 3.0f, -20.0f),  glm::vec3(1.5f,  6.0f, 22.0f) },
        { glm::vec3(30.125f, 3.0f, -9.5f),  glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(33.875f, 3.0f, -9.5f),  glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(30.125f, 3.0f, -30.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(33.875f, 3.0f, -30.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        // Predio B (z -40..-56)
        { glm::vec3(28.75f, 3.0f, -48.0f),  glm::vec3(1.5f,  6.0f, 16.0f) },
        { glm::vec3(35.25f, 3.0f, -48.0f),  glm::vec3(1.5f,  6.0f, 16.0f) },
        { glm::vec3(30.125f, 3.0f, -40.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(33.875f, 3.0f, -40.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(30.125f, 3.0f, -55.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(33.875f, 3.0f, -55.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        // Predio C (z -64..-80)
        { glm::vec3(28.75f, 3.0f, -72.0f),  glm::vec3(1.5f,  6.0f, 16.0f) },
        { glm::vec3(35.25f, 3.0f, -72.0f),  glm::vec3(1.5f,  6.0f, 16.0f) },
        { glm::vec3(30.125f, 3.0f, -64.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(33.875f, 3.0f, -64.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(30.125f, 3.0f, -79.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(33.875f, 3.0f, -79.5f), glm::vec3(1.25f, 6.0f, 1.0f) },
        { glm::vec3(-14.0f, 3.20f, -96.0f), glm::vec3(16.0f, 6.4f, 12.0f) },
        { glm::vec3(3.0f, 3.20f, -97.0f), glm::vec3(12.0f, 6.4f, 9.0f) },
        { glm::vec3(-12.0f, 3.40f, -5.0f), glm::vec3(11.0f, 6.8f, 8.0f) },
        { glm::vec3(13.0f, 3.40f, -5.0f), glm::vec3(11.0f, 6.8f, 8.0f) },

        // Carros no estacionamento esquerdo.
        { glm::vec3(-25.6f, 0.60f, -8.0f), glm::vec3(2.3f, 1.2f, 4.2f) },
        { glm::vec3(-16.8f, 0.60f, -8.0f), glm::vec3(2.3f, 1.2f, 4.2f) },
        { glm::vec3(-25.6f, 0.60f, -16.8f), glm::vec3(2.3f, 1.2f, 4.2f) },
        { glm::vec3(-16.8f, 0.60f, -16.8f), glm::vec3(2.3f, 1.2f, 4.2f) },
        { glm::vec3(-25.6f, 0.60f, -25.6f), glm::vec3(2.3f, 1.2f, 4.2f) },
        { glm::vec3(-16.8f, 0.60f, -25.6f), glm::vec3(2.3f, 1.2f, 4.2f) },
        { glm::vec3(-25.6f, 0.60f, -34.4f), glm::vec3(2.3f, 1.2f, 4.2f) },
        { glm::vec3(-16.8f, 0.60f, -34.4f), glm::vec3(2.3f, 1.2f, 4.2f) },
        { glm::vec3(-25.6f, 0.60f, -43.2f), glm::vec3(2.3f, 1.2f, 4.2f) },
        { glm::vec3(-16.8f, 0.60f, -43.2f), glm::vec3(2.3f, 1.2f, 4.2f) },

        // Carros rotacionados nos estacionamentos da entrada e do fundo.
        { glm::vec3(-18.0f, 0.60f, 2.0f), glm::vec3(4.2f, 1.2f, 2.3f) },
        { glm::vec3(-8.5f, 0.60f, 2.0f), glm::vec3(4.2f, 1.2f, 2.3f) },
        { glm::vec3(1.0f, 0.60f, 2.0f), glm::vec3(4.2f, 1.2f, 2.3f) },
        { glm::vec3(10.5f, 0.60f, 2.0f), glm::vec3(4.2f, 1.2f, 2.3f) },
        { glm::vec3(8.0f, 0.60f, -88.0f), glm::vec3(4.2f, 1.2f, 2.3f) },
        { glm::vec3(17.2f, 0.60f, -88.0f), glm::vec3(4.2f, 1.2f, 2.3f) },
        { glm::vec3(26.4f, 0.60f, -88.0f), glm::vec3(4.2f, 1.2f, 2.3f) },
        { glm::vec3(35.6f, 0.60f, -88.0f), glm::vec3(4.2f, 1.2f, 2.3f) },

        // Troncos de arvores internas.
        { glm::vec3(-38.0f, 1.2f, 8.0f), glm::vec3(0.9f, 2.4f, 0.9f) },
        { glm::vec3(39.0f, 1.2f, 6.0f), glm::vec3(0.9f, 2.4f, 0.9f) },
        { glm::vec3(-38.0f, 1.2f, -13.0f), glm::vec3(0.9f, 2.4f, 0.9f) },
        { glm::vec3(39.0f, 1.2f, -15.0f), glm::vec3(0.9f, 2.4f, 0.9f) },
        { glm::vec3(-38.0f, 1.2f, -34.0f), glm::vec3(0.9f, 2.4f, 0.9f) },
        { glm::vec3(39.0f, 1.2f, -36.0f), glm::vec3(0.9f, 2.4f, 0.9f) },
        { glm::vec3(-38.0f, 1.2f, -55.0f), glm::vec3(0.9f, 2.4f, 0.9f) },
        { glm::vec3(39.0f, 1.2f, -57.0f), glm::vec3(0.9f, 2.4f, 0.9f) },
        { glm::vec3(-38.0f, 1.2f, -76.0f), glm::vec3(0.9f, 2.4f, 0.9f) },
        { glm::vec3(39.0f, 1.2f, -78.0f), glm::vec3(0.9f, 2.4f, 0.9f) },

        // Troncos dos pinheiros de borda.
        { glm::vec3(-50.0f, 1.8f, 18.0f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(-57.0f, 1.6f, 15.4f), glm::vec3(0.9f, 3.2f, 0.9f) },
        { glm::vec3(50.5f, 1.8f, 16.6f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(57.0f, 1.6f, 14.0f), glm::vec3(0.9f, 3.2f, 0.9f) },
        { glm::vec3(-50.0f, 1.8f, -10.8f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(-57.0f, 1.6f, -13.4f), glm::vec3(0.9f, 3.2f, 0.9f) },
        { glm::vec3(50.5f, 1.8f, -12.2f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(57.0f, 1.6f, -14.8f), glm::vec3(0.9f, 3.2f, 0.9f) },
        { glm::vec3(-50.0f, 1.8f, -39.6f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(-57.0f, 1.6f, -42.2f), glm::vec3(0.9f, 3.2f, 0.9f) },
        { glm::vec3(50.5f, 1.8f, -41.0f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(57.0f, 1.6f, -43.6f), glm::vec3(0.9f, 3.2f, 0.9f) },
        { glm::vec3(-50.0f, 1.8f, -68.4f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(-57.0f, 1.6f, -71.0f), glm::vec3(0.9f, 3.2f, 0.9f) },
        { glm::vec3(50.5f, 1.8f, -69.8f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(57.0f, 1.6f, -72.4f), glm::vec3(0.9f, 3.2f, 0.9f) },
        { glm::vec3(-50.0f, 1.8f, -97.2f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(-57.0f, 1.6f, -99.8f), glm::vec3(0.9f, 3.2f, 0.9f) },
        { glm::vec3(50.5f, 1.8f, -98.6f), glm::vec3(1.0f, 3.6f, 1.0f) },
        { glm::vec3(57.0f, 1.6f, -101.2f), glm::vec3(0.9f, 3.2f, 0.9f) },

        // Muros externos do mapa, depois da faixa de arvores.
        { glm::vec3(-84.0f, 2.8f, -53.0f), glm::vec3(2.0f, 6.4f, 218.0f) },
        { glm::vec3(84.0f, 2.8f, -53.0f), glm::vec3(2.0f, 6.4f, 218.0f) },
        { glm::vec3(0.0f, 2.8f, 56.0f), glm::vec3(168.0f, 6.4f, 2.0f) },
        { glm::vec3(0.0f, 2.8f, -162.0f), glm::vec3(168.0f, 6.4f, 2.0f) }
    };

    static bool generated_tree_collisions = false;

    if (!generated_tree_collisions)
    {
        auto add_tree_collision = [&](glm::vec3 position, float scale)
        {
            obstacles.push_back({
                glm::vec3(position.x, 1.20f * scale, position.z),
                glm::vec3(0.62f * scale, 2.40f * scale, 0.62f * scale)
            });
        };

        auto add_pine_collision = [&](glm::vec3 position, float scale)
        {
            obstacles.push_back({
                glm::vec3(position.x, 1.65f * scale, position.z),
                glm::vec3(0.72f * scale, 3.30f * scale, 0.72f * scale)
            });
        };

        auto add_spawn_safe_pine_collision = [&](glm::vec3 position, float scale)
        {
            float dx = position.x - 0.0f;
            float dz = position.z - 27.0f;

            if (dx*dx + dz*dz < 7.0f * 7.0f)
                return;

            add_pine_collision(position, scale);
        };

        for (int i = 0; i < 16; ++i)
        {
            add_tree_collision(glm::vec3(-38.0f, 0.0f, 8.0f - i * 7.0f), 1.45f);
            add_tree_collision(glm::vec3(39.0f, 0.0f, 6.0f - i * 7.0f), 1.45f);
        }

        // Fileiras internas entre os prédios: mantemos apenas árvores alternadas
        // (espaçamento ~7m) para deixar brechas largas o suficiente para a navegação
        // do Pé Grande atravessar sem ficar travado entre troncos.
        for (int i = 0; i < 12; i += 2)
        {
            add_tree_collision(glm::vec3(-18.0f + i * 3.8f, 0.0f, -27.5f), 0.90f);
            add_tree_collision(glm::vec3(-18.0f + i * 3.8f, 0.0f, -69.5f), 0.90f);

            // Pula as árvores da fileira da direita que cairiam dentro dos
            // prédios-corredor (footprint x:[28,36]).
            float xr = 5.0f + i * 3.3f;
            if (xr < 27.0f || xr > 37.0f)
            {
                add_tree_collision(glm::vec3(xr, 0.0f, -27.5f), 0.82f);
                add_tree_collision(glm::vec3(xr, 0.0f, -69.5f), 0.82f);
            }
        }

        for (int i = 0; i < 31; ++i)
        {
            float z = 46.0f - i * 6.6f;
            float a = ((i * 37) % 11 - 5) * 0.42f;
            float b = ((i * 19) % 13 - 6) * 0.36f;

            add_pine_collision(glm::vec3(-50.0f + a, 0.0f, z + b), 1.70f);
            add_pine_collision(glm::vec3(-58.0f - b, 0.0f, z - 3.2f + a), 1.45f);
            add_pine_collision(glm::vec3(-66.5f + b * 0.7f, 0.0f, z + 2.0f - a), 1.55f);
            add_pine_collision(glm::vec3(-75.5f - a * 0.8f, 0.0f, z - 4.6f - b), 1.35f);
            add_pine_collision(glm::vec3(-62.0f + b * 0.6f, 0.0f, z - 1.3f), 1.30f);
            add_pine_collision(glm::vec3(-71.5f + a * 0.6f, 0.0f, z + 3.4f), 1.42f);

            add_pine_collision(glm::vec3(50.5f - b, 0.0f, z - 1.8f + a), 1.70f);
            add_pine_collision(glm::vec3(58.0f + a, 0.0f, z - 4.6f - b), 1.45f);
            add_pine_collision(glm::vec3(66.5f - a * 0.7f, 0.0f, z + 1.2f + b), 1.55f);
            add_pine_collision(glm::vec3(75.5f + b * 0.8f, 0.0f, z - 3.2f + a), 1.35f);
            add_pine_collision(glm::vec3(62.0f - b * 0.6f, 0.0f, z - 2.4f), 1.30f);
            add_pine_collision(glm::vec3(71.5f - a * 0.6f, 0.0f, z + 2.9f), 1.42f);
        }

        for (int i = 0; i < 24; ++i)
        {
            float x = -77.0f + i * 6.8f;
            float a = ((i * 23) % 11 - 5) * 0.44f;
            float b = ((i * 31) % 13 - 6) * 0.34f;

            add_spawn_safe_pine_collision(glm::vec3(x + a, 0.0f, 50.5f + b), 1.30f);
            add_spawn_safe_pine_collision(glm::vec3(x + 3.5f - b, 0.0f, 46.5f + a), 1.22f);
            add_spawn_safe_pine_collision(glm::vec3(x + 1.4f + b, 0.0f, 42.0f - a), 1.55f);
            add_spawn_safe_pine_collision(glm::vec3(x + 4.6f + a, 0.0f, 36.3f + b), 1.30f);
            add_spawn_safe_pine_collision(glm::vec3(x - 0.6f - b, 0.0f, 27.0f + a), 1.55f);
            add_spawn_safe_pine_collision(glm::vec3(x + 3.1f + a, 0.0f, 19.2f - b), 1.35f);
            add_spawn_safe_pine_collision(glm::vec3(x + b, 0.0f, -122.0f - a), 1.60f);
            add_spawn_safe_pine_collision(glm::vec3(x + 3.6f - a, 0.0f, -130.0f + b), 1.35f);
            add_spawn_safe_pine_collision(glm::vec3(x + 1.5f + b, 0.0f, -139.0f - a), 1.50f);
            add_spawn_safe_pine_collision(glm::vec3(x + 4.8f + a, 0.0f, -150.0f + b), 1.32f);
            add_spawn_safe_pine_collision(glm::vec3(x - 0.4f - b, 0.0f, -156.7f - a), 1.25f);
        }

        generated_tree_collisions = true;
    }

    return obstacles;
}

const std::vector<BoxObstacle>& GetSceneLightOccluders()
{
    // Tetos dos 3 prédios-corredor da direita. Cobrem todo o footprint
    // (x:[28,36]) no topo das paredes, bloqueando a luz da rua/sol que
    // entraria por cima do corredor. Não entram em GetSceneObstacles() para
    // não fechar a passagem (a colisão ignora Y e bloquearia o miolo).
    static const std::vector<BoxObstacle> light_occluders = {
        { glm::vec3(32.0f, 6.1f, -20.0f), glm::vec3(8.0f, 0.4f, 22.0f) },
        { glm::vec3(32.0f, 6.1f, -48.0f), glm::vec3(8.0f, 0.4f, 16.0f) },
        { glm::vec3(32.0f, 6.1f, -72.0f), glm::vec3(8.0f, 0.4f, 16.0f) }
    };

    return light_occluders;
}

std::vector<Collectible>& GetSceneCollectibles()
{
    static std::vector<Collectible> collectibles = {
        { glm::vec3(-23.0f, 0.4f, -6.0f), 1.35f, false },
        { glm::vec3(0.0f, 0.4f, -29.0f), 1.35f, false },
        { glm::vec3(25.0f, 0.4f, -49.0f), 1.35f, false },
        { glm::vec3(-2.0f, 0.4f, -71.0f), 1.35f, false },
        { glm::vec3(15.0f, 0.4f, -91.5f), 1.35f, false }
    };

    return collectibles;
}

const SafeZone& GetSafeZone()
{
    static SafeZone safe_zone = {
        glm::vec3(25.0f, 5.0f, -98.0f),
        glm::vec3(8.0f, 10.0f, 6.0f)
    };

    return safe_zone;
}

const std::vector<LightPost>& GetSceneLightPosts()
{
    // Cor amarelo-quente padrão das lâmpadas de sódio; vermelha junto da zona segura.
    static const glm::vec3 warm_yellow = glm::vec3(1.00f, 0.82f, 0.45f);
    static const glm::vec3 safe_red    = glm::vec3(1.00f, 0.30f, 0.20f);
    static const glm::vec3 bulb_offset = glm::vec3(0.0f, 4.6f, 0.0f);

    static const std::vector<LightPost> light_posts = {
        { glm::vec3( -4.57f, 0.0f,  -6.63f), bulb_offset, warm_yellow, 28.0f, 28.0f },
        { glm::vec3(  4.35f, 0.0f, -31.73f), bulb_offset, warm_yellow, 28.0f, 28.0f },
        { glm::vec3( -4.29f, 0.0f, -46.81f), bulb_offset, warm_yellow, 28.0f, 28.0f },
        { glm::vec3(-24.44f, 0.0f, -68.39f), bulb_offset, warm_yellow, 28.0f, 28.0f },
        { glm::vec3( 16.42f, 0.0f, -95.51f), bulb_offset, safe_red,    28.0f, 28.0f },
        { glm::vec3( 28.50f, 0.0f, -59.97f), bulb_offset, warm_yellow, 28.0f, 28.0f },
        { glm::vec3(-35.55f, 0.0f, -103.25f), bulb_offset, warm_yellow, 28.0f, 28.0f },
        { glm::vec3( 29.48f, 0.0f,  -7.34f), bulb_offset, warm_yellow, 28.0f, 28.0f },
        { glm::vec3( 21.44f, 0.0f, -13.40f), bulb_offset, warm_yellow, 28.0f, 28.0f }
    };

    return light_posts;
}
