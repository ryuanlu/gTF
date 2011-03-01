//
//	Marching Tetrahedron - High compatibility version
//


varying	vec4		vertex;

uniform	sampler3D	normal;


void	main(void)
{
	vec4	intensity;
	vec3	gradient;
	vec3	n,l,v,h;		//	normal, light, view, half
	float	dnv,dnl;		//	n dot v, n dot l
	float	a,d,s;			//	attenuation, distance, sign
	vec3	loc;
		

	//	Gradient estimation
	
	gradient=texture3D(normal,gl_TexCoord[0].xyz).xyz;

	
	//	Phong shading

	n=normalize(gl_NormalMatrix*gradient);
	l=normalize(gl_LightSource[0].position.xyz-vertex.xyz);
	v=normalize(-vertex.xyz);
	h=normalize(l+v);

	dnv=dot(n,v);
	dnl=dot(n,l);
	
	intensity=gl_FrontLightProduct[0].ambient;

	if(dnv*dnl > 0.0)
	{
		s=sign(dnv);
		d=length(gl_LightSource[0].position-vertex);
		a=1.0/(gl_LightSource[0].quadraticAttenuation*d*d+gl_LightSource[0].linearAttenuation*d+gl_LightSource[0].constantAttenuation);

		intensity+=a*(gl_FrontLightProduct[0].diffuse*max(s*dnl,0.0)+gl_FrontLightProduct[0].specular*pow(max(s*dot(n,h),0.0),gl_FrontMaterial.shininess));
	}
	
	gl_FragColor=intensity;


}
