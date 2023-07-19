#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
    vec3 DlightPos;
    vec3 DlightDir;		// direction of the direct light
    vec3 DlightColor;	// color of the direct light
    vec3 AmbLightColor;	// ambient light
    vec3 eyePos;		// position of the viewer
} gubo;

layout(set = 1, binding = 0) uniform UniformBufferObject {
    float amb;
    float gamma;
    vec3 sColor;
    mat4 mvpMat;
    mat4 mMat;
    mat4 nMat;
} ubo;

layout(set = 1, binding = 1) uniform sampler2D tex;
layout(set = 1, binding = 2) uniform sampler2D normMap; //DA LEVARE
layout(set = 1, binding = 3) uniform sampler2D matMap;
//layout(set = 1, binding = 4) uniform sampler2D emissionTex;

const float beta = 0.0f;
const float g = 1.5;
const float cosout = 0.85;
const float cosin  = 0.95;

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms, float gamma) {
    //vec3 V  - direction of the viewer
    //vec3 N  - normal vector to the surface
    //vec3 L  - light vector (from the light model)
    //vec3 Md - main color of the surface
    //vec3 Ms - specular color of the surface
    //float gamma - Exponent for power specular term
    vec3 Diffuse = Md * max(dot(L,N), 0.0f);
    vec3 halfVector = normalize(L+V);
    vec3 Specular = Ms * pow(clamp(dot(N, halfVector),0.0f,1.0f), gamma);
    vec3 BRDF = Diffuse + Specular;
    return BRDF;
}

void main() {
    //vec3 N = normalize(fragNorm);				// surface normal
    vec3 N = normalize(texture(normMap, fragUV).xyz);				// surface normal
    vec3 V = normalize(gubo.eyePos - fragPos);	// viewer direction
    vec3 L = normalize(gubo.DlightDir);			// light direction

    vec3 albedo = texture(tex, fragUV).rgb;		// main color

    vec4 MRAO = texture(matMap, fragUV);
    float roughness = MRAO.g;
    float pex = 1000.f * (1.0 - roughness) * (1.0 - roughness);
    float ao = MRAO.b;
    float metallic = MRAO.r;


    //vec3 lightColor = gubo.DlightColor.rgb;
    vec3 lightColor = gubo.DlightColor.rgb * pow(g/length(gubo.DlightPos - fragPos), beta) *
    clamp((dot(normalize(gubo.DlightPos - fragPos), gubo.DlightDir) - cosout)/(cosin-cosout), 0.0f, 1.0f);

    vec3 DiffSpec = BRDF(V, N, L, albedo, 0.9f * vec3(metallic), pex);
    vec3 Ambient = albedo * gubo.AmbLightColor * ao;

/**vec3 emissionFactor = vec3(1.0f,1.0f,1.0f);
	vec3 Emission = emissionFactor *  texture(emissionTex,fragUV).rgb;*/

    outColor = vec4(clamp(0.95 * DiffSpec * lightColor.rgb + Ambient,0.0,1.0), 1.0f);

}