#version 330

out vec4 finalColor;

uniform vec4 colDiffuse;

void main()
{
    finalColor = colDiffuse;
}