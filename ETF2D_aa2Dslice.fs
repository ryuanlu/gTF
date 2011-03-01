varying	vec4	vertex;

uniform	sampler3D	tex;
uniform	sampler1D	map;
uniform	sampler3D	stencil;
uniform	float		coef;
uniform	float		shift;
uniform	int		sw;

void	main(void)
{
	vec4	tex_color;
	vec3	l,h,v,n,o,g;

	float	r,a,d,b,c;
	
	tex_color=texture1D(map,texture3D(tex,gl_TexCoord[0].xyz).r*coef-shift);
	//if(sw==1) tex_color.a*=texture3D(stencil,gl_TexCoord[0].xyz).r;

	gl_FragColor=tex_color;
}
