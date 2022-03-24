
varying vec3 position;
varying vec3 normal;
varying vec3 color;

uniform int isShadow;

void main()
{
	if (isShadow == 1) {
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {

		gl_FragColor = vec4(color, 1.0);
		
	}
}
