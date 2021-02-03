#version 300 es
precision highp float;

out vec4 Fragcolor;

in vec2 TexCoords;

uniform sampler2D image;

const float offset = 1.0 / 256.0;

void main()
{             
	vec2 offsets[9] = vec2[](
	    vec2(-offset,  offset), // top-left
	    vec2( 0.0f,    offset), // top-center
	    vec2( offset,  offset), // top-right
	    vec2(-offset,  0.0f),   // center-left
	    vec2( 0.0f,    0.0f),   // center-center
	    vec2( offset,  0.0f),   // center-right
	    vec2(-offset, -offset), // bottom-left
	    vec2( 0.0f,   -offset), // bottom-center
	    vec2( offset, -offset)  // bottom-right
	);

	float kernel[9] = float[](
	    1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
	    2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
	    1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
	);

	vec4 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
	    sampleTex[i] = texture(image, TexCoords.st + offsets[i]);
	}

	vec4 col = vec4(0.0);
	for(int i = 0; i < 9; i++)
	{
	    col += sampleTex[i] * kernel[i];
	}
	
	//Fragcolor = col;
	
    float pi = 6.28318530718;
    
    // GAUSSIAN BLUR SETTINGS
    float directions = 16.0; // BLUR directions (Default 16.0 - More is better but slower)
    float quality = 3.0; // BLUR quality (Default 4.0 - More is better but slower)
    float size = 16.0; // BLUR size (radius)
    // GAUSSIAN BLUR SETTINGS
   
    vec2 radius = size / vec2(1280.0, 720.0);
    
    vec4 color = texture(image, TexCoords);
    
    // Blur calculations
    for (float d = 0.0; d < pi; d += pi / directions)
    {
		for (float i = 1.0 / quality; i <= 1.0; i += 1.0 / quality)
        {
			color += texture(image, TexCoords + vec2(cos(d), sin(d)) * radius * i);		
        }
    }
    
    // Output to screen
    color /= quality * directions - 15.0;
    Fragcolor = color;
}