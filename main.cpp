#include <stdlib.h>
#include <fstream> 
#include <string>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//预设模型点数最大值
const int maxn = 2003;
//模型参数
int nFaces, nVertices, nEdges;
GLfloat vertices[maxn][3];
GLint faces[maxn][3];
GLfloat face_fx[maxn][3];
GLfloat vertices_fx[maxn][3];
//模型变换参数
double sx = 1, sy = 1, sz = 1;
double dx = 0, dy = 0, dz = 0;
static GLfloat theta[] = { 0.0,0.0,0.0 };
static GLint axis = 2;

//光源变换参数
int spin;
//模型显示方式
int mode;
//光源开关
int light0, light1;

//向量单位化
void normalize(GLfloat *d) {
	double mo = sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
	for (int i = 0; i < 3; ++i)
		d[i] /= mo;
}
//由面的顶点计算面法向
void cross(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d) {
	d[0] = (b[1] - a[1])*(c[2] - a[2]) - (b[2] - a[2])*(c[1] - a[1]);
	d[1] = (b[2] - a[2])*(c[0] - a[0]) - (b[0] - a[0])*(c[2] - a[2]);
	d[2] = (b[0] - a[0])*(c[1] - a[1]) - (b[1] - a[1])*(c[0] - a[0]);
	normalize(d);
}

//读取模型文件
void load_off(char* filename)//载入模型文件
{
	std::ifstream fin;
	fin.open(filename);
	std::string off;
	std::getline(fin, off);//读入首行字符串"OFF"

	fin >> nVertices >> nFaces >> nEdges; //读入点、面、边数目
	double x, y, z;
	for (int i = 0; i < nVertices; i++)
	{
		fin >> x >> y >> z;    //逐行读入顶点坐标;
		vertices[i][0] = x;
		vertices[i][1] = y;
		vertices[i][2] = z;
	}
	int n, vid1, vid2, vid3;
	for (int i = 0; i < nFaces; i++)
	{
		fin >> n >> vid1 >> vid2 >> vid3;  //逐行读入面的顶点序列;
		faces[i][0] = vid1;
		faces[i][1] = vid2;
		faces[i][2] = vid3;
		//计算面的法向
		cross(vertices[vid1], vertices[vid2], vertices[vid3], face_fx[i]);
		//累加点的法向
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				vertices_fx[faces[i][j]][k] += face_fx[i][k];
			}
		}
	}
	//单位化点法向
	for (int i = 0; i < nVertices; ++i)
		normalize(vertices_fx[i]);
	fin.close();
}

//绘制一个面，采用面法向
void polygon(int i)
{
	glBegin(GL_POLYGON);
	glNormal3fv(face_fx[i]);
	for(int j=0;j<3;++j)
		glVertex3fv(vertices[faces[i][j]]);
	glEnd();
}
//绘制一个面，采用点法向
void polygon1(int i)
{
	glBegin(GL_POLYGON);
	for (int j = 0; j < 3; ++j) {
		glNormal3fv(vertices_fx[faces[i][j]]);
		glVertex3fv(vertices[faces[i][j]]);
	}
	glEnd();
}
//绘制模型
void colormodule()
{
	if (mode == 0) {//采用面法向
		for (int i = 0; i < nFaces; ++i) {
			polygon(i);
		}
	}
	else {//采用点法向
		for (int i = 1; i < nFaces; ++i) {
			polygon1(i);
		}
	}
}

//光源控制函数
void light_control() {
	//光源位置
	GLfloat position[] = { 0.0, 1.5, 0.0, 1.0 };

	//移动光源light0
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
	else {//关灯
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	//固定光源light1
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
	else {//关灯
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT1);
	}
}

//模型显示函数
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	light_control();
	
	//变换模型后绘制
	glTranslatef(dx, dy, dz);
	glScalef(sx, sx, sx);
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);
	colormodule();

	glPopMatrix();
	glFlush();
}

//自适应窗口变换
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//鼠标回调函数控制光源开关
void mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) light0^=1;
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)light1^=1;
	display();
}
//xyz旋转，s缩放，l光源旋转，m模型显示模式切换
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
//模型移动
void s_keys(int key, int x, int y) {
	if (key == GLUT_KEY_UP)    dy += 0.5;
	if (key == GLUT_KEY_RIGHT) dx += 0.5;
	if (key == GLUT_KEY_DOWN)  dy -= 0.5;
	if (key == GLUT_KEY_LEFT)  dx -= 0.5;
	display();
}

//设定模型材料属性，光源属性，背景颜色
void myinit()
{
	//设置材料参数（默认为马棕色）
	GLfloat sss[] = { 192.0 / 255,192.0 / 255,192.0 / 255,1.0 };
	GLfloat mat_specular[] = { sss[0], sss[1], sss[2], 1.0 };
	GLfloat mat_diffuse[] = { sss[0], sss[1], sss[2],  1.0 };
	GLfloat mat_ambient[] = { sss[0], sss[1], sss[2],  1.0 };
	GLfloat mat_shininess = { 100.0 };
	//定义材料属性
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	//定义全局环境光
	GLfloat global_ambient[] = { 0.2 ,0.2,0.2,1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	//定义移动点光源0（绿色）
	GLfloat light_ambient0[] = {0.1,0.1,0.1, 1 };//环境光  
	GLfloat light_diffuse0[] = {0,1,0, 1.0 };//漫反射光
	GLfloat light_specular0[] = {0,1,0, 1.0 };//镜面光

	//定义固定点光源1（红色）
	GLfloat light_ambient1[] = { 0.1,0.1,0.1, 1 };//环境光  
	GLfloat light_diffuse1[] = { 1 ,0,0, 1.0 };//漫反射光
	GLfloat light_specular1[] = { 1,0,0, 1.0 };//镜面光

	//移动光源light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);

	//固定光源light 1
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);

	glShadeModel(GL_SMOOTH); /*enable smooth shading */
	glEnable(GL_DEPTH_TEST); /* enable z buffer */

	//设置背景颜色
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