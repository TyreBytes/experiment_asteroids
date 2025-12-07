#version 150
precision highp float;

in vec2 fragmentTextureUV;
in vec4 fragmentColor;
out vec4 finalFragColor;

uniform sampler2D diffuseTexture;
uniform float uTime;

uniform vec3 uPrimaryColor   = vec3(1.0, 0.31, 0.63);
uniform vec3 uSecondaryColor = vec3(0.24, 0.55, 1.0);

void main()
{
    vec4 tex = texture(diffuseTexture, fragmentTextureUV);
    if (tex.a <= 0.001)
        discard;

    vec2 uv = fragmentTextureUV - 0.5;

    float angle = atan(uv.y, uv.x);
    float t = (angle / 3.14159265) * 0.5 + 0.5;

    // Speed of animation, could be uniform but oh well
    float speed = 0.5;
    t = fract(t + uTime * speed);

    // Ping-pong gradient
    float tt = t * 2.0;
    float ping = 1.0 - abs(tt - 1.0);

    vec3 col = mix(uPrimaryColor, uSecondaryColor, ping);

    finalFragColor = vec4(tex.rgb * col, tex.a);
}
