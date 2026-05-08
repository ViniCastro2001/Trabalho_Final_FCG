#include "scene.h"

// Retorna os blocos retangulares estáticos do cenário.
const std::vector<BoxObstacle>& GetSceneObstacles()
{
    // Cada BoxObstacle é um bloco retangular do cenário.
    // Formato: { center, size }
    // center = posição do centro do bloco no mundo (x, y, z)
    // size = tamanho do bloco (largura X, altura Y, profundidade Z)
    // Para blocos apoiados no chão, usar center.y = size.y / 2.
    // Estes mesmos valores serão usados para desenhar e para colisão.
    static std::vector<BoxObstacle> obstacles = {
        // Bloco central
        { glm::vec3(0.0f, 1.25f, -5.0f), glm::vec3(3.0f, 2.5f, 1.0f) },

        // Bloco esquerdo
        { glm::vec3(-4.0f, 1.5f, -2.0f), glm::vec3(1.5f, 3.0f, 3.0f) },

        // Bloco direito
        { glm::vec3(4.0f, 1.0f, -2.0f), glm::vec3(1.5f, 2.0f, 3.0f) },

        // Parede longa ao fundo
        { glm::vec3(0.0f, 1.75f, -9.0f), glm::vec3(8.0f, 3.5f, 1.0f) },

        // Bloco baixo para testar depois colisão do corpo do jogador
        { glm::vec3(0.0f, 0.5f, -1.5f), glm::vec3(2.5f, 1.0f, 0.8f) }
    };

    return obstacles;
}

// Retorna os itens coletáveis do cenário.
std::vector<Collectible>& GetSceneCollectibles()
{
    // Cada Collectible tem { center, radius, collected }.
    // center = posição no mundo
    // radius = distância necessária para coletar
    // collected = se já foi coletado ou não
    static std::vector<Collectible> collectibles = {
        // Coletável 1
        { glm::vec3(-2.0f, 0.4f, -4.0f), 0.45f, false },

        // Coletável 2
        { glm::vec3(2.0f, 0.4f, -4.0f), 0.45f, false },

        // Coletável 3
        { glm::vec3(0.0f, 0.4f, -7.0f), 0.45f, false }
    };

    return collectibles;
}

// Retorna a zona segura/final do jogo.
const SafeZone& GetSafeZone()
{
    // Formato: { center, size }
    // Para ficar apoiada no chão, usamos center.y = size.y / 2.
    static SafeZone safe_zone = {
        glm::vec3(0.0f, 2.5f, -11.5f),
        glm::vec3(2.0f, 5.0f, 2.0f)
    };

    return safe_zone;
}