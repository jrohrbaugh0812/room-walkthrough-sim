#version 400 core
uniform mat4 proj_matrix;
uniform mat4 camera_matrix;
uniform mat4 model_matrix;

uniform float tilingFactor; // New uniform for tiling (ADDED)

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vTexCoord;

out vec4 Position;
out vec2 texCoord;

void main( )
{
    // Compute transformed vertex position in view space
    gl_Position = proj_matrix*(camera_matrix*(model_matrix*vPosition));

    // Pass scaled texture coordinate to frag shader
    texCoord = vTexCoord * tilingFactor; // (CHANGED)
}
