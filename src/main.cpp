//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Computação Gráfica e Visualização I
//               Prof. Eduardo Gastal
//
//     CÓDIGO BASE PARA O TRABALHO FINAL
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <set>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"
#include "scene.h"
#include "game_state.h"
#include "bigfoot.h"
#include "debug_flags.h"
#include "audio.h"
#include "collisions.h"
#include "upgrades.h"

//headers do jogo
#include "camera.h"
#include "player.h"
#include "map_view.h"


// Estrutura que representa um modelo geométrico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .
struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor lê o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando objetos do arquivo \"%s\"...\n", filename);

        // Se basepath == NULL, então setamos basepath como o dirname do
        // filename, para que os arquivos MTL sejam corretamente carregados caso
        // estejam no mesmo diretório dos arquivos OBJ.
        std::string fullpath(filename);
        std::string dirname;
        if (basepath == NULL)
        {
            auto i = fullpath.find_last_of("/");
            if (i != std::string::npos)
            {
                dirname = fullpath.substr(0, i+1);
                basepath = dirname.c_str();
            }
        }

        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        for (size_t shape = 0; shape < shapes.size(); ++shape)
        {
            if (shapes[shape].name.empty())
            {
                fprintf(stderr,
                        "*********************************************\n"
                        "Erro: Objeto sem nome dentro do arquivo '%s'.\n"
                        "Veja https://www.inf.ufrgs.br/~eslgastal/fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
                        "*********************************************\n",
                    filename);
                throw std::runtime_error("Objeto sem nome.");
            }
            printf("- Objeto '%s'\n", shapes[shape].name.c_str());
        }

        printf("OK.\n");
    }
};


// Declaração de funções utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
void BuildTrianglesAndAddToVirtualScene(ObjModel*); // Constrói representação de um ObjModel como malha de triângulos para renderização
void ComputeNormals(ObjModel* model); // Computa normais de um ObjModel, caso não existam.
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
void LoadTextureImage(const char* filename); // Função que carrega imagens de textura
void DrawVirtualObject(const char* object_name); // Desenha um objeto armazenado em g_VirtualScene
void DrawBigfootModel(glm::vec3 position, float yaw, float time_seconds, float death_progress, float movement_intensity); // Desenha o inimigo com primitivas simples.
void DrawBigfootHealthBar(GLFWwindow* window, float health_ratio); // Desenha a barra grafica de vida do inimigo.
void DrawMainMenuPanel(GLFWwindow* window, int selected_row_in_view, int visible_rows);
void DrawUpgradeShopOverlay(GLFWwindow* window);
void DrawConfirmResetOverlay(GLFWwindow* window);
void DrawFirstPersonWeapon(glm::vec4 camera_position, glm::vec4 camera_view, glm::vec4 camera_up, float recoil_timer, float recoil_duration);
void DrawCampusSurface(glm::vec3 center, glm::vec3 size, float yaw, int material);
void DrawCampusBox(glm::vec3 center, glm::vec3 size, float yaw, int material);
void DrawCampusBuilding(glm::vec3 center, glm::vec3 size, float yaw);
void DrawCampusTree(glm::vec3 position, float scale);
void DrawCampusPine(glm::vec3 position, float scale);
void DrawCampusCar(glm::vec3 position, float yaw, int body_material);
void DrawCampusMap();
void DrawSafeZoneBeacon(const SafeZone& safe_zone, float time_seconds);
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void PrintObjModelInfo(ObjModel*); // Função para debugging

// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
void TextRendering_InitRect();
void TextRendering_DrawRectPx(GLFWwindow* window, int x_px, int y_px, int w_px, int h_px, float r, float g, float b, float a);
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
float TextRendering_StringWidth(GLFWwindow* window, const std::string &str, float scale = 1.0f);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Funções abaixo renderizam como texto na janela OpenGL algumas matrizes e
// outras informações do programa. Definidas após main().
void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void WindowFocusCallback(GLFWwindow* window, int focused);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


bool AllCollectiblesCollected();
bool IsPlayerInsideSafeZone(glm::vec4 player_position);
void RandomizeCollectibleSpawns();
#if MAP_VIEW_ENABLED
void DrawMapViewOverlay(GLFWwindow* window, glm::vec4 player_pos, glm::vec4 player_view, glm::vec3 bigfoot_pos, float bigfoot_yaw);
#endif
void ResetGame(bool start_playing = true);
int ShotHitsBigfoot(glm::vec4 shot_origin, glm::vec4 shot_direction);
bool RayHitsBox(glm::vec3 ray_origin, glm::vec3 ray_direction, BoxObstacle box, float max_distance);
bool RayHitsBox(glm::vec3 ray_origin, glm::vec3 ray_direction, BoxObstacle box, float max_distance, float* hit_distance);
bool ShotBlockedByScene(glm::vec3 ray_origin, glm::vec3 ray_direction, float max_distance);
std::vector<BoxObstacle> GetBigfootShotBoxes();
float UpdateBigfootFacing(size_t index, glm::vec3 current_position, float delta_t);
bool AreAllBigfootsDead();
int GetBigfootCountForLevel(int level);
int GetBigfootTierLevel(int level);
void UpdateSpectatorController(float delta_t);
glm::vec3 GetSpectatorMovementDirection();
bool IsSpectatorRunning();
bool ShouldSpectatorShoot();
static glm::vec3 Normalize3(glm::vec3 v);


// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    std::string  name;        // Nome do objeto
    size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t       num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;
};

// Abaixo definimos variáveis globais utilizadas em várias funções do código.

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;

// Pilha que guardará as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse
bool g_MouseCaptured = false;
bool g_FirstCapturedMouseFrame = true;
double g_LastCursorPosX = 0.0;
double g_LastCursorPosY = 0.0;

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 3.5f; // Distância da câmera para a origem

// Variáveis que controlam rotação do antebraço
float g_ForearmAngleZ = 0.0f;
float g_ForearmAngleX = 0.0f;

// Variáveis que controlam translação do torso
float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;

// Variável que controla o tipo de projeção utilizada: perspectiva ou ortográfica.
bool g_UsePerspectiveProjection = true;

// Variável que controla se o texto informativo será mostrado na tela.
bool g_ShowInfoText = true;

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_object_id_uniform;
GLint g_bbox_min_uniform;
GLint g_bbox_max_uniform;
#if MAP_VIEW_ENABLED
GLint g_map_view_uniform;
#endif

// Número de texturas carregadas pela função LoadTextureImage()
GLuint g_NumLoadedTextures = 0;


// Variáveis globais do jogo

// Câmera em primeira pessoa 
Camera g_Camera;
Player g_Player(&g_Camera);

// Pé Grande
struct BigfootInstance
{
    Bigfoot enemy;
    float render_yaw = 0.0f;
    glm::vec3 previous_position = glm::vec3(0.0f, 0.0f, 0.0f);
    bool has_previous_position = false;
    float movement_intensity = 0.0f;
    float death_timer = 0.0f;
    bool death_animation_started = false;
};

std::vector<BigfootInstance> g_Bigfoots;

// Estado atual do jogo.
GameState g_GameState;

// Mostra a esfera de debug da hitbox do Pé Grande.
bool g_DrawBigfootHitSphere = false;

// Controla se a tecla de tiro já estava pressionada no frame anterior.
bool g_ShootButtonWasPressed = false;

#if MAP_VIEW_ENABLED
MapView g_MapView;
#endif

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

bool g_PlayerInvisibleToBigfoot = false;
bool g_InfiniteAdrenalineCheat = false;
bool g_SpectatorMode = false;
bool g_SpectatorWantsShoot = false;
bool g_SpectatorRunning = false;
float g_SpectatorAutoAdvanceTimer = -1.0f;
float g_SpectatorAutoRetryTimer = -1.0f;
glm::vec3 g_SpectatorMovementDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_SpectatorLastPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_SpectatorDetourDirection = glm::vec3(0.0f, 0.0f, 0.0f);
bool g_SpectatorHasLastPosition = false;
float g_SpectatorStuckTimer = 0.0f;
float g_SpectatorDetourTimer = 0.0f;
float g_ShotgunRecoilTimer = 0.0f;
float g_PlayerFallTimer = 0.0f;
bool g_PlayerFallAnimationStarted = false;
float g_CameraBobTimer = 0.0f;
float g_CameraBobAmount = 0.0f;
const float SHOTGUN_RECOIL_DURATION = 1.5f;
float g_ShotgunCurrentRecoilDuration = SHOTGUN_RECOIL_DURATION;
const int BASE_COLLECTIBLE_COUNT = 5;
const float PRESTIGE_SPEED_STEP = 0.08f;
const float PRESTIGE_HEALTH_STEP = 0.15f;
const float PRESTIGE_SPEED_CAP = 1.45f;
const char* PRESTIGE_SAVE_PATH = "../../data/save_prestige.txt";
int g_SelectedPrestigeLevel = 0;
int g_RunPrestigeLevel = 0;
int g_HighestUnlockedPrestigeLevel = 0;
bool g_PrestigeAwardedForCurrentRun = false;
bool g_LastWinUnlockedNewLevel = false;

// Estado das telas de loja de upgrades e confirmacao de reset.
int  g_SelectedUpgradeRow = 0;            // 0..COUNT-1 = upgrade; depois reset opcional; ultima linha = Fechar.
bool g_LegacyResetUnlocked = false;       // Tornado true quando g_HighestUnlockedPrestigeLevel >= 3.
const int LEGACY_RESET_PRESTIGE_REQUIRED = 3;
bool g_ResetConfirmationFromShop = false;

bool AllCollectiblesCollected()
{
    std::vector<Collectible>& collectibles = GetSceneCollectibles();

    for (const Collectible& collectible : collectibles)
    {
        if (!collectible.collected)
            return false;
    }

    return true;
}

bool AreAllBigfootsDead()
{
    if (g_Bigfoots.empty())
        return false;

    for (const BigfootInstance& instance : g_Bigfoots)
    {
        if (!instance.enemy.IsDead())
            return false;
    }

    return true;
}

int ClampPrestigeLevel(int level)
{
    if (level < 0)
        return 0;

    if (level > g_HighestUnlockedPrestigeLevel)
        return g_HighestUnlockedPrestigeLevel;

    return level;
}

float GetPrestigeSpeedMultiplierForLevel(int level)
{
    int tier_level = GetBigfootTierLevel(level);
    float multiplier = 1.0f + PRESTIGE_SPEED_STEP * (float)tier_level;

    if (multiplier > PRESTIGE_SPEED_CAP)
        multiplier = PRESTIGE_SPEED_CAP;

    return multiplier;
}

float GetPrestigeHealthMultiplierForLevel(int level)
{
    int tier_level = GetBigfootTierLevel(level);
    return 1.0f + PRESTIGE_HEALTH_STEP * (float)tier_level;
}

int GetBigfootCountForLevel(int level)
{
    (void)level;
    return 1;
}

int GetBigfootTierLevel(int level)
{
    if (level < 0)
        return 0;

    return level % 10;
}

int GetPrestigeCollectibleCountForLevel(int level)
{
    if (level <= 5)
        return BASE_COLLECTIBLE_COUNT + level;

    return BASE_COLLECTIBLE_COUNT + 5 + (level - 5 + 1) / 2;
}

int GetPrestigeCollectibleCount()
{
    return GetPrestigeCollectibleCountForLevel(g_RunPrestigeLevel);
}

void SavePrestigeMemory()
{
    std::ofstream save_file(PRESTIGE_SAVE_PATH, std::ios::trunc);

    if (!save_file)
        return;

    save_file << g_HighestUnlockedPrestigeLevel << "\n";
    save_file << GetRawCoins() << "\n";

    for (int i = 0; i < (int)UpgradeId::COUNT; ++i)
    {
        if (i > 0) save_file << " ";
        save_file << GetUpgradeLevel((UpgradeId)i);
    }
    save_file << "\n";
}

void LoadPrestigeMemory()
{
    std::ifstream save_file(PRESTIGE_SAVE_PATH);
    int saved_highest_level = 0;

    if (save_file >> saved_highest_level && saved_highest_level > 0)
        g_HighestUnlockedPrestigeLevel = saved_highest_level;
    else
        g_HighestUnlockedPrestigeLevel = 0;

    g_SelectedPrestigeLevel = g_HighestUnlockedPrestigeLevel;
    g_RunPrestigeLevel = g_SelectedPrestigeLevel;

    // Linha 2: pontos (opcional, default 0).
    int saved_coins = 0;
    if (save_file >> saved_coins)
        SetRawCoins(saved_coins);
    else
        SetRawCoins(0);

    // Linha 3: niveis dos upgrades, separados por espaco.
    for (int i = 0; i < (int)UpgradeId::COUNT; ++i)
    {
        int level = 0;
        if (save_file >> level)
            SetUpgradeLevel((UpgradeId)i, level);
        else
            SetUpgradeLevel((UpgradeId)i, 0);
    }

    g_LegacyResetUnlocked = (g_HighestUnlockedPrestigeLevel >= LEGACY_RESET_PRESTIGE_REQUIRED);
}

void PerformLegacyReset()
{
    // Reset puro: zera dificuldade, pontos e upgrades.
    g_HighestUnlockedPrestigeLevel = 0;
    g_SelectedPrestigeLevel = 0;
    g_RunPrestigeLevel = 0;
    ResetUpgradesState();
    g_LegacyResetUnlocked = false;
    g_SelectedUpgradeRow = 0;
    SavePrestigeMemory();
}

void SelectPrestigeLevel(int delta)
{
    g_SelectedPrestigeLevel = ClampPrestigeLevel(g_SelectedPrestigeLevel + delta);
}

void ConfigureCollectiblesForPrestige()
{
    std::vector<Collectible>& collectibles = GetSceneCollectibles();
    int target_count = GetPrestigeCollectibleCount();

    while ((int)collectibles.size() < target_count)
    {
        collectibles.push_back({ glm::vec3(0.0f, 0.4f, 0.0f), 1.35f, false });
    }

    if ((int)collectibles.size() > target_count)
        collectibles.resize(target_count);
}

void SetGameWon()
{
    g_GameState.status = GameStatus::Won;

    if (!g_PrestigeAwardedForCurrentRun)
    {
        g_LastWinUnlockedNewLevel = false;

        if (g_RunPrestigeLevel >= g_HighestUnlockedPrestigeLevel)
        {
            g_HighestUnlockedPrestigeLevel = g_RunPrestigeLevel + 1;
            g_SelectedPrestigeLevel = g_HighestUnlockedPrestigeLevel;
            g_LastWinUnlockedNewLevel = true;
            g_LegacyResetUnlocked = (g_HighestUnlockedPrestigeLevel >= LEGACY_RESET_PRESTIGE_REQUIRED);
            SavePrestigeMemory();
        }

        g_PrestigeAwardedForCurrentRun = true;
    }

    if (g_SpectatorMode)
    {
        g_SelectedPrestigeLevel = ClampPrestigeLevel(g_RunPrestigeLevel + 1);
        g_SpectatorAutoAdvanceTimer = 2.0f;
    }
}

bool IsPlayerInsideSafeZone(glm::vec4 player_position)
{
    const SafeZone& safe_zone = GetSafeZone();

    float min_x = safe_zone.center.x - safe_zone.size.x / 2.0f;
    float max_x = safe_zone.center.x + safe_zone.size.x / 2.0f;

    float min_z = safe_zone.center.z - safe_zone.size.z / 2.0f;
    float max_z = safe_zone.center.z + safe_zone.size.z / 2.0f;

    return player_position.x >= min_x &&
           player_position.x <= max_x &&
           player_position.z >= min_z &&
           player_position.z <= max_z;
}

static float RandomFloat(float min_value, float max_value)
{
    float t = (float)rand() / (float)RAND_MAX;
    return min_value + (max_value - min_value) * t;
}

static bool IsCollectibleSpawnFree(glm::vec3 position, float radius, const std::vector<glm::vec3>& placed)
{
    const std::vector<BoxObstacle>& obstacles = GetSceneObstacles();

    for (const BoxObstacle& obstacle : obstacles)
    {
        if (CheckCircleBoxCollisionXZ(position, radius + 0.80f, obstacle))
            return false;
    }

    const SafeZone& safe_zone = GetSafeZone();
    BoxObstacle safe_zone_box = { safe_zone.center, safe_zone.size };

    if (CheckCircleBoxCollisionXZ(position, radius + 1.50f, safe_zone_box))
        return false;

    glm::vec3 player_spawn = glm::vec3(0.0f, 1.7f, 27.0f);
    float dx_player = position.x - player_spawn.x;
    float dz_player = position.z - player_spawn.z;

    if (dx_player*dx_player + dz_player*dz_player < 14.0f * 14.0f)
        return false;

    for (const glm::vec3& other : placed)
    {
        float dx = position.x - other.x;
        float dz = position.z - other.z;

        if (dx*dx + dz*dz < 12.0f * 12.0f)
            return false;
    }

    return true;
}

void RandomizeCollectibleSpawns()
{
    struct SpawnZone
    {
        float min_x;
        float max_x;
        float min_z;
        float max_z;
    };

    static const SpawnZone zones[] = {
        { -3.0f,   3.0f, -91.0f,  12.0f }, // avenida central
        { -33.0f, -28.5f, -106.0f, 18.0f }, // rua esquerda
        {  22.0f,  27.0f, -84.0f,  12.0f }, // rua direita
        { -27.0f,  23.0f, -10.5f,  -5.5f }, // travessa da entrada
        { -27.0f,  23.0f, -94.5f, -89.5f }, // travessa do fundo
        { -40.0f, -34.0f, -82.0f,   4.0f }, // faixa aberta oeste
        {  35.0f,  44.0f, -84.0f,   4.0f }, // faixa aberta leste
        { -18.0f,  18.0f, -74.0f, -66.0f }, // corredor entre blocos
        { -18.0f,  18.0f, -53.0f, -47.0f },
        { -18.0f,  18.0f, -32.0f, -26.0f }
    };

    static const glm::vec3 fallback_positions[] = {
        glm::vec3(-23.0f, 0.4f, -6.0f),
        glm::vec3(0.0f, 0.4f, -29.0f),
        glm::vec3(25.0f, 0.4f, -49.0f),
        glm::vec3(-2.0f, 0.4f, -71.0f),
        glm::vec3(15.0f, 0.4f, -91.5f)
    };

    std::vector<Collectible>& collectibles = GetSceneCollectibles();
    std::vector<glm::vec3> placed;
    const int zone_count = (int)(sizeof(zones) / sizeof(zones[0]));
    const int fallback_count = (int)(sizeof(fallback_positions) / sizeof(fallback_positions[0]));

    for (size_t i = 0; i < collectibles.size(); ++i)
    {
        Collectible& collectible = collectibles[i];
        collectible.collected = false;

        bool found = false;

        for (int attempt = 0; attempt < 700; ++attempt)
        {
            const SpawnZone& zone = zones[rand() % zone_count];
            glm::vec3 candidate = glm::vec3(
                RandomFloat(zone.min_x, zone.max_x),
                0.4f,
                RandomFloat(zone.min_z, zone.max_z)
            );

            if (!IsCollectibleSpawnFree(candidate, collectible.radius, placed))
                continue;

            collectible.center = candidate;
            placed.push_back(candidate);
            found = true;
            break;
        }

        if (!found)
        {
            glm::vec3 fallback = fallback_positions[i % fallback_count];
            collectible.center = fallback;
            placed.push_back(fallback);
        }
    }
}

void ResetGame(bool start_playing)
{
    g_Camera = Camera();
    g_RunPrestigeLevel = ClampPrestigeLevel(g_SelectedPrestigeLevel);
    g_Bigfoots.clear();

    BigfootInstance instance;
    instance.enemy.ApplyDifficultyMultipliers(
        GetPrestigeSpeedMultiplierForLevel(g_RunPrestigeLevel),
        GetPrestigeHealthMultiplierForLevel(g_RunPrestigeLevel)
    );
    g_Bigfoots.push_back(instance);

    g_GameState.status = start_playing ? GameStatus::Playing : GameStatus::MainMenu;
    g_PrestigeAwardedForCurrentRun = false;
    g_LastWinUnlockedNewLevel = false;
    g_ShootButtonWasPressed = false;
#if MAP_VIEW_ENABLED
    if (g_MapView.IsActive()) g_MapView.Toggle();
#endif
    g_ShotgunRecoilTimer = 0.0f;
    g_ShotgunCurrentRecoilDuration = SHOTGUN_RECOIL_DURATION;
    g_Player.ResetEnergyBoost();

    g_PlayerInvisibleToBigfoot = false;
    g_InfiniteAdrenalineCheat = false;
    g_SpectatorMode = false;
    g_SpectatorWantsShoot = false;
    g_SpectatorRunning = false;
    g_SpectatorAutoAdvanceTimer = -1.0f;
    g_SpectatorAutoRetryTimer = -1.0f;
    g_SpectatorMovementDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    g_SpectatorLastPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    g_SpectatorDetourDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    g_SpectatorHasLastPosition = false;
    g_SpectatorStuckTimer = 0.0f;
    g_SpectatorDetourTimer = 0.0f;
    g_PlayerFallTimer = 0.0f;
    g_PlayerFallAnimationStarted = false;
    g_CameraBobTimer = 0.0f;
    g_CameraBobAmount = 0.0f;

    ConfigureCollectiblesForPrestige();
    RandomizeCollectibleSpawns();
}

bool RayHitsBox(glm::vec3 ray_origin, glm::vec3 ray_direction, BoxObstacle box, float max_distance)
{
    return RayHitsBox(ray_origin, ray_direction, box, max_distance, NULL);
}

bool RayHitsBox(glm::vec3 ray_origin, glm::vec3 ray_direction, BoxObstacle box, float max_distance, float* hit_distance)
{
    float box_min_x = box.center.x - box.size.x / 2.0f;
    float box_max_x = box.center.x + box.size.x / 2.0f;

    float box_min_y = box.center.y - box.size.y / 2.0f;
    float box_max_y = box.center.y + box.size.y / 2.0f;

    float box_min_z = box.center.z - box.size.z / 2.0f;
    float box_max_z = box.center.z + box.size.z / 2.0f;

    float t_min = 0.0f;
    float t_max = max_distance;

    // Teste no eixo X.
                                // Evita divisão por zero
    if (fabs(ray_direction.x) < 0.0001f)
    {
        if (ray_origin.x < box_min_x || ray_origin.x > box_max_x)
            return false;
    }
    else
    {
        float tx1 = (box_min_x - ray_origin.x) / ray_direction.x;
        float tx2 = (box_max_x - ray_origin.x) / ray_direction.x;

        if (tx1 > tx2)
            std::swap(tx1, tx2);

        t_min = std::max(t_min, tx1);
        t_max = std::min(t_max, tx2);

        if (t_min > t_max)
            return false;
    }

    // Teste no eixo Y.
    if (fabs(ray_direction.y) < 0.0001f)
    {
        if (ray_origin.y < box_min_y || ray_origin.y > box_max_y)
            return false;
    }
    else
    {
        float ty1 = (box_min_y - ray_origin.y) / ray_direction.y;
        float ty2 = (box_max_y - ray_origin.y) / ray_direction.y;

        if (ty1 > ty2)
            std::swap(ty1, ty2);

        t_min = std::max(t_min, ty1);
        t_max = std::min(t_max, ty2);

        if (t_min > t_max)
            return false;
    }

    // Teste no eixo Z.
    if (fabs(ray_direction.z) < 0.0001f)
    {
        if (ray_origin.z < box_min_z || ray_origin.z > box_max_z)
            return false;
    }
    else
    {
        float tz1 = (box_min_z - ray_origin.z) / ray_direction.z;
        float tz2 = (box_max_z - ray_origin.z) / ray_direction.z;

        if (tz1 > tz2)
            std::swap(tz1, tz2);

        t_min = std::max(t_min, tz1);
        t_max = std::min(t_max, tz2);

        if (t_min > t_max)
            return false;
    }

    bool hit = t_max >= 0.0f && t_min <= max_distance;

    if (hit && hit_distance != NULL)
        *hit_distance = t_min;

    return hit;
}

bool ShotBlockedByScene(glm::vec3 ray_origin, glm::vec3 ray_direction, float max_distance)
{
    const std::vector<BoxObstacle>& obstacles = GetSceneObstacles();

    for (const BoxObstacle& obstacle : obstacles)
    {
        if (RayHitsBox(ray_origin, ray_direction, obstacle, max_distance))
        {
            return true;
        }
    }

    return false;
}

static float NormalizeAngle(float angle)
{
    const float pi = 3.141592f;

    while (angle > pi)
        angle -= 2.0f * pi;

    while (angle < -pi)
        angle += 2.0f * pi;

    return angle;
}

static glm::vec3 NormalizeXZ(glm::vec3 v)
{
    v.y = 0.0f;
    float len = sqrt(v.x*v.x + v.z*v.z);

    if (len <= 0.0001f)
        return glm::vec3(0.0f, 0.0f, 0.0f);

    return v / len;
}

static glm::vec3 RotateXZ(glm::vec3 v, float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    return glm::vec3(
        v.x * c - v.z * s,
        0.0f,
        v.x * s + v.z * c
    );
}

static bool SpectatorDirectionBlocked(glm::vec3 start, glm::vec3 dir, float probe_distance)
{
    dir = NormalizeXZ(dir);

    if (dir.x == 0.0f && dir.z == 0.0f)
        return false;

    const float player_radius = 0.50f;
    const float step = 0.85f;
    const std::vector<BoxObstacle>& obstacles = GetSceneObstacles();

    for (float t = step; t <= probe_distance; t += step)
    {
        glm::vec3 sample = start + dir * t;

        for (const BoxObstacle& obstacle : obstacles)
        {
            if (CheckCircleBoxCollisionXZ(sample, player_radius, obstacle))
                return true;
        }
    }

    return false;
}

static float SpectatorClearDistance(glm::vec3 start, glm::vec3 dir, float max_distance)
{
    dir = NormalizeXZ(dir);

    if (dir.x == 0.0f && dir.z == 0.0f)
        return 0.0f;

    const float player_radius = 0.50f;
    const float step = 0.70f;
    const std::vector<BoxObstacle>& obstacles = GetSceneObstacles();

    for (float t = step; t <= max_distance; t += step)
    {
        glm::vec3 sample = start + dir * t;

        for (const BoxObstacle& obstacle : obstacles)
        {
            if (CheckCircleBoxCollisionXZ(sample, player_radius, obstacle))
                return t - step;
        }
    }

    return max_distance;
}

static glm::vec3 SpectatorChooseClearDirection(glm::vec3 start, glm::vec3 desired)
{
    desired = NormalizeXZ(desired);

    if (desired.x == 0.0f && desired.z == 0.0f)
        return desired;

    if (SpectatorClearDistance(start, desired, 4.8f) >= 4.2f)
        return desired;

    static const float kAngles[] = {
        0.35f, -0.35f,
        0.70f, -0.70f,
        1.05f, -1.05f,
        1.45f, -1.45f,
        1.85f, -1.85f,
        2.35f, -2.35f,
        3.141592f
    };

    glm::vec3 best_direction = desired;
    float best_score = -9999.0f;

    for (float angle : kAngles)
    {
        glm::vec3 candidate = NormalizeXZ(RotateXZ(desired, angle));
        float clear_distance = SpectatorClearDistance(start, candidate, 6.0f);
        float alignment = candidate.x * desired.x + candidate.z * desired.z;
        float score = clear_distance * 0.70f + alignment * 2.6f - fabs(angle) * 0.18f;

        if (score > best_score)
        {
            best_score = score;
            best_direction = candidate;
        }
    }

    return best_direction;
}

static bool SpectatorLineOfSight(glm::vec3 from, glm::vec3 to)
{
    glm::vec3 delta = to - from;
    float dist = sqrt(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);

    if (dist <= 0.0001f)
        return true;

    glm::vec3 dir = delta / dist;
    return !ShotBlockedByScene(from, dir, dist);
}

static bool SpectatorNearestLiveBigfoot(glm::vec3 from, glm::vec3* out_position, float* out_distance)
{
    bool found = false;
    float best_dist2 = 0.0f;

    for (const BigfootInstance& instance : g_Bigfoots)
    {
        if (instance.enemy.IsDead())
            continue;

        glm::vec3 position = instance.enemy.GetPosition();
        float dx = position.x - from.x;
        float dz = position.z - from.z;
        float dist2 = dx*dx + dz*dz;

        if (!found || dist2 < best_dist2)
        {
            found = true;
            best_dist2 = dist2;

            if (out_position != NULL)
                *out_position = position;
        }
    }

    if (found && out_distance != NULL)
        *out_distance = sqrt(best_dist2);

    return found;
}

static glm::vec3 SpectatorObjectiveTarget(glm::vec3 player_position)
{
    std::vector<Collectible>& collectibles = GetSceneCollectibles();
    bool found = false;
    float best_score = 0.0f;
    glm::vec3 best = glm::vec3(0.0f, 0.0f, 0.0f);

    for (const Collectible& collectible : collectibles)
    {
        if (collectible.collected)
            continue;

        float dx = collectible.center.x - player_position.x;
        float dz = collectible.center.z - player_position.z;
        float dist2 = dx*dx + dz*dz;
        glm::vec3 to_item = NormalizeXZ(collectible.center - player_position);
        float clear_distance = SpectatorClearDistance(player_position, to_item, 8.0f);
        float blocked_penalty = clear_distance < 5.5f ? 320.0f : 0.0f;
        float score = dist2 + blocked_penalty;

        if (!found || score < best_score)
        {
            found = true;
            best_score = score;
            best = collectible.center;
        }
    }

    if (found)
        return best;

    return GetSafeZone().center;
}

void UpdateSpectatorController(float delta_t)
{
    g_SpectatorWantsShoot = false;
    g_SpectatorRunning = false;
    g_SpectatorMovementDirection = glm::vec3(0.0f, 0.0f, 0.0f);

    if (!g_SpectatorMode || g_GameState.status != GameStatus::Playing)
        return;

    glm::vec4 camera_position = g_Camera.GetPosition();
    glm::vec3 player_position = glm::vec3(camera_position.x, camera_position.y, camera_position.z);
    glm::vec3 objective = SpectatorObjectiveTarget(player_position);
    glm::vec3 desired_move = objective - player_position;
    desired_move.y = 0.0f;

    glm::vec3 bigfoot_position;
    float bigfoot_distance = 0.0f;
    bool has_bigfoot = SpectatorNearestLiveBigfoot(player_position, &bigfoot_position, &bigfoot_distance);

    if (has_bigfoot && bigfoot_distance < 10.0f)
    {
        glm::vec3 away = NormalizeXZ(player_position - bigfoot_position);
        desired_move = NormalizeXZ(desired_move) * 0.75f + away * 1.20f;
    }

    glm::vec3 direct_move = NormalizeXZ(desired_move);

    if (g_SpectatorHasLastPosition)
    {
        glm::vec3 moved = player_position - g_SpectatorLastPosition;
        float moved_distance = sqrt(moved.x*moved.x + moved.z*moved.z);

        if ((direct_move.x != 0.0f || direct_move.z != 0.0f) && moved_distance < 0.035f)
            g_SpectatorStuckTimer += delta_t;
        else
            g_SpectatorStuckTimer -= delta_t * 1.6f;

        if (g_SpectatorStuckTimer < 0.0f)
            g_SpectatorStuckTimer = 0.0f;
    }
    else
    {
        g_SpectatorHasLastPosition = true;
    }

    g_SpectatorLastPosition = player_position;

    if (g_SpectatorDetourTimer > 0.0f)
        g_SpectatorDetourTimer -= delta_t;

    bool path_blocked_soon = SpectatorDirectionBlocked(player_position, direct_move, 2.8f);

    if ((g_SpectatorStuckTimer > 0.35f || path_blocked_soon) &&
        g_SpectatorDetourTimer <= 0.0f)
    {
        g_SpectatorDetourDirection = SpectatorChooseClearDirection(player_position, direct_move);
        g_SpectatorDetourTimer = g_SpectatorStuckTimer > 0.35f ? 1.35f : 0.75f;
    }

    if (g_SpectatorDetourTimer > 0.0f &&
        (g_SpectatorDetourDirection.x != 0.0f || g_SpectatorDetourDirection.z != 0.0f) &&
        !SpectatorDirectionBlocked(player_position, g_SpectatorDetourDirection, 1.7f))
    {
        g_SpectatorMovementDirection = g_SpectatorDetourDirection;
    }
    else
    {
        g_SpectatorMovementDirection = SpectatorChooseClearDirection(player_position, direct_move);
    }

    float objective_dx = objective.x - player_position.x;
    float objective_dz = objective.z - player_position.z;
    float objective_distance = sqrt(objective_dx*objective_dx + objective_dz*objective_dz);
    g_SpectatorRunning = objective_distance > 2.0f;

    glm::vec3 aim_target = objective + glm::vec3(0.0f, 1.0f, 0.0f);

    if (has_bigfoot)
    {
        glm::vec3 bigfoot_aim = bigfoot_position + glm::vec3(0.0f, 1.55f, 0.0f);
        bool can_see_bigfoot = bigfoot_distance < 58.0f &&
            SpectatorLineOfSight(player_position, bigfoot_aim);

        if (can_see_bigfoot)
        {
            aim_target = bigfoot_aim;

            glm::vec4 view = g_Camera.GetViewVector();
            glm::vec3 current_view = Normalize3(glm::vec3(view.x, view.y, view.z));
            glm::vec3 desired_view = Normalize3(bigfoot_aim - player_position);
            float aim_dot = current_view.x*desired_view.x + current_view.y*desired_view.y + current_view.z*desired_view.z;

            if (aim_dot > 0.990f)
                g_SpectatorWantsShoot = true;
        }
    }

    float turn_speed = 4.2f * delta_t;
    g_Camera.LookAt(aim_target, turn_speed);
}

glm::vec3 GetSpectatorMovementDirection()
{
    return g_SpectatorMovementDirection;
}

bool IsSpectatorRunning()
{
    return g_SpectatorRunning;
}

bool ShouldSpectatorShoot()
{
    return g_SpectatorWantsShoot;
}

static glm::vec3 RotateYaw(glm::vec3 v, float yaw)
{
    float c = cos(yaw);
    float s = sin(yaw);

    return glm::vec3(
        c * v.x + s * v.z,
        v.y,
        -s * v.x + c * v.z
    );
}

float UpdateBigfootFacing(size_t index, glm::vec3 current_position, float delta_t)
{
    if (index >= g_Bigfoots.size())
        return 0.0f;

    BigfootInstance& instance = g_Bigfoots[index];

    if (!instance.has_previous_position)
    {
        instance.previous_position = current_position;
        instance.has_previous_position = true;
        return instance.render_yaw;
    }

    glm::vec3 movement = current_position - instance.previous_position;
    movement.y = 0.0f;

    float movement_length = sqrt(movement.x*movement.x + movement.z*movement.z);

    float target_yaw = instance.enemy.GetFacingYaw();
    float yaw_delta = NormalizeAngle(target_yaw - instance.render_yaw);
    instance.render_yaw += yaw_delta * 0.22f;
    instance.render_yaw = NormalizeAngle(instance.render_yaw);

    // Intensidade de locomoção [0..1] usada para modular a animação dos membros.
    // Normalizamos pela velocidade de patrulha (~4.5 m/s) para que andar a passo
    // já produza amplitude próxima de 1.0, e a corrida não exagere o ciclo.
    float speed = (delta_t > 0.0001f) ? (movement_length / delta_t) : 0.0f;
    float target_intensity = speed / 4.5f;

    if (target_intensity > 1.0f)
        target_intensity = 1.0f;

    // Suavização exponencial frame-rate-independent. Constante de tempo ~0.12s
    // dá uma transição rápida o suficiente para o jogador notar a parada,
    // sem cortar bruscamente no meio de uma passada.
    float blend = 1.0f - exp(-delta_t * 8.0f);
    instance.movement_intensity += (target_intensity - instance.movement_intensity) * blend;

    instance.previous_position = current_position;

    return instance.render_yaw;
}

std::vector<BoxObstacle> GetBigfootShotBoxes()
{
    // Caixas locais que acompanham as partes do modelo procedural desenhado em DrawBigfootModel().
    std::vector<BoxObstacle> boxes;
    boxes.push_back({ glm::vec3(0.0f, 0.85f, 0.0f), glm::vec3(1.15f, 1.80f, 0.70f) }); // torso
    boxes.push_back({ glm::vec3(0.0f, 2.00f, 0.0f), glm::vec3(1.04f, 1.16f, 0.96f) }); // cabeca
    boxes.push_back({ glm::vec3(0.0f, 1.55f, 0.0f), glm::vec3(1.65f, 0.32f, 0.78f) }); // ombros
    boxes.push_back({ glm::vec3(-0.95f, 0.75f, 0.0f), glm::vec3(0.32f, 1.55f, 0.84f) }); // braco esquerdo
    boxes.push_back({ glm::vec3(0.95f, 0.75f, 0.0f), glm::vec3(0.32f, 1.55f, 0.84f) }); // braco direito
    boxes.push_back({ glm::vec3(-0.32f, -0.10f, 0.0f), glm::vec3(0.38f, 1.05f, 0.42f) }); // perna esquerda
    boxes.push_back({ glm::vec3(0.32f, -0.10f, 0.0f), glm::vec3(0.38f, 1.05f, 0.42f) }); // perna direita

    return boxes;
}

int ShotHitsBigfoot(glm::vec4 shot_origin, glm::vec4 shot_direction)
{
    // Convertemos de Vector4 para Vector3 porque o cálculo do tiro usa apenas x, y, z.

    glm::vec3 origin = glm::vec3(shot_origin.x, shot_origin.y, shot_origin.z);
    glm::vec3 direction = glm::vec3(shot_direction.x, shot_direction.y, shot_direction.z);

    // Garantimos que a direção do tiro tenha tamanho 1.
    float direction_len = sqrt(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);

    if (direction_len <= 0.0f)
        return -1;

    direction = direction / direction_len;

    // Centro da hitbox do Pé Grande.
    std::vector<BoxObstacle> hit_boxes = GetBigfootShotBoxes();
    int hit_index = -1;
    float closest_hit_distance = 1000.0f;

    for (size_t i = 0; i < g_Bigfoots.size(); ++i)
    {
        const BigfootInstance& instance = g_Bigfoots[i];

        if (instance.enemy.IsDead())
            continue;

        glm::vec3 bigfoot_position = instance.enemy.GetPosition();
        glm::vec3 local_origin = RotateYaw(origin - bigfoot_position, -instance.render_yaw);
        glm::vec3 local_direction = RotateYaw(direction, -instance.render_yaw);

        for (const BoxObstacle& hit_box : hit_boxes)
        {
            float hit_distance = 0.0f;

            if (RayHitsBox(local_origin, local_direction, hit_box, 1000.0f, &hit_distance) &&
                hit_distance < closest_hit_distance &&
                !ShotBlockedByScene(origin, direction, hit_distance))
            {
                hit_index = (int)i;
                closest_hit_distance = hit_distance;
            }
        }
    }

    return hit_index;

/*

    glm::vec3 bigfoot_position = g_Bigfoot.GetPosition();

    glm::vec3 local_origin = RotateYaw(origin - bigfoot_position, -g_BigfootRenderYaw);
    glm::vec3 local_direction = RotateYaw(direction, -g_BigfootRenderYaw);

    bool hit_bigfoot_boxes = false;
    float closest_hit_distance = 1000.0f;

    for (const BoxObstacle& hit_box : hit_boxes)
    {
        float hit_distance = 0.0f;

        if (RayHitsBox(local_origin, local_direction, hit_box, 1000.0f, &hit_distance) &&
            hit_distance < closest_hit_distance)
        {
            hit_bigfoot_boxes = true;
            closest_hit_distance = hit_distance;
        }
    }

    if (!hit_bigfoot_boxes)
        return false;

    if (ShotBlockedByScene(origin, direction, closest_hit_distance))
        return false;

    return true;

/*

    // Vetor da câmera até o Pé Grande.
    glm::vec3 origin_to_bigfoot = bigfoot_position - origin;

    // Projeta o Pé Grande na linha do tiro.
    float t = origin_to_bigfoot.x * direction.x
            + origin_to_bigfoot.y * direction.y
            + origin_to_bigfoot.z * direction.z;

    // Se t < 0, o Pé Grande está atrás da câmera.
    if (t < 0.0f)
        return false;

    // Ponto da linha do tiro mais próximo do centro do Pé Grande.
    glm::vec3 closest_point = origin + direction * t;

    // Distância entre esse ponto e o centro da hitbox.
    glm::vec3 difference = bigfoot_position - closest_point;

    float distance_squared = difference.x*difference.x
                           + difference.y*difference.y
                           + difference.z*difference.z;

    float hit_radius = GetBigfootShotRadius();

    bool hit_bigfoot = distance_squared <= hit_radius * hit_radius;

    if (!hit_bigfoot)
        return false;

    // Se há um bloco entre a câmera e o Pé Grande, o tiro é bloqueado.
    if (ShotBlockedByScene(origin, direction, t))
        return false;

*/
    return true;
}

void DrawBigfootModel(glm::vec3 position, float yaw, float time_seconds, float death_progress, float movement_intensity)
{
    if (death_progress < 0.0f)
        death_progress = 0.0f;

    if (death_progress > 1.0f)
        death_progress = 1.0f;

    death_progress = death_progress * death_progress * (3.0f - 2.0f * death_progress);

    float alive_weight = 1.0f - death_progress;

    // walk_weight: amplitude do ciclo de caminhada, modulada pela velocidade real
    // do Pé Grande. Quando ele está parado (ronda em espera, ataque, fim de fuga),
    // walk_weight tende a 0 e os membros voltam à posição neutra.
    float walk_weight = movement_intensity * alive_weight;

    float walk_cycle = time_seconds * 7.2f;
    float sway = sin(time_seconds * 5.0f) * 0.08f * alive_weight;
    float body_bob = fabs(sin(walk_cycle)) * 0.06f * walk_weight;
    float arm_swing = sin(walk_cycle) * 0.62f * walk_weight;
    float leg_swing = sin(walk_cycle) * 0.48f * walk_weight;
    float shoulder_roll = sin(walk_cycle + 1.570796f) * 0.08f * walk_weight;
    glm::mat4 base_model = Matrix_Translate(position.x, position.y, position.z)
        * Matrix_Rotate_Y(yaw)
        * Matrix_Translate(0.0f, -0.75f * death_progress, 0.35f * death_progress)
        * Matrix_Rotate_X(-1.45f * death_progress);

    auto DrawPart = [](const char* object_name, int object_id, glm::mat4 model)
    {
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, object_id);
        DrawVirtualObject(object_name);
    };

    // Corpo largo e alto, para trocar a leitura de "coelho" por uma silhueta ameaÃ§adora.
    DrawPart(
        "the_cube",
        OBJECT_BIGFOOT,
        base_model
            * Matrix_Translate(0.0f, 0.85f + sway + body_bob, 0.0f)
            * Matrix_Rotate_Z(shoulder_roll * 0.35f)
            * Matrix_Scale(1.15f, 1.80f, 0.70f)
    );

    // CabeÃ§a.
    DrawPart(
        "the_sphere",
        OBJECT_BIGFOOT,
        base_model
            * Matrix_Translate(0.0f, 2.00f + sway + body_bob, 0.0f)
            * Matrix_Rotate_Z(shoulder_roll * 0.25f)
            * Matrix_Scale(0.52f, 0.58f, 0.48f)
    );

    // Ombros.
    DrawPart(
        "the_cube",
        OBJECT_BIGFOOT,
        base_model
            * Matrix_Translate(0.0f, 1.55f + sway + body_bob, 0.0f)
            * Matrix_Rotate_Z(shoulder_roll)
            * Matrix_Scale(1.65f, 0.32f, 0.78f)
    );

    // BraÃ§os longos, com balanÃ§o simples.
    DrawPart(
        "the_cube",
        OBJECT_BIGFOOT,
        base_model
            * Matrix_Translate(-0.95f, 0.72f + sway + body_bob, arm_swing * 0.35f)
            * Matrix_Rotate_X(arm_swing)
            * Matrix_Rotate_Z(-0.10f)
            * Matrix_Scale(0.32f, 1.55f, 0.34f)
    );

    DrawPart(
        "the_cube",
        OBJECT_BIGFOOT,
        base_model
            * Matrix_Translate(0.95f, 0.72f + sway + body_bob, -arm_swing * 0.35f)
            * Matrix_Rotate_X(-arm_swing)
            * Matrix_Rotate_Z(0.10f)
            * Matrix_Scale(0.32f, 1.55f, 0.34f)
    );

    // Pernas.
    DrawPart(
        "the_cube",
        OBJECT_BIGFOOT,
        base_model
            * Matrix_Translate(-0.32f, -0.10f + body_bob * 0.3f, -leg_swing * 0.20f)
            * Matrix_Rotate_X(-leg_swing)
            * Matrix_Scale(0.38f, 1.05f, 0.42f)
    );

    DrawPart(
        "the_cube",
        OBJECT_BIGFOOT,
        base_model
            * Matrix_Translate(0.32f, -0.10f + body_bob * 0.3f, leg_swing * 0.20f)
            * Matrix_Rotate_X(leg_swing)
            * Matrix_Scale(0.38f, 1.05f, 0.42f)
    );

    // Olhos vermelhos: pequenos, mas legÃ­veis Ã  distÃ¢ncia.
    DrawPart(
        "the_sphere",
        OBJECT_BIGFOOT_EYES,
        base_model
            * Matrix_Translate(-0.18f, 2.08f + sway + body_bob, 0.43f)
            * Matrix_Scale(0.075f, 0.075f, 0.075f)
    );

    DrawPart(
        "the_sphere",
        OBJECT_BIGFOOT_EYES,
        base_model
            * Matrix_Translate(0.18f, 2.08f + sway + body_bob, 0.43f)
            * Matrix_Scale(0.075f, 0.075f, 0.075f)
    );
}

void DrawBigfootHealthBar(GLFWwindow* window, float health_ratio)
{
    if (health_ratio < 0.0f)
        health_ratio = 0.0f;

    if (health_ratio > 1.0f)
        health_ratio = 1.0f;

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    int bar_x = (int)(width * 0.30f);
    int bar_y = height - 48;
    int bar_width = (int)(width * 0.48f);
    int bar_height = 4;
    int fill_width = (int)(bar_width * health_ratio);

    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_SCISSOR_TEST);

    glScissor(bar_x - 1, bar_y - 1, bar_width + 2, bar_height + 2);
    glClearColor(0.10f, 0.015f, 0.020f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (fill_width > 0)
    {
        glScissor(bar_x, bar_y, fill_width, bar_height);
        glClearColor(1.0f, 0.10f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glDisable(GL_SCISSOR_TEST);

    TextRendering_PrintString(
        window,
        (g_Bigfoots.size() > 1) ? "PES GRANDES" : "PE GRANDE",
        -0.40f,
        0.865f,
        0.98f
    );
}

void DrawMainMenuPanel(GLFWwindow* window, int selected_row_in_view, int visible_rows)
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    int panel_x      = (int)(width  * 0.13f);
    int panel_y      = (int)(height * 0.12f);
    int panel_width  = (int)(width  * 0.74f);
    int panel_height = (int)(height * 0.76f);
    int border       = 3;

    auto NdcYToPixel = [&](float y_ndc)
    {
        return (int)((y_ndc + 1.0f) * 0.5f * (float)height);
    };

    // Borda externa em verde-floresta (semi-transparente).
    TextRendering_DrawRectPx(window,
        panel_x - border, panel_y - border,
        panel_width + border * 2, panel_height + border * 2,
        0.12f, 0.55f, 0.28f, 0.65f);

    // Fundo do painel (camada externa) ~50% transparente.
    TextRendering_DrawRectPx(window,
        panel_x, panel_y, panel_width, panel_height,
        0.018f, 0.022f, 0.027f, 0.50f);

    // Fundo interno mais claro com leve respiro de 10px.
    TextRendering_DrawRectPx(window,
        panel_x + 10, panel_y + 10,
        panel_width - 20, panel_height - 20,
        0.028f, 0.033f, 0.040f, 0.50f);

    // Faixa decorativa atras do titulo (um pouco mais opaca para destacar).
    int title_bar_top    = NdcYToPixel(0.36f);
    int title_bar_bottom = NdcYToPixel(0.20f);
    int title_bar_h      = title_bar_top - title_bar_bottom;

    if (title_bar_h > 0)
    {
        TextRendering_DrawRectPx(window,
            panel_x + 10, title_bar_bottom, panel_width - 20, title_bar_h,
            0.07f, 0.18f, 0.11f, 0.55f);

        TextRendering_DrawRectPx(window,
            panel_x + 10, title_bar_bottom - 2, panel_width - 20, 2,
            0.30f, 0.78f, 0.42f, 0.95f);
    }

    // Linha divisoria abaixo do cabecalho da tabela.
    int divider_y = NdcYToPixel(-0.115f);
    TextRendering_DrawRectPx(window,
        panel_x + 24, divider_y, panel_width - 48, 2,
        0.18f, 0.42f, 0.24f, 0.95f);

    // Barra de destaque atras da linha selecionada.
    if (selected_row_in_view >= 0 && visible_rows > 0)
    {
        float row_step    = 0.085f;
        float row_y_ndc   = -0.17f - (float)selected_row_in_view * row_step;
        int row_bottom_px = NdcYToPixel(row_y_ndc - 0.020f);
        int row_top_px    = NdcYToPixel(row_y_ndc + row_step - 0.030f);
        int row_height_px = row_top_px - row_bottom_px;

        if (row_height_px > 0)
        {
            TextRendering_DrawRectPx(window,
                panel_x + 18, row_bottom_px, panel_width - 36, row_height_px,
                0.10f, 0.30f, 0.16f, 0.55f);

            // Indicador vertical do lado esquerdo da linha selecionada.
            TextRendering_DrawRectPx(window,
                panel_x + 18, row_bottom_px, 5, row_height_px,
                0.32f, 0.86f, 0.46f, 1.0f);
        }
    }

}

static int GetShopRowCount()
{
    return (int)UpgradeId::COUNT + (g_LegacyResetUnlocked ? 1 : 0) + 1;
}

void DrawUpgradeShopOverlay(GLFWwindow* window)
{
    int total_rows = GetShopRowCount();
    int selected_row = g_SelectedUpgradeRow;

    if (selected_row < 0) selected_row = 0;
    if (selected_row >= total_rows) selected_row = total_rows - 1;

    DrawMainMenuPanel(window, selected_row, total_rows);

    TextRendering_PrintString(window, "Loja de Upgrades", -0.70f, 0.28f, 1.55f);

    char coins_text[64];
    snprintf(coins_text, sizeof(coins_text), "Pontos: %d", GetRawCoins());
    TextRendering_PrintString(window, coins_text, -0.70f, 0.14f, 1.0f);

    // Descricao do upgrade selecionado, logo abaixo do subtitulo.
    const char* hint = "";
    if (selected_row < (int)UpgradeId::COUNT)
        hint = GetUpgradeDescription((UpgradeId)selected_row);
    else if (g_LegacyResetUnlocked && selected_row == (int)UpgradeId::COUNT)
        hint = "Zera tudo: dificuldade, pontos e upgrades.";
    else
        hint = "Volta para o menu principal.";
    TextRendering_PrintString(window, hint, -0.70f, 0.05f, 0.88f);

    float col_marker = -0.58f;
    float col_name   = -0.50f;
    float col_level  = -0.20f;
    float col_value  =  0.00f;
    float col_cost   =  0.30f;
    float header_scale = 0.80f;
    float row_scale    = 0.78f;

    TextRendering_PrintString(window, "Upgrade",  col_name,  -0.08f, header_scale);
    TextRendering_PrintString(window, "Nivel",    col_level, -0.08f, header_scale);
    TextRendering_PrintString(window, "Valor",    col_value, -0.08f, header_scale);
    TextRendering_PrintString(window, "Custo",    col_cost,  -0.08f, header_scale);

    for (int i = 0; i < (int)UpgradeId::COUNT; ++i)
    {
        UpgradeId id = (UpgradeId)i;
        bool is_selected = (i == selected_row);
        const char* marker = is_selected ? ">>" : "  ";
        float row_y = -0.17f - i * 0.085f;

        int level = GetUpgradeLevel(id);
        int max_level = GetUpgradeMaxLevel(id);
        int cost = GetUpgradeCost(id);
        float value = GetUpgradeValue(id);

        char buf[64];

        TextRendering_PrintString(window, marker, col_marker, row_y, row_scale);
        TextRendering_PrintString(window, GetUpgradeName(id), col_name, row_y, row_scale);

        snprintf(buf, sizeof(buf), "%d/%d", level, max_level);
        TextRendering_PrintString(window, buf, col_level, row_y, row_scale);

        snprintf(buf, sizeof(buf), "%.2f", value);
        TextRendering_PrintString(window, buf, col_value, row_y, row_scale);

        if (level >= max_level)
            snprintf(buf, sizeof(buf), "MAX");
        else
            snprintf(buf, sizeof(buf), "%d", cost);
        TextRendering_PrintString(window, buf, col_cost, row_y, row_scale);
    }

    if (g_LegacyResetUnlocked)
    {
        int reset_row = (int)UpgradeId::COUNT;
        bool is_selected = (reset_row == selected_row);
        const char* marker = is_selected ? ">>" : "  ";
        float row_y = -0.17f - reset_row * 0.085f;

        TextRendering_PrintString(window, marker, col_marker, row_y, row_scale);
        TextRendering_PrintString(window, "Recomecar Jornada", col_name, row_y, row_scale);
    }

    int close_row = (int)UpgradeId::COUNT + (g_LegacyResetUnlocked ? 1 : 0);
    bool close_selected = (close_row == selected_row);
    const char* close_marker = close_selected ? ">>" : "  ";
    float close_y = -0.17f - close_row * 0.085f;

    TextRendering_PrintString(window, close_marker, col_marker, close_y, row_scale);
    TextRendering_PrintString(window, "Fechar", col_name, close_y, row_scale);

    TextRendering_PrintString(window,
        "[W/S] navegar  [ESPACO] selecionar",
        -0.55f, -0.70f, 0.92f);
}

void DrawConfirmResetOverlay(GLFWwindow* window)
{
    DrawMainMenuPanel(window, -1, 0);

    TextRendering_PrintString(window, "Resetar progresso?", -0.50f, 0.28f, 1.55f);
    TextRendering_PrintString(window, "Isso vai zerar permanentemente:", -0.70f, 0.10f, 1.05f);

    char buf[96];
    snprintf(buf, sizeof(buf), "  - Nivel maximo de dificuldade  (atual: %d)", g_HighestUnlockedPrestigeLevel + 1);
    TextRendering_PrintString(window, buf, -0.70f, -0.02f, 0.95f);

    snprintf(buf, sizeof(buf), "  - Pontos                       (atual: %d)", GetRawCoins());
    TextRendering_PrintString(window, buf, -0.70f, -0.12f, 0.95f);

    TextRendering_PrintString(window, "  - Todos os upgrades", -0.70f, -0.22f, 0.95f);

    TextRendering_PrintString(window, "Voce vai recomecar do zero. Esta acao nao pode ser desfeita.", -0.70f, -0.38f, 0.95f);

    TextRendering_PrintString(window,
        "[Y] confirmar     [N] cancelar",
        -0.40f, -0.62f, 1.05f);
}

static glm::vec3 Normalize3(glm::vec3 v)
{
    float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);

    if (len <= 0.0f)
        return glm::vec3(0.0f, 0.0f, 0.0f);

    return v / len;
}

static glm::vec3 Cross3(glm::vec3 a, glm::vec3 b)
{
    return glm::vec3(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    );
}

static glm::mat4 Matrix_WeaponPart(glm::vec3 origin, glm::vec3 right, glm::vec3 up, glm::vec3 forward, glm::vec3 local_position, glm::vec3 scale)
{
    glm::vec3 p = origin
        + right * local_position.x
        + up * local_position.y
        + forward * local_position.z;

    glm::mat4 basis = Matrix(
        right.x, up.x, forward.x, p.x,
        right.y, up.y, forward.y, p.y,
        right.z, up.z, forward.z, p.z,
        0.0f,    0.0f, 0.0f,      1.0f
    );

    return basis * Matrix_Scale(scale.x, scale.y, scale.z);
}

void DrawFirstPersonWeapon(glm::vec4 camera_position, glm::vec4 camera_view, glm::vec4 camera_up, float recoil_timer, float recoil_duration)
{
    glm::vec3 forward = Normalize3(glm::vec3(camera_view.x, camera_view.y, camera_view.z));
    glm::vec3 up = Normalize3(glm::vec3(camera_up.x, camera_up.y, camera_up.z));
    glm::vec3 right = Normalize3(Cross3(forward, up));

    if (right.x == 0.0f && right.y == 0.0f && right.z == 0.0f)
        right = glm::vec3(1.0f, 0.0f, 0.0f);

    up = Normalize3(Cross3(right, forward));

    if (recoil_duration <= 0.0f)
        recoil_duration = SHOTGUN_RECOIL_DURATION;

    float recoil_progress = recoil_timer / recoil_duration;

    if (recoil_progress < 0.0f)
        recoil_progress = 0.0f;

    if (recoil_progress > 1.0f)
        recoil_progress = 1.0f;

    float recoil = sin(recoil_progress * 3.141592f * 0.5f);

    if (recoil < 0.0f)
        recoil = 0.0f;

    if (recoil > 1.0f)
        recoil = 1.0f;

    glm::vec3 origin = glm::vec3(camera_position.x, camera_position.y, camera_position.z)
        + forward * (0.68f - 0.30f * recoil)
        + right * 0.34f
        - up * (0.42f - 0.14f * recoil);

    glm::mat4 recoil_rotation = Matrix_Rotate_X(-0.34f * recoil)
        * Matrix_Rotate_Z(-0.12f * recoil);

    auto DrawPart = [](const char* object_name, int object_id, glm::mat4 model)
    {
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, object_id);
        DrawVirtualObject(object_name);
    };

    glDisable(GL_DEPTH_TEST);

    if (g_Player.IsEnergyBoostActive())
    {
        float t = (float)glfwGetTime();

        // Halo: 4 orbes orbitando perto da boca do cano, raio amplo.
        const float TWO_PI = 6.2831853f;
        const float ORBIT_RADIUS = 0.22f;
        for (int i = 0; i < 4; ++i)
        {
            float angle = t * 3.2f + i * (TWO_PI / 4.0f);
            glm::vec3 offset(
                cos(angle) * ORBIT_RADIUS,
                0.06f + sin(angle) * ORBIT_RADIUS,
                0.94f);
            float s = 0.022f;
            glm::mat4 orb = Matrix_WeaponPart(
                origin, right, up, forward, offset,
                glm::vec3(s, s, s)) * recoil_rotation;
            DrawPart("the_sphere", OBJECT_SAFE_ZONE, orb);
        }
    }

    auto WeaponPart = [&](const char* object_name, int object_id, glm::vec3 local_position, glm::vec3 scale)
    {
        DrawPart(object_name, object_id, Matrix_WeaponPart(origin, right, up, forward, local_position, scale) * recoil_rotation);
    };

    auto AngledWeaponPart = [&](const char* object_name, int object_id, glm::vec3 local_position, glm::vec3 scale, float pitch, float roll)
    {
        glm::mat4 model = Matrix_WeaponPart(origin, right, up, forward, local_position, scale)
            * Matrix_Rotate_X(pitch)
            * Matrix_Rotate_Z(roll)
            * recoil_rotation;
        DrawPart(object_name, object_id, model);
    };

    // O eixo Z local aponta para frente: coronha perto da camera, boca do cano longe.
    AngledWeaponPart("the_cube", OBJECT_WEAPON_WOOD, glm::vec3(0.12f, -0.18f, -0.45f), glm::vec3(0.20f, 0.11f, 0.34f), -0.10f, -0.04f);
    WeaponPart("the_cube", OBJECT_WEAPON_METAL, glm::vec3(0.04f, -0.04f, -0.12f), glm::vec3(0.22f, 0.13f, 0.30f));
    AngledWeaponPart("the_cube", OBJECT_WEAPON_WOOD, glm::vec3(0.08f, -0.27f, -0.13f), glm::vec3(0.10f, 0.22f, 0.11f), 0.0f, -0.10f);
    WeaponPart("the_cube", OBJECT_WEAPON_METAL, glm::vec3(0.00f, -0.07f, 0.20f), glm::vec3(0.17f, 0.10f, 0.30f));
    WeaponPart("the_cube", OBJECT_WEAPON_WOOD, glm::vec3(0.00f, -0.17f, 0.38f), glm::vec3(0.22f, 0.09f, 0.28f));

    WeaponPart("the_cube", OBJECT_WEAPON_METAL, glm::vec3(-0.055f, 0.015f, 0.62f), glm::vec3(0.040f, 0.040f, 0.68f));
    WeaponPart("the_cube", OBJECT_WEAPON_METAL, glm::vec3(0.055f, 0.015f, 0.62f), glm::vec3(0.040f, 0.040f, 0.68f));
    WeaponPart("the_cube", OBJECT_WEAPON_METAL, glm::vec3(0.00f, -0.035f, 0.62f), glm::vec3(0.030f, 0.030f, 0.58f));
    WeaponPart("the_cube", OBJECT_WEAPON_METAL, glm::vec3(-0.055f, 0.015f, 0.98f), glm::vec3(0.055f, 0.055f, 0.045f));
    WeaponPart("the_cube", OBJECT_WEAPON_METAL, glm::vec3(0.055f, 0.015f, 0.98f), glm::vec3(0.055f, 0.055f, 0.045f));
    WeaponPart("the_cube", OBJECT_WEAPON_METAL, glm::vec3(0.00f, 0.075f, 0.24f), glm::vec3(0.045f, 0.020f, 0.38f));
    WeaponPart("the_sphere", OBJECT_WEAPON_ACCENT, glm::vec3(0.00f, 0.105f, 1.00f), glm::vec3(0.018f, 0.018f, 0.018f));
    WeaponPart("the_cube", OBJECT_WEAPON_ACCENT, glm::vec3(0.135f, -0.04f, 0.02f), glm::vec3(0.018f, 0.040f, 0.12f));
    WeaponPart("the_cube", OBJECT_WEAPON_METAL, glm::vec3(0.00f, -0.205f, -0.02f), glm::vec3(0.090f, 0.025f, 0.11f));

    WeaponPart("the_cube", OBJECT_HANDS, glm::vec3(-0.14f, -0.26f, 0.31f), glm::vec3(0.075f, 0.055f, 0.20f));
    WeaponPart("the_sphere", OBJECT_HANDS, glm::vec3(-0.14f, -0.21f, 0.30f), glm::vec3(0.105f, 0.080f, 0.12f));
    WeaponPart("the_cube", OBJECT_HANDS, glm::vec3(0.16f, -0.32f, -0.20f), glm::vec3(0.070f, 0.055f, 0.18f));
    WeaponPart("the_sphere", OBJECT_HANDS, glm::vec3(0.16f, -0.26f, -0.22f), glm::vec3(0.110f, 0.080f, 0.105f));

    glEnable(GL_DEPTH_TEST);
}

void DrawCampusSurface(glm::vec3 center, glm::vec3 size, float yaw, int material)
{
    glm::mat4 model = Matrix_Translate(center.x, center.y, center.z)
        * Matrix_Rotate_Y(yaw)
        * Matrix_Scale(size.x * 0.5f, 1.0f, size.z * 0.5f);

    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, material);
    DrawVirtualObject("the_plane");
}

void DrawCampusBox(glm::vec3 center, glm::vec3 size, float yaw, int material)
{
    glm::mat4 model = Matrix_Translate(center.x, center.y, center.z)
        * Matrix_Rotate_Y(yaw)
        * Matrix_Scale(size.x, size.y, size.z);

    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, material);
    DrawVirtualObject("the_cube");
}

void DrawCampusBuilding(glm::vec3 center, glm::vec3 size, float yaw)
{
    DrawCampusBox(center, size, yaw, OBJECT_WALL);
    DrawCampusBox(
        glm::vec3(center.x, center.y + size.y * 0.52f, center.z),
        glm::vec3(size.x * 1.06f, 0.22f, size.z * 1.06f),
        yaw,
        OBJECT_METAL_ROOF
    );

    DrawCampusBox(
        glm::vec3(center.x, center.y + size.y * 0.22f, center.z + size.z * 0.51f),
        glm::vec3(size.x * 0.78f, 0.38f, 0.08f),
        yaw,
        OBJECT_WINDOW
    );
}

void DrawCampusTree(glm::vec3 position, float scale)
{
    DrawCampusBox(
        glm::vec3(position.x, position.y + 1.25f * scale, position.z),
        glm::vec3(0.42f * scale, 2.50f * scale, 0.42f * scale),
        0.0f,
        OBJECT_TREE_TRUNK
    );

    DrawCampusBox(
        glm::vec3(position.x, position.y + 3.05f * scale, position.z),
        glm::vec3(2.40f * scale, 1.85f * scale, 2.40f * scale),
        0.0f,
        OBJECT_TREE_LEAVES
    );

    DrawCampusBox(
        glm::vec3(position.x + 0.45f * scale, position.y + 3.65f * scale, position.z - 0.35f * scale),
        glm::vec3(1.75f * scale, 1.55f * scale, 1.75f * scale),
        0.0f,
        OBJECT_TREE_LEAVES
    );
}

void DrawCampusPine(glm::vec3 position, float scale)
{
    DrawCampusBox(
        glm::vec3(position.x, position.y + 1.65f * scale, position.z),
        glm::vec3(0.42f * scale, 3.30f * scale, 0.42f * scale),
        0.0f,
        OBJECT_TREE_TRUNK
    );

    DrawCampusBox(
        glm::vec3(position.x, position.y + 2.35f * scale, position.z),
        glm::vec3(2.80f * scale, 0.85f * scale, 2.80f * scale),
        0.0f,
        OBJECT_TREE_LEAVES
    );

    DrawCampusBox(
        glm::vec3(position.x, position.y + 3.05f * scale, position.z),
        glm::vec3(2.20f * scale, 0.85f * scale, 2.20f * scale),
        0.0f,
        OBJECT_TREE_LEAVES
    );

    DrawCampusBox(
        glm::vec3(position.x, position.y + 3.70f * scale, position.z),
        glm::vec3(1.55f * scale, 0.80f * scale, 1.55f * scale),
        0.0f,
        OBJECT_TREE_LEAVES
    );

    DrawCampusBox(
        glm::vec3(position.x, position.y + 4.28f * scale, position.z),
        glm::vec3(0.90f * scale, 0.72f * scale, 0.90f * scale),
        0.0f,
        OBJECT_TREE_LEAVES
    );
}

void DrawCampusCar(glm::vec3 position, float yaw, int body_material)
{
    DrawCampusBox(
        glm::vec3(position.x, position.y + 0.36f, position.z),
        glm::vec3(2.15f, 0.72f, 4.15f),
        yaw,
        body_material
    );

    DrawCampusBox(
        glm::vec3(position.x, position.y + 0.94f, position.z - 0.25f),
        glm::vec3(1.45f, 0.46f, 1.65f),
        yaw,
        OBJECT_CAR_GLASS
    );

    DrawCampusBox(
        glm::vec3(position.x - 1.14f, position.y + 0.16f, position.z + 1.20f),
        glm::vec3(0.24f, 0.32f, 0.56f),
        yaw,
        OBJECT_SHOTGUN
    );

    DrawCampusBox(
        glm::vec3(position.x + 1.14f, position.y + 0.16f, position.z + 1.20f),
        glm::vec3(0.24f, 0.32f, 0.56f),
        yaw,
        OBJECT_SHOTGUN
    );
}

void DrawCampusMap()
{
    auto draw_spawn_safe_pine = [](glm::vec3 position, float scale)
    {
        float dx = position.x - 0.0f;
        float dz = position.z - 27.0f;

        if (dx*dx + dz*dz < 7.0f * 7.0f)
            return;

        DrawCampusPine(position, scale);
    };

    DrawCampusSurface(glm::vec3(0.0f, -0.03f, -53.0f), glm::vec3(164.0f, 1.0f, 214.0f), 0.0f, OBJECT_PLANE);

    // Roads and paved circulation, based on the top-view campus plan.
    DrawCampusSurface(glm::vec3(-31.0f, 0.01f, -45.0f), glm::vec3(6.0f, 1.0f, 118.0f), 0.0f, OBJECT_ROAD);
    DrawCampusSurface(glm::vec3(0.0f, 0.01f, -48.0f), glm::vec3(5.6f, 1.0f, 104.0f), 0.0f, OBJECT_ROAD);
    DrawCampusSurface(glm::vec3(25.0f, 0.01f, -49.0f), glm::vec3(4.6f, 1.0f, 102.0f), 0.0f, OBJECT_ROAD);
    DrawCampusSurface(glm::vec3(-2.5f, 0.012f, -8.0f), glm::vec3(52.0f, 1.0f, 4.6f), 0.0f, OBJECT_ROAD);
    DrawCampusSurface(glm::vec3(-2.5f, 0.012f, -92.0f), glm::vec3(52.0f, 1.0f, 4.6f), 0.0f, OBJECT_ROAD);
    DrawCampusSurface(glm::vec3(-11.0f, 0.012f, -29.0f), glm::vec3(29.0f, 1.0f, 3.2f), 0.0f, OBJECT_SIDEWALK);
    DrawCampusSurface(glm::vec3(12.5f, 0.012f, -29.0f), glm::vec3(21.0f, 1.0f, 3.2f), 0.0f, OBJECT_SIDEWALK);
    DrawCampusSurface(glm::vec3(-11.0f, 0.012f, -50.0f), glm::vec3(29.0f, 1.0f, 3.2f), 0.0f, OBJECT_SIDEWALK);
    DrawCampusSurface(glm::vec3(12.5f, 0.012f, -50.0f), glm::vec3(21.0f, 1.0f, 3.2f), 0.0f, OBJECT_SIDEWALK);
    DrawCampusSurface(glm::vec3(-11.0f, 0.012f, -71.0f), glm::vec3(29.0f, 1.0f, 3.2f), 0.0f, OBJECT_SIDEWALK);
    DrawCampusSurface(glm::vec3(12.5f, 0.012f, -71.0f), glm::vec3(21.0f, 1.0f, 3.2f), 0.0f, OBJECT_SIDEWALK);

    // Parking lots.
    DrawCampusSurface(glm::vec3(-21.2f, 0.014f, -25.6f), glm::vec3(14.5f, 1.0f, 42.0f), 0.0f, OBJECT_CONCRETE);
    DrawCampusSurface(glm::vec3(-3.8f, 0.014f, -2.0f), glm::vec3(38.0f, 1.0f, 12.0f), 0.0f, OBJECT_CONCRETE);
    DrawCampusSurface(glm::vec3(22.0f, 0.014f, -88.0f), glm::vec3(34.0f, 1.0f, 10.0f), 0.0f, OBJECT_CONCRETE);

    // Long classroom/warehouse rows.
    DrawCampusBuilding(glm::vec3(-11.0f, 3.20f, -18.0f), glm::vec3(17.0f, 6.4f, 12.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(12.0f, 3.20f, -18.0f), glm::vec3(17.0f, 6.4f, 12.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(-11.0f, 3.20f, -39.0f), glm::vec3(17.0f, 6.4f, 12.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(12.0f, 3.20f, -39.0f), glm::vec3(17.0f, 6.4f, 12.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(-11.0f, 3.20f, -60.0f), glm::vec3(17.0f, 6.4f, 12.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(12.0f, 3.20f, -60.0f), glm::vec3(17.0f, 6.4f, 12.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(-11.0f, 3.20f, -81.0f), glm::vec3(17.0f, 6.4f, 12.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(12.0f, 3.20f, -81.0f), glm::vec3(17.0f, 6.4f, 12.0f), 0.0f);

    // Right-side service row and front/admin blocks.
    DrawCampusBuilding(glm::vec3(32.0f, 3.00f, -20.0f), glm::vec3(8.0f, 6.0f, 22.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(32.0f, 3.00f, -48.0f), glm::vec3(8.0f, 6.0f, 16.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(32.0f, 3.00f, -72.0f), glm::vec3(8.0f, 6.0f, 16.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(-14.0f, 3.20f, -96.0f), glm::vec3(16.0f, 6.4f, 12.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(3.0f, 3.20f, -97.0f), glm::vec3(12.0f, 6.4f, 9.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(-12.0f, 3.40f, -5.0f), glm::vec3(11.0f, 6.8f, 8.0f), 0.0f);
    DrawCampusBuilding(glm::vec3(13.0f, 3.40f, -5.0f), glm::vec3(11.0f, 6.8f, 8.0f), 0.0f);

    // Parking cars.
    for (int i = 0; i < 5; ++i)
    {
        DrawCampusCar(glm::vec3(-25.6f, 0.0f, -8.0f - i * 8.8f), 0.0f, OBJECT_CAR_BODY);
        DrawCampusCar(glm::vec3(-16.8f, 0.0f, -8.0f - i * 8.8f), 0.0f, OBJECT_CONCRETE);
    }

    for (int i = 0; i < 4; ++i)
    {
        DrawCampusCar(glm::vec3(-18.0f + i * 9.5f, 0.0f, 2.0f), 3.141592f / 2.0f, (i % 2 == 0) ? OBJECT_CAR_BODY : OBJECT_CONCRETE);
        DrawCampusCar(glm::vec3(8.0f + i * 9.2f, 0.0f, -88.0f), 3.141592f / 2.0f, (i % 2 == 0) ? OBJECT_CONCRETE : OBJECT_CAR_BODY);
    }

    // Tree belts and courtyard trees.
    for (int i = 0; i < 16; ++i)
    {
        DrawCampusTree(glm::vec3(-38.0f, 0.0f, 8.0f - i * 7.0f), 1.45f);
        DrawCampusTree(glm::vec3(39.0f, 0.0f, 6.0f - i * 7.0f), 1.45f);
    }

    for (int i = 0; i < 12; i += 2)
    {
        DrawCampusTree(glm::vec3(-18.0f + i * 3.8f, 0.0f, -27.5f), 0.90f);
        DrawCampusTree(glm::vec3(5.0f + i * 3.3f, 0.0f, -27.5f), 0.82f);
        DrawCampusTree(glm::vec3(-18.0f + i * 3.8f, 0.0f, -69.5f), 0.90f);
        DrawCampusTree(glm::vec3(5.0f + i * 3.3f, 0.0f, -69.5f), 0.82f);
    }

    // Outer forest: trees after the campus blocks, before the enclosing walls.
    for (int i = 0; i < 31; ++i)
    {
        float z = 46.0f - i * 6.6f;
        float a = ((i * 37) % 11 - 5) * 0.42f;
        float b = ((i * 19) % 13 - 6) * 0.36f;

        DrawCampusPine(glm::vec3(-50.0f + a, 0.0f, z + b), 1.70f);
        DrawCampusPine(glm::vec3(-58.0f - b, 0.0f, z - 3.2f + a), 1.45f);
        DrawCampusPine(glm::vec3(-66.5f + b * 0.7f, 0.0f, z + 2.0f - a), 1.55f);
        DrawCampusPine(glm::vec3(-75.5f - a * 0.8f, 0.0f, z - 4.6f - b), 1.35f);
        DrawCampusPine(glm::vec3(-62.0f + b * 0.6f, 0.0f, z - 1.3f), 1.30f);
        DrawCampusPine(glm::vec3(-71.5f + a * 0.6f, 0.0f, z + 3.4f), 1.42f);

        DrawCampusPine(glm::vec3(50.5f - b, 0.0f, z - 1.8f + a), 1.70f);
        DrawCampusPine(glm::vec3(58.0f + a, 0.0f, z - 4.6f - b), 1.45f);
        DrawCampusPine(glm::vec3(66.5f - a * 0.7f, 0.0f, z + 1.2f + b), 1.55f);
        DrawCampusPine(glm::vec3(75.5f + b * 0.8f, 0.0f, z - 3.2f + a), 1.35f);
        DrawCampusPine(glm::vec3(62.0f - b * 0.6f, 0.0f, z - 2.4f), 1.30f);
        DrawCampusPine(glm::vec3(71.5f - a * 0.6f, 0.0f, z + 2.9f), 1.42f);
    }

    for (int i = 0; i < 24; ++i)
    {
        float x = -77.0f + i * 6.8f;
        float a = ((i * 23) % 11 - 5) * 0.44f;
        float b = ((i * 31) % 13 - 6) * 0.34f;

        draw_spawn_safe_pine(glm::vec3(x + a, 0.0f, 50.5f + b), 1.30f);
        draw_spawn_safe_pine(glm::vec3(x + 3.5f - b, 0.0f, 46.5f + a), 1.22f);
        draw_spawn_safe_pine(glm::vec3(x + 1.4f + b, 0.0f, 42.0f - a), 1.55f);
        draw_spawn_safe_pine(glm::vec3(x + 4.6f + a, 0.0f, 36.3f + b), 1.30f);
        draw_spawn_safe_pine(glm::vec3(x - 0.6f - b, 0.0f, 27.0f + a), 1.55f);
        draw_spawn_safe_pine(glm::vec3(x + 3.1f + a, 0.0f, 19.2f - b), 1.35f);
        draw_spawn_safe_pine(glm::vec3(x + b, 0.0f, -122.0f - a), 1.60f);
        draw_spawn_safe_pine(glm::vec3(x + 3.6f - a, 0.0f, -130.0f + b), 1.35f);
        draw_spawn_safe_pine(glm::vec3(x + 1.5f + b, 0.0f, -139.0f - a), 1.50f);
        draw_spawn_safe_pine(glm::vec3(x + 4.8f + a, 0.0f, -150.0f + b), 1.32f);
        draw_spawn_safe_pine(glm::vec3(x - 0.4f - b, 0.0f, -156.7f - a), 1.25f);
    }

    DrawCampusBox(glm::vec3(-84.0f, 2.8f, -53.0f), glm::vec3(2.0f, 6.4f, 218.0f), 0.0f, OBJECT_SHOTGUN);
    DrawCampusBox(glm::vec3(84.0f, 2.8f, -53.0f), glm::vec3(2.0f, 6.4f, 218.0f), 0.0f, OBJECT_SHOTGUN);
    DrawCampusBox(glm::vec3(0.0f, 2.8f, 56.0f), glm::vec3(168.0f, 6.4f, 2.0f), 0.0f, OBJECT_SHOTGUN);
    DrawCampusBox(glm::vec3(0.0f, 2.8f, -162.0f), glm::vec3(168.0f, 6.4f, 2.0f), 0.0f, OBJECT_SHOTGUN);
}

void DrawSafeZoneBeacon(const SafeZone& safe_zone, float time_seconds)
{
    float pulse = 0.5f + 0.5f * sin(time_seconds * 3.2f);
    float beam_height = safe_zone.size.y * (2.80f + 0.25f * pulse);

    DrawCampusSurface(
        glm::vec3(safe_zone.center.x, 0.035f, safe_zone.center.z),
        glm::vec3(safe_zone.size.x * 1.35f, 1.0f, safe_zone.size.z * 1.35f),
        0.0f,
        OBJECT_SAFE_ZONE
    );

    DrawCampusBox(
        glm::vec3(safe_zone.center.x, 0.10f, safe_zone.center.z),
        glm::vec3(safe_zone.size.x * 1.45f, 0.20f, safe_zone.size.z * 1.45f),
        0.0f,
        OBJECT_CONCRETE
    );

    for (int side = -1; side <= 1; side += 2)
    {
        DrawCampusBox(
            glm::vec3(safe_zone.center.x + side * safe_zone.size.x * 0.58f, 1.70f, safe_zone.center.z - safe_zone.size.z * 0.55f),
            glm::vec3(0.28f, 3.40f, 0.28f),
            0.0f,
            OBJECT_METAL_ROOF
        );

        DrawCampusBox(
            glm::vec3(safe_zone.center.x + side * safe_zone.size.x * 0.58f, 1.70f, safe_zone.center.z + safe_zone.size.z * 0.55f),
            glm::vec3(0.28f, 3.40f, 0.28f),
            0.0f,
            OBJECT_METAL_ROOF
        );
    }

    DrawCampusBox(
        glm::vec3(safe_zone.center.x, 3.45f, safe_zone.center.z - safe_zone.size.z * 0.55f),
        glm::vec3(safe_zone.size.x * 1.35f, 0.28f, 0.28f),
        0.0f,
        OBJECT_LAMP_LIGHT
    );

    DrawCampusBox(
        glm::vec3(safe_zone.center.x, 3.45f, safe_zone.center.z + safe_zone.size.z * 0.55f),
        glm::vec3(safe_zone.size.x * 1.35f, 0.28f, 0.28f),
        0.0f,
        OBJECT_LAMP_LIGHT
    );

    DrawCampusBox(
        glm::vec3(safe_zone.center.x, beam_height * 0.5f, safe_zone.center.z),
        glm::vec3(safe_zone.size.x * (0.45f + 0.08f * pulse), beam_height, safe_zone.size.z * (0.36f + 0.06f * pulse)),
        0.0f,
        OBJECT_SAFE_ZONE
    );
}

// =============================================================
// Câmera de mapa: helper de desenho e overlay
// =============================================================
#if MAP_VIEW_ENABLED

static void DrawMapFlatMarker(glm::vec3 pos, float yaw, float sx, float sz, int obj_id)
{
    glm::mat4 model = Matrix_Translate(pos.x, 0.3f, pos.z)
        * Matrix_Rotate_Y(yaw)
        * Matrix_Scale(sx, 0.12f, sz);
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, obj_id);
    DrawVirtualObject("the_cube");
}

void DrawMapViewOverlay(GLFWwindow* window, glm::vec4 player_pos, glm::vec4 player_view,
    glm::vec3 bigfoot_pos, float bigfoot_yaw)
{
    // --- Marcador do jogador (ciano) ---
    glm::vec3 ppos = glm::vec3(player_pos.x, 0.0f, player_pos.z);
    DrawMapFlatMarker(ppos, 0.0f, 1.2f, 1.2f, MAP_MARKER_PLAYER);

    // Seta de direção do jogador
    float player_yaw = atan2f(player_view.x, player_view.z);
    glm::vec3 pfwd   = glm::vec3(sinf(player_yaw), 0.0f, cosf(player_yaw));
    DrawMapFlatMarker(ppos + pfwd * 1.6f, player_yaw, 0.35f, 1.0f, MAP_MARKER_PLAYER);

    // --- Marcador do Pé Grande (vermelho) ---
    DrawMapFlatMarker(bigfoot_pos, 0.0f, 1.5f, 1.5f, MAP_MARKER_BIGFOOT);

    // Seta de direção do Pé Grande
    glm::vec3 bfwd = glm::vec3(sinf(bigfoot_yaw), 0.0f, cosf(bigfoot_yaw));
    DrawMapFlatMarker(bigfoot_pos + bfwd * 1.9f, bigfoot_yaw, 0.4f, 1.2f, MAP_MARKER_BIGFOOT);

    // --- Coletáveis não recolhidos (amarelo) ---
    const std::vector<Collectible>& collectibles = GetSceneCollectibles();
    for (const Collectible& item : collectibles)
    {
        if (!item.collected)
            DrawMapFlatMarker(item.center, 0.0f, 0.8f, 0.8f, MAP_MARKER_ITEM);
    }

    // --- Zona segura (amarelo, maior) ---
    const SafeZone& sz = GetSafeZone();
    DrawMapFlatMarker(
        glm::vec3(sz.center.x, 0.0f, sz.center.z),
        0.0f,
        sz.size.x * 0.65f,
        sz.size.z * 0.65f,
        MAP_MARKER_ITEM
    );

    // --- HUD de legenda ---
    TextRendering_PrintString(window, "MAPA  [ M: fechar ]",        -0.26f,  0.92f, 1.0f);
    TextRendering_PrintString(window, "Ciano=Jogador  Verm=PeGrande  Amar=Item/Zona",
                                                                    -0.60f, -0.91f, 0.72f);
}

#endif // MAP_VIEW_ENABLED

int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 1280 colunas e 720 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "PE GRANDE", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetWindowFocusCallback(window, WindowFocusCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, WINDOW_WIDTH, WINDOW_HEIGHT); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

    // Carregamos duas imagens para serem utilizadas como textura
LoadTextureImage("../../data/textures/textura_tijolos.png");      // TextureImage0
   LoadTextureImage("../../data/textures/textura_grama.png");         // TextureImage1
   LoadTextureImage("../../data/textures/monster-zero-ultra/MonsterUltra_em.png"); // TextureImage2
    

    // Construímos a representação de objetos geométricos através de malhas de triângulos
    ObjModel spheremodel("../../data/models/sphere.obj");
    ComputeNormals(&spheremodel);
    BuildTrianglesAndAddToVirtualScene(&spheremodel);

    ObjModel bunnymodel("../../data/models/bunny.obj");
    ComputeNormals(&bunnymodel);
    BuildTrianglesAndAddToVirtualScene(&bunnymodel);

    ObjModel planemodel("../../data/models/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    ObjModel cubemodel("../../data/models/cube.obj");
    ComputeNormals(&cubemodel);
    BuildTrianglesAndAddToVirtualScene(&cubemodel);

    ObjModel monsterdrinkmodel("../../data/models/monster-zero-ultra/MonsterSubs.obj", "../../data/models/monster-zero-ultra/");
    ComputeNormals(&monsterdrinkmodel);
    BuildTrianglesAndAddToVirtualScene(&monsterdrinkmodel);

    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();
    TextRendering_InitRect();
    LoadPrestigeMemory();
    StartBackgroundMusic();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
    g_MouseCaptured = true;
    g_FirstCapturedMouseFrame = true;

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    float prev_time = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float current_time = (float)glfwGetTime();
        float delta_t = current_time - prev_time;
        prev_time = current_time;

        if (g_ShotgunRecoilTimer > 0.0f)
        {
            g_ShotgunRecoilTimer -= delta_t;

            if (g_ShotgunRecoilTimer < 0.0f)
                g_ShotgunRecoilTimer = 0.0f;
        }

        for (BigfootInstance& instance : g_Bigfoots)
        {
            if (instance.enemy.IsDead() && instance.death_animation_started)
                instance.death_timer += delta_t;
        }

        if (g_PlayerFallAnimationStarted)
            g_PlayerFallTimer += delta_t;

        if (g_GameState.status == GameStatus::Won &&
            g_SpectatorMode &&
            g_SpectatorAutoAdvanceTimer >= 0.0f)
        {
            g_SpectatorAutoAdvanceTimer -= delta_t;

            if (g_SpectatorAutoAdvanceTimer <= 0.0f)
            {
                ResetGame(true);
                g_SpectatorMode = true;
                g_SpectatorWantsShoot = false;
                g_SpectatorRunning = false;
                g_SpectatorMovementDirection = glm::vec3(0.0f, 0.0f, 0.0f);
                g_SpectatorLastPosition = glm::vec3(0.0f, 0.0f, 0.0f);
                g_SpectatorDetourDirection = glm::vec3(0.0f, 0.0f, 0.0f);
                g_SpectatorHasLastPosition = false;
                g_SpectatorStuckTimer = 0.0f;
                g_SpectatorDetourTimer = 0.0f;
                g_SpectatorAutoAdvanceTimer = -1.0f;
                g_SpectatorAutoRetryTimer = -1.0f;
            }
        }

        if (g_GameState.status == GameStatus::Lost &&
            g_SpectatorMode &&
            g_SpectatorAutoRetryTimer >= 0.0f)
        {
            g_SpectatorAutoRetryTimer -= delta_t;

            if (g_SpectatorAutoRetryTimer <= 0.0f)
            {
                g_SelectedPrestigeLevel = ClampPrestigeLevel(g_RunPrestigeLevel);
                ResetGame(true);
                g_SpectatorMode = true;
                g_SpectatorWantsShoot = false;
                g_SpectatorRunning = false;
                g_SpectatorMovementDirection = glm::vec3(0.0f, 0.0f, 0.0f);
                g_SpectatorLastPosition = glm::vec3(0.0f, 0.0f, 0.0f);
                g_SpectatorDetourDirection = glm::vec3(0.0f, 0.0f, 0.0f);
                g_SpectatorHasLastPosition = false;
                g_SpectatorStuckTimer = 0.0f;
                g_SpectatorDetourTimer = 0.0f;
                g_SpectatorAutoAdvanceTimer = -1.0f;
                g_SpectatorAutoRetryTimer = -1.0f;
            }
        }

        UpdateSpectatorController(delta_t);

        bool movement_key_pressed =
            glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
            (g_SpectatorMode &&
             (GetSpectatorMovementDirection().x != 0.0f || GetSpectatorMovementDirection().z != 0.0f));

        bool running_key_pressed =
            glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS ||
            (g_SpectatorMode && IsSpectatorRunning());

        if (g_GameState.status == GameStatus::Playing && movement_key_pressed)
        {
            g_CameraBobTimer += delta_t * (running_key_pressed ? 11.5f : 7.2f);

            float target_bob = running_key_pressed ? 1.0f : 0.45f;
            g_CameraBobAmount += (target_bob - g_CameraBobAmount) * 8.0f * delta_t;
        }
        else
        {
            g_CameraBobAmount += (0.0f - g_CameraBobAmount) * 8.0f * delta_t;
        }

        if (g_GameState.status == GameStatus::Playing
#if MAP_VIEW_ENABLED
            && !g_MapView.IsActive()
#endif
           )
        {
            // Snapshot do estado dos coletáveis para detectar pickups e alertar
            // o Pé Grande para a posição onde o jogador "fez barulho".
            std::vector<Collectible>& collectibles_before = GetSceneCollectibles();
            std::vector<bool> was_collected;
            was_collected.reserve(collectibles_before.size());
            for (const Collectible& c : collectibles_before)
                was_collected.push_back(c.collected);

            if (g_SpectatorMode)
                g_Player.UpdateAutonomous(GetSpectatorMovementDirection(), IsSpectatorRunning(), delta_t);
            else
                g_Player.Update(window, delta_t);

            const std::vector<Collectible>& collectibles_after = GetSceneCollectibles();
            for (size_t i = 0; i < collectibles_after.size() && i < was_collected.size(); ++i)
            {
                if (!was_collected[i] && collectibles_after[i].collected)
                {
                    // 10 segundos cobrem ~45m a velocidade de ronda — atravessa
                    // boa parte do mapa sem deixá-lo eternamente "obcecado".
                    (void)collectibles_after;
                }
            }
        }

        glm::vec4 player_position = g_Camera.GetPosition();

        if (g_GameState.status == GameStatus::Playing
#if MAP_VIEW_ENABLED
            && !g_MapView.IsActive()
#endif
           )
        {
            for (BigfootInstance& instance : g_Bigfoots)
            {
                instance.enemy.Update(
                    glm::vec3(player_position.x, player_position.y, player_position.z),
                    delta_t
                );
            }
        }

        bool bigfoot_attacking = false;

        for (const BigfootInstance& instance : g_Bigfoots)
        {
            if (instance.enemy.GetState() == BigfootState::Attacking)
            {
                bigfoot_attacking = true;
                break;
            }
        }

        if (g_GameState.status == GameStatus::Playing && bigfoot_attacking)
        {
            PlayGameSound(GameSound::BigfootKillsPlayer);
            g_PlayerFallAnimationStarted = true;
            g_PlayerFallTimer = 0.0f;
            g_GameState.status = GameStatus::Lost;

            if (g_SpectatorMode)
                g_SpectatorAutoRetryTimer = 2.0f;
        }

        if (g_GameState.status == GameStatus::Playing &&
            AllCollectiblesCollected() &&
            IsPlayerInsideSafeZone(player_position))
        {
            SetGameWon();
        }

        // Tiro com o botão esquerdo do mouse.
        bool shoot_button_pressed =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
            (g_SpectatorMode && ShouldSpectatorShoot() && g_ShotgunRecoilTimer <= 0.0f);

        if (g_GameState.status == GameStatus::Playing &&
            shoot_button_pressed &&
            !g_ShootButtonWasPressed &&
            g_ShotgunRecoilTimer <= 0.0f)
        {
            float reload_mult = GetUpgradeValue(UpgradeId::ReloadSpeed);
            if (reload_mult < 0.1f) reload_mult = 0.1f;
            g_ShotgunCurrentRecoilDuration = g_Player.IsEnergyBoostActive()
                ? SHOTGUN_RECOIL_DURATION * 0.5f / reload_mult
                : SHOTGUN_RECOIL_DURATION / reload_mult;
            g_ShotgunRecoilTimer = g_ShotgunCurrentRecoilDuration;
            PlayGameSound(GameSound::Shotgun);

            int hit_bigfoot_index = ShotHitsBigfoot(g_Camera.GetPosition(), g_Camera.GetViewVector());

            if (hit_bigfoot_index >= 0)
            {
                printf("Acertou o Pe Grande!\n");

                BigfootInstance& hit_bigfoot = g_Bigfoots[(size_t)hit_bigfoot_index];

                hit_bigfoot.enemy.TakeDamage(
                    12.5f,
                    glm::vec3(player_position.x, player_position.y, player_position.z)
                );

                if (hit_bigfoot.enemy.IsDead())
                {
                    hit_bigfoot.death_animation_started = true;
                    hit_bigfoot.death_timer = 0.0f;
                    PlayGameSound(GameSound::BigfootDies);

                    if (AreAllBigfootsDead())
                        SetGameWon();
                }
                else
                {
                    PlayRandomBigfootRoar();
                }
            }
            else
            {
                printf("Errou o tiro.\n");
            }
        }

        g_ShootButtonWasPressed = shoot_button_pressed;

        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(0.015f, 0.018f, 0.026f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);


        glm::vec4 camera_position_c  = g_Camera.GetPosition();
        glm::vec4 camera_view_vector = g_Camera.GetViewVector();
        glm::vec4 camera_up_vector   = g_Camera.GetUpVector();

        glm::mat4 view;
        glm::mat4 projection;

#if MAP_VIEW_ENABLED
        if (g_MapView.IsActive())
        {
            view       = g_MapView.GetViewMatrix();
            projection = g_MapView.GetProjectionMatrix(g_ScreenRatio);
        }
        else
#endif
        {
            // Computamos a matriz "View" utilizando os parâmetros da câmera para
            // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
            view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

            if (!g_PlayerFallAnimationStarted && g_CameraBobAmount > 0.001f)
            {
                float vertical_bob = sin(g_CameraBobTimer) * 0.045f * g_CameraBobAmount;
                float roll_bob = sin(g_CameraBobTimer * 0.5f) * 0.018f * g_CameraBobAmount;

                view = Matrix_Rotate_Z(roll_bob)
                    * Matrix_Translate(0.0f, vertical_bob, 0.0f)
                    * view;
            }

            if (g_PlayerFallAnimationStarted)
            {
                float fall = g_PlayerFallTimer / 1.10f;

                if (fall > 1.0f)
                    fall = 1.0f;

                fall = fall * fall * (3.0f - 2.0f * fall);

                view = Matrix_Rotate_Z(1.35f * fall)
                    * Matrix_Rotate_X(-0.65f * fall)
                    * Matrix_Translate(0.0f, -1.05f * fall, 0.0f)
                    * view;
            }

            // Agora computamos a matriz de Projeção.
            // Note que, no sistema de coordenadas da câmera, os planos near e far
            // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
            float nearplane = -0.1f;  // Posição do "near plane"
            float farplane  = -220.0f; // Posição do "far plane"

            if (g_UsePerspectiveProjection)
            {
                // Projeção Perspectiva.
                // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
                float field_of_view = 3.141592 / 3.0f;
                projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
            }
            else
            {
                // Projeção Ortográfica.
                // Para definição dos valores l, r, b, t ("left", "right", "bottom", "top"),
                // PARA PROJEÇÃO ORTOGRÁFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
                // Para simular um "zoom" ortográfico, computamos o valor de "t"
                // utilizando a variável g_CameraDistance.
                float t = 1.5f*g_CameraDistance/2.5f;
                float b = -t;
                float r = t*g_ScreenRatio;
                float l = -r;
                projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
            }
        }

        glm::mat4 model = Matrix_Identity(); // Transformação identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));
#if MAP_VIEW_ENABLED
        glUniform1i(g_map_view_uniform, g_MapView.IsActive() ? 1 : 0);
#endif

        #define SPHERE 0
        #define BUNNY  1
        #define PLANE  2
        #define SAFE_ZONE 3
        #define BIGFOOT 4
        DrawCampusMap();


        // Desenhamos os blocos retangulares do cenário.
        // A mesma lista será usada depois para colisão.
        // Desenhamos o Pé Grande como placeholder.
        // A funcao abaixo substitui o modelo do coelho do template.
        glm::vec3 map_bigfoot_position = glm::vec3(0.0f, 0.0f, 0.0f);
        float map_bigfoot_yaw = 0.0f;
        bool has_map_bigfoot = false;

        for (size_t i = 0; i < g_Bigfoots.size(); ++i)
        {
            BigfootInstance& instance = g_Bigfoots[i];
            glm::vec3 bigfoot_position = instance.enemy.GetPosition();
            float bigfoot_yaw = UpdateBigfootFacing(i, bigfoot_position, delta_t);
            float bigfoot_death_progress = instance.death_animation_started ? instance.death_timer / 1.15f : 0.0f;

            DrawBigfootModel(bigfoot_position, bigfoot_yaw, current_time, bigfoot_death_progress, instance.movement_intensity);

            if (!has_map_bigfoot && !instance.enemy.IsDead())
            {
                map_bigfoot_position = bigfoot_position;
                map_bigfoot_yaw = bigfoot_yaw;
                has_map_bigfoot = true;
            }
        }

        // Esfera de debug da hitbox do Pé Grande.
        // Usamos o mesmo raio que será usado para tiro/colisão.
        if (g_DrawBigfootHitSphere)
        {
            std::vector<BoxObstacle> shot_boxes = GetBigfootShotBoxes();

            glDisable(GL_CULL_FACE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            for (const BigfootInstance& instance : g_Bigfoots)
            {
                glm::vec3 bigfoot_position = instance.enemy.GetPosition();

                for (const BoxObstacle& shot_box : shot_boxes)
                {
                    model = Matrix_Translate(bigfoot_position.x, bigfoot_position.y, bigfoot_position.z)
                        * Matrix_Rotate_Y(instance.render_yaw)
                        * Matrix_Translate(shot_box.center.x, shot_box.center.y, shot_box.center.z)
                        * Matrix_Scale(shot_box.size.x, shot_box.size.y, shot_box.size.z);

                    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(g_object_id_uniform, SAFE_ZONE); // Reaproveita o verde do shader.
                    DrawVirtualObject("the_cube");
                }
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);
        }

        // Desenhamos os itens coletáveis.
        // Por enquanto usamos esferas pequenas como placeholder visual.
        std::vector<Collectible>& collectibles = GetSceneCollectibles();

        for (const Collectible& collectible : collectibles)
        {
            if (collectible.collected)
                continue;

            float bob = 0.18f * sin(current_time * 2.4f + collectible.center.x * 0.37f);

            model = Matrix_Translate(collectible.center.x, 0.18f + bob, collectible.center.z)
                * Matrix_Rotate_Y(current_time * 1.9f)
                * Matrix_Rotate_Z(0.22f)
                * Matrix_Scale(0.34f, 0.34f, 0.34f);

            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, OBJECT_MONSTER_DRINK);
            DrawVirtualObject("pCylinder2");
        }

        // Desenhamos a zona segura/final somente depois que todos os itens forem coletados.
        if (AllCollectiblesCollected())
        {
            const SafeZone& safe_zone = GetSafeZone();

            DrawSafeZoneBeacon(safe_zone, current_time);
        }

#if MAP_VIEW_ENABLED
        if (g_MapView.IsActive())
        {
            DrawMapViewOverlay(window, camera_position_c, camera_view_vector, map_bigfoot_position, map_bigfoot_yaw);
        }
#endif

        // Imprimimos na tela os ângulos de Euler que controlam a rotação do
        // terceiro cubo.
        if (g_GameState.status != GameStatus::MainMenu &&
            g_GameState.status != GameStatus::UpgradeShop &&
            g_GameState.status != GameStatus::ConfirmReset
#if MAP_VIEW_ENABLED
            && !g_MapView.IsActive()
#endif
           )
        {
            DrawFirstPersonWeapon(camera_position_c, camera_view_vector, camera_up_vector, g_ShotgunRecoilTimer, g_ShotgunCurrentRecoilDuration);
        }

        // Filtro esverdeado de "adrenalina" enquanto o energy boost estiver ativo.
        if (g_GameState.status == GameStatus::Playing && g_Player.IsEnergyBoostActive())
        {
            int fb_w, fb_h;
            glfwGetFramebufferSize(window, &fb_w, &fb_h);

            float remaining = g_Player.GetEnergyBoostTimeRemaining();
            float fade_out  = g_Player.IsInfiniteBoostCheatActive()
                ? 1.0f
                : (remaining < 0.6f ? remaining / 0.6f : 1.0f);
            float pulse     = 0.5f + 0.5f * sin((float)glfwGetTime() * 4.5f);
            float alpha     = (0.12f + 0.05f * pulse) * fade_out;

            TextRendering_DrawRectPx(window, 0, 0, fb_w, fb_h, 0.20f, 1.00f, 0.35f, alpha);
        }

        // TextRendering_ShowEulerAngles(window);

        // Imprimimos na informação sobre a matriz de projeção sendo utilizada.
        // TextRendering_ShowProjection(window);

        // Imprimimos na tela informação sobre o número de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        if (g_GameState.status == GameStatus::MainMenu)
        {
            int max_visible_levels = 6;
            int first_level = g_SelectedPrestigeLevel - max_visible_levels / 2;

            if (first_level < 0)
                first_level = 0;

            if (first_level > g_HighestUnlockedPrestigeLevel - max_visible_levels + 1)
                first_level = g_HighestUnlockedPrestigeLevel - max_visible_levels + 1;

            if (first_level < 0)
                first_level = 0;

            int last_level = first_level + max_visible_levels - 1;

            if (last_level > g_HighestUnlockedPrestigeLevel)
                last_level = g_HighestUnlockedPrestigeLevel;

            int visible_rows = last_level - first_level + 1;
            int selected_row_in_view = g_SelectedPrestigeLevel - first_level;

            DrawMainMenuPanel(window, selected_row_in_view, visible_rows);

            TextRendering_PrintString(window, "Pe Grande do Vale", -0.70f, 0.28f, 1.55f);
            TextRendering_PrintString(window, "Colete os energeticos e fuja para a zona segura.", -0.70f, 0.14f, 1.0f);

            // Cabecalho e colunas alinhadas (fonte e proporcional, entao posicionamos
            // cada coluna em um x fixo em NDC).
            float col_marker = -0.58f;
            float col_level  = -0.50f;
            float col_status = -0.32f;
            float col_pes    = -0.12f;
            float col_latas  =  0.02f;
            float col_vida   =  0.18f;
            float col_vel    =  0.40f;
            float header_scale = 0.80f;
            float row_scale    = 0.78f;

            TextRendering_PrintString(window, "Nivel",      col_level,  -0.08f, header_scale);
            TextRendering_PrintString(window, "Status",     col_status, -0.08f, header_scale);
            TextRendering_PrintString(window, "Pes",        col_pes,    -0.08f, header_scale);
            TextRendering_PrintString(window, "Latas",      col_latas,  -0.08f, header_scale);
            TextRendering_PrintString(window, "Vida",       col_vida,   -0.08f, header_scale);
            TextRendering_PrintString(window, "Velocidade", col_vel,    -0.08f, header_scale);

            for (int level = first_level; level <= last_level; ++level)
            {
                float health_multiplier = GetPrestigeHealthMultiplierForLevel(level);
                float speed_multiplier = GetPrestigeSpeedMultiplierForLevel(level);
                bool is_selected = (level == g_SelectedPrestigeLevel);
                bool is_frontier = (level == g_HighestUnlockedPrestigeLevel);
                const char* marker = is_selected ? ">>" : "  ";
                const char* status = is_frontier ? "NOVO" : "vencido";

                float row_y = -0.17f - (level - first_level) * 0.085f;

                char buf[32];

                TextRendering_PrintString(window, marker, col_marker, row_y, row_scale);

                snprintf(buf, sizeof(buf), "%02d", level + 1);
                TextRendering_PrintString(window, buf, col_level, row_y, row_scale);

                TextRendering_PrintString(window, status, col_status, row_y, row_scale);

                snprintf(buf, sizeof(buf), "%d", GetBigfootCountForLevel(level));
                TextRendering_PrintString(window, buf, col_pes, row_y, row_scale);

                snprintf(buf, sizeof(buf), "%d", GetPrestigeCollectibleCountForLevel(level));
                TextRendering_PrintString(window, buf, col_latas, row_y, row_scale);

                snprintf(buf, sizeof(buf), "x%.2f", health_multiplier);
                TextRendering_PrintString(window, buf, col_vida, row_y, row_scale);

                snprintf(buf, sizeof(buf), "x%.2f", speed_multiplier);
                TextRendering_PrintString(window, buf, col_vel, row_y, row_scale);
            }

            TextRendering_PrintString(window, "[SPACE] iniciar  [W/S] nivel  [U] loja", -0.52f, -0.66f, 0.92f);
            TextRendering_PrintString(window, "[X] Resetar progresso", -0.22f, -0.76f, 0.86f);

            char coins_hud[64];
            snprintf(coins_hud, sizeof(coins_hud), "Pontos: %d", GetRawCoins());
            TextRendering_PrintString(window, coins_hud, -0.95f, 0.82f, 1.12f);
        }
        else if (g_GameState.status == GameStatus::UpgradeShop)
        {
            DrawUpgradeShopOverlay(window);
        }
        else if (g_GameState.status == GameStatus::ConfirmReset)
        {
            DrawConfirmResetOverlay(window);
        }

        // Barra textual de vida do Pé Grande no topo da tela.
        if (g_GameState.status != GameStatus::MainMenu &&
            g_GameState.status != GameStatus::UpgradeShop &&
            g_GameState.status != GameStatus::ConfirmReset)
        {
        float total_bigfoot_health = 0.0f;
        float total_bigfoot_max_health = 0.0f;

        for (const BigfootInstance& instance : g_Bigfoots)
        {
            total_bigfoot_health += instance.enemy.GetHealth();
            total_bigfoot_max_health += instance.enemy.GetMaxHealth();
        }

        float health_ratio = (total_bigfoot_max_health > 0.0f)
            ? total_bigfoot_health / total_bigfoot_max_health
            : 0.0f;

        if (health_ratio < 0.0f)
            health_ratio = 0.0f;

        if (health_ratio > 1.0f)
            health_ratio = 1.0f;

        DrawBigfootHealthBar(window, health_ratio);

        if (g_Player.IsEnergyBoostActive())
        {
            char boost_text[64];
            if (g_Player.IsInfiniteBoostCheatActive())
                snprintf(boost_text, sizeof(boost_text), "ENERGIA x2");
            else
                snprintf(boost_text, sizeof(boost_text), "ENERGIA x2  %.1fs", g_Player.GetEnergyBoostTimeRemaining());

            TextRendering_PrintString(
                window,
                boost_text,
                -0.19f,
                -0.72f,
                1.0f
            );
        }

        // HUD temporário dos coletáveis.
        std::vector<Collectible>& hud_collectibles = GetSceneCollectibles();

        int collected_count = 0;
        int total_count = (int)hud_collectibles.size();

        for (const Collectible& collectible : hud_collectibles)
        {
            if (collectible.collected)
                collected_count++;
        }

        char collectibles_text[32];
        snprintf(
            collectibles_text,
            32,
            "Coletados: %d/%d",
            collected_count,
            total_count
        );

        TextRendering_PrintString(
            window,
            collectibles_text,
            -0.95f,
            0.82f,
            1.12f
        );

        char coins_hud[64];
        snprintf(coins_hud, sizeof(coins_hud), "Pontos: %d", GetRawCoins());
        TextRendering_PrintString(window, coins_hud, -0.95f, 0.72f, 1.02f);

        if (g_GameState.status == GameStatus::Playing && g_SpectatorMode)
        {
            TextRendering_PrintString(window, "SPECTATOR IA", 0.58f, 0.82f, 1.0f);
        }
        else if (g_GameState.status == GameStatus::Won &&
                 g_SpectatorMode &&
                 g_SpectatorAutoAdvanceTimer >= 0.0f)
        {
            char spectator_next_text[64];
            snprintf(
                spectator_next_text,
                sizeof(spectator_next_text),
                "SPECTATOR: proximo nivel em %.1fs",
                g_SpectatorAutoAdvanceTimer
            );
            TextRendering_PrintString(window, spectator_next_text, -0.34f, 0.44f, 0.98f);
        }
        else if (g_GameState.status == GameStatus::Lost &&
                 g_SpectatorMode &&
                 g_SpectatorAutoRetryTimer >= 0.0f)
        {
            char spectator_retry_text[64];
            snprintf(
                spectator_retry_text,
                sizeof(spectator_retry_text),
                "SPECTATOR: tentando de novo em %.1fs",
                g_SpectatorAutoRetryTimer
            );
            TextRendering_PrintString(window, spectator_retry_text, -0.38f, 0.44f, 0.98f);
        }

        if (g_GameState.status == GameStatus::Won)
        {
            char win_prestige_text[64];

            if (g_LastWinUnlockedNewLevel)
            {
                snprintf(
                    win_prestige_text,
                    sizeof(win_prestige_text),
                    "Nivel %d liberado.",
                    g_HighestUnlockedPrestigeLevel + 1
                );
            }
            else
            {
                snprintf(
                    win_prestige_text,
                    sizeof(win_prestige_text),
                    "Nivel %d concluido.",
                    g_RunPrestigeLevel + 1
                );
            }

            TextRendering_PrintString(
                window,
                "VITORIA!",
                -0.35f,
                0.80f,
                1.35f
            );
            TextRendering_PrintString(
                window,
                win_prestige_text,
                -0.38f,
                0.68f,
                1.08f
            );
            TextRendering_PrintString(
                window,
                "Aperte R para voltar ao menu.",
                -0.42f,
                0.56f,
                1.08f
            );
        }
        else if (g_GameState.status == GameStatus::Lost)
        {
            TextRendering_PrintString(
                window,
                "DERROTA! Foi papado",
                -0.40f,
                0.80f,
                1.35f
            );
            TextRendering_PrintString(
                window,
                "Aperte R para voltar ao menu.",
                -0.42f,
                0.68f,
                1.08f
            );
        }
        else if (collected_count == total_count)
        {
            TextRendering_PrintString(
                window,
                "Volte para a zona segura.",
                -0.40f,
                0.80f,
                1.12f
            );
        }

        // Mira simples no centro da tela.
#if MAP_VIEW_ENABLED
        if (!g_MapView.IsActive())
#endif
        TextRendering_PrintString(
            window,
            "x",
            -0.01f,
            0.0f,
            1.5f
        );
        }

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    StopBackgroundMusic();

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Função que carrega uma imagem para ser utilizada como textura
void LoadTextureImage(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    g_NumLoadedTextures += 1;
}

// Função que desenha um objeto armazenado em g_VirtualScene. Veja definição
// dos objetos na função BuildTrianglesAndAddToVirtualScene().
void DrawVirtualObject(const char* object_name)
{
    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTrianglesAndAddToVirtualScene(). Veja
    // comentários detalhados dentro da definição de BuildTrianglesAndAddToVirtualScene().
    glBindVertexArray(g_VirtualScene[object_name].vertex_array_object_id);

    // Setamos as variáveis "bbox_min" e "bbox_max" do fragment shader
    // com os parâmetros da axis-aligned bounding box (AABB) do modelo.
    glm::vec3 bbox_min = g_VirtualScene[object_name].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene[object_name].bbox_max;
    glUniform4f(g_bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(g_bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definição de
    // g_VirtualScene[""] dentro da função BuildTrianglesAndAddToVirtualScene(), e veja
    // a documentação da função glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene[object_name].rendering_mode,
        g_VirtualScene[object_name].num_indices,
        GL_UNSIGNED_INT,
        (void*)(g_VirtualScene[object_name].first_index * sizeof(GLuint))
    );

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Função que carrega os shaders de vértices e de fragmentos que serão
// utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../data/shaders/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../data/shaders/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl" e "shader_fragment.glsl".
    g_model_uniform      = glGetUniformLocation(g_GpuProgramID, "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_GpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    g_object_id_uniform  = glGetUniformLocation(g_GpuProgramID, "object_id"); // Variável "object_id" em shader_fragment.glsl
    g_bbox_min_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_min");
    g_bbox_max_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_max");
#if MAP_VIEW_ENABLED
    g_map_view_uniform   = glGetUniformLocation(g_GpuProgramID, "u_map_view_active");
#endif

    // Variáveis em "shader_fragment.glsl" para acesso das imagens de textura
    glUseProgram(g_GpuProgramID);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage0"), 0);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage1"), 1);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage2"), 2);
    glUseProgram(0);
}

// Função que pega a matriz M e guarda a mesma no topo da pilha
void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

// Função que remove a matriz atualmente no topo da pilha e armazena a mesma na variável M
void PopMatrix(glm::mat4& M)
{
    if ( g_MatrixStack.empty() )
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}

// Função que computa as normais de um ObjModel, caso elas não tenham sido
// especificadas dentro do arquivo ".obj"
void ComputeNormals(ObjModel* model)
{
    if ( !model->attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto
    // por Gouraud, onde a normal de cada vértice vai ser a média das normais de
    // todas as faces que compartilham este vértice e que pertencem ao mesmo "smoothing group".

    // Obtemos a lista dos smoothing groups que existem no objeto
    std::set<unsigned int> sgroup_ids;
    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        assert(model->shapes[shape].mesh.smoothing_group_ids.size() == num_triangles);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);
            unsigned int sgroup = model->shapes[shape].mesh.smoothing_group_ids[triangle];
            assert(sgroup >= 0);
            sgroup_ids.insert(sgroup);
        }
    }

    size_t num_vertices = model->attrib.vertices.size() / 3;
    model->attrib.normals.reserve( 3*num_vertices );

    // Processamos um smoothing group por vez
    for (const unsigned int & sgroup : sgroup_ids)
    {
        std::vector<int> num_triangles_per_vertex(num_vertices, 0);
        std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

        // Acumulamos as normais dos vértices de todos triângulos deste smoothing group
        for (size_t shape = 0; shape < model->shapes.size(); ++shape)
        {
            size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

            for (size_t triangle = 0; triangle < num_triangles; ++triangle)
            {
                unsigned int sgroup_tri = model->shapes[shape].mesh.smoothing_group_ids[triangle];

                if (sgroup_tri != sgroup)
                    continue;

                glm::vec4  vertices[3];
                for (size_t vertex = 0; vertex < 3; ++vertex)
                {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                    const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                    const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                    const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                    vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
                }

                const glm::vec4  a = vertices[0];
                const glm::vec4  b = vertices[1];
                const glm::vec4  c = vertices[2];

                const glm::vec4  n = crossproduct(b-a,c-a);

                for (size_t vertex = 0; vertex < 3; ++vertex)
                {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                    num_triangles_per_vertex[idx.vertex_index] += 1;
                    vertex_normals[idx.vertex_index] += n;
                }
            }
        }

        // Computamos a média das normais acumuladas
        std::vector<size_t> normal_indices(num_vertices, 0);

        for (size_t vertex_index = 0; vertex_index < vertex_normals.size(); ++vertex_index)
        {
            if (num_triangles_per_vertex[vertex_index] == 0)
                continue;

            glm::vec4 n = vertex_normals[vertex_index] / (float)num_triangles_per_vertex[vertex_index];
            n /= norm(n);

            model->attrib.normals.push_back( n.x );
            model->attrib.normals.push_back( n.y );
            model->attrib.normals.push_back( n.z );

            size_t normal_index = (model->attrib.normals.size() / 3) - 1;
            normal_indices[vertex_index] = normal_index;
        }

        // Escrevemos os índices das normais para os vértices dos triângulos deste smoothing group
        for (size_t shape = 0; shape < model->shapes.size(); ++shape)
        {
            size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

            for (size_t triangle = 0; triangle < num_triangles; ++triangle)
            {
                unsigned int sgroup_tri = model->shapes[shape].mesh.smoothing_group_ids[triangle];

                if (sgroup_tri != sgroup)
                    continue;

                for (size_t vertex = 0; vertex < 3; ++vertex)
                {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                    model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index =
                        normal_indices[ idx.vertex_index ];
                }
            }
        }

    }
}

// Constrói triângulos para futura renderização a partir de um ObjModel.
void BuildTrianglesAndAddToVirtualScene(ObjModel* model)
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
        glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

                // Inspecionando o código da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no ObjModel é
                // comparando se o índice retornado é -1. Fazemos isso abaixo.

                if ( idx.normal_index != -1 )
                {
                    const float nx = model->attrib.normals[3*idx.normal_index + 0];
                    const float ny = model->attrib.normals[3*idx.normal_index + 1];
                    const float nz = model->attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }

                if ( idx.texcoord_index != -1 )
                {
                    const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                    texture_coefficients.push_back( u );
                    texture_coefficients.push_back( v );
                }
            }
        }

        size_t last_index = indices.size() - 1;

        SceneObject theobject;
        theobject.name           = model->shapes[shape].name;
        theobject.first_index    = first_index; // Primeiro índice
        theobject.num_indices    = last_index - first_index + 1; // Número de indices
        theobject.rendering_mode = GL_TRIANGLES;       // Índices correspondem ao tipo de rasterização GL_TRIANGLES.
        theobject.vertex_array_object_id = vertex_array_object_id;

        theobject.bbox_min = bbox_min;
        theobject.bbox_max = bbox_max;

        g_VirtualScene[model->shapes[shape].name] = theobject;
    }

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if ( !normal_coefficients.empty() )
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if ( !texture_coefficients.empty() )
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Os "Shader Objects" podem ser marcados para deleção após serem linkados 
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Retornamos o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_MouseCaptured)
    {
        if (g_FirstCapturedMouseFrame)
        {
            g_LastCursorPosX = xpos;
            g_LastCursorPosY = ypos;
            g_FirstCapturedMouseFrame = false;
            return;
        }

        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        const float mouse_sensitivity = 0.005f;

        g_Camera.AddYaw((float)dx * mouse_sensitivity);
        g_Camera.AddPitch((float)-dy * mouse_sensitivity);

        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
        return;
    }

    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;
    
        // Atualizamos a direção da câmera em primeira pessoa com os deslocamentos do mouse.
        // dx altera o yaw -> olhar para esquerda/direita.
        // dy altera o pitch -> olhar para cima/baixo.
        const float mouse_sensitivity = 0.005f;

        g_Camera.AddYaw((float)dx * mouse_sensitivity);
        g_Camera.AddPitch((float)-dy * mouse_sensitivity);
            
        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_RightMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;
    
        // Atualizamos parâmetros da antebraço com os deslocamentos
        g_ForearmAngleZ -= 0.01f*dx;
        g_ForearmAngleX += 0.01f*dy;
    
        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_MiddleMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;
    
        // Atualizamos parâmetros da antebraço com os deslocamentos
        g_TorsoPositionX += 0.01f*dx;
        g_TorsoPositionY -= 0.01f*dy;
    
        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

void WindowFocusCallback(GLFWwindow* window, int focused)
{
    if (focused)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MouseCaptured = true;
        g_FirstCapturedMouseFrame = true;
    }
}

void Correcao_KeyCallback(int key, int action, int mod);

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // =======================
    // Não modifique esta chamada! Ela é utilizada para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    Correcao_KeyCallback(key, action, mod);
    // =======================

    // Snapshot do estado no inicio do callback. Sem isso, um bloco abaixo pode
    // mudar o estado e fazer o proximo bloco reagir de novo no mesmo press.
    GameStatus status_at_press = g_GameState.status;

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        g_MouseCaptured = false;
        g_FirstCapturedMouseFrame = true;
    }

    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS &&
        g_GameState.status == GameStatus::MainMenu)
    {
        ResetGame();
    }

    if (status_at_press == GameStatus::MainMenu && action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP ||
            key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
            SelectPrestigeLevel(-1);

        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN ||
            key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
            SelectPrestigeLevel(1);

        if (key == GLFW_KEY_X)
        {
            g_ResetConfirmationFromShop = false;
            g_GameState.status = GameStatus::ConfirmReset;
            return;
        }
    }

    // Abrir loja de upgrades de qualquer estado (exceto se ja esta nela ou no ConfirmReset).
    if (key == GLFW_KEY_U && action == GLFW_PRESS &&
        status_at_press != GameStatus::UpgradeShop &&
        status_at_press != GameStatus::ConfirmReset)
    {
        g_SelectedUpgradeRow = 0;
        g_GameState.status = GameStatus::UpgradeShop;
    }

    if (status_at_press == GameStatus::UpgradeShop && action == GLFW_PRESS)
    {
        int row_count = GetShopRowCount();

        if (key == GLFW_KEY_W || key == GLFW_KEY_UP)
        {
            g_SelectedUpgradeRow--;
            if (g_SelectedUpgradeRow < 0) g_SelectedUpgradeRow = row_count - 1;
        }

        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)
        {
            g_SelectedUpgradeRow++;
            if (g_SelectedUpgradeRow >= row_count) g_SelectedUpgradeRow = 0;
        }

        if (key == GLFW_KEY_SPACE)
        {
            if (g_SelectedUpgradeRow < (int)UpgradeId::COUNT)
            {
                UpgradeId id = (UpgradeId)g_SelectedUpgradeRow;
                if (TryPurchaseUpgrade(id))
                {
                    SavePrestigeMemory();
                    fprintf(stdout, "Upgrade '%s' agora nivel %d.\n",
                        GetUpgradeName(id), GetUpgradeLevel(id));
                    fflush(stdout);
                }
            }
            else if (g_LegacyResetUnlocked && g_SelectedUpgradeRow == (int)UpgradeId::COUNT)
            {
                g_ResetConfirmationFromShop = true;
                g_GameState.status = GameStatus::ConfirmReset;
            }
            else
            {
                g_GameState.status = GameStatus::MainMenu;
            }
        }
    }

    if (status_at_press == GameStatus::ConfirmReset && action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_Y)
        {
            PerformLegacyReset();
            fprintf(stdout, "Jornada reiniciada.\n");
            fflush(stdout);
            g_GameState.status = GameStatus::MainMenu;
            g_SelectedUpgradeRow = 0;
            g_ResetConfirmationFromShop = false;
            return;
        }

        if (key == GLFW_KEY_N)
        {
            g_GameState.status = g_ResetConfirmationFromShop
                ? GameStatus::UpgradeShop
                : GameStatus::MainMenu;
            g_ResetConfirmationFromShop = false;
            return;
        }
    }

    float delta = 3.141592 / 16; // 22.5 graus, em radianos.

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    // Se o usuário apertar a tecla espaço, resetamos os ângulos de Euler para zero.
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
    {
        g_AngleX = 0.0f;
        g_AngleY = 0.0f;
        g_AngleZ = 0.0f;
        g_ForearmAngleX = 0.0f;
        g_ForearmAngleZ = 0.0f;
        g_TorsoPositionX = 0.0f;
        g_TorsoPositionY = 0.0f;
    }

    // Se o usuário apertar a tecla O, utilizamos projeção ortográfica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = false;
    }

    // Se o usuário apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_I && action == GLFW_PRESS)
    {
        g_ShowInfoText = !g_ShowInfoText;
    }

    // Apos vitoria ou derrota, R volta ao menu.
    if (key == GLFW_KEY_R && action == GLFW_PRESS &&
        (status_at_press == GameStatus::Won || status_at_press == GameStatus::Lost))
    {
        ResetGame(false);
        fprintf(stdout,"Voltando ao menu.\n");
        fflush(stdout);
        return;
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        if (g_GameState.status == GameStatus::Playing)
        {
            g_SpectatorMode = !g_SpectatorMode;
            g_SpectatorWantsShoot = false;
            g_SpectatorRunning = false;
            g_SpectatorMovementDirection = glm::vec3(0.0f, 0.0f, 0.0f);
            g_SpectatorLastPosition = glm::vec3(0.0f, 0.0f, 0.0f);
            g_SpectatorDetourDirection = glm::vec3(0.0f, 0.0f, 0.0f);
            g_SpectatorHasLastPosition = false;
            g_SpectatorStuckTimer = 0.0f;
            g_SpectatorDetourTimer = 0.0f;
            g_SpectatorAutoAdvanceTimer = -1.0f;
            g_SpectatorAutoRetryTimer = -1.0f;
            fprintf(stdout, "Modo Spectator IA: %s\n", g_SpectatorMode ? "ON" : "OFF");
        }
        else
        {
            fprintf(stdout, "Modo Spectator IA so pode ser alternado durante a partida.\n");
        }
        fflush(stdout);
    }

    if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout,"Shaders recarregados!\n");
        fflush(stdout);
    }

#if MAP_VIEW_ENABLED
    if (key == GLFW_KEY_M && action == GLFW_PRESS &&
        g_GameState.status == GameStatus::Playing)
    {
        g_MapView.Toggle();
    }
#endif
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Esta função recebe um vértice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transformações.
void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
)
{
    if ( !g_ShowInfoText )
        return;

    glm::vec4 p_world = model*p_model;
    glm::vec4 p_camera = view*p_world;
    glm::vec4 p_clip = projection*p_camera;
    glm::vec4 p_ndc = p_clip / p_clip.w;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f-pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f-2*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-6*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-7*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-8*pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f-9*pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f-10*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-14*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-15*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-16*pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f-17*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f-18*pad, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glm::vec2 a = glm::vec2(-1, -1);
    glm::vec2 b = glm::vec2(+1, +1);
    glm::vec2 p = glm::vec2( 0,  0);
    glm::vec2 q = glm::vec2(width, height);

    glm::mat4 viewport_mapping = Matrix(
        (q.x - p.x)/(b.x-a.x), 0.0f, 0.0f, (b.x*p.x - a.x*q.x)/(b.x-a.x),
        0.0f, (q.y - p.y)/(b.y-a.y), 0.0f, (b.y*p.y - a.y*q.y)/(b.y-a.y),
        0.0f , 0.0f , 1.0f , 0.0f ,
        0.0f , 0.0f , 0.0f , 1.0f
    );

    TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f-22*pad, 1.0f);
    TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f-23*pad, 1.0f);
    TextRendering_PrintString(window, "                            V                           ", -1.0f, 1.0f-24*pad, 1.0f);

    TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f-25*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f-26*pad, 1.0f);
}

// Escrevemos na tela os ângulos de Euler definidos nas variáveis globais
// g_AngleX, g_AngleY, e g_AngleZ.
void TextRendering_ShowEulerAngles(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n", g_AngleZ, g_AngleY, g_AngleX);

    TextRendering_PrintString(window, buffer, -1.0f+pad/10, -1.0f+2*pad/10, 1.0f);
}

// Escrevemos na tela qual matriz de projeção está sendo utilizada.
void TextRendering_ShowProjection(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    if ( g_UsePerspectiveProjection )
        TextRendering_PrintString(window, "Perspective", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
    else
        TextRendering_PrintString(window, "Orthographic", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
}

// Escrevemos na tela o número de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Variáveis estáticas (static) mantém seus valores entre chamadas
    // subsequentes da função!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);
    
        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}

// Função para debugging: imprime no terminal todas informações de um modelo
// geométrico carregado de um arquivo ".obj".
// Veja: https://github.com/syoyo/tinyobjloader/blob/22883def8db9ef1f3ffb9b404318e7dd25fdbb51/loader_example.cc#L98
void PrintObjModelInfo(ObjModel* model)
{
  const tinyobj::attrib_t                & attrib    = model->attrib;
  const std::vector<tinyobj::shape_t>    & shapes    = model->shapes;
  const std::vector<tinyobj::material_t> & materials = model->materials;

  printf("# of vertices  : %d\n", (int)(attrib.vertices.size() / 3));
  printf("# of normals   : %d\n", (int)(attrib.normals.size() / 3));
  printf("# of texcoords : %d\n", (int)(attrib.texcoords.size() / 2));
  printf("# of shapes    : %d\n", (int)shapes.size());
  printf("# of materials : %d\n", (int)materials.size());

  for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
    printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.vertices[3 * v + 0]),
           static_cast<const double>(attrib.vertices[3 * v + 1]),
           static_cast<const double>(attrib.vertices[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
    printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.normals[3 * v + 0]),
           static_cast<const double>(attrib.normals[3 * v + 1]),
           static_cast<const double>(attrib.normals[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
    printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.texcoords[2 * v + 0]),
           static_cast<const double>(attrib.texcoords[2 * v + 1]));
  }

  // For each shape
  for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\n", static_cast<long>(i),
           shapes[i].name.c_str());
    printf("Size of shape[%ld].indices: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.indices.size()));

    size_t index_offset = 0;

    assert(shapes[i].mesh.num_face_vertices.size() ==
           shapes[i].mesh.material_ids.size());

    printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

    // For each face
    for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
      size_t fnum = shapes[i].mesh.num_face_vertices[f];

      printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
             static_cast<unsigned long>(fnum));

      // For each vertex in the face
      for (size_t v = 0; v < fnum; v++) {
        tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
        printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
               static_cast<long>(v), idx.vertex_index, idx.normal_index,
               idx.texcoord_index);
      }

      printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
             shapes[i].mesh.material_ids[f]);

      index_offset += fnum;
    }

    printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.tags.size()));
    for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
      printf("  tag[%ld] = %s ", static_cast<long>(t),
             shapes[i].mesh.tags[t].name.c_str());
      printf(" ints: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
        printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
        if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" floats: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
        printf("%f", static_cast<const double>(
                         shapes[i].mesh.tags[t].floatValues[j]));
        if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" strings: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
        printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
        if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");
      printf("\n");
    }
  }

  for (size_t i = 0; i < materials.size(); i++) {
    printf("material[%ld].name = %s\n", static_cast<long>(i),
           materials[i].name.c_str());
    printf("  material.Ka = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].ambient[0]),
           static_cast<const double>(materials[i].ambient[1]),
           static_cast<const double>(materials[i].ambient[2]));
    printf("  material.Kd = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].diffuse[0]),
           static_cast<const double>(materials[i].diffuse[1]),
           static_cast<const double>(materials[i].diffuse[2]));
    printf("  material.Ks = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].specular[0]),
           static_cast<const double>(materials[i].specular[1]),
           static_cast<const double>(materials[i].specular[2]));
    printf("  material.Tr = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].transmittance[0]),
           static_cast<const double>(materials[i].transmittance[1]),
           static_cast<const double>(materials[i].transmittance[2]));
    printf("  material.Ke = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].emission[0]),
           static_cast<const double>(materials[i].emission[1]),
           static_cast<const double>(materials[i].emission[2]));
    printf("  material.Ns = %f\n",
           static_cast<const double>(materials[i].shininess));
    printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
    printf("  material.dissolve = %f\n",
           static_cast<const double>(materials[i].dissolve));
    printf("  material.illum = %d\n", materials[i].illum);
    printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
    printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
    printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
    printf("  material.map_Ns = %s\n",
           materials[i].specular_highlight_texname.c_str());
    printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
    printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
    printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
    printf("  <<PBR>>\n");
    printf("  material.Pr     = %f\n", materials[i].roughness);
    printf("  material.Pm     = %f\n", materials[i].metallic);
    printf("  material.Ps     = %f\n", materials[i].sheen);
    printf("  material.Pc     = %f\n", materials[i].clearcoat_thickness);
    printf("  material.Pcr    = %f\n", materials[i].clearcoat_thickness);
    printf("  material.aniso  = %f\n", materials[i].anisotropy);
    printf("  material.anisor = %f\n", materials[i].anisotropy_rotation);
    printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
    printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
    printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
    printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
    printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
    std::map<std::string, std::string>::const_iterator it(
        materials[i].unknown_parameter.begin());
    std::map<std::string, std::string>::const_iterator itEnd(
        materials[i].unknown_parameter.end());

    for (; it != itEnd; it++) {
      printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
    }
    printf("\n");
  }
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :
