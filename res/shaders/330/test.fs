#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom variables
uniform vec2 ballPos;
uniform float ballRad;
uniform float zoom;

uniform vec2 screenSize;

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord) * fragColor * colDiffuse;

    vec2 pixel = vec2(fragTexCoord.x*screenSize.x, fragTexCoord.y*screenSize.y);
    float screenDis = sqrt((ballPos.x - pixel.x)*(ballPos.x - pixel.x) + (ballPos.y - pixel.y)*(ballPos.y - pixel.y));
    float worldDis = screenDis / zoom;

    // Calculate final fragment color
    float lightDistance = 10;
    float colorMultiplier = (lightDistance/(worldDis - ballRad));
    
    finalColor = vec4(texelColor.r + colorMultiplier, texelColor.g * (1 + colorMultiplier), texelColor.b * (1 + colorMultiplier), texelColor.a);

    if (worldDis < ballRad)
        finalColor = vec4(0, 0, 255, 255);
        
}