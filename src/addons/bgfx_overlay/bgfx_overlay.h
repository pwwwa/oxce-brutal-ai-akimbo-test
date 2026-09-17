#ifndef BGFX_OVERLAY_H
#define BGFX_OVERLAY_H

#include <SDL/SDL.h>
#include <bgfx/bgfx.h>

// Константы экрана
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int SCREEN_BPP;

// Шейдеры
extern bgfx::ShaderHandle vsh;
extern bgfx::ShaderHandle fsh;

// Структура для хранения ресурсов overlay
struct BgfxOverlay {
    bgfx::TextureHandle backgroundTexture = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle passthroughProgram = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_texColor = BGFX_INVALID_HANDLE;
    bgfx::VertexLayout vertexLayout;
    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle u_time_res = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle u_waterParams = BGFX_INVALID_HANDLE;
};

// Глобальный объект оверлея
extern BgfxOverlay g_overlay;

// Структура вершины
struct PosTexCoordVertex {
    float x, y, z;
    float u, v;
};

// Прототипы функций
bgfx::ShaderHandle loadShader(const char* filePath);
int initBgfx(int width, int height);
void UpdateAndRenderBgfxOverlay(SDL_Surface* surface);
void UpdateAndRenderBgfxOverlayWH(SDL_Surface* surface, int viewWidth, int viewHeight);

void shutdownBgfx();

#endif // BGFX_OVERLAY_H
