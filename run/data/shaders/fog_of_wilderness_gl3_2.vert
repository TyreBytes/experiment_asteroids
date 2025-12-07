
#version 150

// vertexPosition was bound to attribute index 0 and vertexColor was bound to attribute index 1
in vec2 vertexPosition;
in vec4 vertexColor;
in vec2 vertexTextureUV;

// We output the ex_Color variable to the next shader in the chain
out vec4 fragmentColor;
out vec2 fragmentTextureUV;

uniform mat4 uObjectToProjection;

void main(void)
{
	// Since we are using flat lines, our input only had two points: x and y.
	// Set the Z coordinate to 0 and W coordinate to 1

	//The flooring of the position avoided jittery movement for sprites/text.
	vec2 floorPosition = floor(vertexPosition);
	gl_Position = vec4(floorPosition, 0.0, 1.0) * uObjectToProjection;

	fragmentColor = vertexColor;
	fragmentColor.rgb *= fragmentColor.a;
	fragmentTextureUV = vertexTextureUV;
}
