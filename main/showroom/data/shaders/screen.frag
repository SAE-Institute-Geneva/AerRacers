#version 300 es
precision highp float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform sampler2D outline;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    vec3 outlineColor = texture(outline, TexCoords).rgb;
    float hdrAlpha = texture(screenTexture, TexCoords).a;
    float bloomAlpha = texture(bloomBlur, TexCoords).a;
    
    hdrColor += bloomColor;
        
    vec3 result = vec3(1.0) - exp(-hdrColor * 1.0);
    
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result + outlineColor, bloomAlpha);
}