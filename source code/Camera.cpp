#include "Camera.h"

Camera::Camera(vec4 move_scale) {
	initCamera();
	updateCamera(move_scale); 
};
Camera::~Camera() {}

mat4 Camera::getViewMatrix()
{
	return this->lookAt(eye, at, up);
}

mat4 Camera::getProjectionMatrix(bool isOrtho)
{
	if (isOrtho) {
		return this->ortho(-scale, scale, -scale, scale, this->zNear, this->zFar);
	}
	else {
		return this->perspective(this->fovy, this->aspect, this->zNear, this->zFar);
	}
}

mat4 Camera::lookAt(const vec4& eye, const vec4& at, const vec4& up)
{
	// @TODO: Task1:请按照实验课内容补全相机观察矩阵的计算
	vec4 n = normalize(eye - at);					// normalize():归一化函数
	vec4 u = normalize(vec4(cross(up, n), 0.0));	// cross()计算向量积
	vec4 v = normalize(vec4(cross(n, u), 0.0));
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = mat4(u, v, n, t);
	return c * Translate(-eye);						// 计算最后需要沿-eye方向平移
}

mat4 Camera::ortho(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	// @TODO: Task2:请按照实验课内容补全正交投影矩阵的计算
	mat4 c;
	c[0][0] = 2.0 / (right - left);
	c[1][1] = 2.0 / (top - bottom);
	c[2][2] = -2.0 / (zFar - zNear);
	c[3][3] = 1.0;
	c[0][3] = -(right + left) / (right - left);
	c[1][3] = -(top + bottom) / (top - bottom);
	c[2][3] = -(zFar + zNear) / (zFar - zNear);
	return c;
}

mat4 Camera::perspective(const GLfloat fovy, const GLfloat aspect,
	const GLfloat zNear, const GLfloat zFar)
{
	// @TODO: Task2:请按照实验课内容补全透视投影矩阵的计算
	GLfloat top = tan(fovy * M_PI / 180 / 2) * zNear;
	GLfloat right = top * aspect;

	mat4 c;
	c[0][0] = zNear / right;
	c[1][1] = zNear / top;
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -(2.0 * zFar * zNear) / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;
	return c;
}

mat4 Camera::frustum(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	// 任意视锥体矩阵
	mat4 c;
	c[0][0] = 2.0 * zNear / (right - left);
	c[0][2] = (right + left) / (right - left);
	c[1][1] = 2.0 * zNear / (top - bottom);
	c[1][2] = (top + bottom) / (top - bottom);
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -2.0 * zFar * zNear / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;
	return c;
}


void Camera::updateCamera(const vec4 move_scale)
{
	// 使用相对于at的角度控制相机的时候，注意在upAngle大于90的时候，相机坐标系的u向量会变成相反的方向，
	// 要将up的y轴改为负方向才不会发生这种问题

	// 也可以考虑直接控制相机自身的俯仰角，
	// 保存up，eye-at 这些向量，并修改这些向量方向来控制
	// 看到这里的有缘人可以试一试
	up = vec4(0.0, 1.0, 0.0, 0.0);
	if (upAngle > 90){
		up.y = -1;
	}
	else if (upAngle < -90){
		up.y = -1;
	}
	
	float eyex = radius * cos(upAngle * M_PI / 180.0) * sin(rotateAngle * M_PI / 180.0);
	float eyey = radius * sin(upAngle * M_PI / 180.0);
	float eyez = radius * cos(upAngle * M_PI / 180.0) * cos(rotateAngle * M_PI / 180.0);
	
	//加上移动，让相机跟着人物移动
	eye = vec4(eyex, eyey, eyez, 1.0) + move_scale;

}

void Camera::initCamera(){
	at = vec4(0.0, 0.0, 0.0, 1.0);

	radius = 35;
	rotateAngle = 10.0;
	upAngle = 25.0;//初始化相机角度
	fovy = 45.0;
	aspect = 1.0;
	scale = 1.5;
	zNear = 0.01;
	zFar = 100.0;
	//清除相机移动轨迹
	updateCamera(vec4(0.0,0.0,0.0,0.0));
}

void Camera::keyboard(unsigned char key, int x, int y)
{
	// 键盘事件处理
	switch (key)
	{
		// 通过按键改变相机和投影的参数
	case 'u':
		rotateAngle += 5.0;
		if (rotateAngle > 180)
			rotateAngle = rotateAngle - 360;
		break;
	case 'U':
		rotateAngle -= 5.0;
		if (rotateAngle < -180)
			rotateAngle = rotateAngle + 360;
		break;

	case 'i':
		upAngle += 5.0;
		if (upAngle >= 180)
			upAngle = upAngle - 360;
		break;
	case 'I':
		upAngle -= 5.0;
		if (upAngle <= -180)
			upAngle = upAngle + 360;
		break;

	case 'o':
		radius += 0.1;
		break;
	case 'O':
		radius -= 0.1;
		break;

	//case 'f':
	//	fovy += 5.0;
	//	break;
	//case 'F':
	//	fovy -= 5.0;
	//	break;

	//case 'a':
	//	aspect += 0.1;
	//	break;
	//case 'A':
	//	aspect -= 0.1;
	//	break;

	// case 'o':
	// 	scale += 0.1;
	// 	break;
	// case 'O':
	// 	scale -= 0.1;
	// 	break;

		// 空格键初始化所有参数
	// case ' ':
	// 	initCamera();
	// 	break;
	}
	
}
