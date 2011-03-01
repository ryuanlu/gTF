varying	vec4	vertex;

uniform	sampler3D	volume;
uniform	sampler1D	tf1d;
uniform	sampler3D	stencil;
uniform	sampler3D	normal;
uniform	float		coef;
uniform	float		shift;
uniform	int		sw;
uniform	vec3		delta;
uniform int		levoy_sw;
uniform	float		levoy_value;
uniform	float		levoy_thickness;


vec3	calculate_gradient(sampler3D volume,vec3 uvw)
{
	vec3	result;
	vec3	v[2];
	
	v[0].x=texture3D(volume,uvw+vec3(delta.x,0,0)).r;
	v[1].x=texture3D(volume,uvw-vec3(delta.x,0,0)).r;
	v[0].y=texture3D(volume,uvw+vec3(0,delta.y,0)).r;
	v[1].y=texture3D(volume,uvw-vec3(0,delta.y,0)).r;
	v[0].z=texture3D(volume,uvw+vec3(0,0,delta.z)).r;
	v[1].z=texture3D(volume,uvw-vec3(0,0,delta.z)).r;

	result=v[0]-v[1];
	return result;
}

void	main(void)
{
	vec4	intensity,tf_color;
	vec3	gradient;
	vec3	n,l,v,h;		//	normal, light, view, half
	float	dnv,dnl;		//	n dot v, n dot l
	float	a,d,s;			//	attenuation, distance, sign
	float	value,levoy,gm;

		

	//	1D transfer function - table lookup

	value=texture3D(volume,gl_TexCoord[0].xyz).r*65535.0;
	gm=texture3D(normal,gl_TexCoord[0].xyz).a;
	
	tf_color=texture1D(tf1d,texture3D(volume,gl_TexCoord[0].xyz).r*coef-shift);


	//	Levoy's classifier
	
	if(levoy_sw)
	{
		if(gm==0.0&&value==levoy_value) levoy=1.0;
		
		if(gm>0&&((value-levoy_thickness*gm)<=value)&&((value+levoy_thickness*gm)>=value))
			levoy=1.0-abs(levoy_value-value)/gm/levoy_thickness;
		else	levoy=0.0;
	
		tf_color.a*=levoy;
	}


	//	Gradient estimation
	
	//gradient=calculate_gradient(volume,gl_TexCoord[0].xyz);			//	On-the-fly gradient estimation
	
	gradient=texture3D(normal,gl_TexCoord[0].xyz).xyz;		//	Pre-calculated gradient estimation


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
	
	gl_FragColor=intensity*tf_color;

}
