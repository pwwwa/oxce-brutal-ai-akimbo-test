$input a_position, a_texcoord0
$output v_texcoord0

#include "bgfx_shader.sh"

void main()
{
    // Передаем координаты напрямую. 
    // Поскольку a_position уже (-1.0...1.0), квад идеально займет весь экран.
    gl_Position = vec4(a_position, 1.0);
    
    v_texcoord0 = a_texcoord0;
}
