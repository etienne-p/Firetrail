#version 330 core

#define MODULUS 61.0
#define OCTIVES 4

uniform sampler2D fireTex;
uniform sampler2D noiseTex;

uniform float lacunarity = 2.0;
uniform float gain = .5;
uniform float magnitude = 1.3;
uniform float time = .0;

smooth in vec2 vTexCoord;

out vec4 fColor;

vec2 mBBS(vec2 val, float mdls)
{
    val = mod(val, mdls);
    return mod(val * val, mdls);
}

float mnoise(vec3 pos)
{
    float intArg = floor(pos.z);
    float fracArg = fract(pos.z);
    vec2 hash = mBBS(intArg * 3.0 + vec2(0, 3), MODULUS);
    vec4 g = vec4(
                  texture(noiseTex, vec2(pos.x, pos.y + hash.x) / MODULUS).xy,
                  texture(noiseTex, vec2(pos.x, pos.y + hash.y) / MODULUS).xy * 2.0 - 1.0);
    return mix(g.x + g.y * fracArg, g.z + g.w * (fracArg - 1.0), smoothstep(0.0, 1.0, fracArg));
}

float turbulence(vec3 pos)
{
    float sum = 0.0;
    float freq = 1.0;
    float amp = 1.0;
    
    for(int i = 0; i < OCTIVES; ++i)
    {
        sum += abs(mnoise(pos * freq)) * amp;
        freq *= lacunarity;
        amp *= gain;
    }
    return sum;
}

void main()
{
    // use vertex' normal for tex location.
    vec3 loc = vec3(vTexCoord.xy, .5);
    
    loc.y -= time;
    
    vec2 st = vec2(vTexCoord.y, .5);
    
    st.y += sqrt(st.y) * magnitude * turbulence(loc / );
    
    vec4 t = texture(fireTex, st);
    
    fColor = t + vec4(.2, .0, .0, .2);
    
    //fColor = texture( fireTex, vec2(vTexCoord.y, .5)) + vec4(.2, .0, .0, .2);
}