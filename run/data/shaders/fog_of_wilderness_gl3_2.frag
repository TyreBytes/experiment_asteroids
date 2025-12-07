#version 150

// This came from some tutorial, mentioning some drivers required this to function
//   properly but did not delve into why.
precision highp float;

in vec2 fragmentTextureUV;
in vec4 fragmentColor;

out vec4 finalFragColor;

// diffuseTexture is a 2D texture sampler for diffuse texture.
uniform sampler2D diffuseTexture;

uniform float uTime; // Timestep from CPU side to scroll noises
uniform bool uForceTransparency = false; // Used to override the "bottom" to be also transparent
uniform float uFogTransparency = 0.3;

uniform vec2 uFogSize;
uniform vec2 uScreenSize;

// Could be changed per zone
uniform vec3 uPrimaryColor = vec3(0.7, 0.5, 0.4);
uniform vec3 uSecondaryColor = vec3(0.3, 0.2, 0.2);

// Fog speed and animations
uniform vec2 uFogDirection = vec2(0.3, -0.3);
uniform float uFogSpeed = 0.3;

layout (std140) uniform ubFogOfWilderness //NOT A STRUCT
{
	vec4 fog[512];
};

// This will make gl_FragCoord 0,0 for top/left, to windowWidth,windowHeight
layout(origin_upper_left) in vec4 gl_FragCoord;

// Noise parameters
// Controls seamless tiling, lower is more cells
vec2 tileSize = vec2(32.0, 32.0); 

// Higher value is more water like. [0.01-0.1]
float swirlIntensity = 0.02;

// Controls how much the warping bends the noise.
float domainWarpStrength = 0.3; 

// Higher value, less dense. [0.0-1.0]
float noiseDissipation = 0.7; 

// Higher amplitude and lower frequency will give large blobs
// Lower amplitude and higher frequency will give a rippled effect
// Amplitude of the noise [0.0-1.0]
float amplitude = 0.7; 

// Frequency of the noise, higher is more detailed. [0.0-4.0]
float frequency = 1.8; 

// Number of octave in the noise [1-8]
int octaveNumbers = 4; 

// Minimum density threshold, lower is more filled
float densityThresholdMin = 0.0; 

// Maximum density threshold, lower is softer
float densityThresholdMax = 0.6; 

// Noise helper functions
float hash(vec2 p) 
{
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453123);
}

float seamlessNoise(vec2 st, vec2 period) 
{
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = hash(mod(i, period));
    float b = hash(mod(i + vec2(1.0, 0.0), period));
    float c = hash(mod(i + vec2(0.0, 1.0), period));
    float d = hash(mod(i + vec2(1.0, 1.0), period));

    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

vec2 domainWarp(vec2 p, float time) 
{
    vec2 offset = vec2(
        seamlessNoise(p + vec2(0.0, time * 0.1), tileSize * 5.0),
        seamlessNoise(p + vec2(5.3, time * 0.15), tileSize * 5.0)
    );
    return p + offset * domainWarpStrength;
}

vec2 curl(vec2 p, float t) 
{
    float eps = 0.01;
    float n1, n2, a, b;

    // Use seamless noise for curl calculation
    n1 = seamlessNoise(vec2(p.x, p.y + eps) + t, tileSize);
    n2 = seamlessNoise(vec2(p.x, p.y - eps) + t, tileSize);
    a = (n1 - n2) / (2.0 * eps);

    n1 = seamlessNoise(vec2(p.x + eps, p.y) + t, tileSize);
    n2 = seamlessNoise(vec2(p.x - eps, p.y) + t, tileSize);
    b = (n1 - n2) / (2.0 * eps);

    return vec2(a, -b);
}

float getFog(uint cx, uint cy) 
{
    if (cx >= uFogSize.x) { return 1.0; }
    //if (cx < 0) { return 1.0f; }

    uint index = cx + cy * uint(uFogSize.x);
    return fog[index / uint(4)][index % uint(4)];
}

float smoothFogValue()
{
    //vec2 fogUV = gl_FragCoord.xy / uScreenSize.xy * uFogSize;
    vec2 fogUV = gl_FragCoord.xy / uScreenSize.xy * uFogSize;
    fogUV.x = gl_FragCoord.x / clamp(uScreenSize.x, 0, 1920) * uFogSize.x;

    ivec2 cell = ivec2(floor(fogUV));
    vec2 f = fract(fogUV);

    // Clamped corner samples for falloff
    //uint cx0 = min(uint(cell.x), uint(uFogSize.x) - uint(1));
    uint cx0 = uint(cell.x); //unclamped

    uint cy0 = min(uint(cell.y), uint(uFogSize.y) - uint(1));
    //uint cx1 = min(uint(cell.x + 1), uint(uFogSize.x) - uint(1));
    uint cx1 = uint(cell.x + 1); //unclamped
    uint cy1 = min(uint(cell.y + 1), uint(uFogSize.y) - uint(1));

    // Get fog value for each corner of the cell
    float a = getFog(cx0, cy0);
    float b = getFog(cx1, cy0);
    float c = getFog(cx0, cy1);
    float d = getFog(cx1, cy1);

    float fogValue = mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
    return smoothstep(0.0, 1.0, fogValue);    
}

void main(void)
{
    vec2 uv = fragmentTextureUV; //gl_FragCoord.xy / uScreenSize.xy;

	vec4 color = texture(diffuseTexture, fragmentTextureUV);
	finalFragColor = color;
	finalFragColor = clamp(finalFragColor, vec4(0,0,0,0), vec4(1,1,1,1));
	finalFragColor = (finalFragColor) * fragmentColor;

    float fogValue = smoothFogValue();
    finalFragColor.a = fogValue;

    // Adjust UVs by applying the curl effect
    vec2 warpedUV = domainWarp(uv, uTime);
    vec2 curlForce = curl(warpedUV + uTime, uTime);
    vec2 swirledUV = warpedUV + curlForce * swirlIntensity * 0.1;

    // Need to invert the direction as the base swirl effect moves the texture toward top left by default
    vec2 invertedDirection = -normalize(uFogDirection);
    float fog = 0.0;
    
    // Falloff effect for each fog blobs.
    for (int i = 0; i < octaveNumbers; i++) 
    {
        vec2 noiseUV = swirledUV * frequency + invertedDirection * vec2(uTime * uFogSpeed);
        fog += seamlessNoise(noiseUV, tileSize * frequency) * amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }

    fog = smoothstep(densityThresholdMin, densityThresholdMax, fog * noiseDissipation);

    vec3 fogColor = mix(uPrimaryColor, uSecondaryColor, fog);

    float cellHeight = 1.0 / uFogSize.y;
    float limitTop = 3 * cellHeight; // ~0.3333
    float limitBottom = 10 * cellHeight; // ~0.3333

    float cellWidth = 1.0 / uFogSize.x;
    float limitLeft = 6 * cellWidth;
    float limitRight = 1.0 - limitLeft;

    if (uv.y <= limitTop)
    {
        finalFragColor = vec4(1.0, 1.0, 1.0, 0.0);
    }
    else if (true == uForceTransparency)// && uv.x > limitLeft && uv.x < limitRight)
    {
        finalFragColor = vec4(fogColor * uFogTransparency, uFogTransparency);
    }
    // else if (uv.x < limitLeft || uv.x > limitRight)
    // {
    //     finalFragColor = vec4(fogColor, clamp(pow(smoothstep(limitTop, limitBottom, uv.y), 1.2), 0.0, 1.0));
    // }
    else if (uv.y > limitTop && uv.y <= limitBottom)
    {
        finalFragColor = vec4(fogColor * fogValue, clamp(pow(smoothstep(limitTop, limitBottom, uv.y), 1.2), 0.0, 1.0) * finalFragColor.a);
    }
    else
    {
	    finalFragColor = vec4(fogColor * fogValue, finalFragColor.a);
    }

	if (finalFragColor.a < 0.01)
	{
		discard;
	}
}
