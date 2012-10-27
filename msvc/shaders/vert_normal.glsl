varying vec3 normal;



void main(void)
{
    	

 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;


 normal = gl_NormalMatrix * gl_Normal;    

}
