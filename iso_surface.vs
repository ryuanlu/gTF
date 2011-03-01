//
//


varying	vec4		vertex;

uniform	sampler3D	normal;
uniform	sampler3D	stencil;
uniform	vec3		resol;
uniform	int		level;

void	main(void)
{
	vertex=gl_ModelViewMatrix*gl_Vertex;

	gl_TexCoord[0].x=(gl_Vertex.x+0.5)/resol.x;
	gl_TexCoord[0].y=(gl_Vertex.y+0.5)/resol.y;
	gl_TexCoord[0].z=(gl_Vertex.z+0.5)/resol.z;

	gl_Position=ftransform();

}
