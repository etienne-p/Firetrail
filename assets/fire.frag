#version 330 core

uniform sampler2D fireTex;
uniform sampler2D noiseTex;

uniform float time;
uniform float fragMul;
uniform float lacunarity;
uniform float gain;
uniform float magnitude;
uniform float noiseScale;

smooth in vec2 texCoord;
smooth in vec3 vPosition;
in float fSize;

out vec4 fColor;

vec2 mBBS(vec2 val, float mdls)
{
    val = mod(val, mdls);
    return mod(val * val, mdls);
}

float mnoise(vec3 pos)
{
    const float MODULUS = 61.0;
    float intArg = floor(pos.z);
    float fracArg = fract(pos.z);
    vec2 hash = mBBS(intArg * 3.0 + vec2(0, 3), MODULUS);
    vec4 g = vec4(texture(noiseTex, vec2(pos.x, pos.y + hash.x) / MODULUS).xy,
                  texture(noiseTex, vec2(pos.x, pos.y + hash.y) / MODULUS).xy * 2.0 - 1.0);
    return mix(g.x + g.y * fracArg, g.z + g.w * (fracArg - 1.0), smoothstep(0.0, 1.0, fracArg));
}

float turbulence(vec3 pos)
{
    float sum = 0.0;
    float freq = 1.0;
    float amp = 1.0;
    
    sum += abs(mnoise(pos * freq)) * amp;
    freq *= lacunarity;
    amp *= gain;
    //
    sum += abs(mnoise(pos * freq)) * amp;
    freq *= lacunarity;
    amp *= gain;
    //
    sum += abs(mnoise(pos * freq)) * amp;
    freq *= lacunarity;
    amp *= gain;
    //
    sum += abs(mnoise(pos * freq)) * amp;
    freq *= lacunarity;
    amp *= gain;
    
    return sum;
}

void main()
{
    vec2 uv = texCoord;
    
    uv.y += sqrt(uv.y) * magnitude * turbulence((vPosition - vec3(.0, time, .0)) * noiseScale);
    
    fColor = texture(fireTex, uv) * fragMul;// * fSize;
}