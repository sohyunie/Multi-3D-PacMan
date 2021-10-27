#version 330

layout (location = 0)in vec3 in_position;
layout (location = 1)in vec3 in_normal;
layout (location = 2)in vec2 vTexCoord;

out vec3 v2f_normal;
out vec2 TexCoord;
out vec3 v2f_worldPos; 

uniform mat4 g_modelTransform;
uniform mat4 g_view;
uniform mat4 g_projection;

void main(void)
{
	vec4 worldPos = g_modelTransform * vec4(in_position, 1.0);
	gl_Position = g_projection * g_view * worldPos;
	
	v2f_normal = in_normal;
	TexCoord = vTexCoord;
	v2f_worldPos = vec3(worldPos.x, worldPos.y, worldPos.z);
}
