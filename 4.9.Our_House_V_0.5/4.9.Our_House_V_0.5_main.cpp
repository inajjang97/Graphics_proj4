#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp>
#include "geometry.h"

#define CNUM 6
glm::mat4 mouse_ViewMatrix, mouse_ProjectionMatrix;
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ModelViewMatrix, ViewMatrix, Nor_ViewMatrix, ProjectionMatrix,Nor_ProjectionMatrix, ViewProjectionMatrix;
glm::vec3 cur_eye, cur_center, cur_up, axis[3];
glm::mat4 cctv_ModelViewMatrix[CNUM], cctv_ViewMatrix[CNUM], cctv_ViewProjectionMatrix[CNUM], cctv_ProjectionMatrix[CNUM], cctv_ModelViewProjectionMatrix[CNUM], cctvMatrix,cctvProjection;

glm::mat4 ModelMatrix_CAR_BODY, ModelMatrix_CAR_WHEEL, ModelMatrix_CAR_NUT, ModelMatrix_CAR_DRIVER;
glm::mat4 ModelMatrix_CAR_BODY_to_DRIVER, ModelMatrix_PATH, Matrix_CAMERA_driver_inverse, ModelMatrix_Tiger; // computed only once in initialize_camera()
glm::mat3 cctv_ModelViewMatrixInvTrans[CNUM];
glm::vec3 main_light_pos = glm::vec3(140, 80, 150);

glm::vec3 tpos[1000];
int car_time = 0;
float tangle[1000], cangle[1000];
int tstop[100], cstop[100];
float tiger_x=25.0f, tiger_y=25.0f, tiger_z=0.0f, cctv_tiger_x=25.0f, cctv_tiger_y=25.0f, cctv_tiger_z=0.0f;
float win_aspect_ratio ,cur_fov, aspect_ratio, cur_width, cur_height, cur_nc=1.0f, cur_fc=10000.0f, cctv_as,cctv_fov, cctv_nc,cctv_fc;
glm::vec3 cctv_eye, cctv_center,cctv_up;
int main_ca = 3;
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

void initialize_lights_and_material(void);
void initialize_GS();
void initialize_PS();
void prepare_shader_program(void);
void set_up_scene_lights(void);

int flag_GS = 0;

#include "Object_Definitions.h"
void tiger_route() {
	int i;
	float angle;
	glm::vec3 cur;

	//tiger_data.time = 240;
	tstop[0] = 1;
	tstop[1] = 45;
	tpos[0] = glm::vec3(25, 25, 0);
	tangle[0] = 90 * TO_RADIAN;
	
	for (i = tstop[0]; i < tstop[1]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(1, 0, 0);
		tangle[i] = 90 * TO_RADIAN;
	}
	
	for (i = tstop[1]; i < tstop[1]+ 9; i++) {
		angle = (i - tstop[1]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(2*cos(angle), 2*sin(angle), 0);
		tangle[i] = tangle[i-1]+10*TO_RADIAN;
	}

	tstop[1] += 9;
	tstop[2] = 110;

	for (i = tstop[1]; i < tstop[2]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(0, 1, 0);
		tangle[i] = 180 * TO_RADIAN;
	}

	for (i = tstop[2]; i < tstop[2] + 9; i++) {
		angle = (i - tstop[2]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(1*cos(angle), 1*sin(angle), 0);
		tangle[i] = tangle[i - 1] - 10 * TO_RADIAN;
	}

	tstop[2] += 9;
	tstop[3] = 240;
	
	for (i = tstop[2]; i < tstop[3]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(1, 0, 0);
		tangle[i] = 90 * TO_RADIAN;
	}

	for (i = tstop[3]; i < tstop[3] + 9; i++) {
		angle = (i - tstop[3]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(1*cos(angle), -1*sin(angle), 0);
		tangle[i] = tangle[i - 1] - 10 * TO_RADIAN;
	}

	tstop[3] += 9;
	tstop[4] = 285;
	for (i = tstop[3]; i < tstop[4]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(0, -1, 0);
		tangle[i] = 0 * TO_RADIAN;
	}

	for (i = tstop[4]; i < tstop[4] + 9; i++) {
		angle = (i - tstop[4]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(1*cos(angle), -1*sin(angle), 0);
		tangle[i] = tangle[i - 1] - 10 * TO_RADIAN;
	}

	tstop[4] += 9;
	tstop[5] = 320;
	
	for (i = tstop[4]; i < tstop[5]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(-1, 0, 0);
		tangle[i] = -90 * TO_RADIAN;
	}

	for (i = tstop[5]; i < tstop[5] + 9; i++) {
		angle = (i - tstop[5]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(-1*cos(angle), 1*sin(angle), 0);
		tangle[i] = tangle[i - 1] - 10 * TO_RADIAN;
	}

	tstop[5] += 9;
	tstop[6] = 330;
	
	for (i = tstop[5]; i < tstop[6]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(0, 1, 0);
		tangle[i] = 180 * TO_RADIAN;
	}

	for (i = tstop[6]; i < tstop[6] + 9; i++) {
		angle = (i - tstop[6]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(1 * cos(angle), 1 * sin(angle), 0);
		tangle[i] = tangle[i - 1] - 10 * TO_RADIAN;
	}

	tstop[6] += 9;
	tstop[7] = 355;
	for (i = tstop[6]; i < tstop[7]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(1, 0, 0);
		tangle[i] = 90 * TO_RADIAN;
	}

	for (i = tstop[7]; i < tstop[7] + 9; i++) {
		angle = (i - tstop[7]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(1 * cos(angle), 1 * sin(angle), 0);
		tangle[i] = tangle[i - 1] +10 * TO_RADIAN;
	}

	tstop[7] += 9;
	tstop[8] = 395;
	for (i = tstop[7]; i < tstop[8]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(0, 1, 0);
		tangle[i] = 180 * TO_RADIAN;
	}

	for (i = tstop[8]; i < tstop[8] + 9; i++) {
		angle = (i - tstop[8]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(-1 * cos(angle), 1 * sin(angle), 0);
		tangle[i] = tangle[i - 1] + 10 * TO_RADIAN;
	}

	tstop[8] += 9;
	tstop[9] = 530;
	for (i = tstop[8]; i < tstop[9]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(-1, 0, 0);
		tangle[i] = -90 * TO_RADIAN;
	}

	for (i = tstop[9]; i < tstop[9] + 9; i++) {
		angle = (i - tstop[9]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(-1 * cos(angle), -1 * sin(angle), 0);
		tangle[i] = tangle[i - 1] + 10 * TO_RADIAN;
	}

	tstop[9] += 9;
	tstop[10] = 605;

	for (i = tstop[9]; i < tstop[10]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(0, -1, 0);
		tangle[i] = 0 * TO_RADIAN;
	}

	for (i = tstop[10]; i < tstop[10] + 9; i++) {
		angle = (i - tstop[10]) * 10 * TO_RADIAN;
		tpos[i] = tpos[i - 1] + glm::vec3(-1 * cos(angle), -1 * sin(angle), 0);
		tangle[i] = tangle[i - 1] - 10 * TO_RADIAN;
	}

	tstop[10] += 9;
	tstop[11] = 660;

	for (i = tstop[10]; i < tstop[11]; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(-1, 0, 0);
		tangle[i] = -90 * TO_RADIAN;
	}

	for (i = tstop[11]; i < tstop[11] + 18; i++) {
		tpos[i] = tpos[i - 1] + glm::vec3(0, 0, 0);
		tangle[i] = tangle[i - 1] -10 * TO_RADIAN;
	}
}
 
void display(void) {
	float as = 300.0f / (800.0f / 3.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 tmp = glm::perspective(15.0f*TO_RADIAN, as, 1.0f, 10000.0f);;

	ModelMatrix_PATH = glm::mat4(1.0f);
	ModelMatrix_PATH = glm::translate(ModelMatrix_PATH, glm::vec3(-10, -10, 0));
	ModelMatrix_PATH = glm::rotate(ModelMatrix_PATH, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	ModelMatrix_CAR_BODY = glm::mat4(1.0f);
	ModelMatrix_CAR_BODY = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-10, -10, 16));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_BODY = glm::scale(ModelMatrix_CAR_BODY, glm::vec3(3, 3, 3));
	ModelMatrix_CAR_BODY = glm::translate(ModelMatrix_CAR_BODY, cpos[car_time]);
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, (car_angle_z() + 180 * TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	ModelMatrix_Tiger = glm::translate(glm::mat4(1.0f), tpos[tiger_data.time]);
	//ModelMatrix_Tiger = glm::translate(ModelMatrix_Tiger, glm::vec3(0, 50, 20));
	ModelMatrix_Tiger = glm::rotate(ModelMatrix_Tiger, tangle[tiger_data.time], glm::vec3(0, 0, 1));
	ModelMatrix_Tiger = glm::rotate(ModelMatrix_Tiger, 100*TO_RADIAN, glm::vec3(1, 0, 0));
	ModelMatrix_Tiger = glm::rotate(ModelMatrix_Tiger, 180* TO_RADIAN, glm::vec3(0, 1, 0));
	ModelMatrix_Tiger *= tiger[tiger_data.cur_frame].ModelMatrix[0];

	//cctv0
	glViewport(10, 0, cur_width / 4-10, cur_height / 3);
	glLoadIdentity();
	cctv_ProjectionMatrix[0] = tmp;
	cctv_ViewMatrix[0] = glm::lookAt(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f),glm::vec3(-10.0f, 0.0f, 0.0f));
	draw_cctv(0);
	draw_VV(glm::vec3(150.0f, 150.0f, 80.0f), glm::vec3(30, 80.0f, 25.0f), cctv_fov,cctv_as,cctv_nc,cctv_fc,0);
	
	//cctv1
	glViewport(10, cur_height / 3, cur_width / 4-10, cur_height / 3);
	glLoadIdentity();
	cctv_ProjectionMatrix[1] = tmp;
	cctv_ViewMatrix[1] = glm::lookAt(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	draw_cctv(1);
	draw_VV(glm::vec3(150.0f, 150.0f, 80.0f), glm::vec3(30, 80.0f, 25.0f), cctv_fov, cctv_as, cctv_nc, cctv_fc, 1);

	//cctv2
	glViewport(10, 2*cur_height/3, cur_width / 4-10, cur_height / 3);
	glLoadIdentity();
	cctv_ProjectionMatrix[2] = tmp;
	cctv_ViewMatrix[2] = glm::lookAt(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),glm::vec3(0.0f, 0.0f, 1.0f));
	draw_cctv(2);
	draw_VV(glm::vec3(150.0f, 150.0f, 80.0f), glm::vec3(30, 80.0f, 25.0f), cctv_fov, cctv_as, cctv_nc, cctv_fc, 2);

	//cctv4
	glViewport(cur_width/4+10, 2 * cur_height / 3, cur_width / 4 , cur_height/3);
	glLoadIdentity();
	cctv_ProjectionMatrix[4] = cctvProjection;
	cctv_ViewMatrix[4] = cctvMatrix;
	draw_cctv(4);
	draw_VV(glm::vec3(150.0f, 150.0f, 80.0f), glm::vec3(30, 80.0f, 25.0f), cctv_fov, cctv_as, cctv_nc, cctv_fc, 4);

	//cctv5
	glViewport( 2*cur_width / 4 + 10, 2 * cur_height / 3  , cur_width / 4, cur_height / 3);
	glLoadIdentity();
	cctv_ProjectionMatrix[5] = mouse_ProjectionMatrix;
	cctv_ViewMatrix[5] = mouse_ViewMatrix;
	draw_cctv(5);
	draw_VV(glm::vec3(150.0f, 150.0f, 80.0f), glm::vec3(30, 80.0f, 25.0f), cctv_fov, cctv_as, cctv_nc, cctv_fc, 4);


	//cctv3
	glViewport(cur_width / 4+10, 0, 3 * cur_width / 4-10, 2*cur_height/3);
	glLoadIdentity();
	cctv_ProjectionMatrix[3] = ProjectionMatrix;
	
	if (main_ca == 3) {
		cctv_ViewMatrix[3] = ViewMatrix;
	}

	//버스시점
	else if (main_ca == 4) {
		cctv_ViewMatrix[3] = glm::affineInverse(ModelMatrix_CAR_BODY*ModelMatrix_CAR_BODY_to_DRIVER);
		cctv_ProjectionMatrix[3] = Nor_ProjectionMatrix;
	}
	
	//호랑이 시점
	else if (main_ca == 5) {
		glm::mat4 tiger_tmp= glm::translate(glm::mat4(1.0f), glm::vec3(0, -20, -50));
		//tiger_tmp = glm::rotate(tiger_tmp, 180 * TO_RADIAN, glm::vec3(0, 0, 1));
		cctv_ViewMatrix[3] = tiger_tmp*glm::affineInverse(ModelMatrix_Tiger);
		cctv_ProjectionMatrix[3] = glm::perspective(50*TO_RADIAN,cctv_as, 10.0f, 500.0f);
	}
	else if (main_ca == 6) {

		cctv_ViewMatrix[3] = glm::affineInverse(ModelMatrix_Tiger);
		//cur_fov = 60;
		cctv_ProjectionMatrix[3] = glm::perspective(60 * TO_RADIAN, cctv_as, cctv_nc, cctv_fc);
	}

	draw_cctv(3);
	draw_VV(glm::vec3(150.0f, 150.0f, 80.0f), glm::vec3(30, 80.0f, 25.0f), cctv_fov, cctv_as, cctv_nc, cctv_fc, 3);

	glutSwapBuffers();
}
void set_ViewMat(glm::vec3 eye, glm::vec3 center, glm::vec3 up, int move, int axis_num) {
	//printf("cctv %d , %f,%f %f  / %f %f %f/ %f %f %f\n", move, eye.x, eye.y, eye.z, center.x, center.y, center.z);
	if (move == 4 || move == -4) {
		cctv_eye = eye;
		cctv_center = center;
		cctv_up = up;
		axis[0] = glm::vec3(cctvMatrix[0][0], cctvMatrix[1][0], cctvMatrix[2][0]);
		axis[1] = glm::vec3(cctvMatrix[0][1], cctvMatrix[1][1], cctvMatrix[2][1]);
		axis[2] = glm::vec3(cctvMatrix[0][2], cctvMatrix[1][2], cctvMatrix[2][2]);
	}
	else {
		cur_eye = eye;
		cur_center = center;
		cur_up = up;
		axis[0] = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		axis[1] = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		axis[2] = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
	}

	if (!move) {
		ViewMatrix = glm::lookAt(cur_eye, cur_center, cur_up);
		return;
	}

	//카메라 회전 (카메라 고정 x)
	else if (move == 1) {
		cur_eye += axis[axis_num]+ axis[axis_num];
	}
	else if (move == -1) {
		cur_eye -= axis[axis_num]+ axis[axis_num];
	}

	//카메라 이동
	else if (move == 2) {
		cur_eye += axis[axis_num]+ axis[axis_num];
		cur_center += axis[axis_num]+ axis[axis_num];
	}
	else if (move == -2) {
		cur_eye -= axis[axis_num]+ axis[axis_num];
		cur_center -= axis[axis_num]+ axis[axis_num];
	}

	//카메라 회전 (카메라 고정)
	else if (move == 3) {
		cur_center += axis[axis_num] + axis[axis_num];
	}

	else if (move == -3) {
		cur_center -= axis[axis_num] + axis[axis_num];
	}

	else if (move == 4)
		cctv_center += axis[axis_num] + axis[axis_num];

	else if(move==-4)
		cctv_center -= axis[axis_num] + axis[axis_num];

	if (move != 4 && move!=-4)
		ViewMatrix = glm::lookAt(cur_eye, cur_center, cur_up);
	
	else 
		cctvMatrix = glm::lookAt(cctv_eye, cctv_center, cctv_up);

	return;
}
void set_ProjMat(int width, int height, int op, float nc, float fc) {
	aspect_ratio = (float)width / height;
	cur_fov += op;
	cur_nc = nc;
	cur_fc = fc;
	ProjectionMatrix = glm::perspective((cur_fov)*TO_RADIAN, aspect_ratio, 1.0f, 10000.0f);
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0, cctv_on=0, camera3=0;
	glm::vec3 axis;
	char string[256];

	if ((key >= '0') && (key <= '0' + NUMBER_OF_LIGHT_SUPPORTED - 1)) {
		int light_ID = (int)(key - '0');

		glUseProgram(h_ShaderProgram_CUR);
		light[light_ID].light_on = 1 - light[light_ID].light_on;
		glUniform1i(loc_light[light_ID].light_on, light[light_ID].light_on);
		glUseProgram(0);

		glutPostRedisplay();
		return;
	}

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		break;
	case 'f':
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'd':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;

		//카메라 변환
	case '!':
		camera3 = 0;
		main_ca = 3;
		//cctv_on = 0;
		cur_fov = 15.0f;;
		set_ProjMat(cur_width, cur_height, -1, 1.0f, 10000.0f);
		set_ViewMat(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f), 0, 0);
		break;
	case '@':
		camera3 = 0;
		main_ca = 3;
		//cctv_on = 0;
		cur_fov = 15.0f;;
		set_ProjMat(cur_width, cur_height, -1, 1.0f, 10000.0f);
		set_ViewMat(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f), 0, 0);
		break;

	case '#':
		camera3 = 0;
		main_ca = 3;
		cur_fov = 15.0f;
		set_ViewMat(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f), glm::vec3(-10.0f, 0.0f, 0.0f),0,0);
		set_ProjMat(cur_width, cur_height, -1, 1.0f, 10000.0f);
		break;

	//호랑이 시점
	case '$':
		camera3 = 0;
		main_ca = 5;
		//cctv_on = 0;
		break;

	//측면도
	case '%':
		camera3 = 0;
		main_ca = 3;
		//cctv_on = 0;
		cur_fov = 15.0f;;
		ViewMatrix = glm::lookAt(glm::vec3(125, 95, 1505), glm::vec3(125, 95, 0), glm::vec3(-1, 0, 0));
		ProjectionMatrix = glm::ortho(-100.0f*win_aspect_ratio, win_aspect_ratio*100.0f, -200.0f, 200.0f, 0.1f, 1500.0f);
		break;

		//정면도
	case '^':
		camera3 = 0;
		main_ca = 3;
		//cctv_on = 0;
		cur_fov = 15.0f;;
		ViewMatrix = glm::lookAt(glm::vec3(1000, 55, 0), glm::vec3(0, 55, 0), glm::vec3(0, 0, 1));
		ProjectionMatrix = glm::ortho(-80.0f*win_aspect_ratio, win_aspect_ratio*80.0f, -50.0f, 50.0f, 0.1f, 1000.0f);
		break;
	
	//측면도
	case '&':
		camera3 = 0;
		main_ca = 3;
		//cctv_on = 0;
		cur_fov = 15.0f;;
		ViewMatrix = glm::lookAt(glm::vec3(105, 1505, 0), glm::vec3(105, 0, 0), glm::vec3(0, 0, 1));
		ProjectionMatrix = glm::ortho(-100.0f*win_aspect_ratio, win_aspect_ratio*100.0f, -40.0f, 50.0f, 0.1f, 2000.0f);
		break;

	//버스 시점
	case '*':
		camera3 = 0;
		//cctv_on = 0;
		main_ca = 4;
		break;

	//주 카메라에서 ( 주카메라가 1,2,3인 경우)
	//확대
	case 'b':
		if (cur_fov > 0)
			set_ProjMat(cur_width, cur_height, -1, 1.0f, 10000.0f);
		break;

	case 's':
		if (!cctv_on && cur_fov < 50)
			set_ProjMat(cur_width, cur_height, 1, 1.0f, 10000.0f);

		break;

		//동적 cctv에서 
	case 'B':
		if (cctv_fov > 0) {
			cctv_fov -= 1;
			cctvProjection = glm::perspective(cctv_fov*TO_RADIAN, cctv_as, cctv_nc, cctv_fc);
		}
		break;

	case 'S':
		if (cctv_fov < 100) {
			cctv_fov += 1;
			cctvProjection = glm::perspective(cctv_fov*TO_RADIAN, cctv_as, cctv_nc, cctv_fc);
		}
		break;

	case '>':
		if (cctv_fc < 500) {
			cctv_fc += 1;
			cctvProjection = glm::perspective(cctv_fov*TO_RADIAN, cctv_as, cctv_nc, cctv_fc);
		}
		break;
	case '<':
		if (cctv_fc > 50) {
			cctv_fc -= 1;
			cctvProjection = glm::perspective(cctv_fov*TO_RADIAN, cctv_as, cctv_nc, cctv_fc);
		}
		break;

		//주 카메라
		//카메라 고정, 오른쪽 방향으로 회전 
	case'h':
		set_ViewMat(cur_eye, cur_center, cur_up, 3, 0);
		break;

	case 'j':
		set_ViewMat(cur_eye, cur_center, cur_up, -3, 0);
		break;

	case'k':
		set_ViewMat(cur_eye, cur_center, cur_up, 3, 1);
		break;
	case 'l':
		set_ViewMat(cur_eye, cur_center, cur_up, -3, 1);
		break;

		//cctv
		//카메라 고정, 오른쪽 방향으로 회전 
	case'H':
		if (!camera3)
			set_ViewMat(cctv_eye, cctv_center, cctv_up, 4, 0);
		break;

	case 'J':
		if (!camera3)
			set_ViewMat(cctv_eye, cctv_center, cctv_up, -4, 0);
		break;

	case'K':
		if (!camera3)
			set_ViewMat(cctv_eye, cctv_center, cctv_up, 4, 1);
		break;

	case 'L':
		if (!camera3)
			set_ViewMat(cctv_eye, cctv_center, cctv_up, -4, 1);
		break;

		//이동
	case 'x':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, 2, 0);
		break;

	case 'y':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, 2, 1);
		break;

	case 'z':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, 2, 2);
		break;

	case 'X':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, -2, 0);
		break;

	case 'Y':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, -2, 1);
		break;

	case 'Z':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, -2, 2);
		break;

		//축으로 회전
	case 'u':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, 1, 0);
		break;

	case 'v':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, 1, 1);
		break;

	case 'n':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, 1, 2);
		break;

	case 'U':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, -1, 0);
		break;

	case 'V':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, -1, 1);
		break;

	case 'N':
		if (!cctv_on)
			set_ViewMat(cur_eye, cur_center, cur_up, -1, 2);
		break;

	//screen
	case 'q':
		flag_draw_screen = 1 - flag_draw_screen;
		glutPostRedisplay();
		break;
	//screen effect
	case 'Q':
		if (flag_draw_screen) {
			flag_screen_effect = 1 - flag_screen_effect;
			glutPostRedisplay();
		}
		break;
	
	//screen width ++
	case 'w':
		//printf("screen width %f height %f \n", width_num, height_num);
		if (flag_draw_screen) {
			if (screen_width_num > 20)
				screen_width_num = 0;
			screen_width_num= screen_width_num+1.0f;
		}
		break;

	//screen height++
	case 'W':
		//printf("screen width %f height %f \n", width_num, height_num);
		if (flag_draw_screen) {
			if (screen_height_num > 20)
				screen_height_num = 0;
			screen_height_num=screen_height_num+1.0f;
		}
		break;

	//blind
	case 'e':
		flag_blind_effect = 1 - flag_blind_effect;
		glUseProgram(h_ShaderProgram_CUR);
		glUniform1i(loc_blind_effect, flag_blind_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;

	case 'E':
		if (flag_blind_effect) {
			if (blind_width < -180)
				blind_width = 90;
			blind_width -= 5;
			glUseProgram(h_ShaderProgram_CUR);
			glUniform1i(loc_blind_effect, flag_blind_effect);
			glUniform1f(loc_blind_width, blind_width);
			glUseProgram(0);
			glutPostRedisplay();
		}
		break;

	case 'r':
		flag_cartoon_effect = 1 - flag_cartoon_effect;
		glUseProgram(h_ShaderProgram_CUR);
		glUniform1i(loc_cartoon_effect, flag_cartoon_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 't':
		if (flag_cartoon_effect) {
			if (cartoon_levels >= 1.0f) {
				cartoon_levels -= 1.0f;
				glUseProgram(h_ShaderProgram_CUR);
				glUniform1i(loc_cartoon_effect, flag_cartoon_effect);
				glUniform1i(loc_cartoon_levels, cartoon_levels);
				glUseProgram(0);
				glutPostRedisplay();
			}
		}
		break;
	case 'T':
		if (flag_cartoon_effect) {
			if (cartoon_levels <= 20.0f) {
				cartoon_levels += 1.0f;
				glUseProgram(h_ShaderProgram_CUR);
				glUniform1i(loc_cartoon_effect, flag_cartoon_effect);
				glUniform1i(loc_cartoon_levels, cartoon_levels);
				glUseProgram(0);
				glutPostRedisplay();
			}
		}
		break;
	case '+':
		flag_ina_light_effect = 1 - flag_ina_light_effect;
		glUseProgram(h_ShaderProgram_CUR);
		glUniform1i(loc_ina_timer, ina_timer);
		glUniform1i(loc_ina_light_effect, flag_ina_light_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;

	}
}


int left_bt = 0;
int right_bt = 0;
int shift = 0;
void mouse(int button, int state, int x, int y) {

	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		left_bt = 1;
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		left_bt = 0;

	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		right_bt = 1;
	}

	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)) {
		right_bt = 0;
	}

	if (shift && right_bt) {
		flag_GS = 1;
		initialize_GS();
	}
	if (flag_GS && !right_bt) {
		flag_GS = 0;
		shift = 0;
		initialize_PS();
	}
}

void special(int key, int x, int y) {
#define SENSITIVITY 2.0
	
	if (right_bt && (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)) {
		flag_GS = 1;
		initialize_GS();
		return;
	}

	if ((key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R))
		shift = 1;
	else
		shift = 0;

	switch (key) {
	case GLUT_KEY_LEFT:
		main_light_pos.y -= 5;
		light[5].position[1] = main_light_pos.y;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		main_light_pos.y += 5;
		light[5].position[1] = main_light_pos.y;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		main_light_pos.x += 5;
		light[5].position[0] = main_light_pos.x;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		main_light_pos.x -= 5;
		light[5].position[0] = main_light_pos.x;
		glutPostRedisplay();
		break;
	case GLUT_KEY_CTRL_L:
	
		flag_ina_effect = 1 - flag_ina_effect;
		glUseProgram(h_ShaderProgram_CUR);
		glUniform1i(loc_ina_effect, flag_ina_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	
	}
}

void motion(int x, int y) {
	static int delay = 0;
	static float tmpx = 0.0, tmpy = 0.0;
	float dx, dy,ina_x,ina_y;
	if (left_bt) {
/*
		ina_x = x - cur_width / 2.0f, ina_y = (cur_height - y) - cur_height / 2.0f;
		dx = ina_x - tmpx;
		dy = ina_y - tmpy;
		tmpx = ina_x, tmpy = ina_y;

		set_ViewMat(glm::vec3(x, y, 100), cur_center, glm::vec3(0, 0, 1), 0, 0);
		cur_center = cur_center+ glm::vec3(dx, dy, 0);
		//ViewMatrix = glm::lookAt(glm::vec3(x, y, 100), cur_center, glm::vec3(0, 0, 1));
		//mouse_ProjectionMatrix = glm::perspective(60, 1, 10, 300);
	*/
	}

	if (right_bt) {
	}
}

void reshape(int width, int height) {
	cur_width = width;
	cur_height = height;

	aspect_ratio = (float)width / height;
	cur_fov = 15.0f;
	win_aspect_ratio = aspect_ratio;

	ProjectionMatrix = glm::perspective(15.0f*TO_RADIAN, aspect_ratio, 1.0f, 10000.0f);
	cctvProjection = glm::perspective(40.0f*TO_RADIAN, aspect_ratio, 1.0f, 200.0f);
	Nor_ProjectionMatrix = ProjectionMatrix;
	cctv_fov = 40.0f;
	cctv_as = aspect_ratio;
	cctv_nc = 1.0f;
	cctv_fc = 200.0f;
	glutPostRedisplay();

}

void timer_scene(int timestamp_scene) {
	tiger_data.time = (tiger_data.time + 1) % 678;
	tiger_data.cur_frame = timestamp_scene % N_TIGER_FRAMES;
	//tiger_data.rotation_angle = 90 * TO_RADIAN; //(timestamp_scene % 360)*TO_RADIAN;
	light_timer = (light_timer + 1) % 360;
	cctv_tiger_data.cur_frame = timestamp_scene % N_TIGER_FRAMES;
	cctv_tiger_data.rotation_angle = 90 * TO_RADIAN;

	car_time = (car_time + 2) % 751;
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup_OpenGL_stuffs);
}

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
	{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_PS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/Phong.frag" },
	{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_GS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Gouraud.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" },
	{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_PS = LoadShaders(shader_info_PS);
	loc_ModelViewProjectionMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrixInvTrans");

	h_ShaderProgram_GS = LoadShaders(shader_info_GS);
	loc_ModelViewProjectionMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrixInvTrans");
	
	loc_screen_effect = glGetUniformLocation(h_ShaderProgram_PS, "screen_effect");
	loc_screen_width = glGetUniformLocation(h_ShaderProgram_PS, "screen_width");
	loc_width_num = glGetUniformLocation(h_ShaderProgram_PS, "screen_width_num");
	loc_height_num = glGetUniformLocation(h_ShaderProgram_PS, "screen_height_num");

	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_PS, "blind_effect");
	loc_blind_width = glGetUniformLocation(h_ShaderProgram_PS, "blind_width");

	loc_ina_effect = glGetUniformLocation(h_ShaderProgram_PS, "ina_effect");
	loc_ina_light_effect = glGetUniformLocation(h_ShaderProgram_PS, "ina_light_effect")	;
	loc_ina_timer = glGetUniformLocation(h_ShaderProgram_PS, "ina_timer");

	loc_cartoon_effect = glGetUniformLocation(h_ShaderProgram_PS, "cartoon_effect");
	loc_cartoon_levels = glGetUniformLocation(h_ShaderProgram_PS, "cartoon_levels");

	initialize_PS();
}

void initialize_PS() {
	int i;
	char string[256];

	loc_ModelViewProjectionMatrix_cur = loc_ModelViewProjectionMatrix_PS;
	loc_ModelViewMatrix_cur = loc_ModelViewMatrix_PS;
	loc_ModelViewMatrixInvTrans_cur = loc_ModelViewMatrixInvTrans_PS;

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_PS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_exponent");

	h_ShaderProgram_CUR = h_ShaderProgram_PS;
}
void initialize_GS() {
	int i;
	char string[256];
	h_ShaderProgram_CUR = h_ShaderProgram_GS;
	loc_ModelViewProjectionMatrix_cur = loc_ModelViewProjectionMatrix_GS;
	loc_ModelViewMatrix_cur = loc_ModelViewMatrix_GS;
	loc_ModelViewMatrixInvTrans_cur = loc_ModelViewMatrixInvTrans_GS;

	//set_up_scene_lights();
	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_global_ambient_color");

	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_GS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_exponent");

}
void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_CUR);

	glUniform4f(loc_global_ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	glUniform1i(loc_screen_effect, 0);
	glUniform1f(loc_screen_width, 0.125f);

	glUniform1i(loc_blind_effect, 0);
	glUniform1f(loc_blind_width, 90.0f);

	glUniform1i(loc_ina_effect, 0);
	glUniform1i(loc_ina_light_effect, 0);
	glUniform1i(loc_ina_timer, 0);

	glUniform1i(loc_cartoon_effect, 0);
	glUniform1f(loc_cartoon_levels, 3.0f);
	glUseProgram(0);
}

void set_up_scene_lights(void) {

	// point_light_EC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 10.0f; 	// point light position in EC
	light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.3f; light[0].ambient_color[1] = 0.3f;
	light[0].ambient_color[2] = 0.3f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.3f; light[0].diffuse_color[1] = 0.3f;
	light[0].diffuse_color[2] = 0.3f; light[0].diffuse_color[3] = 1.0f;

	light[0].specular_color[0] = 0.4f; light[0].specular_color[1] = 0.4f;
	light[0].specular_color[2] = 0.4f; light[0].specular_color[3] = 1.0f;

	// spot_light_WC: use light 1
	light[1].light_on = 1;
	light[1].position[0] = 80.0f;
	light[1].position[1] = 45.0f;
	light[1].position[2] = 50.0f;
	light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.2f;
	light[1].ambient_color[1] = 0.2f;
	light[1].ambient_color[2] = 0.2f;
	light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.82f;
	light[1].diffuse_color[1] = 0.82f;
	light[1].diffuse_color[2] = 0.82f; 
	light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.82f;
	light[1].specular_color[1] = 0.82f;
	light[1].specular_color[2] = 0.82f;
	light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f;
	light[1].spot_direction[1] = 0.0f;
	light[1].spot_direction[2] = -1.f;

	light[1].spot_cutoff_angle = 20.0f;
	light[1].spot_exponent = 50.0f;

	//light2
	light[2].light_on = 1;
	light[2].position[0] = 0.0f; light[2].position[1] = 0.0f;
	light[2].position[2] = 150.0f; light[2].position[3] = 1.0f;

	light[2].ambient_color[0] = 0.2f; light[2].ambient_color[1] = 0.2f;
	light[2].ambient_color[2] = 0.2f; light[2].ambient_color[3] = 1.0f;

	light[2].diffuse_color[0] = 0.82f; light[2].diffuse_color[1] = 0.82f;
	light[2].diffuse_color[2] = 0.82f; light[2].diffuse_color[3] = 1.0f;

	light[2].specular_color[0] = 0.82f; light[2].specular_color[1] = 0.82f;
	light[2].specular_color[2] = 0.82f; light[2].specular_color[3] = 1.0f;

	light[2].spot_direction[0] = 0.0f; light[2].spot_direction[1] = 0.0f; // spot light direction in MC for tiger
	light[2].spot_direction[2] = -1.0f;
	light[2].spot_cutoff_angle = 50.0f;
	light[2].spot_exponent = 27.0f;
	
	//light3
	light[3].light_on = 1;
	light[3].position[0] = 34.0f; 
	light[3].position[1] = 133.0f;
	light[3].position[2] = 50.0f; 
	light[3].position[3] = 1.0f;

	light[3].ambient_color[0] = 0.2f; 
	light[3].ambient_color[1] = 0.2f;
	light[3].ambient_color[2] = 0.2f; 
	light[3].ambient_color[3] = 1.0f;

	light[3].diffuse_color[0] = 0.82f; 
	light[3].diffuse_color[1] = 0.82f;
	light[3].diffuse_color[2] = 0.82f; 
	light[3].diffuse_color[3] = 1.0f;

	light[3].specular_color[0] = 0.82f; 
	light[3].specular_color[1] = 0.82f;
	light[3].specular_color[2] = 0.82f; 
	light[3].specular_color[3] = 1.0f;

	light[3].spot_direction[0] = 0.0f; 
	light[3].spot_direction[1] = 0.0f; 
	light[3].spot_direction[2] = -1.0f;

	light[3].spot_cutoff_angle = 50.0f;
	light[3].spot_exponent = 50.0f;

	//light4
	light[4].light_on = 1;
	light[4].position[0] = 200.0f; 
	light[4].position[1] = 50.0f;
	light[4].position[2] = 50.0f; 
	light[4].position[3] = 1.0f;

	light[4].ambient_color[0] = 0.2f; 
	light[4].ambient_color[1] = 0.2f;
	light[4].ambient_color[2] = 0.2f; 
	light[4].ambient_color[3] = 1.0f;

	light[4].diffuse_color[0] = 0.82f; 
	light[4].diffuse_color[1] = 0.82f;
	light[4].diffuse_color[2] = 0.82f; 
	light[4].diffuse_color[3] = 1.0f;

	light[4].specular_color[0] = 0.82f; 
	light[4].specular_color[1] = 0.82f;
	light[4].specular_color[2] = 0.82f; 
	light[4].specular_color[3] = 1.0f;

	light[4].spot_direction[0] = 0.0f; 
	light[4].spot_direction[1] = 0.0f;
	light[4].spot_direction[2] = -1.0f;

	light[4].spot_cutoff_angle =20.0f;
	light[4].spot_exponent = 50.0f;

	//light 5
	light[5].light_on = 1;
	light[5].position[0] = main_light_pos.x;
	light[5].position[1] = main_light_pos.y;
	light[5].position[2] = main_light_pos.z;
	light[5].position[3] = 1.0f;

	light[5].ambient_color[0] = 0.2f;
	light[5].ambient_color[1] = 0.2f;
	light[5].ambient_color[2] = 0.2f;
	light[5].ambient_color[3] = 1.0f;

	light[5].diffuse_color[0] = 0.82f;
	light[5].diffuse_color[1] = 0.82f;
	light[5].diffuse_color[2] = 0.82f;
	light[5].diffuse_color[3] = 1.0f;

	light[5].specular_color[0] = 0.82f;
	light[5].specular_color[1] = 0.82f;
	light[5].specular_color[2] = 0.82f;
	light[5].specular_color[3] = 1.0f;

	light[5].spot_direction[0] = 0.0f;
	light[5].spot_direction[1] = 0.0f;
	light[5].spot_direction[2] = -1.0f;

	light[5].spot_cutoff_angle = 60.0f;
	light[5].spot_exponent = 40.0f;

	//light 6 (line light)
	light[6].light_on = 1;
	light[6].position[0] = 70.0f;
	light[6].position[1] = 200.0f;
	light[6].position[2] = 300.0f;
	light[6].position[3] = 0.0f;

	light[6].ambient_color[0] = 0.2f;
	light[6].ambient_color[1] = 0.2f;
	light[6].ambient_color[2] = 0.2f;
	light[6].ambient_color[3] = 1.0f;

	light[6].diffuse_color[0] = 0.52f;
	light[6].diffuse_color[1] = 0.52f;
	light[6].diffuse_color[2] = 0.52f;
	light[6].diffuse_color[3] = 1.0f;

	light[6].specular_color[0] = 0.52f;
	light[6].specular_color[1] = 0.52f;
	light[6].specular_color[2] = 0.52f;
	light[6].specular_color[3] = 1.0f;

	light[6].spot_direction[0] = 0.0f;
	light[6].spot_direction[1] = 0.0f;
	light[6].spot_direction[2] = -1.0f;

	light[6].spot_cutoff_angle = 20.0f;
	light[6].spot_exponent = 70.0f;

}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);

	if (0) {
		set_ViewMat(glm::vec3(120.0f, 90.0f, 1000.0f), glm::vec3(120.0f, 90.0f, 0.0f),
			glm::vec3(-10.0f, 0.0f, 0.0f),0,0);
	}
	if (0) {
		set_ViewMat(glm::vec3(800.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),0,0);
	}

	if (1) {
		set_ViewMat(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),0,0);
		Nor_ViewMatrix = ViewMatrix;
	
		cctv_eye = glm::vec3(150, 150, 80);
		cctv_center = glm::vec3(30, 80, 25);
		cctv_up = glm::vec3(0, 0, 1);
		cctvMatrix = glm::lookAt(glm::vec3(150.0f, 150.0f, 80.0f), glm::vec3(30, 80.0f, 25.0f), glm::vec3(0, 0, 1));

		mouse_ViewMatrix = glm::lookAt(glm::vec3(100, 150, 80), glm::vec3(30, 80, 20), glm::vec3(0, 0, 1));
		mouse_ProjectionMatrix = glm::perspective(60, 1, 100, 1000);
	}

	initialize_lights_and_material();
}

void prepare_scene(void) {
	
	define_axes();
	prepare_path();
	
	define_static_objects();
	set_up_scene_lights();
	
	prepare_rectangle();
	prepare_tiger();
	
	initialize_ina();
	initialize_ina_light();
	initialize_blind();
	initialize_cartoon();
	initialize_screen();

	prepare_geom_obj(GEOM_OBJ_ID_CAR_BODY, (char*)"Data/car_body_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_WHEEL, (char*)"Data/car_wheel_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_NUT, (char*)"Data/car_nut_triangles_v.txt", GEOM_OBJ_TYPE_V);
	
	ModelMatrix_CAR_BODY_to_DRIVER = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_BODY_to_DRIVER = glm::rotate(ModelMatrix_CAR_BODY_to_DRIVER,
		TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	tiger_route();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) { 
	char program_name[256] = "Sogang CSE4170 Our_House_GLSL_V_0.5";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: 'c', 'f', 'd', 'ESC', 'x,y,z', 'X,Y,Z', 'u,v,n', 'U,V,N', 'h,j,k,l', 'b,s', '1,2,3,4' " };
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
