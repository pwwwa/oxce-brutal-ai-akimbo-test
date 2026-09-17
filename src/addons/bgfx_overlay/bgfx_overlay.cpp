#include "bgfx_overlay.h"
#include <SDL/SDL_syswm.h>
#include <iostream>
#include <fstream>

// Определение глобальных констант и переменных, объявленных в хедере
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

bgfx::ShaderHandle vsh = BGFX_INVALID_HANDLE;
bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;

BgfxOverlay g_overlay;


// Геометрия квада в NDC (Normalized Device Coordinates)
static PosTexCoordVertex s_waterQuadVertices[] = {
	{-1.0f, 1.0f, 0.0f, 0.0f, 0.0f},  // Верхний левый угол
	{1.0f, 1.0f, 0.0f, 1.0f, 0.0f},   // Верхний правый угол
	{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f}, // Нижний левый угол
	{1.0f, -1.0f, 0.0f, 1.0f, 1.0f},  // Нижний правый угол
};

static const uint16_t s_waterQuadIndices[] = {
	0,
	1,
	2,
	1,
	3,
	2,
};


// Внутренние статические структуры и геометрия квада
static PosTexCoordVertex s_quadVertices[] = {
    { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f },
    {  1.0f,  1.0f, 0.0f, 1.0f, 0.0f },
    { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
    {  1.0f, -1.0f, 0.0f, 1.0f, 1.0f },
};

static const uint16_t s_quadIndices[] = {
    0, 1, 2,
    1, 3, 2,
};

// Загрузка бинарного шейдера
bgfx::ShaderHandle loadShader(const char* filePath) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) return BGFX_INVALID_HANDLE;

    size_t fileSize = (size_t)file.tellg();
    file.seekg(0);

    const bgfx::Memory* mem = bgfx::alloc(fileSize + 1);
    file.read((char*)mem->data, fileSize);
    mem->data[fileSize] = '\0';
    file.close();

    return bgfx::createShader(mem);
}

// Инициализация графического бэкенда bgfx


int initBgfxCrt(int width, int height)
{
	bgfx::Init init;
	init.type = bgfx::RendererType::Count; // Авто-выбор бэкенда
	init.vendorId = BGFX_PCI_ID_NONE;

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);

	if (SDL_GetWMInfo(&wmInfo) == 1)
	{
		init.swapChain.nwh = (void*)wmInfo.window;
		init.swapChain.ndt = nullptr;
	}
	else
	{
		return -1;
	}

	// Задаем физические размеры окна при старте
	init.swapChain.width = uint32_t(width);
	init.swapChain.height = uint32_t(height);
	init.swapChain.formatColor = bgfx::TextureFormat::BGRA8;

	if (!bgfx::init(init))
	{
		std::cerr << "Ошибка инициализации bgfx!" << std::endl;
		return -1;
	}

	// ИСПРАВЛЕНИЕ: Вызываем reset в соответствии с вашим API (по умолчанию BGFX_RESET_NONE)
	bgfx::reset();

	bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

	// Создаем Uniform для текстуры
	g_overlay.s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

	// ДОБАВИТЬ СЮДА: Создаем Uniform для времени и разрешения (тип vec4)
	g_overlay.u_time_res = bgfx::createUniform("u_time_res", bgfx::UniformType::Vec4);



	// Описываем формат вершин
	g_overlay.vertexLayout
		.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();

	// Создаем Uniform для текстуры
	g_overlay.s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

	// Загружаем шейдеры
	//vsh = loadShader("vs_quad.bin");
	//fsh = loadShader("fs_crt.bin");

	vsh = loadShader("vs2.bin");
	fsh = loadShader("fs2.bin");

	//fsh = loadShader("crtshader.bin");
	


	/*vsh = loadShader("vcrtaber.bin");
	fsh = loadShader("fcrtaber.bin");*/

	if (!bgfx::isValid(vsh) || !bgfx::isValid(fsh))
	{
		std::cerr << "[BGFX ERROR] Shaders failed to load!" << std::endl;
	}

	g_overlay.passthroughProgram = bgfx::createProgram(vsh, fsh, true);

	return 1;
}


// ============================================================================
// ПОЛНЫЙ КОД ИНИЦИАЛИЗАЦИИ 2D WATER BGFX
// ============================================================================
int initBgfx(int width, int height)
{
	bgfx::Init init;
	init.type = bgfx::RendererType::Count; // Авто-выбор оптимального бэкенда (D3D11/Vulkan/GL)
	init.vendorId = BGFX_PCI_ID_NONE;      //

	// Привязка контекста рендеринга к окну ОС через SDL
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version); //

	if (SDL_GetWMInfo(&wmInfo) == 1) //
	{
		init.swapChain.nwh = (void*)wmInfo.window; //
		init.swapChain.ndt = nullptr;              //
	}
	else
	{
		std::cerr << "Не удалось получить системную информацию окна SDL!" << std::endl;
		return -1;
	}

	// Задаем физические размеры буфера кадра
	init.swapChain.width = uint32_t(width);
	init.swapChain.height = uint32_t(height);
	init.swapChain.formatColor = bgfx::TextureFormat::BGRA8;     //
	init.swapChain.flags = BGFX_RESET_VSYNC;                     //
	init.reset = BGFX_RESET_NONE | BGFX_RESET_FLIP_AFTER_RENDER; //

	// Инициализация bgfx core
	if (!bgfx::init(init))
	{
		std::cerr << "Ошибка инициализации bgfx ядра!" << std::endl;
		return -1;
	}

	// Сброс параметров и обновление размеров вьюпорта
	bgfx::reset();

	// Настраиваем View 0 и параметры очистки
	bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));                   //
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0); //

	// Описываем формат вершин (Позиция XYZ + Координаты текстуры UV)
	g_overlay.vertexLayout
		.begin()                                                  //
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)  //
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float) //
		.end();                                                   //

	// Создаем статические буферы геометрии для рендеринга квада
	g_overlay.vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_waterQuadVertices, sizeof(s_waterQuadVertices)), g_overlay.vertexLayout); //
	g_overlay.ibh = bgfx::createIndexBuffer(bgfx::makeRef(s_waterQuadIndices, sizeof(s_waterQuadIndices)));                            //

	// Создаем Uniform-самплер для текстуры экрана
	g_overlay.s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler); //

	// Создаем Uniform-параметр типа Vec4 для передачи времени и настроек волн
	g_overlay.u_waterParams = bgfx::createUniform("u_waterParams", bgfx::UniformType::Vec4);

	// ИСПОЛЬЗОВАНИЕ СУЩЕСТВУЮЩЕЙ ФУНКЦИИ: Загружаем шейдеры рефракции воды через loadShader
	//vsh = loadShader("vwater.bin");
	//fsh = loadShader("fwater.bin");


	vsh = loadShader("vjimswater.bin");
	fsh = loadShader("fjimswater.bin");

	//fjimswater.bin

	// Проверяем валидность бинарников в памяти
	if (!bgfx::isValid(vsh))
	{
		std::cerr << "[BGFX ERROR] Failed to load vs_water2d.bin! Check file path." << std::endl; //
	}
	if (!bgfx::isValid(fsh))
	{
		std::cerr << "[BGFX ERROR] Failed to load fs_water2d.bin! Check file path." << std::endl; //
	}

	// Линкуем шейдерную программу
	g_overlay.passthroughProgram = bgfx::createProgram(vsh, fsh, true); //

	// Проверка успешности сборки шейдерной программы
	if (!bgfx::isValid(g_overlay.passthroughProgram)) //
	{
		bgfx::setDebug(BGFX_DEBUG_TEXT);                                                               //
		bgfx::dbgTextPrintf(2, 5, 0x0c, ">>>>>>> CRITICAL ERROR: Water Shader program is NOT valid!"); //
		return -1;
	}
	else
	{
		bgfx::setDebug(BGFX_DEBUG_TEXT);
		bgfx::dbgTextPrintf(2, 5, 0x0a, "WATER SHADER PROGRAM SUCCESSFULLY INITIALIZED");
	}

	return 1;
}


// Захват кадра SDL_Surface, перенос в текстуру bgfx и отрисовка с шейдером
// Модифицированный initBgfx
int initBgfx2(int width, int height)
{
	bgfx::Init init;
	init.type = bgfx::RendererType::Count; // Авто-выбор бэкенда (D3D11)
	init.vendorId = BGFX_PCI_ID_NONE;

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);

	if (SDL_GetWMInfo(&wmInfo) == 1)
	{
		init.swapChain.nwh = (void*)wmInfo.window;
		init.swapChain.ndt = nullptr;
	}
	else
	{
		return -1;
	}

	init.swapChain.width = width;
	init.swapChain.height = height;
	init.swapChain.formatColor = bgfx::TextureFormat::BGRA8;
	//init.swapChain.formatColor = bgfx::TextureFormat::BGRA8;
	init.swapChain.flags = BGFX_RESET_VSYNC;
	init.reset = BGFX_RESET_NONE | BGFX_RESET_FLIP_AFTER_RENDER;

	if (!bgfx::init(init))
	{
		std::cerr << "Ошибка инициализации bgfx!" << std::endl;
		return -1;
	}

	// VIEW 0: Отвечает ТОЛЬКО за очистку экрана в черный цвет
	bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

	// VIEW 1: Отвечает за отрисовку текстуры игры поверх очищенного экрана
	//bgfx::setViewRect(1, 0, 0, uint16_t(width), uint16_t(height));
	// ВАЖНО: Для View 1 очистку НЕ включаем, чтобы не затирать кадр!


	


	// Описываем формат вершин
	g_overlay.vertexLayout
		.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();

	g_overlay.vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_quadVertices, sizeof(s_quadVertices)), g_overlay.vertexLayout);
	g_overlay.ibh = bgfx::createIndexBuffer(bgfx::makeRef(s_quadIndices, sizeof(s_quadIndices)));
	g_overlay.s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

	/*vsh = loadShader("vs_quad.bin");
	fsh = loadShader("fs_crt.bin");*/

	vsh = loadShader("vs_passthrough.bin");
	fsh = loadShader("fs_passthrough.bin");
	

	if (!bgfx::isValid(vsh))
		std::cerr << "[BGFX ERROR] Failed to load vs_quad.bin!" << std::endl;
	if (!bgfx::isValid(fsh))
		std::cerr << "[BGFX ERROR] Failed to load fs_crt.bin!" << std::endl;

	g_overlay.passthroughProgram = bgfx::createProgram(vsh, fsh, true);

	return 1;
}

// Модифицированный UpdateAndRenderBgfxOverlay


void UpdateAndRenderBgfxOverlayCRT(SDL_Surface* surface)
{
	if (!surface)
		return;

	// 1. Получаем физические размеры окна вывода bgfx
	// ВАЖНО: Вместо размеров surface здесь должны быть размеры окна (например, 1280x800).
	// Если у вас есть глобальные переменные или функции доступа к ним, используйте их.
	// Пока для безопасности берем текущие размеры из текстуры, если она создана.
	uint16_t viewWidth = 1280; // Замените на реальную ширину вашего окна (Screen::getWidth())
	uint16_t viewHeight = 800; // Замените на реальную высоту вашего окна (Screen::getHeight())

	uint16_t texWidth = static_cast<uint16_t>(surface->w);
	uint16_t texHeight = static_cast<uint16_t>(surface->h);

	// Блокируем поверхность для безопасного чтения пикселей
	if (SDL_MUSTLOCK(surface))
	{
		if (SDL_LockSurface(surface) < 0)
			return;
	}

	// Копируем пиксели из SDL_Surface в память bgfx
	const bgfx::Memory* mem = bgfx::copy(surface->pixels, surface->pitch * texHeight);

	if (SDL_MUSTLOCK(surface))
	{
		SDL_UnlockSurface(surface);
	}

	// Создаем или обновляем текстуру игры
	if (!bgfx::isValid(g_overlay.backgroundTexture))
	{
		g_overlay.backgroundTexture = bgfx::createTexture2D(
			texWidth, texHeight, false, 1,
			bgfx::TextureFormat::RGBA8, // Смените на BGRA8, если перепутаны цвета (синий/красный)
			BGFX_TEXTURE_NONE,
			nullptr);
	}
	bgfx::updateTexture2D(g_overlay.backgroundTexture, 0, 0, 0, 0, texWidth, texHeight, mem);

	// 2. Расчет соотношения сторон (Aspect Ratio Correction)
	float targetAspect = (float)texWidth / (float)texHeight;
	float viewAspect = (float)viewWidth / (float)viewHeight;

	float xFactor = 1.0f;
	float yFactor = 1.0f;

	if (viewAspect > targetAspect)
	{
		xFactor = targetAspect / viewAspect;
	}
	else
	{
		yFactor = viewAspect / targetAspect;
	}

	// Создаем динамическую геометрию квада с учетом пропорций
	PosTexCoordVertex dynamicQuad[] = {
		{-xFactor, yFactor, 0.0f, 0.0f, 0.0f},
		{xFactor, yFactor, 0.0f, 1.0f, 0.0f},
		{-xFactor, -yFactor, 0.0f, 0.0f, 1.0f},
		{xFactor, -yFactor, 0.0f, 1.0f, 1.0f},
	};

	// Индексы для отрисовки двух треугольников квада
	const uint16_t dynamicIndices[] = {
		0,
		1,
		2,
		1,
		3,
		2,
	};

	// 3. Отрисовка кадра через bgfx
	bgfx::setViewRect(0, 0, 0, viewWidth, viewHeight);
	bgfx::setTexture(0, g_overlay.s_texColor, g_overlay.backgroundTexture);
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);

	//CRT DINAMIC

	  // --- ДОБАВИТЬ ЭТОТ БЛОК ПЕРЕД SUBMIT ---
	// Получаем время в секундах (SDL_GetTicks() возвращает миллисекунды)
	float currentTime = SDL_GetTicks() / 1000.0f;

	// Формируем vec4: x - время, y - ширина окна, z - высота окна, w - не используется
	float timeResData[4] = {currentTime, (float)surface->w, (float)surface->h, 0.0f};

	// Загружаем данные в юниформ
	bgfx::setUniform(g_overlay.u_time_res, timeResData);
	// ---------------------------------------

	bgfx::TransientVertexBuffer tvb;
	bgfx::TransientIndexBuffer tib;
	// ... здесь идет ваш код заполнения буферов ...


	/*bgfx::TransientVertexBuffer tvb;
	bgfx::TransientIndexBuffer tib;*/

	// Вызываем аллокацию (в актуальном bgfx API они возвращают void)
	bgfx::allocTransientVertexBuffer(&tvb, 4, g_overlay.vertexLayout);
	bgfx::allocTransientIndexBuffer(&tib, 6);

	// Исправленная проверка успешности выделения памяти через указатели .data
	if (tvb.data != nullptr && tib.data != nullptr)
	{
		// Копируем данные геометрии во временные буферы GPU
		std::memcpy(tvb.data, dynamicQuad, sizeof(dynamicQuad));
		std::memcpy(tib.data, dynamicIndices, sizeof(dynamicIndices));

		// Привязываем буферы к контексту рендера
		bgfx::setVertexBuffer(0, &tvb);
		bgfx::setIndexBuffer(&tib);

		// Отправляем команду отрисовки с вашим CRT-шейдером
		bgfx::submit(0, g_overlay.passthroughProgram);
	}
	else
	{
		std::cerr << "[BGFX ERROR] Failed to allocate transient buffers data!" << std::endl;
	}

	// Выводим отладочную информацию поверх экрана
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf(2, 2, 0x0e, "API: %d", bgfx::getRendererType());
	bgfx::dbgTextPrintf(2, 3, 0x0f, "Render Resolution: %dx%d", viewWidth, viewHeight);
	bgfx::dbgTextPrintf(2, 4, 0x0f, "Texture Resolution: %dx%d", texWidth, texHeight);

	// Переворачиваем кадр bgfx
	bgfx::frame();
}

// ============================================================================
// ПОЛНЫЙ КОД ФУНКЦИИ ОБНОВЛЕНИЯ И РЕНДЕРИНГА ЭФФЕКТА ВОДЫ
// ============================================================================
void UpdateAndRenderBgfxOverlayW(SDL_Surface* surface)
{
	if (!surface)
		return;

	// 1. Получаем актуальные физические размеры окна вывода bgfx
	// Для OpenXcom задаем целевое разрешение апскейла (например, 1280x800 или 1920x1080)
	// Вы можете заменить эти константы на динамические вызовы Screen::getWidth() / getHeight()
	uint16_t viewWidth = 1280;
	uint16_t viewHeight = 800;

	uint16_t texWidth = static_cast<uint16_t>(surface->w);
	uint16_t texHeight = static_cast<uint16_t>(surface->h);

	// Блокируем поверхность SDL для безопасного чтения пикселей из оперативной памяти
	if (SDL_MUSTLOCK(surface))
	{
		if (SDL_LockSurface(surface) < 0)
			return;
	}

	// Копируем пиксели из SDL_Surface во внутренний буфер памяти bgfx
	// bgfx автоматически освободит эту память после отправки текстуры в GPU
	const bgfx::Memory* mem = bgfx::copy(surface->pixels, surface->pitch * texHeight);

	if (SDL_MUSTLOCK(surface))
	{
		SDL_UnlockSurface(surface);
	}

	// Создаем или динамически обновляем текстуру оригинального кадра игры
	if (!bgfx::isValid(g_overlay.backgroundTexture))
	{
		g_overlay.backgroundTexture = bgfx::createTexture2D(
			texWidth, texHeight, false, 1,
			bgfx::TextureFormat::RGBA8, // Смените на BGRA8, если перепутаны цвета (красный/синий)
			BGFX_TEXTURE_NONE,
			nullptr);
	}
	bgfx::updateTexture2D(g_overlay.backgroundTexture, 0, 0, 0, 0, texWidth, texHeight, mem);

	// 2. Расчет соотношения сторон (Aspect Ratio Correction)
	// Позволяет сохранить пропорции оригинального кадра (320x200) на больших экранах
	float targetAspect = (float)texWidth / (float)texHeight;
	float viewAspect = (float)viewWidth / (float)viewHeight;

	float xFactor = 1.0f;
	float yFactor = 1.0f;

	if (viewAspect > targetAspect)
	{
		xFactor = targetAspect / viewAspect;
	}
	else
	{
		yFactor = viewAspect / targetAspect;
	}

	// Формируем динамический квад с учетом вычисленных пропорций экрана
	PosTexCoordVertex dynamicQuad[] = {
		{-xFactor, yFactor, 0.0f, 0.0f, 0.0f},  // Верхний левый угол
		{xFactor, yFactor, 0.0f, 1.0f, 0.0f},   // Верхний правый угол
		{-xFactor, -yFactor, 0.0f, 0.0f, 1.0f}, // Нижний левый угол
		{xFactor, -yFactor, 0.0f, 1.0f, 1.0f},  // Нижний правый угол
	};

	// Таблица индексов для сборки двух треугольников (quad)
	const uint16_t dynamicIndices[] = {
		0,
		1,
		2,
		1,
		3,
		2,
	};

	// 3. Конфигурация конвейера рендеринга bgfx
	bgfx::setViewRect(0, 0, 0, viewWidth, viewHeight);
	bgfx::setTexture(0, g_overlay.s_texColor, g_overlay.backgroundTexture);
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);

	// --- БЛОК РАСЧЕТА И ПЕРЕДАЧИ ПАРАМЕТРОВ ВОДЫ ---
	// Переводим миллисекунды от старта SDL в секунды с плавающей запятой
	float currentTime = SDL_GetTicks() / 1000.0f;

	float distortionStrength = 0.015f; // Интенсивность преломления (рекомендуется от 0.01 до 0.03)
	float waveSpeed = 3.5f;            // Скорость движения волн (чем выше, тем быстрее колебания)

	// Пакуем данные в структуру vec4 для шейдера: x - время, y - сила, z - скорость, w - резерв
	float waterParamsData[4] = {currentTime, distortionStrength, waveSpeed, 0.0f};

	// Отправляем массив данных в зарегистрированный юниформ u_waterParams
	bgfx::setUniform(g_overlay.u_waterParams, waterParamsData);
	// ----------------------------------------------

	// Выделяем временные буферы в видеопамяти, которые живут ровно один кадр
	bgfx::TransientVertexBuffer tvb;
	bgfx::TransientIndexBuffer tib;

	bgfx::allocTransientVertexBuffer(&tvb, 4, g_overlay.vertexLayout);
	bgfx::allocTransientIndexBuffer(&tib, 6);

	// Если память в GPU успешно выделена, копируем туда геометрию и отправляем команду на отрисовку
	if (tvb.data != nullptr && tib.data != nullptr)
	{
		std::memcpy(tvb.data, dynamicQuad, sizeof(dynamicQuad));
		std::memcpy(tib.data, dynamicIndices, sizeof(dynamicIndices));

		bgfx::setVertexBuffer(0, &tvb);
		bgfx::setIndexBuffer(&tib);

		// Передаем команду в View 0 с использованием программы рефракции воды
		bgfx::submit(0, g_overlay.passthroughProgram);
	}
	else
	{
		std::cerr << "[BGFX ERROR] Failed to allocate transient buffers for water overlay!" << std::endl;
	}

	// 4. Отрисовка отладочной информации поверх кадра игры
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf(2, 2, 0x0e, "API: %d", bgfx::getRendererType());
	bgfx::dbgTextPrintf(2, 3, 0x0f, "Render Resolution: %dx%d", viewWidth, viewHeight);
	bgfx::dbgTextPrintf(2, 4, 0x0f, "Texture Resolution: %dx%d", texWidth, texHeight);
	bgfx::dbgTextPrintf(2, 5, 0x0b, "Water Effect: ACTIVE (Time: %.2f)", currentTime);

	// Переворачиваем задний буфер и выводим изображение на экран
	bgfx::frame();
}


void UpdateAndRenderBgfxOverlay(SDL_Surface* surface)
{
	if (!surface)
		return;

	// 1. Получаем актуальные физические размеры окна вывода bgfx
	// Для OpenXcom задаем целевое разрешение апскейла (например, 1280x800)
	uint16_t viewWidth = 1280;
	uint16_t viewHeight = 800;

	uint16_t texWidth = static_cast<uint16_t>(surface->w);
	uint16_t texHeight = static_cast<uint16_t>(surface->h);

	// Блокируем поверхность SDL для безопасного чтения пикселей из оперативной памяти
	if (SDL_MUSTLOCK(surface))
	{
		if (SDL_LockSurface(surface) < 0)
			return;
	}

	// Копируем пиксели из SDL_Surface во внутренний буфер памяти bgfx
	const bgfx::Memory* mem = bgfx::copy(surface->pixels, surface->pitch * texHeight);

	if (SDL_MUSTLOCK(surface))
	{
		SDL_UnlockSurface(surface);
	}

	// Создаем или динамически обновляем текстуру оригинального кадра игры
	if (!bgfx::isValid(g_overlay.backgroundTexture))
	{
		g_overlay.backgroundTexture = bgfx::createTexture2D(
			texWidth, texHeight, false, 1,
			bgfx::TextureFormat::RGBA8, // Смените на BGRA8, если перепутаны цвета (красный/синий)
			BGFX_TEXTURE_NONE,
			nullptr);
	}
	bgfx::updateTexture2D(g_overlay.backgroundTexture, 0, 0, 0, 0, texWidth, texHeight, mem);

	// 2. Расчет соотношения сторон (Aspect Ratio Correction)
	float targetAspect = (float)texWidth / (float)texHeight;
	float viewAspect = (float)viewWidth / (float)viewHeight;

	float xFactor = 1.0f;
	float yFactor = 1.0f;

	if (viewAspect > targetAspect)
	{
		xFactor = targetAspect / viewAspect;
	}
	else
	{
		yFactor = viewAspect / targetAspect;
	}

	// Формируем динамический квад с учетом вычисленных пропорций экрана
	PosTexCoordVertex dynamicQuad[] = {
		{-xFactor, yFactor, 0.0f, 0.0f, 0.0f},  // Верхний левый угол
		{xFactor, yFactor, 0.0f, 1.0f, 0.0f},   // Верхний правый угол
		{-xFactor, -yFactor, 0.0f, 0.0f, 1.0f}, // Нижний левый угол
		{xFactor, -yFactor, 0.0f, 1.0f, 1.0f},  // Нижний правый угол
	};

	// Таблица индексов для сборки двух треугольников (quad)
	const uint16_t dynamicIndices[] = {
		0,
		1,
		2,
		1,
		3,
		2,
	};

	// 3. Конфигурация конвейера рендеринга bgfx
	bgfx::setViewRect(0, 0, 0, viewWidth, viewHeight);
	bgfx::setTexture(0, g_overlay.s_texColor, g_overlay.backgroundTexture);
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);

	// --- БЛОК ПОДГОТОВКИ ДАННЫХ ДЛЯ ГЛИТЧ-ШЕЙДЕРА ---
	// Переводим миллисекунды от старта SDL в секунды с плавающей запятой
	float currentTime = SDL_GetTicks() / 1000.0f;

	// Пакуем данные в структуру vec4 для существующего юниформа u_waterParams:
	// x - текущее время в секундах
	// y - физическая высота текстуры (нужна шейдеру для расчета шага в 2 пикселя)
	// z - резерв (0.0f)
	// w - резерв (0.0f)
	float glitchParams[4] = {currentTime, (float)texHeight, 0.0f, 0.0f};

	// Отправляем массив данных в зарегистрированный юниформ u_waterParams
	bgfx::setUniform(g_overlay.u_waterParams, glitchParams);
	// ------------------------------------------------

	// Выделяем временные буферы в видеопамяти, которые живут ровно один кадр
	bgfx::TransientVertexBuffer tvb;
	bgfx::TransientIndexBuffer tib;

	bgfx::allocTransientVertexBuffer(&tvb, 4, g_overlay.vertexLayout);
	bgfx::allocTransientIndexBuffer(&tib, 6);

	// Если память в GPU успешно выделена, копируем туда геометрию и отправляем команду на отрисовку
	if (tvb.data != nullptr && tib.data != nullptr)
	{
		std::memcpy(tvb.data, dynamicQuad, sizeof(dynamicQuad));
		std::memcpy(tib.data, dynamicIndices, sizeof(dynamicIndices));

		bgfx::setVertexBuffer(0, &tvb);
		bgfx::setIndexBuffer(&tib);

		// Передаем команду в View 0
		bgfx::submit(0, g_overlay.passthroughProgram);
	}
	else
	{
		std::cerr << "[BGFX ERROR] Failed to allocate transient buffers for glitch overlay!" << std::endl;
	}

	// 4. Отрисовка отладочной информации поверх кадра игры
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf(2, 2, 0x0e, "API: %d", bgfx::getRendererType());
	bgfx::dbgTextPrintf(2, 3, 0x0f, "Render Resolution: %dx%d", viewWidth, viewHeight);
	bgfx::dbgTextPrintf(2, 4, 0x0f, "Texture Resolution: %dx%d", texWidth, texHeight);
	bgfx::dbgTextPrintf(2, 5, 0x0a, "Glitch Stripe Shift (2px): ACTIVE");

	// Переворачиваем задний буфер и выводим изображение на экран
	bgfx::frame();
}






// Освобождение ресурсов
void shutdownBgfx() {
    if (bgfx::isValid(g_overlay.backgroundTexture)) bgfx::destroy(g_overlay.backgroundTexture);
    if (bgfx::isValid(g_overlay.passthroughProgram)) bgfx::destroy(g_overlay.passthroughProgram);
    if (bgfx::isValid(g_overlay.s_texColor)) bgfx::destroy(g_overlay.s_texColor);
    if (bgfx::isValid(g_overlay.vbh)) bgfx::destroy(g_overlay.vbh);
    if (bgfx::isValid(g_overlay.ibh)) bgfx::destroy(g_overlay.ibh);

    bgfx::shutdown();
}

