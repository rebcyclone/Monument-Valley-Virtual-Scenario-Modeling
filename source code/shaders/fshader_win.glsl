#version 330 core

// 给光源数据一个结构体
struct Light{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	vec3 position;

    // 光源衰减系数的三个参数
    float constant; // 常数项
    float linear;	// 一次项
    float quadratic;// 二次项

};

// 给物体材质数据一个结构体
struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float shininess;
};

in vec3 position;
in vec3 normal;
in vec3 color;
// in vec4 color;
in vec2 texCoord;


// 相机坐标
uniform vec3 eye_position;
// 光源
uniform Light light;
// 物体材质
uniform Material material;

uniform int isShadow;
// 纹理数据
uniform sampler2D texture;

out vec4 fColor;

void main()
{
	if (isShadow == 1) {
		fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {
		//计算四个归一化的向量 N,V,L,R(或半角向量H)
		vec3 N= normalize(normal);
    	vec3 V= normalize(eye_position - position);
    	vec3 L= normalize(light.position - position);
    	vec3 R= reflect(-L,N);

		// 环境光分量I_a
		vec4 I_a = light.ambient * material.ambient;

		//计算系数和漫反射分量I_d
		float alpha = max(dot(L,N),0);
    	vec4 I_d = alpha *  light.diffuse * material.diffuse;

		//计算系数和镜面反射分量I_s
		float beta = pow(max(dot(R,V),0),material.shininess);
    	vec4 I_s = beta * light.specular * material.specular;

		//计算高光系数beta和镜面反射分量I_s
		// 注意如果光源在背面则去除高光
		if( dot(L, N) < 0.0 ) {
		    I_s = vec4(0.0, 0.0, 0.0, 1.0);
		} 

		// 合并三个分量的颜色，修正透明度
		vec4 mcolor = I_a + I_d + I_s;
		mcolor.a = 1.0;
		// fColor = I_a + I_d + I_s;
		// fColor.a = 1.0;

		fColor = texture2D( texture, texCoord )*0.7 + mcolor*0.3;
		// fColor = texture2D( texture, texCoord );
		// fColor = vec4(color, 1.0);
		// fColor = vec4(normal, 1.0);
		// fColor = vec4(normalize(position) , 1.0);
		// fColor = vec4(1.0/position.z, position.z, position.z , 1.0);
	}
}
