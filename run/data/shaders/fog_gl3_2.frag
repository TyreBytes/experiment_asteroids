#version 150
precision highp float;

in vec2 fragmentTextureUV;
in vec4 fragmentColor;
out vec4 finalFragColor;

uniform sampler2D diffuseTexture;
uniform float uTime;
uniform bool uForceTransparency = false;
uniform float uFogTransparency = 0.1;

// Fog colors
uniform vec3 uPrimaryColor = vec3(0.7, 0.5, 0.4);
uniform vec3 uSecondaryColor = vec3(0.1, 0.2, 0.2);

// Fog movement
uniform vec2 uFogDirection = vec2(0.3, -0.3);
uniform float uFogSpeed = 0.3;

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
float hash(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453123);
}

float seamlessNoise(vec2 st, vec2 period) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = hash(mod(i, period));
    float b = hash(mod(i + vec2(1.0, 0.0), period));
    float c = hash(mod(i + vec2(0.0, 1.0), period));
    float d = hash(mod(i + vec2(1.0, 1.0), period));

    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

vec2 domainWarp(vec2 p, float time) {
    vec2 offset = vec2(
        seamlessNoise(p + vec2(0.0, time * 0.1), tileSize * 5.0),
        seamlessNoise(p + vec2(5.3, time * 0.15), tileSize * 5.0)
    );
    return p + offset * domainWarpStrength;
}

vec2 curl(vec2 p, float t) {
    float eps = 0.01;
    float n1, n2, a, b;

    n1 = seamlessNoise(vec2(p.x, p.y + eps) + t, tileSize);
    n2 = seamlessNoise(vec2(p.x, p.y - eps) + t, tileSize);
    a = (n1 - n2) / (2.0 * eps);

    n1 = seamlessNoise(vec2(p.x + eps, p.y) + t, tileSize);
    n2 = seamlessNoise(vec2(p.x - eps, p.y) + t, tileSize);
    b = (n1 - n2) / (2.0 * eps);

    return vec2(a, -b);
}

void main(void) {
    vec2 uv = fragmentTextureUV;
    vec4 baseColor = texture(diffuseTexture, uv) * fragmentColor;

    vec2 warpedUV = domainWarp(uv, uTime);
    vec2 curlForce = curl(warpedUV + uTime, uTime);
    vec2 swirledUV = warpedUV + curlForce * swirlIntensity * 0.1;

    vec2 invertedDirection = -normalize(uFogDirection);
    float fog = 0.0;

    for (int i = 0; i < octaveNumbers; i++) {
        vec2 noiseUV = swirledUV * frequency + invertedDirection * (uTime * uFogSpeed);
        fog += seamlessNoise(noiseUV, tileSize * frequency) * amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }

    fog = smoothstep(densityThresholdMin, densityThresholdMax, fog * noiseDissipation);

    vec3 fogColor = mix(uPrimaryColor, uSecondaryColor, fog);
    float alpha = fog * uFogTransparency;

    vec3 finalColor = mix(baseColor.rgb, fogColor, alpha);
    finalFragColor = vec4(finalColor, alpha);

    if (finalFragColor.a < 0.01)
    {
        discard;
    }
}
