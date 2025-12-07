#version 150
precision highp float;

in vec2 fragmentTextureUV;
in vec4 fragmentColor;
out vec4 finalFragColor;

uniform sampler2D diffuseTexture;
uniform float uTime;
uniform float outlineThickness = 0.04;

uniform vec3 uPrimaryColor = vec3(0.5, 0.0, 0.0);
uniform vec3 uSecondaryColor = vec3(0.0, 0.0, 0.5);

void main(void) {
    vec4 texColor = texture(diffuseTexture, fragmentTextureUV);
    float alpha = texColor.a;

    float t = outlineThickness;

    // Sample 8 neighboring alphas
    float a1 = texture(diffuseTexture, fragmentTextureUV + vec2(+t,  0.0)).a;
    float a2 = texture(diffuseTexture, fragmentTextureUV + vec2(-t,  0.0)).a;
    float a3 = texture(diffuseTexture, fragmentTextureUV + vec2( 0.0, +t)).a;
    float a4 = texture(diffuseTexture, fragmentTextureUV + vec2( 0.0, -t)).a;
    float a5 = texture(diffuseTexture, fragmentTextureUV + vec2(+t, +t)).a;
    float a6 = texture(diffuseTexture, fragmentTextureUV + vec2(+t, -t)).a;
    float a7 = texture(diffuseTexture, fragmentTextureUV + vec2(-t, +t)).a;
    float a8 = texture(diffuseTexture, fragmentTextureUV + vec2(-t, -t)).a;

    // Find minimum neighbor alpha (to detect edges *inside* the shape)
    float neighborMin = min(min(min(a1, a2), min(a3, a4)), min(min(a5, a6), min(a7, a8)));

    // Inward outline mask: where current pixel is opaque but near transparency
    float edge = smoothstep(0.0, 1.0, alpha - neighborMin);
    float outlineMask = edge * alpha;

    // Animate color (wave effect)
    float wave = sin((fragmentTextureUV.x + fragmentTextureUV.y) * 4.0 + uTime * 2.0) * 0.5 + 0.5;
    vec3 animatedColor = mix(uPrimaryColor, uSecondaryColor, wave);

    // Blend inward outline with base texture
    vec3 color = mix(animatedColor, texColor.rgb, 0.8 - outlineMask);
    float finalAlpha = alpha;


    if (finalAlpha < 0.01)
    {
        discard;
    }

    finalFragColor = vec4(color, finalAlpha);
}
