#version 330 core
#define max_bones 120
#define real_component 0 // the real component of a dual quaternion encodes rotation information
#define dual_component 1 // the dual component encodes the position information
// vertex shader for skinning using dual quaternions instead of 4x4 matrices
// dual quaternions do not contain scale information, so an animation that uses scaling will render poorly with this vertex shader
uniform mat4 model; // model space to world space transform
uniform mat4 view; // world space to view space (the camera transform)
uniform mat4 projection; // convert from world3D to screen space (NDC)

// every bone is now a dual quaternion
uniform mat2x4 posedBones[max_bones];
uniform mat2x4 inverseBindPose[max_bones];

// per vertex data attributes, in local space
in vec3 position;
in vec3 normal;
in vec2 textureCoordinate;
in vec4 boneWeights; // 'invalid' bones will have zero weight, eliminating their influence in the skinning calculation
in ivec4 boneIndices;

// data needed by the fragment shader
out vec3 world_normal;
out vec3 fragment_world_position;
out vec2 uv;

/**
 * multiply two quaternions together.
 * Taken from quaternion.cpp
 */
vec4 qMul(vec4 qLeft, vec4 qRight) {
  return vec4(
	  (qRight.x * qLeft.w) + (qRight.y * qLeft.z) - (qRight.z * qLeft.y) + (qRight.w * qLeft.x),
	  -(qRight.x * qLeft.z) + (qRight.y * qLeft.w) + (qRight.z * qLeft.x) + (qRight.w * qLeft.y),
	  (qRight.x * qLeft.y) - (qRight.y * qLeft.x) + (qRight.z * qLeft.w) + (qRight.w * qLeft.z),
	  -(qRight.x * qLeft.x) - (qRight.y * qLeft.y) - (qRight.z * qLeft.z) + (qRight.w * qLeft.w)
	);
}

/**
 * normalize a dual quaternion
 */
mat2x4 normalizeDualQ(mat2x4 dualQ) {
  float invMag = 1.0 / length(dualQ[real_component]);
  dualQ[real_component] *= invMag;
  dualQ[dual_component] *= invMag;
  return dualQ;
}

/**
 * combine two dual quaternions together. 
 * used in the same way martix transforms are combined by multiplying them together.
 * taken from DualQUaternion.cpp *operator
 */
mat2x4 combineDualQ(mat2x4 left, mat2x4 right) {
  left = normalizeDualQ(left);
  right = normalizeDualQ(right);
  vec4 real = qMul(left[real_component], right[real_component]);
  vec4 dual = qMul(left[real_component], right[dual_component]) + qMul(left[dual_component], right[real_component]);
  return mat2x4(real, dual);
}

/**
 * Apply a dual quaternion transform to a vector.
 * Taken from quaternion.cpp *operator
 */
vec4 applyVector(mat2x4 dualQ, vec3 vector) {
  vec4 real = dualQ[real_component]; // rotation data
  vec3 realVector = real.xyz;
  float realScalar = real.w;

  // dot product of a vector with itself is just the magnitude squared
  vec3 v = realVector * 2.0f * dot(realVector, vector) + 
  vector * (realScalar * realScalar - dot(realVector, realVector)) +
  cross(realVector, vector) * 2.0f * realScalar;
  return vec4(v, 0.0);
}

/**
 * Apply a dual quaternion transform to a point.
 * Taken from DualQuaternion.cpp applyPoint
 */
vec4 applyPoint(mat2x4 dualQ, vec3 point) {
  vec4 real = dualQ[real_component];
  vec4 dual = dualQ[dual_component];
  vec4 conjugate = vec4(-real.xyz, real.w);
  vec3 translation = qMul(conjugate, dual * 2.0).xyz;
  vec3 v = applyVector(dualQ, point).xyz;
  return vec4(v + translation, 1);
}

void main() {
	vec4 weights = boneWeights;
	// neighborhood the quaternions, see ../rotation/quaternion.h for an explaination of what neighborhooding is
	if (dot(posedBones[boneIndices.x][real_component], posedBones[boneIndices.y][real_component]) < 0.0) {
		weights.y *= -1.0; 
	}
	if (dot(posedBones[boneIndices.x][real_component], posedBones[boneIndices.z][real_component]) < 0.0) {
		weights.z *= -1.0; 
	}
	if (dot(posedBones[boneIndices.x][real_component], posedBones[boneIndices.w][real_component]) < 0.0) {
		weights.w *= -1.0; 
	}
	// combine animated bone with inverse bind bone, for all bones that affect this vertex
	// note that dual quaternion multiplication is left to right, instead of right to left matrix multiplications
	mat2x4 dualQ0 = combineDualQ(inverseBindPose[boneIndices.x], posedBones[boneIndices.x]);
	mat2x4 dualQ1 = combineDualQ(inverseBindPose[boneIndices.y], posedBones[boneIndices.y]);
	mat2x4 dualQ2 = combineDualQ(inverseBindPose[boneIndices.z], posedBones[boneIndices.z]);
	mat2x4 dualQ3 = combineDualQ(inverseBindPose[boneIndices.w], posedBones[boneIndices.w]);
	// create the skinning dual quaternion
	mat2x4 skinningDualQ = (weights.x * dualQ0) + (weights.y * dualQ1) + (weights.z * dualQ2) + (weights.w * dualQ3);
	skinningDualQ = normalizeDualQ(skinningDualQ);
	// skin the vertex and normal
	vec4 vertex = applyPoint(skinningDualQ, position);
	vec4 n = applyVector(skinningDualQ, normal);
	world_normal = vec3(model * n);
	fragment_world_position = vec3(model * vertex);
	uv = textureCoordinate;
	// convert the vertex to NDC
	gl_Position = projection * view * model * vertex;
}
