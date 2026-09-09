#<!--
#   Combined Openxcom Shader v3
#   Properly adjustable smoothing + Bloom + Scanlines
#shouts out:
#Dot 'n bloom shader
#     Author: Themaister
#     License: Public domain
#AND
#   AUTHOR: Powerc80 for making openxcom shader
#   
#   ALL PARAMETERS ARE ADJUSTABLE AT THE TOP OF THE FRAGMENT SHADER
#-->

language: "GLSL"

vertex: |
    #version 110
    uniform vec2 rubyTextureSize;
    varying vec2 pixel_no;
    
    // These are now passed as varyings so they can be controlled
    // from the fragment shader
    varying vec2 texCoord1;
    varying vec2 texCoord2;
    varying vec2 texCoord3;
    varying vec2 texCoord4;
    varying vec2 texCoord5;
    varying vec2 texCoord6;
    varying vec2 texCoord1_zw;
    varying vec2 texCoord2_zw;
    varying vec2 texCoord3_zw;
    varying vec2 texCoord4_zw;
    varying vec2 texCoord5_zw;
    varying vec2 texCoord6_zw;
    
    void main()
    {
        // SAMPLE_RADIUS - Controls pixel smearing
        // 0.0004 = subtle smoothing
        // 0.00077 = original default
        // 0.0015 = strong smoothing/smearing
        // 0.0025 = very blurry (not recommended)
        const float x_value = 0.00077;
        const float y_value = 0.00077;
        
        const vec2 tr = vec2(x_value, y_value);
        const vec2 tl = vec2(-x_value, y_value);
        const vec2 br = vec2(x_value, 0.0);
        const vec2 bl = vec2(-x_value, 0.0);
        
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
        gl_TexCoord[0] = gl_MultiTexCoord0;
        
        // These create a 3x3 sampling grid around the center pixel
        texCoord1 = gl_TexCoord[0].xy - (tr * 0.55);
        texCoord2 = gl_TexCoord[0].xy - (tl * 0.55);
        texCoord3 = gl_TexCoord[0].xy + (tr * 0.55);
        texCoord4 = gl_TexCoord[0].xy + (tl * 0.55);
        texCoord5 = gl_TexCoord[0].xy - tr;
        texCoord6 = gl_TexCoord[0].xy + tr;
        
        texCoord1_zw = gl_TexCoord[0].xy - bl;
        texCoord2_zw = gl_TexCoord[0].xy + br;
        texCoord3_zw = gl_TexCoord[0].xy + bl;
        texCoord4_zw = gl_TexCoord[0].xy - br;
        texCoord5_zw = gl_TexCoord[0].xy - tl;
        texCoord6_zw = gl_TexCoord[0].xy + tl;
        
        pixel_no = gl_MultiTexCoord0.xy * rubyTextureSize;
    }

fragment: |
    #version 110
    uniform sampler2D rubyTexture;
    uniform vec2 rubyTextureSize;
    varying vec2 pixel_no;
    
    // ============================================================
    // ADJUSTABLE PARAMETERS - CHANGE THESE VALUES
    // ============================================================
    
    // SMOOTHING STRENGTH (0.0 to 1.0)
    // 0.0 = pure pixel art, 0.5 = moderate, 1.0 = full smoothing, default = 0.8
    #define SMOOTHING_STRENGTH 0.2
    
    // BLOOM INTENSITY (0.0 to 2.0)
    // 0.0 = no bloom, 0.5 = subtle, 1.0 = strong, default = 0.5
    #define BLOOM_STRENGTH 0.3
    
    // SCANLINE STRENGTH (0.0 to 1.0)
    // 0.0 = no scanlines, 0.18 = subtle, 0.5 = strong, default = 0.18
    #define SCANLINE_STRENGTH 0.0
    
    // SCANLINE THICKNESS (0.3 to 0.8)
    // Lower = thinner lines, Higher = thicker lines, default = 0.65
    #define SCANLINE_THICKNESS 0.3
    
    // SCANLINE SOFTNESS (0.05 to 0.4)
    // Lower = sharper edges, Higher = softer edges, default = 0.1
    #define SCANLINE_SOFTNESS 0.10
    
    // EDGE SHARPNESS (0.5 to 2.0)
    // Lower = softer edges/more smearing, Higher = sharper edges, default = 1
    #define EDGE_SHARPNESS 2.0
    
    // ============================================================
    
    varying vec2 texCoord1;
    varying vec2 texCoord2;
    varying vec2 texCoord3;
    varying vec2 texCoord4;
    varying vec2 texCoord5;
    varying vec2 texCoord6;
    varying vec2 texCoord1_zw;
    varying vec2 texCoord2_zw;
    varying vec2 texCoord3_zw;
    varying vec2 texCoord4_zw;
    varying vec2 texCoord5_zw;
    varying vec2 texCoord6_zw;
    
    // Get original pixel first (needed for blending)
    vec3 original = texture2D(rubyTexture, gl_TexCoord[0].xy).xyz;
    vec3 centre = original;
    const vec3 ref = vec3(1.0, 1.0, 1.0);
    
    // Weight parameters - these control edge detection sensitivity
    // These are now adjustable via EDGE_SHARPNESS
    float start_weight = 1.04 * EDGE_SHARPNESS;
    const float k = -1.07;
    float max_weight = 0.73 * EDGE_SHARPNESS;
    float min_weight = 0.05 / EDGE_SHARPNESS;
    
    float dp(vec3 v) { return dot(abs(v - centre), ref); }
    
    void main()
    {
        // ----- SMOOTHING PASS -----
        // Sample all 8 surrounding pixels
        vec3 i1 = texture2D(rubyTexture, texCoord1).xyz;
        vec3 i2 = texture2D(rubyTexture, texCoord2).xyz;
        vec3 i3 = texture2D(rubyTexture, texCoord3).xyz;
        vec3 i4 = texture2D(rubyTexture, texCoord4).xyz;
        vec3 o1 = texture2D(rubyTexture, texCoord5).xyz;
        vec3 o3 = texture2D(rubyTexture, texCoord6).xyz;
        vec3 o2 = texture2D(rubyTexture, texCoord5_zw).xyz;
        vec3 o4 = texture2D(rubyTexture, texCoord6_zw).xyz;
        vec3 s1 = texture2D(rubyTexture, texCoord1_zw).xyz;
        vec3 s2 = texture2D(rubyTexture, texCoord2_zw).xyz;
        vec3 s3 = texture2D(rubyTexture, texCoord3_zw).xyz;
        vec3 s4 = texture2D(rubyTexture, texCoord4_zw).xyz;
        
        // Calculate weights based on pixel similarity
        float w1 = min(dot(abs(i2 - i4), ref), max(dp(o2), dp(o4)));
        float w2 = min(dot(abs(i1 - i3), ref), max(dp(o1), dp(o3)));
        float w3 = min(dot(abs(i2 - i4), ref), max(dp(o2), dp(o4)));
        float w4 = min(dot(abs(i1 - i3), ref), max(dp(o1), dp(o3)));
        
        // Adjust weights based on edge detection
        if (dp(o3) < dp(o1)) w1 *= dp(o3) / dp(o1);
        if (dp(o4) < dp(o2)) w2 *= dp(o4) / dp(o2);
        if (dp(o1) < dp(o3)) w3 *= dp(o1) / dp(o3);
        if (dp(o2) < dp(o4)) w4 *= dp(o2) / dp(o4);
        
        // Refine center pixel
        centre = (w1 * o1 + w2 * o2 + w3 * o3 + w4 * o4 + 0.00077 * centre) / (w1 + w2 + w3 + w4 + 0.00077);
        
        // Final weight calculation
        w1 = k * dot(abs(i1 - centre) + abs(i3 - centre), ref) / (0.125 * dot(i1 + i3, ref));
        w2 = k * dot(abs(i2 - centre) + abs(i4 - centre), ref) / (0.125 * dot(i2 + i4, ref));
        w3 = k * dot(abs(s1 - centre) + abs(s3 - centre), ref) / (0.125 * dot(s1 + s3, ref));
        w4 = k * dot(abs(s2 - centre) + abs(s4 - centre), ref) / (0.125 * dot(s2 + s4, ref));
        
        w1 = clamp(w1 + start_weight, min_weight, max_weight);
        w2 = clamp(w2 + start_weight, min_weight, max_weight);
        w3 = clamp(w3 + start_weight, min_weight, max_weight);
        w4 = clamp(w4 + start_weight, min_weight, max_weight);
        
        // Apply smoothing
        vec3 smooth_result = (w1 * (i1 + i3) + w2 * (i2 + i4) + w3 * (s1 + s3) + w4 * (s2 + s4) + centre) / (2.0 * (w1 + w2 + w3 + w4) + 1.0);
        
        // Blend between original and smoothed based on STRENGTH
        vec3 blended = mix(original, smooth_result, SMOOTHING_STRENGTH);
        
        // ----- BLOOM PASS (on smoothed image) -----
        vec4 bloom_sum = vec4(0.0);
        vec4 bloom_bum = vec4(0.0);
        vec2 texcoord = vec2(gl_TexCoord[0]);
        vec2 glaresize = vec2(0.896) / rubyTextureSize;
        
        for(int i = -2; i < 2; i++)
        {
            for(int j = -1; j < 1; j++)
            {
                bloom_sum += texture2D(rubyTexture, texcoord + vec2(-i, j) * glaresize) * BLOOM_STRENGTH;
                bloom_bum += texture2D(rubyTexture, texcoord + vec2(j, i) * glaresize) * BLOOM_STRENGTH;
            }
        }
        
        // Apply bloom with proper blending
        vec4 bloom_result = bloom_sum * bloom_sum * bloom_sum * 0.0007 + 
                           bloom_bum * bloom_bum * bloom_bum * 0.0065 + 
                           vec4(blended, 1.0);
        
        // ----- SCANLINE PASS -----
        float row = fract(pixel_no.y);
        float distance_from_center = abs(row - 0.5);
        float scanline = smoothstep(
            SCANLINE_THICKNESS * 0.5,
            SCANLINE_THICKNESS * 0.5 + SCANLINE_SOFTNESS,
            distance_from_center
        );
        
        vec4 final = bloom_result;
        final.rgb *= 1.0 - scanline * SCANLINE_STRENGTH;
        final.a = 1.0;
        
        gl_FragColor = final;
    }

linear: false