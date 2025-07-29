#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

// https://www.shadertoy.com/view/Xltfzj
void main() {
    float Pi = 6.28318530718; // Pi*2
    
    // GAUSSIAN BLUR SETTINGS {{{
    float Directions = 20.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 3.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 30.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}

    // vec2 iResolution = textureSize(texture0, 0);
    vec2 iResolution = vec2(1280, 720);
   
    vec2 Radius = Size/iResolution.xy;
    
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragTexCoord;
    // Pixel colour
    vec4 Color = texture(texture0, uv);
    
    // Blur calculations
    for( float d=0.0; d<Pi; d+=Pi/Directions)
    {
		for(float i=1.0/Quality; i<1.001; i+=1.0/Quality)
        {
			Color += texture( texture0, uv+vec2(cos(d),sin(d))*Radius*i);		
        }
    }
    
    // Output to screen
    Color /= Quality * Directions + 1.0;
    finalColor =  Color;
}
