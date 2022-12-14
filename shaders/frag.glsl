// ==================================================================
#version 330 core
out vec4 color;

// Take in our previous texture coordinates from a previous stage
// in the pipeline. In this case, texture coordinates are specified
// on a per-vertex level, so these would be coming in from the vertex
// shader.
in vec2 v_texCoord;

// If we have texture coordinates,
// they are stored in a sampler.
// By convention, we often name uniforms
// with a 'u_'
uniform sampler2D u_Texture;

// Our light source data structure
struct Light {
    vec3 lightColor;
    vec3 lightPos;
    vec3 lightDir;
    float ambientIntensity;

    float specularStrength;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 myNormal;
in vec3 FragPos;
uniform int lightingEnabled;

#define NUM_LIGHTS 1
uniform Light lights[NUM_LIGHTS];

vec3 calcLighting(Light light, vec3 norm) {
    // (1) Compute ambient light
    vec3 ambient = light.ambientIntensity * light.lightColor;

    // (2) Compute diffuse light
    // From our lights position and the fragment, we can get
    // a vector indicating direction
    // Note it is always good to 'normalize' values.
    vec3 lightDir = normalize(-light.lightDir);
    // vec3 lightDir = normalize(light.lightPos - FragPos);
    float diffImpact = max(dot(norm, lightDir), 0.0);

    vec3 diffuseLight = diffImpact * light.lightColor; // TODO: normalize?

    // (3) Compute Specular lighting
    vec3 viewPos = vec3(0.0, 0.0, 0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 specular = light.specularStrength * spec * light.lightColor;

    return diffuseLight + ambient + specular;
}

void main()
{
    if (lightingEnabled == 1) {
        // Compute the normal direction
        vec3 norm = normalize(myNormal);

        // Store our final texture color
        vec3 diffuseColor = texture(u_Texture, v_texCoord).rgb;

        vec3 totalLighting = vec3(0, 0, 0);
        for (int i = 0; i < NUM_LIGHTS; i++) {
            totalLighting += calcLighting(lights[i], norm);
        }

        color = vec4(diffuseColor * totalLighting, 1.0);
    }
    else {
        color = texture(u_Texture, v_texCoord);
    }

}
// ==================================================================
