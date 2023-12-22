#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}
void chet(double A[], double G[], double H[])
{
	double V[] = { G[0] - A[0], G[1] - A[1] ,G[2] - A[2] };
	double K[] = { H[0] - A[0], H[1] - A[1] ,H[2] - A[2] };
	double res[] = { V[1] * K[2] - V[2] * K[1],V[2] * K[0] - V[0] * K[2],V[0] * K[1] - K[0] * V[1] };
	double dlin = sqrt((res[0] * res[0]) + (res[1] * res[1]) + (res[2] * res[2]));
	res[0] = res[0] / dlin;
	res[1] = res[1] / dlin;
	res[2] = res[2] / dlin;
	glNormal3d(res[0], res[1], res[2]);

}
void convexity()
{
	double V[] = { 0.5,4.5,0 };
	glColor3d(0.5, 0.5, 1);
	glTexCoord2f(2.54951f, 2.54951f);
	for (int i = 10; i < 189; i += 2)
	{
		double x = 0.5 + 2.54951 * cos(i * (3.14 / 180.0));
		double y = 4.5 + 2.54951 * sin(i * (3.14 / 180.0));
		double x1 = 0.5 + 2.54951 * cos((i + 5) * (3.14 / 180.0));
		double y1 = 4.5 + 2.54951 * sin((i + 5) * (3.14 / 180.0));
		double X[] = { x,y,0 };
		double X1[] = { x1,y1,0 };
		glTexCoord2f(x, y);
		glBegin(GL_TRIANGLES);
		chet(X1, X, V);
		glVertex3d(0.5, 4.5, 0);
		glVertex3d(x, y, 0);
		glVertex3d(x1, y1, 0);
		glEnd();
		V[2] = 7;
		X[2] = 7;
		X1[2] = 7;
		glBegin(GL_TRIANGLES);
		chet(X1, V, X);
		//chet(V, X, X1);
		glVertex3d(0.5, 4.5, 7);
		glVertex3d(x, y, 7);
		glVertex3d(x1, y1, 7);
		glEnd();

		double A[] = { x, y, 0 };
		double B[] = { x1, y1, 0 };
		double C[] = { x1, y1, 7 };
		double D[] = { x, y, 7 };
		glBegin(GL_QUADS);
		chet(B, C, A);
		glVertex3dv(A);
		glVertex3dv(B);
		glVertex3dv(C);
		glVertex3dv(D);
		glEnd();
	}
}
void convexity1()
{
	glColor3d(0.5, 0.5, 1);
	glTexCoord2f(3.15f, 3.15f);
	glNormal3d(0, 0, 1);
	for (int i = 346; i < 441; i += 2)
	{
		double x = -6.31818 + 3.15 * cos(i * (3.14 / 180.0));
		double y = -5.13636 + 3.15 * sin(i * (3.14 / 180.0));
		double x1 = -6.31818 + 3.15 * cos((i + 5) * (3.14 / 180.0));
		double y1 = -5.13636 + 3.15 * sin((i + 5) * (3.14 / 180.0));

		double A[] = { x, y, 0 };
		double B[] = { x1, y1, 0 };
		double C[] = { x1, y1, 7 };
		double D[] = { x, y, 7 };
		glTexCoord2f(x, y);
		glBegin(GL_QUADS);
		chet(C, B, A);
		glVertex3dv(A);
		glVertex3dv(B);
		glVertex3dv(C);
		glVertex3dv(D);
		glEnd();
	}
}




void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);

	glColor3d(0.2, 0.4, 0.8);
	double A4[] = { 1,0,0 };
	double B4[] = { 1,0,7 };
	double C4[] = { 3,5,7 };
	double D4[] = { 3,5,0 };
	chet(B4, A4, C4);
	
	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A4);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B4);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C4);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D4);

	glColor3d(0.2, 0.4, 0.8);
	double A8[] = { -2,4,0 };
	double B8[] = { -2,4,7 };
	double C8[] = { -2,0,7 };
	double D8[] = { -2,0,0 };
	chet(B8, A8, C8);

	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A8);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B8);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C8);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D8);

	glColor3d(0.2, 0.4, 0.8);
	double A111[] = { -2,0,0 };
	double B11[] = { -2,0,7 };
	double C11[] = { -6,-2,7 };
	double D11[] = { -6,-2,0 };
	chet(B11, A111, C11);

	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A111);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B11);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C11);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D11);

	glColor3d(0.2, 0.4, 0.8);
	double A6[] = { -5,-8,0 };
	double B6[] = { -5,-8,7 };
	double C6[] = { 0,-2,7 };
	double D6[] = { 0,-2,0};
	chet(B6, A6, C6);

	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A6);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B6);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C6);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D6);

	glColor3d(0.2, 0.4, 0.8);
	double A9[] = { 0,-2,0 };
	double B9[] = { 0,-2,7 };
	double C9[] = { 7,-4,7 };
	double D9[] = { 7,-4,0 };
	chet(B9, A9, C9);

	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A9);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B9);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C9);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D9);

	glColor3d(0.2, 0.4, 0.8);
	double A7[] = { 7,-4,0 };
	double B7[] = { 7,-4,7 };
	double C7[] = { 1,0,7 };
	double D7[] = { 1,0,0 };
	chet(B7, A7, C7);

	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A7);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B7);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C7);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D7);

	
	glColor3d(0.2, 0.4, 0.4);
	double A101[] = { 1,0,0 };
	double B101[] = { 3,5,0 };
	double C101[] = { -2,4,0 };
	double D101[] = { -2,0,0 };
	chet(A101, C101, B101);
	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A101);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B101);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C101);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D101);
	glColor3d(0.2, 0.4, 0.4);
	double A1001[] = { -2,0,0 };
	double B1001[] = { -6,-2,0 };
	double C1001[] = { -5,-8,0 };
	double D1001[] = { 0,-2,0 };
	chet(C1001, B1001, D1001);
	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A1001);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B1001);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C1001);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D1001);
	glEnd();


	glBegin(GL_TRIANGLES);
	glColor3d(0.2, 0.4, 0.4);
	double A1002[] = { 0,-2,0 };
	double B1002[] = { 7,-4,0 };
	double C1002[] = { 1,0,0 };
	chet(B1002, A1002, C1002);
	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A1002);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B1002);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C1002);
	
	glColor3d(0.2, 0.4, 0.4);
	double A1005[] = { -2,0,0 };
	double B1005[] = { 0,-2,0 };
	double C1005[] = { 1,0,0 };
	chet(B1005, A1005, C1005);
	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A1005);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B1005);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C1005);

	glEnd();

	convexity();
	convexity1();

	
	glBindTexture(GL_TEXTURE_2D, texId);

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.5f);  // ����� ��������
	glVertex2d(10, 10);
	glNormal3d(0, 0, 1);
	for (int i = 0; i <= 100; ++i) {
		float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(100);
		float x = 5 * cos(angle);
		float y = 5 * sin(angle);

		float textureX = 0.5f * (1 + x / 5);  // ����������� ���������� � ��������
		float textureY = 0.5f * (1 + y / 5);

		glTexCoord2f(textureX, textureY);
		glVertex2d(x + 10, y + 10);
	}
	glEnd();

	//��������������
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	glBegin(GL_QUADS);
	glColor4d(0.4, 0.2, 0.4, 0.5);
	double A10[] = { 1,0,7 };
	double B10[] = { 3,5,7 };
	double C10[] = { -2,4,7 };
	double D10[] = { -2,0,7 };
	chet(C10, D10, B10);

	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A10);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B10);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C10);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D10);
	glColor4d(0.4, 0.2, 0.4, 0.5);
	double A100[] = { -2,0,7 };
	double B100[] = { -6,-2,7 };
	double C100[] = { -5,-8,7 };
	double D100[] = { 0,-2,7 };
	chet(A100, B100, C100);
	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A100);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B100);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C100);
	glTexCoord2f(0.489, 0.745);
	glVertex3dv(D100);
	glEnd();
	
	glBegin(GL_TRIANGLES);
	glColor4d(0.4, 0.2, 0.4, 0.5);
	double A1003[] = { 0,-2,7 };
	double B1003[] = { 7,-4,7 };
	double C1003[] = { 1,0,7 };
	chet(A1003, B1003, C1003);
	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A1003);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B1003);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C1003);
	glColor4d(0.4, 0.2, 0.4, 0.5);
	double A1004[] = { -2,0,7 };
	double B1004[] = { 0,-2,7 };
	double C1004[] = { 1,0,7 };
	chet(A1004, B1004, C1004);
	glTexCoord2f(0.345, 0.654);
	glVertex3dv(A1004);
	glTexCoord2f(0.242, 0.545);
	glVertex3dv(B1004);
	glTexCoord2f(0.751, 0.475);
	glVertex3dv(C1004);
	glEnd();

	//������ ��������� ���������� ��������
	/*double A1080[2] = {-4, -4};
	double B1080[2] = { 4, -4 };
	double C1080[2] = { 4, 4 };
	double D1080[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glVertex2dv(A1080);
	glVertex2dv(B1080);
	glVertex2dv(C1080);
	glVertex2dv(D1080);

	glEnd();*/

	//����� ��������� ���������� ��������


   //��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}