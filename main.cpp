#include <stdlib.h>
#include <fstream> 
#include <string>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//Ԥ��ģ�͵������ֵ
const int maxn = 2003;
//ģ�Ͳ���
int nFaces, nVertices, nEdges;
GLfloat vertices[maxn][3];
GLint faces[maxn][3];
GLfloat face_fx[maxn][3];
GLfloat vertices_fx[maxn][3];
//ģ�ͱ任����
double sx = 1, sy = 1, sz = 1;
double dx = 0, dy = 0, dz = 0;
static GLfloat theta[] = { 0.0,0.0,0.0 };
static GLint axis = 2;

//��Դ�任����
int spin;
//ģ����ʾ��ʽ
int mode;
//��Դ����
int light0, light1;

//������λ��
void normalize(GLfloat *d) {
	double mo = sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
	for (int i = 0; i < 3; ++i)
		d[i] /= mo;
}
//����Ķ�������淨��
void cross(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d) {
	d[0] = (b[1] - a[1])*(c[2] - a[2]) - (b[2] - a[2])*(c[1] - a[1]);
	d[1] = (b[2] - a[2])*(c[0] - a[0]) - (b[0] - a[0])*(c[2] - a[2]);
	d[2] = (b[0] - a[0])*(c[1] - a[1]) - (b[1] - a[1])*(c[0] - a[0]);
	normalize(d);
}

//��ȡģ���ļ�
void load_off(char* filename)//����ģ���ļ�
{
	std::ifstream fin;
	fin.open(filename);
	std::string off;
	std::getline(fin, off);//���������ַ���"OFF"

	fin >> nVertices >> nFaces >> nEdges; //����㡢�桢����Ŀ
	double x, y, z;
	for (int i = 0; i < nVertices; i++)
	{
		fin >> x >> y >> z;    //���ж��붥������;
		vertices[i][0] = x;
		vertices[i][1] = y;
		vertices[i][2] = z;
	}
	int n, vid1, vid2, vid3;
	for (int i = 0; i < nFaces; i++)
	{
		fin >> n >> vid1 >> vid2 >> vid3;  //���ж�����Ķ�������;
		faces[i][0] = vid1;
		faces[i][1] = vid2;
		faces[i][2] = vid3;
		//������ķ���
		cross(vertices[vid1], vertices[vid2], vertices[vid3], face_fx[i]);
		//�ۼӵ�ķ���
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				vertices_fx[faces[i][j]][k] += face_fx[i][k];
			}
		}
	}
	//��λ���㷨��
	for (int i = 0; i < nVertices; ++i)
		normalize(vertices_fx[i]);
	fin.close();
}

//����һ���棬�����淨��
void polygon(int i)
{
	glBegin(GL_POLYGON);
	glNormal3fv(face_fx[i]);
	for(int j=0;j<3;++j)
		glVertex3fv(vertices[faces[i][j]]);
	glEnd();
}
//����һ���棬���õ㷨��
void polygon1(int i)
{
	glBegin(GL_POLYGON);
	for (int j = 0; j < 3; ++j) {
		glNormal3fv(vertices_fx[faces[i][j]]);
		glVertex3fv(vertices[faces[i][j]]);
	}
	glEnd();
}
//����ģ��
void colormodule()
{
	if (mode == 0) {//�����淨��
		for (int i = 0; i < nFaces; ++i) {
			polygon(i);
		}
	}
	else {//���õ㷨��
		for (int i = 1; i < nFaces; ++i) {
			polygon1(i);
		}
	}
}

//��Դ���ƺ���
void light_control() {
	//��Դλ��
	GLfloat position[] = { 0.0, 1.5, 0.0, 1.0 };

	//�ƶ���Դlight0
	if (light0 == 0) {
		glPushMatrix();
		glRotated((GLdouble)spin, 0, 0.0, 1.0);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glTranslated(0.0, 1.5, 0);
		glDisable(GL_LIGHTING);
		glColor3f(0.0, 1.0, 1.0);
		glutWireCube(0.1);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);  /* enable light 0 */
		glPopMatrix();
	}
	else {//�ص�
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	//�̶���Դlight1
	if (light1 == 0) {
		glPushMatrix();
		glLightfv(GL_LIGHT1, GL_POSITION, position);
		glEnable(GL_LIGHT1); /* enable light 1 */
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 1.0, 0.0);
		glTranslated(0.0, 1.5, 0);
		glutWireCube(0.1);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
	else {//�ص�
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT1);
	}
}

//ģ����ʾ����
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	light_control();
	
	//�任ģ�ͺ����
	glTranslatef(dx, dy, dz);
	glScalef(sx, sx, sx);
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);
	colormodule();

	glPopMatrix();
	glFlush();
}

//����Ӧ���ڱ任
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//���ص��������ƹ�Դ����
void mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) light0^=1;
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)light1^=1;
	display();
}
//xyz��ת��s���ţ�l��Դ��ת��mģ����ʾģʽ�л�
void keys(unsigned char key, int x, int y)
{
	if (key == 's' || key == 'S') sx *= key == 's' ? 0.9 : 1.1;
	if (key == 'x' || key == 'X') axis = 0;
	if (key == 'y' || key == 'Y') axis = 1;
	if (key == 'z' || key == 'Z') axis = 2;

	if (key == 'k') spin = (spin + 30) % 360;
	if (key == 'm') mode ^= 1;
	
	int dt = 0;
	if (key == 'x' || key == 'y' || key == 'z')			dt = 2.0;
	else if (key == 'X' || key == 'Y' || key == 'Z')	dt = -2.0;

	theta[axis] += dt;
	if (theta[axis] > 360.0)	theta[axis] -= 360.0;
	if (theta[axis] < 0)		theta[axis] += 360.0;
	display();
}
//ģ���ƶ�
void s_keys(int key, int x, int y) {
	if (key == GLUT_KEY_UP)    dy += 0.5;
	if (key == GLUT_KEY_RIGHT) dx += 0.5;
	if (key == GLUT_KEY_DOWN)  dy -= 0.5;
	if (key == GLUT_KEY_LEFT)  dx -= 0.5;
	display();
}

//�趨ģ�Ͳ������ԣ���Դ���ԣ�������ɫ
void myinit()
{
	//���ò��ϲ�����Ĭ��Ϊ����ɫ��
	GLfloat sss[] = { 192.0 / 255,192.0 / 255,192.0 / 255,1.0 };
	GLfloat mat_specular[] = { sss[0], sss[1], sss[2], 1.0 };
	GLfloat mat_diffuse[] = { sss[0], sss[1], sss[2],  1.0 };
	GLfloat mat_ambient[] = { sss[0], sss[1], sss[2],  1.0 };
	GLfloat mat_shininess = { 100.0 };
	//�����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	//����ȫ�ֻ�����
	GLfloat global_ambient[] = { 0.2 ,0.2,0.2,1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	//�����ƶ����Դ0����ɫ��
	GLfloat light_ambient0[] = {0.1,0.1,0.1, 1 };//������  
	GLfloat light_diffuse0[] = {0,1,0, 1.0 };//�������
	GLfloat light_specular0[] = {0,1,0, 1.0 };//�����

	//����̶����Դ1����ɫ��
	GLfloat light_ambient1[] = { 0.1,0.1,0.1, 1 };//������  
	GLfloat light_diffuse1[] = { 1 ,0,0, 1.0 };//�������
	GLfloat light_specular1[] = { 1,0,0, 1.0 };//�����

	//�ƶ���Դlight 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);

	//�̶���Դlight 1
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);

	glShadeModel(GL_SMOOTH); /*enable smooth shading */
	glEnable(GL_DEPTH_TEST); /* enable z buffer */

	//���ñ�����ɫ
	glClearColor(0,0,0, 1.0);
}

int main(int argc, char** argv)
{
	char s1[20] = "horse.off", s2[20] = "cube.off";
	load_off(s1);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	myinit();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keys);
	glutSpecialFunc(s_keys);
	glutMainLoop();
	return 0;
}
