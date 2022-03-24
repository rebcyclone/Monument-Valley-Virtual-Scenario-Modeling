#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Angel.h"

class Camera
{
public:
	Camera(vec4 move_scale);
	~Camera();

	mat4 getViewMatrix();
	mat4 getProjectionMatrix( bool isOrtho );

	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up);

	mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar);

	mat4 perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar);

	mat4 frustum(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar);

	// 每次更改相机参数后更新一下相关的数值
	void updateCamera(const vec4 move_scale);
	void initCamera();

	// 处理相机的键盘操作
	void keyboard(unsigned char key, int x, int y);


	// 模视矩阵
	mat4 viewMatrix;
	mat4 projMatrix;

	// 相机位置参数
	float radius;
	float rotateAngle;
	float upAngle;
	vec4 eye;
	vec4 at;
	vec4 up;

	// 投影参数
	float zNear;
	float zFar;
	// 透视投影参数
	float fovy;
	float aspect;
	// 正交投影参数
	float scale;

};
#endif