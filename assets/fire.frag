#version 330 core

uniform sampler2D fireTex;
uniform sampler2D noiseTex;

uniform float lacunarity = 2.0;
uniform float gain = .5;
uniform float magnitude = 1.3;
uniform float time = .0;

smooth in vec2 vTexCoord;

out vec4 fColor;

void main()
{
    // use vertex' normal for tex location.
    vec2 st = vec2(vTexCoord.y, .5);
    
    vec2 nst = vTexCoord.xy - vec2(.0, time);
    nst.x *= 10.0f;
    
    vec4 noise = texture(noiseTex, nst);
    
    st.y = length(noise) * .5;
    
    vec4 t = texture(fireTex, st);
    
    fColor = t;// + vec4(.1, .1, .1, 1.0);
    
    //fColor = texture( fireTex, vec2(vTexCoord.y, .5)) + vec4(.2, .0, .0, .2);
}