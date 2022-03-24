#include "Angel.h"
#include "mat.h"
#include "vec.h"
#include "TriMesh.h"
#include "Camera.h"

#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct openGLObject
{
	// 顶点数组对象
	GLuint vao;
	// 顶点缓存对象
	GLuint vbo;

	// 着色器程序
	GLuint program;
	// 着色器文件
	std::string vshader;
	std::string fshader;
	// 着色器变量
	GLuint pLocation;
	GLuint cLocation;
	GLuint nLocation;
	GLuint tLocation;

    // 纹理
    std::string texture_image;
    GLuint texture;

	// 投影变换变量
	GLuint modelLocation;
	GLuint viewLocation;
	GLuint projectionLocation;

	// 阴影变量
	GLuint shadowLocation;
};

int WIDTH = 600;
int HEIGHT = 600;

int mainWindow;

class MatrixStack {
	int		_index;
    int		_size;
    mat4*	_matrices;

public:
	MatrixStack(int numMatrices = 100):_index(0), _size(numMatrices)
        { _matrices = new mat4[numMatrices]; }

    ~MatrixStack()
		{ delete[]_matrices; }

    void push(const mat4& m){
		assert( _index + 1 < _size );
		_matrices[_index++] = m;	 
    }

    mat4& pop(){
        assert(_index - 1 >= 0);
        _index--;
        return _matrices[_index];
    }
};


#define White	vec3(1.0, 1.0, 1.0)
#define Yellow	vec3(1.0, 1.0, 0.0)
#define Green	vec3(0.0, 1.0, 0.0)
#define Cyan	vec3(0.0, 1.0, 1.0)
#define Magenta	vec3(1.0, 0.0, 1.0)
#define Red		vec3(1.0, 0.0, 0.0)
#define Black	vec3(0.0, 0.0, 0.0)
#define Blue	vec3(0.0, 0.0, 1.0)
#define Brown	vec3(0.5, 0.5, 0.5)

struct Robot
{
	// 关节大小
	float TORSO_HEIGHT = 5.5;  //身体长度
	float TORSO_WIDTH = 4.0;  //裙摆直径
	float UPPER_ARM_HEIGHT = 2.0;//大臂长度
	float LOWER_ARM_HEIGHT = 2.0;//小臂长度
	float UPPER_ARM_WIDTH =  0.5;//大臂半径
	float LOWER_ARM_WIDTH =  0.5;//小臂半径
	float UPPER_LEG_HEIGHT = 2.7;//大腿长度
	float LOWER_LEG_HEIGHT = 1.5;//小腿长度
	float UPPER_LEG_WIDTH =  0.5;//大腿直径
	float LOWER_LEG_WIDTH =  0.5;//小腿直径 脚长1.1
	float HEAD_HEIGHT = 3.5;//头直径（竖直） /水平 3.0
	float HEAD_WIDTH = 6.6;//帽子长度

	// 关节角和菜单选项值
	enum {
		Torso,			// 躯干
		Head,			// 头部
		RightUpperArm,	// 右大臂
		RightLowerArm,	// 右小臂
		LeftUpperArm,	// 左大臂
		LeftLowerArm,	// 左小臂
		RightUpperLeg,	// 右大腿
		RightLowerLeg,	// 右小腿
		LeftUpperLeg,	// 左大腿
		LeftLowerLeg,	// 左小腿
	};

	// 关节角大小
	GLfloat theta[10] = {
		0.0,    // Torso
		0.0,    // Head
		0.0,    // RightUpperArm
		0.0,    // RightLowerArm
		0.0,    // LeftUpperArm
		0.0,    // LeftLowerArm
		0.0,    // RightUpperLeg
		0.0,    // RightLowerLeg
		0.0,    // LeftUpperLeg
		0.0     // LeftLowerLeg
	};
	int frame=0;
	//控制走路摆动速度
	int changespeed=1000;
	//控制人物大小
	float ida_size = 0.3;
};
Robot robot;
// 被选中的物体
int Selected_mesh = robot.Torso;


TriMesh* Torso = new TriMesh();
TriMesh* Head = new TriMesh();
TriMesh* RightUpperArm = new TriMesh();
TriMesh* RightLowerArm = new TriMesh();
TriMesh* LeftUpperArm = new TriMesh();
TriMesh* LeftLowerArm = new TriMesh();
TriMesh* RightUpperLeg = new TriMesh();
TriMesh* RightLowerLeg = new TriMesh();
TriMesh* LeftUpperLeg = new TriMesh();
TriMesh* LeftLowerLeg = new TriMesh();

openGLObject TorsoObject;
openGLObject HeadObject;
openGLObject RightUpperArmObject;
openGLObject RightLowerArmObject;
openGLObject LeftUpperArmObject;
openGLObject LeftLowerArmObject;
openGLObject RightUpperLegObject;
openGLObject RightLowerLegObject;
openGLObject LeftUpperLegObject;
openGLObject LeftLowerLegObject;

struct Crow
{
	// 关节大小
	float BODY_HEIGHT = 1.8;  //身体长度
	float BODY_WIDTH = 1.3;  //身体宽度
	float LEG_HEIGHT = 1.0;//腿长度 1.0
	float LEG_WIDTH =  0.3;//腿直径 脚长0.8
	float HEAD_HEIGHT = 1.2;//头直径（竖直） /水平 1.1
	float HEAD_WIDTH = 2.7;//头长度

	// 关节角和菜单选项值
	enum {
		Body,			// 躯干
		Head,			// 头部
		RightLeg,	// 右大臂
		LeftLeg,	// 左大臂
	};

	// 关节角大小
	GLfloat theta[10] = {
		0.0,    // Torso
		0.0,    // Head
		0.0,    // RightLeg
		0.0,    // LeftLeg
	};
	int crowframe=0;
	//控制走路摆动速度
	int changespeed=1000;
	//控制人物大小
	float crow_size = 0.8;
};
Crow crow;
// 乌鸦被选中的物体
int Selected_mesh1 = crow.Body;

TriMesh* Body = new TriMesh();
TriMesh* CrowHead = new TriMesh();
TriMesh* LeftLeg = new TriMesh();
TriMesh* RightLeg = new TriMesh();

openGLObject BodyObject;
openGLObject CrowHeadObject;
openGLObject LeftLegObject;
openGLObject RightLegObject;

//城堡
struct MONU
{
	// 关节大小
	float HOUSE_HEIGHT = 12.1;  //城堡高度
	float HOUSE_LENGTH = 9.3;  //长9.3
	float HOUSE_WIDTH = 7.0;//宽7.0
	float STAIR_LENGTH = 2.7;//宽度
	float STAIR_WIDTH = 1.2;//厚度
	// 关节角和菜单选项值
	enum {
		house,			//城堡
		stair,			//楼梯
	};

	// 关节角大小
	GLfloat theta[10] = {
		0.0,    // Torso
		0.0,    // Head
	};
	//控制人物大小
	float monu_size = 1.0;
};
MONU monument;

TriMesh* house = new TriMesh();
TriMesh* stair = new TriMesh();

openGLObject houseObject;
openGLObject stairObject;

//乌鸦
// TriMesh *crow = new TriMesh();
// openGLObject crow_object;

//地面
TriMesh *plane = new TriMesh();
openGLObject plane_object;
//城堡(绿)
TriMesh* monu = new TriMesh();
openGLObject monuObject;
//城堡旋转部分
TriMesh *part = new TriMesh();
openGLObject partObject;

//城堡2
TriMesh* monu2 = new TriMesh();
openGLObject monu2Object;

//三角
TriMesh* penrose = new TriMesh();
openGLObject penroseObject;
//图腾
TriMesh* totem = new TriMesh();
openGLObject totemObject;

//皇冠
TriMesh* crown = new TriMesh();
openGLObject crownObject;
//镂空三角
TriMesh* emptri = new TriMesh();
openGLObject emptriObject;
//镂空正方体
TriMesh* cube = new TriMesh();
openGLObject cubeObject;

//树木
TriMesh* tree = new TriMesh();
openGLObject treeObject;

//荷花池
TriMesh* pool = new TriMesh();
openGLObject poolObject;

//初始化相机位移矩阵
vec4 movescale = vec4(0.0, 0.0, 0.0, 0.0);
Camera* camera = new Camera(movescale);
Light* light = new Light();

// vec3 light_position = vec3(10.0,10.0,10.0);

void load_texture_STBImage(const std::string &file_name, GLuint& texture){
    // 读取纹理图片，并将其传递给着色器

	int width, height, channels = 0;
    unsigned char *pixels = NULL;
    // 读取图片的时候先翻转一下图片，如果不设置的话显示出来是反过来的图片
    stbi_set_flip_vertically_on_load(true);
    // 读取图片数据
    pixels = stbi_load(file_name.c_str(), &width, &height, &channels, 0);

    // 调整行对齐格式
    if (width * channels % 4 != 0)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLenum format = GL_RGB;
    // 设置通道格式
    switch (channels)
    {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        format = GL_RGB;
        break;
    }

    // 绑定纹理对象
    glBindTexture(GL_TEXTURE_2D, texture);

    // 指定纹理的放大，缩小滤波，使用线性方式，即当图片放大的时候插值方式
    // 将图片的rgb数据上传给opengl
    glTexImage2D(
        GL_TEXTURE_2D,    // 指定目标纹理，这个值必须是GL_TEXTURE_2D
        0,                // 执行细节级别，0是最基本的图像级别，n表示第N级贴图细化级别
        format,           // 纹理数据的颜色格式(GPU显存)
        width,            // 宽度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
        height,           // 高度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
        0,                // 指定边框的宽度。必须为0
        format,           // 像素数据的颜色格式(CPU内存)
        GL_UNSIGNED_BYTE, // 指定像素数据的数据类型
        pixels            // 指定内存中指向图像数据的指针
    );

    // 生成多级渐远纹理，多消耗1/3的显存，较小分辨率时获得更好的效果
    // glGenerateMipmap(GL_TEXTURE_2D);

    // 指定插值方法
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // 恢复初始对齐格式
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    // 释放图形内存
    stbi_image_free(pixels);
};


// 获取生成的所有模型，用于结束程序时释放内存
std::vector<TriMesh*> meshList;


void bindObjectAndData(TriMesh *mesh, openGLObject &object, const std::string &vshader, const std::string &fshader, const std::string &texture_image){
    // 初始化各种对象

    std::vector<vec3> points = mesh->getPoints();
    std::vector<vec3> normals = mesh->getNormals();
    std::vector<vec3> colors = mesh->getColors();
    std::vector<vec2> textures = mesh->getTextures();

	// 创建顶点数组对象
#ifdef __APPLE__	// for MacOS
	glGenVertexArraysAPPLE(1, &object.vao);		// 分配1个顶点数组对象
	glBindVertexArrayAPPLE(object.vao);		// 绑定顶点数组对象
#else				// for Windows
	glGenVertexArrays(1, &object.vao);  	// 分配1个顶点数组对象
	glBindVertexArray(object.vao);  	// 绑定顶点数组对象
#endif

	// 创建并初始化顶点缓存对象
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 points.size() * sizeof(vec3) +
                     normals.size() * sizeof(vec3) +
                     colors.size() * sizeof(vec3) +
                     textures.size() * sizeof(vec2),
                 NULL, GL_STATIC_DRAW);

    // 绑定顶点数据
    glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec3), points.data());
    // 绑定颜色数据
    glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), colors.size() * sizeof(vec3), colors.data());
    // 绑定法向量数据
    glBufferSubData(GL_ARRAY_BUFFER, (points.size() + colors.size()) * sizeof(vec3), normals.size() * sizeof(vec3), normals.data());
    // 绑定纹理数据
    glBufferSubData(GL_ARRAY_BUFFER, (points.size() + normals.size() + colors.size()) * sizeof(vec3), textures.size() * sizeof(vec2), textures.data());

	object.vshader = vshader;
	object.fshader = fshader;
	object.program = InitShader(object.vshader.c_str(), object.fshader.c_str());

    // 将顶点传入着色器
	object.pLocation = glGetAttribLocation(object.program, "vPosition");
	glEnableVertexAttribArray(object.pLocation);
	glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // 将颜色传入着色器
	object.cLocation = glGetAttribLocation(object.program, "vColor");
	glEnableVertexAttribArray(object.cLocation);
	glVertexAttribPointer(object.cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec3)));

    // 将法向量传入着色器
	object.nLocation = glGetAttribLocation(object.program, "vNormal");
	glEnableVertexAttribArray(object.nLocation);
	glVertexAttribPointer(object.nLocation, 3, 
		GL_FLOAT, GL_FALSE, 0, 
		BUFFER_OFFSET( (points.size() + colors.size())  * sizeof(vec3)));

	//将将纹理坐标传入着色器
	object.tLocation = glGetAttribLocation(object.program, "vTexture");
	glEnableVertexAttribArray(object.tLocation);
	glVertexAttribPointer(object.tLocation, 2, 
		GL_FLOAT, GL_FALSE, 0, 
		BUFFER_OFFSET( ( points.size() + colors.size() + normals.size())  * sizeof(vec3)));


	// 获得矩阵位置
	object.modelLocation = glGetUniformLocation(object.program, "model");
	object.viewLocation = glGetUniformLocation(object.program, "view");
	object.projectionLocation = glGetUniformLocation(object.program, "projection");

	object.shadowLocation = glGetUniformLocation(object.program, "isShadow");

    // 读取纹理图片数
    object.texture_image = texture_image;
    // 创建纹理的缓存对象
    glGenTextures(1, &object.texture);
    // 调用stb_image生成纹理
    load_texture_STBImage(object.texture_image, object.texture);
    
    // Clean up
    glUseProgram(0);
#ifdef __APPLE__
    glBindVertexArrayAPPLE(0);
#else
    glBindVertexArray(0);
#endif

};

void bindLightAndMaterial(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera) {

	// 传递相机的位置
	glUniform3fv( glGetUniformLocation(object.program, "eye_position"), 1, vec3(camera->eye[0], camera->eye[1], camera->eye[2]) );

	// 传递物体的材质
	vec4 meshAmbient = mesh->getAmbient();
	vec4 meshDiffuse = mesh->getDiffuse();
	vec4 meshSpecular = mesh->getSpecular();
	float meshShininess = mesh->getShininess();
	glUniform4fv(glGetUniformLocation(object.program, "material.ambient"), 1, meshAmbient);
	glUniform4fv(glGetUniformLocation(object.program, "material.diffuse"), 1, meshDiffuse);
	glUniform4fv(glGetUniformLocation(object.program, "material.specular"), 1, meshSpecular);
	glUniform1f(glGetUniformLocation(object.program, "material.shininess"), meshShininess);

	// 传递光源信息
	vec4 lightAmbient = light->getAmbient();
	vec4 lightDiffuse = light->getDiffuse();
	vec4 lightSpecular = light->getSpecular();
	vec3 lightPosition = light->getTranslation();

	glUniform4fv(glGetUniformLocation(object.program, "light.ambient"), 1, lightAmbient);
	glUniform4fv(glGetUniformLocation(object.program, "light.diffuse"), 1, lightDiffuse);
	glUniform4fv(glGetUniformLocation(object.program, "light.specular"), 1, lightSpecular);
	glUniform3fv(glGetUniformLocation(object.program, "light.position"), 1, lightPosition);

	glUniform1f(glGetUniformLocation(object.program, "light.constant"), light->getConstant() );
	glUniform1f(glGetUniformLocation(object.program, "light.linear"), light->getLinear() );
	glUniform1f(glGetUniformLocation(object.program, "light.quadratic"), light->getQuadratic() );

}

void drawMesh(mat4 modelMatrix,TriMesh* mesh, openGLObject &object, Light *light, Camera* camera){
    
    // 相机矩阵计算////////////////
	// camera->updateCamera();
	// camera->viewMatrix = camera->getViewMatrix();
	// camera->projMatrix = camera->getProjectionMatrix(false);

#ifdef __APPLE__	// for MacOS
	glBindVertexArrayAPPLE(object.vao);
#else
	glBindVertexArray(object.vao);
#endif
	glUseProgram(object.program);

	// 物体的变换矩阵
	// mat4 modelMatrix = mesh->getModelMatrix();
	
	// 设置材质
	mesh->setAmbient(vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	mesh->setDiffuse(vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	mesh->setSpecular(vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	mesh->setShininess(1.0); //高光系数

	// 传递矩阵
	glUniformMatrix4fv(object.modelLocation, 1, GL_TRUE, &modelMatrix[0][0]);
	glUniformMatrix4fv(object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
	glUniformMatrix4fv(object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
	// 将着色器 isShadow 设置为0，表示正常绘制的颜色，如果是1着表示阴影
	glUniform1i(object.shadowLocation, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, object.texture);// 该语句必须，否则将只使用同一个纹理进行绘制

    
	// 将材质和光源数据传递给着色器
	bindLightAndMaterial(mesh, object, light, camera);
	// 绘制
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

	// 阴影绘制
	if(mesh->getDrawShadow())
	{
		//根据光源位置，计算阴影投影矩阵
		// mat4 shadowProjecttoinMatrix = {
		// 	vec4(-light->getTranslation().y,light->getTranslation().x,0,0),
		// 	vec4(0,0,0,0),
		// 	vec4(0,light->getTranslation().z,-light->getTranslation().y,0),
		// 	vec4(0,1,0,-light->getTranslation().y)
		// };
		mat4 shadowProjecttoinMatrix = light->getShadowProjectionMatrix();
		// 计算阴影的模型变换矩阵
		mat4 shadowmodelMatrix = shadowProjecttoinMatrix*modelMatrix;
		// 传递 isShadow 变量
		glUniform1i(object.shadowLocation, 1);
		// 传递 unifrom关键字的矩阵数据
		glUniformMatrix4fv(object.modelLocation, 1, GL_TRUE, &shadowmodelMatrix[0][0]);
		glUniformMatrix4fv(object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
		glUniformMatrix4fv(object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
		// 绘制
	 	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());
	}

	#ifdef __APPLE__	// for MacOS
		glBindVertexArrayAPPLE(0);
	#else
		glBindVertexArray(0);
	#endif
		glUseProgram(0);
};

//该部分变换用于把旋转点挪到物体局部坐标系原点,并拜摆放好位置（要组装的角度）/按 平移 * 旋转 * 缩放 矩阵顺序
// 躯体
void torso(mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	mat4 translate = Translate(vec3(0.0, 0.5 * robot.TORSO_HEIGHT, 0.0)*robot.ida_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
    mat4 instance = translate * scale;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, Torso, TorsoObject,light,camera);
}

// 头部
void head(mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	mat4 translate = Translate(vec3(0.0, 0.5 * robot.HEAD_HEIGHT, 0.0)*robot.ida_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
    mat4 instance = translate * scale;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, Head, HeadObject,light,camera);
}


// 左大臂
void left_upper_arm(mat4 modelMatrix)
{
    // 本节点局部变换矩阵
    mat4 translate = Translate(vec3(0.0, -0.5 * robot.UPPER_ARM_HEIGHT, 0.0)*robot.ida_size);
	mat4 rotate = RotateZ(45);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
	mat4 instance = translate * rotate * scale;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh( modelMatrix * instance, LeftUpperArm, LeftUpperArmObject,light,camera);
}


// 左小臂
void left_lower_arm(mat4 modelMatrix)
{
    mat4 translate = Translate(vec3(0.0, -0.5*robot.LOWER_ARM_HEIGHT, 0.0)*robot.ida_size);
	mat4 rotate = RotateZ(45);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
	mat4 instance = translate * rotate * scale;
	drawMesh(modelMatrix*instance, LeftLowerArm,LeftLowerArmObject,light,camera);
}

// 右大臂
void right_upper_arm(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(0.0, -0.5 * robot.UPPER_ARM_HEIGHT, 0.0)*robot.ida_size);
	mat4 rotate = RotateZ(-45);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
	mat4 instance = translate * rotate * scale;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh( modelMatrix * instance, RightUpperArm, RightUpperArmObject,light,camera);
}

//  右小臂
void right_lower_arm(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(0.0, -0.5*robot.LOWER_ARM_HEIGHT, 0.0)*robot.ida_size);
	mat4 rotate = RotateZ(-45);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
	mat4 instance = translate * rotate * scale;
	drawMesh(modelMatrix*instance, RightLowerArm,RightLowerArmObject,light,camera);
}

// 左大腿
void left_upper_leg(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(0.0, -0.5*robot.UPPER_LEG_HEIGHT, 0.0)*robot.ida_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
    mat4 instance = translate * scale;
	drawMesh(modelMatrix*instance, LeftUpperLeg,LeftUpperLegObject,light,camera);
}

//左小腿
void left_lower_leg(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(0.0, -0.5*robot.LOWER_LEG_HEIGHT, 0.0)*robot.ida_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
    mat4 instance = translate * scale;
	// mat4 instance = translate ;
	drawMesh(modelMatrix*instance, LeftLowerLeg,LeftLowerLegObject,light,camera);
}

//右大腿
void right_upper_leg(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(0.0, -0.5*robot.UPPER_LEG_HEIGHT, 0.0)*robot.ida_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
    mat4 instance = translate * scale;
	// mat4 instance = translate ;
	drawMesh(modelMatrix*instance, RightUpperLeg,RightUpperLegObject,light,camera);
}

//右小腿
void right_lower_leg(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(0.0, -0.5*robot.LOWER_LEG_HEIGHT, 0.0)*robot.ida_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*robot.ida_size);
    mat4 instance = translate * scale;
	// mat4 instance = translate ;
	drawMesh(modelMatrix*instance, RightLowerLeg,RightLowerLegObject,light,camera);
}

// =========== 乌鸦 ===========
//乌鸦身体
void body(mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	mat4 translate = Translate(vec3(0.0, 0.5 * crow.BODY_HEIGHT, 0.0)*crow.crow_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*crow.crow_size);
    mat4 instance = translate * scale;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, Body, BodyObject,light,camera);
}

//乌鸦头
void crowhead(mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	mat4 translate = Translate(vec3(0.0, 0.5 * crow.HEAD_HEIGHT, 0.0)*crow.crow_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*crow.crow_size);
    mat4 instance = translate * scale;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, CrowHead, CrowHeadObject,light,camera);
}

//乌鸦左腿
void left_leg(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(0.0, -0.5*crow.LEG_HEIGHT, 0.0)*crow.crow_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*crow.crow_size);
    mat4 instance = translate * scale;
	drawMesh(modelMatrix*instance, LeftLeg,LeftLegObject,light,camera);
}

//乌鸦右腿
void right_leg(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(0.0, -0.5*crow.LEG_HEIGHT, 0.0)*crow.crow_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*crow.crow_size);
    mat4 instance = translate * scale;
	// mat4 instance = translate ;
	drawMesh(modelMatrix*instance, RightLeg,RightLegObject,light,camera);
}

// =========== 粉城堡 ===========
void House(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(0.0, 0.5 * monument.HOUSE_HEIGHT, 0.0)*monument.monu_size);
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*monument.monu_size);
    mat4 instance = translate * scale;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, house, houseObject,light,camera);
}

void Stair(mat4 modelMatrix)
{
	mat4 translate = Translate(vec3(-monument.STAIR_WIDTH, 0.0, 0.0)*monument.monu_size);//旋转中心左移厚度
	mat4 scale = Scale(vec3(1.0,1.0,1.0)*monument.monu_size);
    mat4 instance = translate * scale;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	drawMesh(modelMatrix * instance, stair, stairObject,light,camera);
}

void Part(mat4 modelMatrix)////////////////////////////////////////
{
	mat4 translate = Translate(vec3(0.85, 0.0, -0.85));
	drawMesh(modelMatrix * translate, part, partObject,light,camera);
}

void init()
{
	std::string vshader, fshader;
	// 读取着色器并使用
#ifdef __APPLE__	// for MacOS
	vshader = "shaders/vshader_mac.glsl";
	fshader = "shaders/fshader_mac.glsl";
#else				// for Windows
	vshader = "shaders/vshader_win.glsl";
	fshader = "shaders/fshader_win.glsl";
#endif

	// 设置光源位置
	light->setTranslation(vec3(10.0, 10.0, 7.0));
	light->setAmbient(vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射

	//1：绘制艾达
	Torso->readObj("./assets/ida2/body.obj");
	Head->readObj("./assets/ida2/head.obj");
	RightUpperArm->readObj("./assets/ida2/upper_arm.obj");
	LeftUpperArm->readObj("./assets/ida2/upper_arm.obj");
	RightUpperLeg->readObj("./assets/ida2/upper_leg.obj");
	LeftUpperLeg->readObj("./assets/ida2/upper_leg.obj");
	RightLowerArm->readObj("./assets/ida2/right_lower_arm.obj");
	LeftLowerArm->readObj("./assets/ida2/left_lower_arm.obj");
	RightLowerLeg->readObj("./assets/ida2/lower_leg.obj");
	LeftLowerLeg->readObj("./assets/ida2/lower_leg.obj");

	//设置初始位置
	Torso->setTranslation(vec3(0.0,4.2,0.0));//控制刚好站在y=0的地面上

	// 将物体的顶点数据传递
	bindObjectAndData(Torso, TorsoObject, vshader, fshader,"./assets/ida2/body.png");
	bindObjectAndData(Head, HeadObject, vshader, fshader,"./assets/ida2/head.png");
	bindObjectAndData(RightUpperArm, RightUpperArmObject, vshader, fshader,"./assets/ida2/upper_arm.png");
	bindObjectAndData(LeftUpperArm, LeftUpperArmObject, vshader, fshader,"./assets/ida2/upper_arm.png");
	bindObjectAndData(RightUpperLeg, RightUpperLegObject, vshader, fshader,"./assets/ida2/upper_leg.png");
	bindObjectAndData(LeftUpperLeg, LeftUpperLegObject, vshader, fshader,"./assets/ida2/upper_leg.png");
	bindObjectAndData(RightLowerArm, RightLowerArmObject, vshader, fshader,"./assets/ida2/right_lower_arm.png");
	bindObjectAndData(LeftLowerArm, LeftLowerArmObject, vshader, fshader,"./assets/ida2/left_lower_arm.png");
	bindObjectAndData(RightLowerLeg, RightLowerLegObject, vshader, fshader,"./assets/ida2/lower_leg.png");
	bindObjectAndData(LeftLowerLeg, LeftLowerLegObject, vshader, fshader,"./assets/ida2/lower_leg.png");

	//2:乌鸦
	Body->readObj("./assets/crow/body.obj");
	CrowHead->readObj("./assets/crow/head.obj");
	LeftLeg->readObj("./assets/crow/leg.obj");
	RightLeg->readObj("./assets/crow/leg.obj");

	Body->setTranslation(vec3(5.0,crow.LEG_HEIGHT,0.0));//控制刚好站在y=0的地面上

	bindObjectAndData(Body, BodyObject, vshader, fshader, "./assets/crow/body.png");
	bindObjectAndData(CrowHead, CrowHeadObject, vshader, fshader, "./assets/crow/head.png");
	bindObjectAndData(LeftLeg, LeftLegObject, vshader, fshader, "./assets/crow/leg.png");
	bindObjectAndData(RightLeg, RightLegObject, vshader, fshader, "./assets/crow/leg.png");
	
	//粉城堡
	house->readObj("./assets/monu/monu2/monu2.obj");
	stair->readObj("./assets/monu/monu2/stair.obj");

	house->setTranslation(vec3(10.0,0.0,0.0));

	bindObjectAndData(house,houseObject,vshader, fshader, "./assets/monu/monu2/monu2.png");
	bindObjectAndData(stair,stairObject,vshader, fshader, "./assets/monu/monu2/stair.png");

	//2:地面
	plane->generateSquare(vec3(0.6, 0.8, 0.0));
	// 设置正方形的位置和旋转，注意这里我们将正方形平面下移了一点点距离，
	// 这是为了防止和阴影三角形重叠在同个平面上导致颜色交叉
	plane->setRotation(vec3(90, 0, 0));
	plane->setTranslation(vec3(0, -0.002, 0));
	plane->setScale(vec3(60, 60, 60));
	bindObjectAndData(plane, plane_object, vshader, fshader,"./assets/gray.png");

	//3:城堡 monu1(绿) 高7.8 长8.6 宽8.9
	float monu_scale = 1;
	// monu->setNormalize(true);
	monu->readObj("./assets/monu/monu1/monu1.obj");
	monu->setTranslation(vec3(-10, 3.9*monu_scale, 0.0));
	monu->setRotation(vec3(0.0, 0.0, 0.0));
	monu->setScale(vec3(monu_scale, monu_scale, monu_scale));
	bindObjectAndData(monu, monuObject, vshader, fshader,"./assets/monu/monu1/monu1.png");
	//绿城堡旋转部分
	part->readObj("./assets/monu/monu1/part2.obj");
	// part->setTranslation(vec3(-10-0.6, 2.35*monu_scale, 0.0-0.7));
	part->setTranslation(vec3(-10-0.9, -0.1, 0.8));
	part->setRotation(vec3(0.0, 0.0, 0.0));
	part->setScale(vec3(monu_scale, monu_scale, monu_scale));
	bindObjectAndData(part, partObject, vshader, fshader,"./assets/monu/monu1/part2.png");

	//城堡2（粉） 高12.1 ,x 9.3, y 7.0
	// float monu2_scale = 1;
	// // monu->setNormalize(true);
	// monu2->readObj("./assets/monu/monu2/monu2.obj");
	// monu2->setTranslation(vec3(10, 6.05*monu2_scale, 0.0));
	// monu2->setRotation(vec3(0.0, 0.0, 0.0));
	// monu2->setScale(vec3(monu2_scale, monu2_scale, monu2_scale));
	// bindObjectAndData(monu2, monu2Object, vshader, fshader,"./assets/monu/monu2/monu2.png");

	//潘洛斯三角 6*6*6
	// penrose->setNormalize(true);
	penrose->readObj("./assets/penrose.obj");
	penrose->setTranslation(vec3(0.0, 3.1, 0.0));
	penrose->setRotation(vec3(0.0, 0.0, 0.0));
	penrose->setScale(vec3(1.0, 1.0, 1.0));
	bindObjectAndData(penrose, penroseObject, vshader, fshader,"./assets/penrose.png");

	//图腾 高6 宽1.8
	// totem->setNormalize(true);
	totem->readObj("./assets/totem.obj");
	totem->setTranslation(vec3(-4, 3.001, 9));
	totem->setRotation(vec3(0.0, 0.0, 0.0));
	totem->setScale(vec3(1.0, 1.0, 1.0));
	bindObjectAndData(totem, totemObject, vshader, fshader,"./assets/totem.png");

	//皇冠
	// crown->setNormalize(true);
	crown->readObj("./assets/crown-0-Crown.obj");
	crown->setTranslation(vec3(0.0, 0.0, -5));
	crown->setRotation(vec3(0.0, 0.0, 0.0));
	crown->setScale(vec3(1.0, 1.0, 1.0));
	bindObjectAndData(crown, crownObject, vshader, fshader,"./assets/crown-0-Crown.png");

	//镂空三角
	// emptri->setNormalize(true);
	emptri->readObj("./assets/crown-2-Zen_PyramidHollow_Counter.obj");
	emptri->setTranslation(vec3(10, 0.0, 0.0));
	emptri->setRotation(vec3(0.0, 0.0, 0.0));
	emptri->setScale(vec3(1.0, 1.0, 1.0));
	bindObjectAndData(emptri, emptriObject, vshader, fshader,"./assets/crown-2-Zen_PyramidHollow_Counter.png");

	//镂空正方体
	// cube->setNormalize(true);
	cube->readObj("./assets/crown-1-HollowCube.obj");
	cube->setTranslation(vec3(-10, 0.0, 5));
	cube->setRotation(vec3(0.0, 0.0, 0.0));
	cube->setScale(vec3(1.0, 1.0, 1.0));
	bindObjectAndData(cube, cubeObject, vshader, fshader,"./assets/crown-1-HollowCube.png");

	//树
	// tree->setNormalize(true);
	tree->readObj("./assets/tree.obj");
	tree->setTranslation(vec3(0.0, 0.0, 0.0));
	tree->setRotation(vec3(0.0, 0.0, 0.0));
	tree->setScale(vec3(2.0, 1.0, 2.0));
	bindObjectAndData(tree, treeObject, vshader, fshader,"./assets/tree.png");

	//绘制乌鸦
	// crow->setNormalize(true);
	// crow->readObj("./assets/crow-0-Crow.obj");
	// crow->setTranslation(vec3(2.0, 0.5, 0.0));
	// crow->setRotation(vec3(0.0, 0.0, 0.0));
	// crow->setScale(vec3(1.0, 1.0, 1.0));
	// bindObjectAndData(crow, crow_object, vshader, fshader,"./assets/crow-0-Crow.png");

	//荷花池
	// pool->setNormalize(true);
	pool->readObj("./assets/pool-0-Lotus1.obj");
	pool->setTranslation(vec3(14, 0.0, 0.0));
	pool->setRotation(vec3(0.0, 0.0, 0.0));
	pool->setScale(vec3(0.7, 0.7, 0.7));
	bindObjectAndData(pool, poolObject, vshader, fshader,"./assets/pool-0-Lotus1.png");

	// glClearColor(0.3, 0.3, 0.3, 1.0);//灰色
	glClearColor(0.69, 0.87, 0.9, 1.0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 相机矩阵计算
	camera->updateCamera(movescale);
	// camera->updateCamera(Torso->getTranslation());
	camera->viewMatrix = camera->getViewMatrix();
	camera->projMatrix = camera->getProjectionMatrix(false);

	// 物体的变换矩阵
	mat4 modelMatrix = mat4(1.0);

	// 保存变换矩阵的栈
	MatrixStack mstack;

	// 躯干（这里我们希望机器人的躯干只绕Y轴旋转，所以只计算了RotateY）
	// modelMatrix = Translate(vec3(0.0, 4.2, 0.0)*robot.ida_size)
    //              * RotateY(robot.theta[robot.Torso]);
	modelMatrix = Translate(Torso->getTranslation()*robot.ida_size)
                 * RotateY(robot.theta[robot.Torso]);
	torso(modelMatrix);

	mstack.push(modelMatrix); // 保存躯干变换矩阵
    // 头部（这里我们希望机器人的头部只绕Y轴旋转，所以只计算了RotateY）
    modelMatrix *= Translate(vec3(0.0, robot.TORSO_HEIGHT, -1.5)*robot.ida_size)
				* RotateY(robot.theta[robot.Head]);
	head(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵

    // =========== 左臂 ===========
	mstack.push(modelMatrix);   // 保存躯干变换矩阵
    // 左大臂（这里我们希望机器人的左大臂只绕Z轴旋转，所以只计算了RotateZ，后面同理）
	modelMatrix *= Translate(vec3(-0.25*robot.TORSO_WIDTH - 0.5*robot.UPPER_ARM_WIDTH, robot.TORSO_HEIGHT*0.8, 0.5)*robot.ida_size)
				* RotateX( robot.theta[ robot.LeftUpperArm ]);
	left_upper_arm(modelMatrix);

    //左小臂
	modelMatrix *= Translate(vec3(-1.1, -robot.UPPER_ARM_HEIGHT*0.58, 0.0)*robot.ida_size)
				  * RotateX(robot.theta[robot.LeftLowerArm]);
	left_lower_arm(modelMatrix);
	modelMatrix = mstack.pop();

    // =========== 右臂 ===========
	// 右大臂
	mstack.push(modelMatrix);
	modelMatrix *= Translate(vec3(0.25*robot.TORSO_WIDTH + 0.5*robot.UPPER_ARM_WIDTH, robot.TORSO_HEIGHT*0.8, 0.5)*robot.ida_size)
				  * RotateX(robot.theta[ robot.RightUpperArm ]);
	right_upper_arm(modelMatrix);

    // 右小臂
	modelMatrix *= Translate(vec3(1.1, -robot.UPPER_ARM_HEIGHT*0.58, 0.0)*robot.ida_size)
				  * RotateX(robot.theta[robot.RightLowerArm]);
	right_lower_arm(modelMatrix);
	modelMatrix = mstack.pop();

    // =========== 左腿 ===========
    // 左大腿
	mstack.push(modelMatrix);
	modelMatrix *= Translate(vec3(-0.15*robot.TORSO_WIDTH, 0.0, 0.25)*robot.ida_size)
				  *RotateX(robot.theta[robot.LeftUpperLeg]);
	left_upper_leg(modelMatrix);

	// 左小腿
	modelMatrix *= Translate(vec3(0.0, -robot.UPPER_LEG_HEIGHT, 0.26)*robot.ida_size)
					*RotateX(robot.theta[robot.LeftLowerLeg]);
	left_lower_leg(modelMatrix);
	modelMatrix = mstack.pop();

    // =========== 右腿 ===========
    // 右大腿
	mstack.push(modelMatrix);
	modelMatrix *= Translate(vec3(0.15*robot.TORSO_WIDTH, 0.0, 0.25)*robot.ida_size)
				  *RotateX(robot.theta[robot.RightUpperLeg]);
	right_upper_leg(modelMatrix);
    //右小腿
	modelMatrix *= Translate(vec3(0.0, -robot.UPPER_LEG_HEIGHT, 0.26)*robot.ida_size)
					*RotateX(robot.theta[robot.RightLowerLeg]);
	right_lower_leg(modelMatrix);
	modelMatrix = mstack.pop();

    // =========== 乌鸦 ===========
	//绘制乌鸦///////////////////////////////////////////////////////////////////////////////////测试
	// modelMatrix = crow1->getModelMatrix();
	// drawMesh(modelMatrix,crow1,crow1Object,light,camera);

	modelMatrix = Translate(Body->getTranslation()*crow.crow_size)
                 * RotateY(crow.theta[crow.Body]);
	body(modelMatrix);

	mstack.push(modelMatrix); // 保存躯干变换矩阵
    // 头部（这里我们希望机器人的头部只绕Y轴旋转，所以只计算了RotateY）
    modelMatrix *= Translate(vec3(0.0, crow.BODY_HEIGHT, 0.7)*crow.crow_size)
				* RotateY(crow.theta[crow.Head]);
	crowhead(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵

	 //  左腿
	mstack.push(modelMatrix);
	modelMatrix *= Translate(vec3(-0.15*crow.BODY_WIDTH, 0.0, 0.25)*crow.crow_size)
				  *RotateX(crow.theta[crow.LeftLeg]);
	left_leg(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵

    // 右腿
	mstack.push(modelMatrix);
	modelMatrix *= Translate(vec3(0.15*crow.BODY_WIDTH, 0.0, 0.25)*crow.crow_size)
				  *RotateX(crow.theta[crow.RightLeg]);
	right_leg(modelMatrix);

	// =========== 粉城堡 ===========
	//城堡
	modelMatrix = Translate(house->getTranslation()*monument.monu_size)
                 * RotateY(monument.theta[monument.house]);
	House(modelMatrix);
	//楼梯
	modelMatrix *= Translate(vec3(-1.8, 3, -1.1)*monument.monu_size)
				* RotateX(monument.theta[monument.stair]);
	Stair(modelMatrix);

	//绘制城堡
	modelMatrix = monu->getModelMatrix();
	drawMesh(modelMatrix,monu,monuObject,light,camera);
	//绿城堡旋转部分
	// mat4 translate = Translate(vec3(0.85, 0.0,0.0));//移动旋转轴
	// modelMatrix = Translate(part->getTranslation())
	// 			 *RotateY(part->getRotation().y);
	// drawMesh(modelMatrix*translate,part,partObject,light,camera);
	modelMatrix = part->getModelMatrix();
	Part(modelMatrix);

	//城堡2
	// modelMatrix = monu2->getModelMatrix();
	// drawMesh(modelMatrix,monu2,monu2Object,light,camera);
	
	//绘制地面
	modelMatrix = plane->getModelMatrix();
	plane->setDrawShadow(false);//地面不需要绘制阴影
	drawMesh(modelMatrix,plane,plane_object,light,camera);

	//绘制三角
	modelMatrix = penrose->getModelMatrix();
	drawMesh(modelMatrix,penrose,penroseObject,light,camera);

	//绘制图腾
	modelMatrix = totem->getModelMatrix();
	drawMesh(modelMatrix,totem,totemObject,light,camera);

	//皇冠
	modelMatrix = crown->getModelMatrix();
	drawMesh(modelMatrix,crown,crownObject,light,camera);

	//镂空三角形
	// modelMatrix = emptri->getModelMatrix();
	// drawMesh(modelMatrix,emptri,emptriObject,light,camera);

	//镂空正方体
	modelMatrix = cube->getModelMatrix();
	drawMesh(modelMatrix,cube,cubeObject,light,camera);

	// //树
	// modelMatrix = tree->getModelMatrix();
	// drawMesh(modelMatrix,tree,treeObject,light,camera);

	//荷花池
	modelMatrix = pool->getModelMatrix();
	drawMesh(modelMatrix,pool,poolObject,light,camera);

	glutSwapBuffers();
}


void printHelp()
{
	std::cout << "Keyboard Usage" << std::endl;
	std::cout <<
		"[Window]" << std::endl <<
		"ESC:		Exit" << std::endl <<
		"h:		Print help message" << std::endl <<
		std::endl <<

		"[Model]" << std::endl <<
		"c/C:	Increase rotate angle" << std::endl <<
		"v/V:	Decrease rotate angle" << std::endl <<

		std::endl <<
		"[Camera]" << std::endl <<
		"SPACE:		Reset camera parameters" << std::endl <<
		"u/U:		Increase/Decrease the rotate angle" << std::endl <<
		"i/I:		Increase/Decrease the up angle" << std::endl <<
		"o/O:		Increase/Decrease the camera radius" << std::endl << 

		std::endl <<
		"[IDA]" << std::endl <<
		"↑:		控制人物向z轴负方向移动" <<std::endl <<
		"↓:		控制人物向z轴正方向移动" <<std::endl <<
		"←:		控制人物向x轴负方向移动" <<std::endl <<
		"→:		控制人物向x轴正方向移动" <<std::endl <<

		std::endl <<
		"[CROW]" << std::endl <<
		"w:		控制乌鸦向z轴负方向移动" <<std::endl <<
		"s:		控制乌鸦向z轴正方向移动" <<std::endl <<
		"a:		控制乌鸦向x轴负方向移动" <<std::endl <<
		"d:		控制乌鸦向x轴正方向移动" <<std::endl <<

		std::endl <<
		"[CASTLE]" << std::endl <<
		"b:		控制绿城堡部件旋转" <<std::endl <<
		"n:		控制粉城堡楼梯旋转" <<std::endl;

}

// 鼠标响应函数
void mouse(int x, int y)
{
	//用鼠标控制光源的位置lightPos，以实时更新光照效果（已完成）
	// 窗口坐标系与图像坐标系转换
	float half_winx = WIDTH / 2.0;
	float half_winy = HEIGHT / 2.0;
	float lx = float(x - half_winx) / half_winx;
	float ly = float(HEIGHT - y - half_winy) / half_winy;
}

void mouseWheel(int button, int dir, int x, int y)
{
	
}

//控制机器人动作变换
void changestate()
{
	if(robot.frame==1)
	{
		robot.theta[0] = 0.0; // Torso
		robot.theta[1] = 0.0;    // Head
		robot.theta[2] = -20.0;    // RightUpperArm
		robot.theta[3] = -20.0;    // RightLowerArm
		robot.theta[4] = 20.0;    // LeftUpperArm
		robot.theta[5] = -20.0;   // LeftLowerArm
		robot.theta[6] = 20.0;    // RightUpperLeg
		robot.theta[7] = 20.0;    // RightLowerLeg
		robot.theta[8] = -20.0;    // LeftUpperLeg
		robot.theta[9] = 20.0;     // LeftLowerLeg
	}
	else if(robot.frame==2||robot.frame==4)
	{
		robot.theta[0] = 0.0; // Torso
		robot.theta[1] = 0.0;    // Head
		robot.theta[2] = 0.0;    // RightUpperArm
		robot.theta[3] = 0.0;    // RightLowerArm
		robot.theta[4] = 0.0;    // LeftUpperArm
		robot.theta[5] = 0.0;   // LeftLowerArm
		robot.theta[6] = 0.0;    // RightUpperLeg
		robot.theta[7] = 0.0;    // RightLowerLeg
		robot.theta[8] = 0.0;    // LeftUpperLeg
		robot.theta[9] = 0.0;     // LeftLowerLeg
	}
	else if(robot.frame==3)
	{
		robot.theta[0] = 0.0; // Torso
		robot.theta[1] = 0.0;    // Head
		robot.theta[2] = 20.0;    // RightUpperArm
		robot.theta[3] = -20.0;    // RightLowerArm
		robot.theta[4] = -20.0;    // LeftUpperArm
		robot.theta[5] = -20.0;   // LeftLowerArm
		robot.theta[6] = -20.0;    // RightUpperLeg
		robot.theta[7] = 20.0;    // RightLowerLeg
		robot.theta[8] = 20.0;    // LeftUpperLeg
		robot.theta[9] = 20.0;     // LeftLowerLeg
	}
}

//行走动作
void walk()
{
	robot.frame=(robot.frame++)%4;
	changestate();
	// std::cout<<"frame="<<robot.frame<<std::endl;
	glutPostRedisplay();
}

//控制乌鸦不同状态的动作
void crowchangestate()
{
	if(crow.crowframe==1)
	{
		crow.theta[0] = 0.0;    // Torso
		crow.theta[1] = 0.0;    // Head
		crow.theta[2] = 30.0;    // RightLeg
		crow.theta[3] = -30.0;    // LeftLeg
	}
	else if(crow.crowframe==2||crow.crowframe==4)
	{
		crow.theta[0] = 0.0;    // Torso
		crow.theta[1] = 0.0;    // Head
		crow.theta[2] = 0.0;    // RightLeg
		crow.theta[3] = 0.0;    // LeftLeg
	}
	else if(crow.crowframe==3)
	{
		crow.theta[0] = 0.0;    // Torso
		crow.theta[1] = 0.0;    // Head
		crow.theta[2] = -30.0;    // RightLeg
		crow.theta[3] = 30.0;    // LeftLeg
	}
}

//乌鸦行走
void crowwalk()
{
	crow.crowframe=(crow.crowframe++)%4;
	crowchangestate();
	// std::cout<<"crowframe="<<crow.crowframe<<std::endl;
	glutPostRedisplay();
}

// 键盘响应函数
void keyboard(unsigned char key, int x, int y)
{
	float tmp;
	vec4 ambient;
	int rtscale = part->getRotation().y+45;//绿城堡旋转角度
	switch (key)
	{
		// ESC键退出游戏
	case 033: exit(EXIT_SUCCESS); break;
	case 'h': printHelp(); break;
	// 通过按键旋转
    case 'c': case 'C':
        robot.theta[Selected_mesh] += 5.0;
	    if (robot.theta[Selected_mesh] > 360.0)
            robot.theta[Selected_mesh] -= 360.0;
		break;
    case 'v': case 'V':
        robot.theta[Selected_mesh] -= 5.0;
	    if (robot.theta[Selected_mesh] < 0.0)
            robot.theta[Selected_mesh] += 360.0;
		break;
	//wsad键控制乌鸦移动方向
	case 'w'://上
		crowwalk();
		crow.theta[Selected_mesh1] = 180.0;
		Body->setTranslation(Body->getTranslation()+vec3(0.0,0.0,-0.2));
		break;
	case 's'://下
		crowwalk();
		crow.theta[Selected_mesh1] = 0.0;
		Body->setTranslation(Body->getTranslation()+vec3(0.0,0.0,0.2));
		break;
	case 'a'://左
		crowwalk();
		crow.theta[Selected_mesh1] = -90.0;
		Body->setTranslation(Body->getTranslation()+vec3(-0.2,0.0,0.0));
		break;
	case 'd'://右
		crowwalk();
		crow.theta[Selected_mesh1] = 90.0;
		Body->setTranslation(Body->getTranslation()+vec3(0.2,0.0,0.0));
		break;
	case 'b'://控制绿城堡旋转
		// int rtscale = part->getRotation().y+45;
		if(abs(rtscale)>360)
		{
			rtscale-=360;
		}
		part->setRotation(vec3(0.0,rtscale,0.0));
		break;
	case 'n'://控制粉城堡楼梯旋转
		monument.theta[monument.stair] += 45.0;
		if (monument.theta[monument.stair] > 360.0)
            monument.theta[monument.stair] -= 360.0;
		break;
	case ' ':
		camera->initCamera();
		movescale = vec4(0.0,0.0,0.0,0.0);
		break;
	default:
		camera->keyboard(key, x, y);
		break;
	}
	glutPostRedisplay();
}

float step_size = 1.0;
float c_move_size = 0.4;
// 键盘响应事件中的特殊按键响应:控制人物移动
void special(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			walk();
			robot.theta[Selected_mesh] = 180.0;
			Torso->setTranslation(Torso->getTranslation()+vec3(0.0,0.0,-step_size));
			//movescale控制相机移动幅度
			movescale += vec4(0.0,0.0,-c_move_size,0.0);
			//设置相机观察点
			// camera->at = vec4(Torso->getTranslation(),1.0);
			break;
		case GLUT_KEY_DOWN:
			walk();
			robot.theta[Selected_mesh] = 0.0;
			Torso->setTranslation(Torso->getTranslation()+vec3(0.0,0.0,step_size));
			movescale += vec4(0.0,0.0,c_move_size,0.0);
			// camera->at = vec4(Torso->getTranslation(),1.0);
			break;
		case GLUT_KEY_LEFT:
			walk();
			robot.theta[Selected_mesh] = -90.0;
			Torso->setTranslation(Torso->getTranslation()+vec3(-step_size,0.0,0.0));
			movescale += vec4(-c_move_size,0.0,0.0,0.0);
			// camera->at = vec4(Torso->getTranslation(),1.0);
			break;
		case GLUT_KEY_RIGHT:
			walk();
			robot.theta[Selected_mesh] = 90.0;
			Torso->setTranslation(Torso->getTranslation()+vec3(step_size,0.0,0.0));
			movescale += vec4(c_move_size,0.0,0.0,0.0);
			// camera->at = vec4(Torso->getTranslation(),1.0);
			break;
		}
}

void menu(int option)
{
	Selected_mesh = option;
}

void buildMean(){
    glutCreateMenu(menu);
    glutAddMenuEntry("torso", robot.Torso);
    glutAddMenuEntry("head",  robot.Head);
    glutAddMenuEntry("right_upper_arm",	robot.RightUpperArm);
    glutAddMenuEntry("right_lower_arm",	robot.RightLowerArm);
    glutAddMenuEntry("left_upper_arm",	robot.LeftUpperArm);
    glutAddMenuEntry("left_lower_arm",	robot.LeftLowerArm);
    glutAddMenuEntry("right_upper_leg",	robot.RightUpperLeg);
    glutAddMenuEntry("right_lower_leg",	robot.RightLowerLeg);
    glutAddMenuEntry("left_upper_leg",	robot.LeftUpperLeg);
    glutAddMenuEntry("left_lower_leg",	robot.LeftLowerLeg);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void idle()
{
	Sleep(100);
	glutPostRedisplay();
}

// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

void cleanData() {
	
	// 释放内存
	delete camera;
	camera = NULL;

	for (int i=0; i<meshList.size(); i++) {
		meshList[i]->cleanData();
		delete meshList[i];
	}
	meshList.clear();

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	// 窗口支持双重缓冲、深度测试、超采样
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	mainWindow = glutCreateWindow("2018152108-杨芳菲-期末大作业");
	
	#ifdef __APPLE__
	#else
		glewExperimental = GL_TRUE;
		glewInit();
	#endif

	// Init mesh, shaders, buffer
	init();

	// bind callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouse);
	// glutMouseWheelFunc(mouseWheel);
	glutIdleFunc(idle);

	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);

	buildMean();

	glutMainLoop();


	cleanData();


	return 0;
}