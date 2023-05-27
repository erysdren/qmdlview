
//
//
// headers
//
//

// std
#include <iostream>
#include <cstdlib>

// gl
#include <GL/glut.h>
#include <GL/glui.h>

// tinyfd
#include "tinyfiledialogs.h"

// mdl
#include "mdl.h"
#include "palette.h"

//
//
// macros
//
//

#define FOV 90
#define SPEED 2

//
//
// types
//
//

typedef struct
{
	float x, y, z;
} vec3_t;

//
//
// globals
//
//

int glut_window_main;
GLUI *glui_subwindow_right;
float vaspect;
float haspect;
const char *mdl_patterns[2] = {"*.mdl", "*.MDL"};
const char *mdl_filename = NULL;
mdl_t *mdl;
GLint *gl_models;
GLuint gl_texture;
void *gl_texture_pixels;
int gl_frame_num = 0;

struct
{
	vec3_t position;
	vec3_t rotation;
	vec3_t look;
	vec3_t strafe;
	vec3_t velocity;
	vec3_t speedkey;
} camera;

//
//
// functions
//
//

//
// glut_motion_func
//

void glut_motion_func(int x, int y)
{
	glutPostRedisplay(); 
}

//
// glut_keyboard_func
//

void glut_keyboard_func(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			camera.position.x += camera.look.x * SPEED * camera.speedkey.x;
			camera.position.y += camera.look.y * SPEED * camera.speedkey.y;
			camera.position.z += camera.look.z * SPEED * camera.speedkey.z;
			break;
		
		case 's':
			camera.position.x -= camera.look.x * SPEED * camera.speedkey.x;
			camera.position.y -= camera.look.y * SPEED * camera.speedkey.y;
			camera.position.z -= camera.look.z * SPEED * camera.speedkey.z;
			break;

		case 'a':
			camera.position.x += camera.strafe.x * SPEED * camera.speedkey.x;
			camera.position.z += camera.strafe.z * SPEED * camera.speedkey.z;
			break;

		case 'd':
			camera.position.x -= camera.strafe.x * SPEED * camera.speedkey.x;
			camera.position.z -= camera.strafe.z * SPEED * camera.speedkey.z;
			break;
	}

	glutPostRedisplay();
}

//
// glut_display_func
//

void glut_display_func()
{
	// clear screen
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// save matrix
	glPushMatrix();

	// draw model
	glRotatef(-90, 1, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(gl_models[gl_frame_num]);

	// restore matrix
	glPopMatrix();

	// swap buffers
	glutSwapBuffers(); 
}

//
// glut_reshape_func
//

void glut_reshape_func(int x, int y)
{
	int tx, ty, tw, th;

	// set viewport
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);

	// set aspects
	vaspect = (float)y / (float)x;
	haspect = (float)x / (float)y;

	// set perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV * vaspect, haspect, 1.0f, 1024.0f);

	// set camera view
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.position.x + camera.look.x, camera.position.y + camera.look.y, camera.position.z + camera.look.z, 0.0f, 1.0f, 0.0);

	glutPostRedisplay();
}

//
// glut_exit_func
//

void glut_exit_func(int status)
{
	if (mdl) free(mdl);
	if (gl_models) free(gl_models);
	if (gl_texture_pixels) free(gl_texture_pixels);
	exit(status);
}

//
// qmdlview_open_mdl
//

void qmdlview_open_mdl(const char *filename)
{
	// prompt
	if (filename)
	{
		mdl_filename = filename;
	}
	else
	{
		mdl_filename = tinyfd_openFileDialog("Choose an MDL File", "", 2, mdl_patterns, "Quake MDL Files", 0);
		if (mdl_filename == NULL)
		{
			std::cerr << "No file selected" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// open
	mdl = MDL_Load(mdl_filename);
	if (mdl == NULL)
	{
		std::cerr << "Failed to process " << mdl_filename << std::endl;
		exit(EXIT_FAILURE);
	}

	//
	// prepare for opengl
	//

	// allocate model list
	gl_models = (GLint *)malloc(sizeof(GLint) * mdl->header->num_frames);
	if (gl_models == NULL)
	{
		std::cerr << "Failed malloc" << std::endl;
		exit(EXIT_FAILURE);
	}

	// allocate texture
	gl_texture_pixels = malloc(mdl->header->skin_width * mdl->header->skin_height * 3);
	for (int i = 0; i < mdl->header->skin_width * mdl->header->skin_height; i++)
	{
		((uint8_t *)gl_texture_pixels)[i * 3] = palette[((uint8_t *)mdl->skins[0].skin_pixels)[i] * 3];
		((uint8_t *)gl_texture_pixels)[(i * 3) + 1] = palette[(((uint8_t *)mdl->skins[0].skin_pixels)[i] * 3) + 1];
		((uint8_t *)gl_texture_pixels)[(i * 3) + 2] = palette[(((uint8_t *)mdl->skins[0].skin_pixels)[i] * 3) + 2];
	}

	// generate texture
	glGenTextures(1, &gl_texture);
	glBindTexture(GL_TEXTURE_2D, gl_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, mdl->header->skin_width, mdl->header->skin_height, 0, GL_RGB, GL_UNSIGNED_BYTE, gl_texture_pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// create lists
	/* create lists */
	for (int i = 0; i < mdl->header->num_frames; i++)
	{
		/* new list */
		gl_models[i] = glGenLists(1);
		glNewList(gl_models[i], GL_COMPILE);

		/* begin triangles */
		glBindTexture(GL_TEXTURE_2D, gl_texture);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLES);

		/* do vertices */
		for (int f = 0; f < mdl->header->num_faces; f++)
		{
			for (int v = 0; v < 3; v++)
			{
				GLfloat x, y, z, s, t;

				x = (mdl->frames[i].vertices[mdl->faces[f].vertex_indicies[v]].coordinates[0] * mdl->header->scale[0]) + mdl->header->translation[0];
				y = (mdl->frames[i].vertices[mdl->faces[f].vertex_indicies[v]].coordinates[1] * mdl->header->scale[1]) + mdl->header->translation[1];
				z = (mdl->frames[i].vertices[mdl->faces[f].vertex_indicies[v]].coordinates[2] * mdl->header->scale[2]) + mdl->header->translation[2];

				s = (float)mdl->texcoords[mdl->faces[f].vertex_indicies[v]].s;
				t = (float)mdl->texcoords[mdl->faces[f].vertex_indicies[v]].t;

				/* handle onseam */
				if (mdl->faces[f].face_type == 0 && mdl->texcoords[mdl->faces[f].vertex_indicies[v]].onseam)
					s += (float)mdl->header->skin_width / 2.0f;

				s /= (float)mdl->header->skin_width;
				t /= (float)mdl->header->skin_height;

				glTexCoord2f(s, t);
				glVertex3f(x, y, z);
			}
		}

		/* end triangles */
		glEnd();
		glDisable(GL_TEXTURE_2D);

		/* end list */
		glEndList();
	}
}

//
// main
//

int main(int argc, char *argv[])
{
	// open mdl
	qmdlview_open_mdl(argv[1]);

	// initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(64, 64);
	glutInitWindowSize(800, 600);
	glut_window_main = glutCreateWindow("qmdlview");

	// setup glut funcs
	glutMotionFunc(glut_motion_func);
	glutDisplayFunc(glut_display_func);

	// set glui funcs
	GLUI_Master.set_glutReshapeFunc(glut_reshape_func);  
	GLUI_Master.set_glutKeyboardFunc(glut_keyboard_func);
	GLUI_Master.set_glutSpecialFunc(NULL);
	GLUI_Master.set_glutMouseFunc(NULL);

	// open subwindow
	glui_subwindow_right = GLUI_Master.create_glui_subwindow(glut_window_main, GLUI_SUBWINDOW_RIGHT);

	// add quit button
	new GLUI_Button(glui_subwindow_right, "Quit", 0, (GLUI_Update_CB)glut_exit_func);

	// register subwindow
	glui_subwindow_right->set_main_gfx_window(glut_window_main);

	// initialize gl
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	// main loop
	glutMainLoop();

	// return success
	return EXIT_SUCCESS;
}
