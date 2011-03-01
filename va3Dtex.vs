varying	vec4	vertex;

uniform	vec3	delta;
uniform	int	frontindex;
uniform	int	sequence[64];
uniform	vec3	vertices[8];
uniform	int	v1[24];
uniform	int	v2[24];



void	main(void)
{
	int	e,i,j;
	vec3	v[2],t[2];
	vec3	view=vec3(0.0,0.0,-1.0);
	vec3	edge;
	float	denom;
	float	lambda;
	
	vec4	output;
	vec4	tex;
	float	distance;
	
	distance=gl_Vertex.y;
	
	for(e=0;e<4;++e)
	{
		i=sequence[frontindex*8+v1[int(gl_Vertex.x)*4+e]];
		j=sequence[frontindex*8+v2[int(gl_Vertex.x)*4+e]];
		
		v[0]=(gl_ModelViewMatrix*vec4(vertices[i].xyz,1.0)).xyz;
		v[1]=(gl_ModelViewMatrix*vec4(vertices[j].xyz,1.0)).xyz;

		t[0]=vertices[i];
		t[1]=vertices[j];
		
		edge=v[1]-v[0];
		denom=dot(view,edge);

		
		lambda=(denom!=0.0) ? (-1.0*distance-dot(view,v[0]))/denom : -1.0;
		
		if(lambda>=0.0&&lambda<=1.0)
		{
			output=vec4(v[0]+lambda*edge,1.0);
			tex=vec4(t[0]+lambda*(t[1]-t[0]),0.0);
			break;
		}
		
	}

	gl_Position=gl_ProjectionMatrix*output;
	gl_TexCoord[0].x=(tex.x)*delta.x;
	gl_TexCoord[0].y=(tex.y)*delta.y;
	gl_TexCoord[0].z=(tex.z)*delta.z;
	vertex=output;
}
