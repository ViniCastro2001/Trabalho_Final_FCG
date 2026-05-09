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
        // =========================
        // CASAS DO LADO ESQUERDO
        // =========================

        // Casa 1 esquerda
        { glm::vec3(-10.0f, 2.0f, -8.0f),  glm::vec3(6.0f, 4.0f, 6.0f) },

        // Casa 2 esquerda
        { glm::vec3(-10.0f, 2.0f, -22.0f), glm::vec3(6.0f, 4.0f, 6.0f) },

        // Casa 3 esquerda
        { glm::vec3(-10.0f, 2.0f, -36.0f), glm::vec3(6.0f, 4.0f, 6.0f) },

        // Casa 4 esquerda
        { glm::vec3(-10.0f, 2.0f, -50.0f), glm::vec3(6.0f, 4.0f, 6.0f) },

        // =========================
        // CASAS DO LADO DIREITO
        // =========================

        // Casa 1 direita
        { glm::vec3(10.0f, 2.0f, -12.0f), glm::vec3(6.0f, 4.0f, 6.0f) },

        // Casa 2 direita
        { glm::vec3(10.0f, 2.0f, -26.0f), glm::vec3(6.0f, 4.0f, 6.0f) },

        // Casa 3 direita
        { glm::vec3(10.0f, 2.0f, -40.0f), glm::vec3(6.0f, 4.0f, 6.0f) },

        // Casa 4 direita
        { glm::vec3(10.0f, 2.0f, -54.0f), glm::vec3(6.0f, 4.0f, 6.0f) },

        // =========================
        // GARAGENS / ANEXOS
        // =========================

        // Garagem esquerda perto da rua
        { glm::vec3(-6.5f, 1.5f, -22.0f), glm::vec3(2.5f, 3.0f, 3.5f) },

        // Garagem direita perto da rua
        { glm::vec3(6.5f, 1.5f, -26.0f), glm::vec3(2.5f, 3.0f, 3.5f) },

        // Garagem esquerda no fundo
        { glm::vec3(-6.5f, 1.5f, -50.0f), glm::vec3(2.5f, 3.0f, 3.5f) },

        // Garagem direita no fundo
        { glm::vec3(6.5f, 1.5f, -54.0f), glm::vec3(2.5f, 3.0f, 3.5f) },

        // =========================
        // BARREIRA FINAL DO MAPA
        // =========================

        // Limite de fundo do mapa
        { glm::vec3(0.0f, 2.0f, -64.0f), glm::vec3(28.0f, 4.0f, 1.0f) }
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
        // Coletável 1 - lado esquerdo no início
        { glm::vec3(-6.5f, 0.4f, -9.0f), 0.45f, false },

        // Coletável 2 - lado direito no início
        { glm::vec3(6.5f, 0.4f, -14.0f), 0.45f, false },

        // Coletável 3 - lado esquerdo no meio
        { glm::vec3(-6.5f, 0.4f, -30.0f), 0.45f, false },

        // Coletável 4 - lado direito no meio/fundo
        { glm::vec3(6.5f, 0.4f, -44.0f), 0.45f, false },

        // Coletável 5 - mais ao fundo
        { glm::vec3(0.0f, 0.4f, -58.0f), 0.45f, false }
    };

    return collectibles;
}

// Retorna a zona segura/final do jogo.
const SafeZone& GetSafeZone()
{
    // Formato: { center, size }
    // Para ficar apoiada no chão, usamos center.y = size.y / 2.
    static SafeZone safe_zone = {
        glm::vec3(0.0f, 5.0f, -60.0f),
        glm::vec3(8.0f, 10.0f, 4.0f)
    };

    return safe_zone;
}