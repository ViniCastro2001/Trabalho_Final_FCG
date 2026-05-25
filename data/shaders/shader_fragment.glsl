#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define SAFE_ZONE 3
#define BIGFOOT 4
#define WALL 5
#define BIGFOOT_EYES 6
#define HUD_BAR_BACK 7
#define HUD_BAR_FILL 8
#define SHOTGUN 9
#define HANDS 10
#define ROAD 11
#define SIDEWALK 12
#define CONCRETE 13
#define METAL_ROOF 14
#define WINDOW_MATERIAL 15
#define TREE_TRUNK 16
#define TREE_LEAVES 17
#define CAR_BODY 18
#define CAR_GLASS 19
#define LAMP_LIGHT 20
#define MONSTER_DRINK 21
// Marcadores da câmera de mapa (vista de cima). Sem fog.
#define MAP_MARKER_PLAYER  22
#define MAP_MARKER_BIGFOOT 23
#define MAP_MARKER_ITEM    24
#define WEAPON_METAL 25
#define WEAPON_WOOD 26
#define WEAPON_ACCENT 27

uniform int object_id;

// Quando 1, desativa a neblina (usado pela câmera de mapa).
uniform int u_map_view_active;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(0.35,1.0,0.20,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

	// Coeficiente de refletância difusa
	vec3 Kd0;

    if ( object_id == SPHERE )
    {
        // PREENCHA AQUI as coordenadas de textura da esfera, computadas com
        // projeção esférica EM COORDENADAS DO MODELO. Utilize como referência
        // o slides 134-150 do documento Aula_20_Mapeamento_de_Texturas.pdf.
        // A esfera que define a projeção deve estar centrada na posição
        // "bbox_center" definida abaixo.

        // Você deve utilizar:
        //   função 'length( )' : comprimento Euclidiano de um vetor
        //   função 'atan( , )' : arcotangente. Veja https://en.wikipedia.org/wiki/Atan2.
        //   função 'asin( )'   : seno inverso.
        //   constante M_PI
        //   variável position_model

        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 d = position_model - bbox_center;

        float rho   = length(d);
        float theta = atan(d.x,d.z);
        float phi   = asin(d.y / rho);

        U = (theta + M_PI) / 2.0 / M_PI;
        V = (phi + M_PI_2) / M_PI;

		// Obtemos a refletância difusa a partir da leitura da imagem TextureImage0
		Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
    }
    else if ( object_id == BUNNY )
    {
        // PREENCHA AQUI as coordenadas de textura do coelho, computadas com
        // projeção planar XY em COORDENADAS DO MODELO. Utilize como referência
        // o slides 99-104 do documento Aula_20_Mapeamento_de_Texturas.pdf,
        // e também use as variáveis min*/max* definidas abaixo para normalizar
        // as coordenadas de textura U e V dentro do intervalo [0,1]. Para
        // tanto, veja por exemplo o mapeamento da variável 'p_v' utilizando
        // 'h' no slides 158-160 do documento Aula_20_Mapeamento_de_Texturas.pdf.
        // Veja também a Questão 4 do Questionário 4 no Moodle.

        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);

		// Obtemos a refletância difusa a partir da leitura da imagem TextureImage0
		Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
    }
    else if ( object_id == PLANE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

		// Obtemos a refletância difusa a partir da leitura da imagem TextureImage1
		Kd0 = texture(TextureImage1, vec2(U,V)).rgb;
    }
    else if ( object_id == SAFE_ZONE )
    {
        // Verde sólido para destacar a zona segura.
        Kd0 = vec3(0.0, 1.0, 0.0);
    }
    else if ( object_id == BIGFOOT )
    {
        Kd0 = vec3(0.18, 0.11, 0.07);
    }
    else if ( object_id == BIGFOOT_EYES )
    {
        Kd0 = vec3(1.0, 0.02, 0.01);
    }
    else if ( object_id == HUD_BAR_BACK )
    {
        Kd0 = vec3(0.08, 0.02, 0.025);
    }
    else if ( object_id == HUD_BAR_FILL )
    {
        Kd0 = vec3(1.0, 0.12, 0.12);
    }
    else if ( object_id == SHOTGUN )
    {
        Kd0 = vec3(0.035, 0.035, 0.033);
    }
    else if ( object_id == HANDS )
    {
        Kd0 = vec3(0.48, 0.30, 0.22);
    }
    else if ( object_id == WEAPON_METAL )
    {
        Kd0 = vec3(0.075, 0.080, 0.078);
    }
    else if ( object_id == WEAPON_WOOD )
    {
        Kd0 = vec3(0.26, 0.13, 0.055);
    }
    else if ( object_id == WEAPON_ACCENT )
    {
        Kd0 = vec3(0.72, 0.55, 0.25);
    }
    else if ( object_id == ROAD )
    {
        float lane = step(0.965, fract(position_world.x * 0.22));
        Kd0 = mix(vec3(0.095, 0.095, 0.090), vec3(0.70, 0.68, 0.58), lane * 0.40);
    }
    else if ( object_id == SIDEWALK )
    {
        float seams = step(0.93, fract(position_world.x * 0.55)) + step(0.93, fract(position_world.z * 0.55));
        Kd0 = vec3(0.42, 0.42, 0.38) + vec3(0.08) * min(seams, 1.0);
    }
    else if ( object_id == CONCRETE )
    {
        float stains = 0.04 * sin(position_world.x * 2.1) + 0.04 * sin(position_world.z * 1.7);
        Kd0 = vec3(0.50, 0.48, 0.43) + stains;
    }
    else if ( object_id == METAL_ROOF )
    {
        float ribs = step(0.58, fract(position_world.x * 1.8));
        Kd0 = mix(vec3(0.42, 0.43, 0.42), vec3(0.76, 0.78, 0.77), ribs);
    }
    else if ( object_id == WINDOW_MATERIAL )
    {
        Kd0 = vec3(0.025, 0.040, 0.055);
    }
    else if ( object_id == TREE_TRUNK )
    {
        Kd0 = vec3(0.23, 0.13, 0.07);
    }
    else if ( object_id == TREE_LEAVES )
    {
        float variation = 0.08 * sin(position_world.x * 3.0 + position_world.z * 2.0);
        Kd0 = vec3(0.10, 0.28, 0.08) + variation;
    }
    else if ( object_id == CAR_BODY )
    {
        Kd0 = vec3(0.60, 0.08, 0.06);
    }
    else if ( object_id == CAR_GLASS )
    {
        Kd0 = vec3(0.04, 0.07, 0.09);
    }
    else if ( object_id == LAMP_LIGHT )
    {
        Kd0 = vec3(1.0, 0.82, 0.42);
    }
    else if ( object_id == MONSTER_DRINK )
    {
        Kd0 = texture(TextureImage2, texcoords).rgb;
    }
    else if ( object_id == MAP_MARKER_PLAYER )
    {
        Kd0 = vec3(0.15, 0.85, 1.00); // ciano
    }
    else if ( object_id == MAP_MARKER_BIGFOOT )
    {
        Kd0 = vec3(1.00, 0.15, 0.10); // vermelho
    }
    else if ( object_id == MAP_MARKER_ITEM )
    {
        Kd0 = vec3(1.00, 0.88, 0.10); // amarelo
    }
    else if ( object_id == WALL )
    {
        // Paredes/blocos: textura de tijolos.
        Kd0 = texture(TextureImage0, texcoords).rgb;
    }
    // Equação de Iluminação
    float lambert = max(0,dot(n,l));

    vec3 lit_color = Kd0 * (lambert * 0.62 + 0.045);

    // Neblina simples por distancia: ajuda a esconder o fim do mapa e melhora
    // o clima de perseguicao sem exigir novos assets.
    float camera_distance = length(camera_position.xyz - p.xyz);
    float fog_factor = smoothstep(7.0, 42.0, camera_distance);
    vec3 fog_color = vec3(0.018, 0.021, 0.030);

    if ( object_id == SAFE_ZONE || object_id == BIGFOOT_EYES || object_id == HUD_BAR_BACK || object_id == HUD_BAR_FILL || object_id == LAMP_LIGHT || object_id == MONSTER_DRINK || object_id == MAP_MARKER_PLAYER || object_id == MAP_MARKER_BIGFOOT || object_id == MAP_MARKER_ITEM )
    {
        color.rgb = Kd0;
    }
    else if ( u_map_view_active == 1 )
    {
        // Câmera de mapa: cena renderizada sem neblina, com iluminação cheia.
        color.rgb = lit_color;
    }
    else
    {
        color.rgb = mix(lit_color, fog_color, fog_factor);
    }

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
} 

