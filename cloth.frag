#version 430 core


in vec3 Position;
in vec3 Normal;
in vec2 texUV;

out vec4 outColor;
uniform sampler2D texColor;


void main()
{

	vec3 ambient = vec3(0.1);
	vec3 diffuse = vec3(texture(texColor, texUV));
	vec3 specular = 0.1 * pow( (0.2, 0.3, 0.1), 30) * vec3(0.5);

	vec3 color = ambient + diffuse + specular;
	outColor = vec4(color, 1.0);

 }
