#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vTexCoord;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 texCoord;

layout(set = 0, binding = 0) uniform CameraBuffer {   
    mat4 view;
    mat4 proj;
	mat4 viewproj; 
} cameraData;

struct ObjectData {
	mat4 model;
}; 

//all object matrices
layout(std140, set = 1, binding = 0) readonly buffer ObjectBuffer{ 
	ObjectData objects[];
} objectBuffer;

//push constants block
layout( push_constant ) uniform constants {
	vec4 data;
	mat4 render_matrix;
} PushConstants;

void main() {
	mat4 modelMatrix = objectBuffer.objects[gl_BaseInstance].model;
	mat4 transformMatrix = (cameraData.viewproj * modelMatrix);
	float sec = PushConstants.data.x;
	// gl_Position = transformMatrix * vec4(vPosition, 1.0f);
	outColor = vColor;
	vec2 texOut = vec2((vPosition.x - floor(vPosition.x)), (vPosition.y - floor(vPosition.y)));
	vec3 outPos = vPosition;
	// for(float i = 1.0; i < 8.0; i++) {
    	// texOut.x += i * .1 / i * sin(texOut.x * i * i + sec * 0.5) * sin(texOut.y * i * i + sec * 0.5);
    	// outPos.z += i * .2 / i * sin(outPos.y * i * i + sec * 0.5) * cos(outPos.x * i * i + sec * 0.5);
        // outPos.z += sin(sec);
  	// }
    float k = 2. * 3.1415 / 50.f;
    float a = 3.f;
    outPos.z = a * sin(k * (outPos.x - 10.f * sec));

	// outColor = (vNormal + vec3(1.0f, 1.0f, 1.0f)) / 2.0f;
	texCoord = texOut;
	gl_Position = transformMatrix * vec4(outPos, 1.f);
}
