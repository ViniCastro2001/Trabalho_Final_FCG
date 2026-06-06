#ifndef GPU_RENDER_H
#define GPU_RENDER_H

#include <glad/glad.h>

// Primitivas de desenho compartilhadas entre o main.cpp e os módulos que
// desenham modelos procedurais (ex.: player_model). As variáveis abaixo são
// definidas no main.cpp e apontam para uniforms do programa de GPU ativo.

extern GLint g_model_uniform;     // matriz "model" do objeto sendo desenhado
extern GLint g_object_id_uniform; // identificador de material (ver object_ids.h)

// Desenha um objeto da cena virtual (g_VirtualScene) carregado de um .obj.
void DrawVirtualObject(const char* object_name);

#endif
