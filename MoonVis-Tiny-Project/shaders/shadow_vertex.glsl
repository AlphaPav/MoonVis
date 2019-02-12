//----------------------------------------------------------------------------------
// File:   shadow_vertex.glsl
// Author: Rouslan Dimitrov
// Email:  sdkfeedback@nvidia.com
// Copyright (c) NVIDIA Corporation. All rights reserved.
//----------------------------------------------------------------------------------

varying vec4 vPos;
varying vec3 normal;

void main()
{
	vPos = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = ftransform();
	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	gl_FrontColor = gl_Color*gl_LightSource[0].diffuse * 
					vec4(max(dot(normal, gl_LightSource[0].position.xyz), 0.0));

	gl_TexCoord[0] = gl_MultiTexCoord0;
}