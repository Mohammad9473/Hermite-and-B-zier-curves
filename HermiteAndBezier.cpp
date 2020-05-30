#include <GL/glut.h>
#include <bevgrafmath2017.h>
#include <math.h>

GLsizei winWidth = 800, winHeight = 600;
vec2 v1 = vec2(50, 250);
vec2 v2 = vec2(-50, -100);
vec2 points[4] = { // hermit points
	{100.0f , 200.0f}, //point1
	{200.0f , 150.0f}, //pint2
	{250.0f, 300.0f}, //point3
	{100 + 50, 200 + 250}, //vector v1

 };
vec2 points2[7] = { //bezier points
	{250.0f , 300.0f}, //point 1
	{550.0f, 150.0f}, //point2
	{600.0f, 50.0f}, //point3
	{650.0f, 200.0f}, //point4
	{700.0f, 200.0f}, //point5
	{720.0f, 300.0f}, //point6
	{750.0f, 400.0f} //point 7

};
vec2 tangent;
GLint dragged = -1;
GLint dragged2 = -1;
void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, winWidth, 0.0, winHeight);
    glShadeModel(GL_FLAT);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(5.0);
    glLineWidth(1.0);
}




void drawHermite() { //hermit curve with points
	
	v1 = points[3] - points[0]; //recalculating the v1
	
	glBegin(GL_POINTS); //for points
	for (int i = 0; i <4 ; i++)
		glVertex2f(points[i].x, points[i].y);
	glEnd();

	glEnable(GL_LINE_STIPPLE); //lines for curve
	glLineStipple(1, 0xFF00);
	glBegin(GL_LINES); 
	//tangent vector
	glVertex2f(points[0].x, points[0].y);
	glVertex2f(points[0].x + v1.x, points[0].y + v1.y);

	glEnd();
	glDisable(GL_LINE_STIPPLE);

	mat24 G = { points[0],  points[1], points[2],  v1 }; //matrix for calculating the curve

	float tv[3] = { -2.0f, 0.0f , 1.0f };  //values of t as in the pdf

	vec4 columns[4];

	for (int i = 0; i < 3; i++) { //formula in pdf for each coloumn
		columns[i] = vec4(tv[i] * tv[i] * tv[i], tv[i] * tv[i], tv[i], 1);
	}
	columns[3] = vec4(3 * tv[0] * tv[0], 2 * tv[0], 1, 0);

	mat4 M = { columns[0],  columns[1],  columns[2],  columns[3], true }; //matrix 4*4
	mat4 M_ = inverse(M); //inverse of matrix
	mat24 C = G * M_; //formula

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (float t = tv[0]; t <= tv[2]; t += 0.01) { 
		vec4 T = { t * t * t, 
			t * t,
			t,
			1 };
		
		vec2 curvePoint = C * T; 
		glVertex2f(curvePoint.x, curvePoint.y);
	}
	glEnd();

	vec4 T; //calculating the tangent of the las point of hermit connected with the first point of bezier
	T = vec4(
		3 * tv[2] * tv[2],
		2 * tv[2],
		1,
		0);
	 v2 = C * T;
	
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0xFF00);
	glBegin(GL_LINES); //tangent vector for last point of hermit and first point of bezier which should be for hermit curve

	glVertex2f(points[2].x, points[2].y);	
	glVertex2f(points[2].x + v2.x, points[2].y +v2.y);

	glEnd();
	glDisable(GL_LINE_STIPPLE);


}


void calculateFunction() {
	int n2 = 6; //points of bezier so n -1 = 6
	

	points2[0] = points[2]; //the last point of hermit is the first point of bezier

	points2[1] = points2[0] + (v2/ n2); //calculating the first point of bezier considering formula (tangent vectors should be equal)

}


void drawBezier() { //bezier curve using casteljau

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POINTS);//draw the points

	for (int i = 0; i < 7; i++) {
		glVertex2f(points2[i].x, points2[i].y);
	}
	glEnd();


	glBegin(GL_LINE_STRIP); //draw the control polygon coresponding to points
	for (int i = 0; i < 7; i++) {
		glVertex2f(points2[i].x, points2[i].y);
	}
	glEnd();

	int n = 6; //7 points thus n-1 = 6

	vec2 helperPoints[7]; // helper array 
	for (int i = 0; i < n + 1; i++)
		helperPoints[i] = points2[i];

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINE_STRIP);
	for (float t = 0; t < 1; t += 0.01) { // t is in [0,1] corespond to formula
		for (int r = 1; r <= n; r++) { // generations counter
			for (int i = 0; i <= n - r; i++) { 
				vec2 newPoint = //using helper points considering the formula
				{ (1 - t)  * helperPoints[i].x + t * helperPoints[i + 1].x,
				(1 - t) * helperPoints[i].y + t * helperPoints[i + 1].y
				};
				helperPoints[i] = newPoint;
			}
		}
		glVertex2f(helperPoints[0].x, helperPoints[0].y); 

	}
	glVertex2f(points2[n].x, points2[n].y);
	glEnd();


}


void display() {
    glClear(GL_COLOR_BUFFER_BIT); 
    glColor3f(1.0, 0.0, 0.0);
	drawHermite();
	calculateFunction();
	drawBezier();
    glutSwapBuffers();
}

GLint getActivePoint1(vec2 p[], GLint size, GLint sens, GLint x, GLint y) { //active points for mouse
    GLint i, s = sens * sens;
    vec2 P = { (float)x, (float)y };
    
    for (i = 0; i < size; i++)
        if (dist(p[i], P) < s)
            return i;
    return -1;
}

void processMouse(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	GLint i;
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) {
		if ((i = getActivePoint1(points, 4, 8, xMouse, winHeight - yMouse)) != -1) //for hermit
			dragged = i;
		else if ((i = getActivePoint1(points2, 7, 8, xMouse, winHeight - yMouse)) != -1) //for bezier
			dragged2 = i;
	}
	if (button == GLUT_LEFT_BUTTON && action == GLUT_UP) {
		dragged = dragged2 = -1;
	}

}


void processMouseActiveMotion(GLint xMouse, GLint yMouse)
{
	GLint i;
	if (dragged >= 0) {
		points[dragged].x = xMouse;
		points[dragged].y = winHeight - yMouse;
		glutPostRedisplay();
	}
	else if (dragged2 >= 0) {
		points2[dragged2].x = xMouse;
		points2[dragged2].y = winHeight - yMouse;
		glutPostRedisplay();
	}
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Computer Graphics");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(processMouse);
    glutMotionFunc(processMouseActiveMotion);
    glutMainLoop();
    return 0;
}