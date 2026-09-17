$input v_texcoord0

#include "bgfx_shader.sh"

// Объявление сэмплера текстуры в слоте 0
SAMPLER2D(s_texColor, 0);

void main()
{
    // Безопасное приведение к vec2 на случай, если v_texcoord0 объявлен как vec3/vec4
    gl_FragColor = texture2D(s_texColor, v_texcoord0.xy);
}
