# version 330 core

in vec4 vPosition, vColor;
in vec2 vTexCoord;
in vec3 vNormal;
out vec2 vs_texCoord;
out vec3 vs_normal, vs_worldPos;
out vec4 vs_color;

uniform bool IsTexture;
uniform mat3 NormalMatrix;
uniform mat4 ModelViewProjection;
uniform mat4 ModelView;

void main(void) {
  vec4 position = ModelViewProjection * vPosition;
  gl_Position = position;
  
  // TEXTURE //
  if (IsTexture) {
    vs_worldPos = position.xyz;
    vs_normal = NormalMatrix * vNormal;
    vs_texCoord = vTexCoord;
  } else { 
	// LIGHT //
	// caculate world position
	vec4 pos = ModelView * vPosition;
	vs_worldPos = pos.xyz;
	
	// calculate normal vector to passs to fragment shader
	vs_normal = normalize(NormalMatrix * vNormal);
  } 
  // pass color of light
  vs_color = vColor; 
}
