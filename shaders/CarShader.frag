#version 450#extension GL_ARB_separate_shader_objects : enable#define MAX_LIGHTS 4struct PointLight {  vec3 position;  vec4 color;};layout(location = 0) in vec3 fragPos;layout(location = 1) in vec3 fragNorm;layout(location = 2) in vec2 fragUV;layout(location = 0) out vec4 outColor;layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {	vec3 DlightPos;		// position of the point light	vec3 DlightDir;		// direction of the direct light	vec3 DlightColor;	// color of the direct light	vec3 AmbLightColor;	// ambient light	vec3 eyePos;		// position of the viewer	PointLight pointLights[MAX_LIGHTS];} gubo;layout(set = 1, binding = 0) uniform UniformBufferObject {	float amb;	float gamma;	vec3 sColor;	mat4 mvpMat;	mat4 mMat;	mat4 nMat;} ubo;layout(set = 1, binding = 1) uniform sampler2D tex;const float beta = 0.0f;const float g = 1.5;const float cosout = 0.85;const float cosin  = 0.95;vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms, float gamma) {	//vec3 V  - direction of the viewer	//vec3 N  - normal vector to the surface	//vec3 L  - light vector (from the light model)	//vec3 Md - main color of the surface	//vec3 Ms - specular color of the surface	//float gamma - Exponent for power specular term		vec3 Diffuse = Md * max(dot(L,N), 0.0f);	vec3 halfVector = normalize(L+V);	vec3 Specular = Ms * pow(clamp(dot(N, halfVector), 0.0f, 1.0f), gamma);	vec3 BRDF = Diffuse + Specular;	return BRDF;}void main() {	vec3 N = normalize(fragNorm);				// surface normal	vec3 V = normalize(gubo.eyePos - fragPos);	// viewer direction	vec3 L = normalize(gubo.DlightDir);			// light direction	vec3 albedo = texture(tex, fragUV).rgb;		// main color	vec3 lightColor = gubo.DlightColor.rgb * pow(g/length(gubo.DlightPos - fragPos), beta) *	clamp((dot(normalize(gubo.DlightPos - fragPos), gubo.DlightDir) - cosout)/(cosin-cosout), 0.0f, 1.0f);	vec3 DiffSpec = BRDF(V, N, L, albedo, ubo.sColor, ubo.gamma);		for (int i = 0; i < MAX_LIGHTS; i++) {		PointLight light = gubo.pointLights[i];		vec3 L = light.position.xyz - fragPos;		float attenuation = 1.0 / dot(L, L); // distance squared		L = normalize(L);		float cosAngIncidence = max(dot(N, L), 0.0f);		vec3 intensity = light.color.xyz * light.color.w * attenuation; //cambia l'attenuation per aumentare la luce diffusa		DiffSpec += intensity * BRDF(V, N, L, albedo, ubo.sColor, ubo.gamma);  	}		vec3 Ambient = albedo * gubo.AmbLightColor * ubo.amb;		outColor = vec4(clamp(0.95 * DiffSpec * lightColor.rgb + Ambient,0.0,1.0), 1.0f);	}