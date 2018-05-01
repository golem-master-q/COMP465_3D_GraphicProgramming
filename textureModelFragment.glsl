# version 330 core

in vec2 vs_texCoord;
in vec4 vs_color;
in vec3 vs_normal, vs_worldPos;
out vec4 color;

uniform vec3 light_position;
uniform bool IsTexture;
uniform sampler2D Texture;

void main(void) {
  vec3 normal = normalize(vs_normal);
  if(IsTexture) {
    color = texture(Texture, vs_texCoord);
  } else {
	// light = ambient + diffusion + specular 
	float ambient = 0.3f;

	vec3 light_direction = normalize(light_position - vs_worldPos);	
	float diffuse = max(0.0, dot(vs_normal, light_direction) + 0.1f);

	vec3 light_directionHL = vec3(0.0f, 0.0f, 1.0f);
	float diffuseHL = 0.5f*(max(0.0, dot(vs_normal, light_directionHL) + 0.1f));

	float AD = ambient + diffuse + diffuseHL;

    color = AD * vs_color;
  }
}
