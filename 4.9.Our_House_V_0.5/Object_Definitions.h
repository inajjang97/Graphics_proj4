
// The object modelling tasks performed by this file are usually done 
// by reading a scene configuration file or through a help of graphics user interface!!!
// light and material definitions

typedef struct _Light_Parameters {
	int light_on;
	float position[4];
	float ambient_color[4], diffuse_color[4], specular_color[4];
	float spot_direction[3];
	float spot_exponent;
	float spot_cutoff_angle;
	float light_attenuation_factors[4]; // produce this effect only if .w != 0.0f
} Light_Parameters;

typedef struct _loc_LIGHT_Parameters {
	GLint light_on;
	GLint position;
	GLint ambient_color, diffuse_color, specular_color;
	GLint spot_direction;
	GLint spot_exponent;
	GLint spot_cutoff_angle;
	GLint light_attenuation_factors;
} loc_light_Parameters;

typedef struct _Material_Parameters {
	float ambient_color[4], diffuse_color[4], specular_color[4], emissive_color[4];
	float specular_exponent;
} Material_Parameters;

typedef struct _loc_Material_Parameters {
	GLint ambient_color, diffuse_color, specular_color, emissive_color;
	GLint specular_exponent;
} loc_Material_Parameters;

GLuint h_ShaderProgram_simple, h_ShaderProgram_PS, h_ShaderProgram_GS, h_ShaderProgram_CUR; // handles to shader programs
GLint loc_ModelViewProjectionMatrix_simple;

// for Phone Shading shaders
#define NUMBER_OF_LIGHT_SUPPORTED 10
GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_PS, loc_ModelViewMatrix_PS, loc_ModelViewMatrixInvTrans_PS;
GLint loc_ModelViewProjectionMatrix_GS, loc_ModelViewMatrix_GS, loc_ModelViewMatrixInvTrans_GS;
GLint loc_ModelViewProjectionMatrix_cur, loc_ModelViewMatrix_cur, loc_ModelViewMatrixInvTrans_cur;

GLint loc_screen_effect, loc_screen_width;
GLint loc_blind_effect,loc_blind_width, loc_cartoon_effect, loc_cartoon_levels, loc_ina_effect;
glm::mat4 tiger_light_mc;
int flag_draw_screen, flag_screen_effect, flag_blind_effect, flag_cartoon_effect, flag_ina_effect, flag_ina_light_effect, ina_timer;
float screen_width, cartoon_levels, blind_width;
float screen_width_num = 3.0f, screen_height_num = 2.0f;
GLint loc_width_num, loc_height_num, loc_ina_light_effect, loc_ina_timer;

int light_timer = 0;
float color[4][4] = { {1.0f,0.0f,0.0f,1.0f}, { 0.0f,1.0f,0.0f,1.0f },{ 0.0f,0.0f,1.0f,1.0f },{ 0.0f,0.5f,0.5f,1.0f } };
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

// lights in scene
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

typedef struct _material {
	glm::vec4 emission, ambient, diffuse, specular;
	GLfloat exponent;
} Material;

#define N_MAX_GEOM_COPIES 5
typedef struct _Object {
	char filename[512];

	GLenum front_face_mode; // clockwise or counter-clockwise
	int n_triangles;

	int n_fields; // 3 floats for vertex, 3 floats for normal, and 2 floats for texcoord
	GLfloat *vertices; // pointer to vertex array data
	GLfloat xmin, xmax, ymin, ymax, zmin, zmax; // bounding box <- compute this yourself

	GLuint VBO, VAO; // Handles to vertex buffer object and vertex array object

	int n_geom_instances;
	glm::mat4 ModelMatrix[N_MAX_GEOM_COPIES];
	Material material[N_MAX_GEOM_COPIES];
} Object;

#define N_MAX_STATIC_OBJECTS		15
Object static_objects[N_MAX_STATIC_OBJECTS]; // allocage memory dynamically every time it is needed rather than using a static array
int n_static_objects = 0;

#define OBJ_BUILDING		0
#define OBJ_TABLE			1
#define OBJ_LIGHT			2
#define OBJ_TEAPOT			3
#define OBJ_NEW_CHAIR		4
#define OBJ_FRAME			5
#define OBJ_NEW_PICTURE		6
#define OBJ_COW				7
#define OBJ_BUS				8
#define OBJ_BIKE			9
#define OBJ_GODZILLA		10
#define OBJ_IRONMAN			11
#define OBJ_TANK			12
int read_geometry(GLfloat **object, int bytes_per_primitive, char *filename) {
	int n_triangles;
	FILE *fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles*bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(*object, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void compute_AABB(Object *obj_ptr) {
	// Do it yourself.
}
	 
void prepare_geom_of_static_object(Object *obj_ptr) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = obj_ptr->n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	fprintf(stdout, "Object file %s\n", obj_ptr->filename);
	obj_ptr->n_triangles = read_geometry(&(obj_ptr->vertices), n_bytes_per_triangle, obj_ptr->filename);
	// Initialize vertex buffer object.
	glGenBuffers(1, &(obj_ptr->VBO));

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glBufferData(GL_ARRAY_BUFFER, obj_ptr->n_triangles*n_bytes_per_triangle, obj_ptr->vertices, GL_STATIC_DRAW);

	compute_AABB(obj_ptr);
	free(obj_ptr->vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &(obj_ptr->VAO));
	glBindVertexArray(obj_ptr->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void define_static_objects(void) {
	// building
	strcpy(static_objects[OBJ_BUILDING].filename, "Data/Building1_vnt.geom");
	static_objects[OBJ_BUILDING].n_fields = 8;

	static_objects[OBJ_BUILDING].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BUILDING]));

	static_objects[OBJ_BUILDING].n_geom_instances = 1;

    static_objects[OBJ_BUILDING].ModelMatrix[0] = glm::mat4(1.0f);
	
	static_objects[OBJ_BUILDING].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].diffuse = glm::vec4(0.154f, 0.089f, 0.063f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].exponent = 128.0f*0.1f;

	// table
	strcpy(static_objects[OBJ_TABLE].filename, "Data/Table_vn.geom");
	static_objects[OBJ_TABLE].n_fields = 6;

	static_objects[OBJ_TABLE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TABLE]));

	static_objects[OBJ_TABLE].n_geom_instances = 2;

	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(157.0f, 76.5f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[0], 
		glm::vec3(0.5f, 0.5f, 0.5f));

	
	static_objects[OBJ_TABLE].material[0].ambient = glm::vec4(0.14f, 0.21f, 0.12f, 1.0f);
	static_objects[OBJ_TABLE].material[0].diffuse = glm::vec4(0.5f, 0.3f, 0.1f, 1.0f);
	static_objects[OBJ_TABLE].material[0].specular = glm::vec4(0.5f, 0.3f, 0.1f, 1.0f);
	static_objects[OBJ_TABLE].material[0].exponent = 30.0f;

	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(198.0f, 120.0f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[1],
		glm::vec3(0.8f, 0.6f, 0.6f));

	static_objects[OBJ_TABLE].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[1].ambient = glm::vec4(0.015f, 0.3f, 0.055f, 1.0f);
	static_objects[OBJ_TABLE].material[1].diffuse = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	static_objects[OBJ_TABLE].material[1].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[1].exponent = 128.0f*0.078125f;

	// Light
	strcpy(static_objects[OBJ_LIGHT].filename, "Data/Light_vn.geom");
	static_objects[OBJ_LIGHT].n_fields = 6;

	static_objects[OBJ_LIGHT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(static_objects + OBJ_LIGHT);

	static_objects[OBJ_LIGHT].n_geom_instances = 5;

	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 100.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 47.5f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 130.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[2],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[2].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 60.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[3],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[3].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 112.5f, 49.0));
	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[4],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[4].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].exponent = 128.0f*0.4f;

	// teapot
	strcpy(static_objects[OBJ_TEAPOT].filename, "Data/Teapotn_vn.geom");
	static_objects[OBJ_TEAPOT].n_fields = 6;

	static_objects[OBJ_TEAPOT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TEAPOT]));

	static_objects[OBJ_TEAPOT].n_geom_instances = 1;

	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 11.0f));
	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::scale(static_objects[OBJ_TEAPOT].ModelMatrix[0],
		glm::vec3(2.0f, 2.0f, 2.0f));

	static_objects[OBJ_TEAPOT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].exponent = 128.0f*0.6;

	// new_chair
	strcpy(static_objects[OBJ_NEW_CHAIR].filename, "Data/new_chair_vnt.geom");
	static_objects[OBJ_NEW_CHAIR].n_fields = 8;

	static_objects[OBJ_NEW_CHAIR].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_CHAIR]));

	static_objects[OBJ_NEW_CHAIR].n_geom_instances = 1;

	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 104.0f, 0.0f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		glm::vec3(0.8f, 0.8f, 0.8f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	static_objects[OBJ_NEW_CHAIR].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].ambient = glm::vec4(0.13f, 0.25f, 0.15f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].diffuse = glm::vec4(0.5f, 0.5f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].specular = glm::vec4(0.37f, 0.37f, 0.04f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].exponent = 128.0f*0.078125f;

	// frame
	strcpy(static_objects[OBJ_FRAME].filename, "Data/Frame_vn.geom");
	static_objects[OBJ_FRAME].n_fields = 6;

	static_objects[OBJ_FRAME].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_FRAME]));

	static_objects[OBJ_FRAME].n_geom_instances = 1;

	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(188.0f, 116.0f, 30.0f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::scale(static_objects[OBJ_FRAME].ModelMatrix[0],
		glm::vec3(0.6f, 0.6f, 0.6f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_FRAME].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_FRAME].material[0].ambient = glm::vec4(0.19125f, 0.2735f, 0.225f, 1.0f);
	static_objects[OBJ_FRAME].material[0].diffuse = glm::vec4(0.27038f, 0.27048f, 0.0828f, 1.0f);
	static_objects[OBJ_FRAME].material[0].specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	static_objects[OBJ_FRAME].material[0].exponent = 128.0f*0.1f;

	// new_picture
	strcpy(static_objects[OBJ_NEW_PICTURE].filename, "Data/new_picture_vnt.geom");
	static_objects[OBJ_NEW_PICTURE].n_fields = 8;

	static_objects[OBJ_NEW_PICTURE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_PICTURE]));

	static_objects[OBJ_NEW_PICTURE].n_geom_instances = 1;

	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(189.5f, 116.0f, 30.0f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		glm::vec3(13.5f*0.6f, 13.5f*0.6f, 13.5f*0.6f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_NEW_PICTURE].material[0].emission = glm::vec4(0.02f, 0.05f, 0.03f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].exponent = 128.0f*0.6f;

	// new_picture
	strcpy(static_objects[OBJ_COW].filename, "Data/cow_vn.geom");
	static_objects[OBJ_COW].n_fields = 6;

	static_objects[OBJ_COW].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_COW]));

	static_objects[OBJ_COW].n_geom_instances = 1;

	static_objects[OBJ_COW].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 100.0f, 9.5f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::scale(static_objects[OBJ_COW].ModelMatrix[0],
		glm::vec3(30.0f, 30.0f, 30.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
 
	static_objects[OBJ_COW].material[0].emission = glm::vec4(0.03f, 0.05f, 0.2f, 1.0f);
	static_objects[OBJ_COW].material[0].ambient = glm::vec4(0.1329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_COW].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_COW].material[0].specular = glm::vec4(0.0992157f, 0.0941176f, 0.0807843f, 1.0f);
	static_objects[OBJ_COW].material[0].exponent = 0.21794872f*0.6f;

	
	// bus
	strcpy(static_objects[OBJ_BUS].filename, "Data/Bus.geom");
	static_objects[OBJ_BUS].n_fields = 8;

	static_objects[OBJ_BUS].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BUS]));
	static_objects[OBJ_BUS].n_geom_instances = 1;

	static_objects[OBJ_BUS].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 50.0f, 0.0f));
	static_objects[OBJ_BUS].ModelMatrix[0] = glm::scale(static_objects[OBJ_BUS].ModelMatrix[0],
		glm::vec3(1.0f, 1.0f, 3.0f));
	static_objects[OBJ_BUS].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BUS].ModelMatrix[0],
		125.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_BUS].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BUS].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	
	static_objects[OBJ_BUS].material[0].emission = glm::vec4(0.02f, 0.0f, 0.08f, 1.0f);
	static_objects[OBJ_BUS].material[0].ambient = glm::vec4(0.17, 0.18f, 0.21f, 1.0f);
	static_objects[OBJ_BUS].material[0].diffuse = glm::vec4(0.013f, 0.03f, 0.07f, 1.0f);
	static_objects[OBJ_BUS].material[0].specular = glm::vec4(0.15f, 0.14f, 0.18f, 1.0f);
	static_objects[OBJ_BUS].material[0].exponent = 0.21794872f*0.6f;
	
	// bike
	strcpy(static_objects[OBJ_BIKE].filename, "Data/Bike.geom");
	static_objects[OBJ_BIKE].n_fields = 8;

	static_objects[OBJ_BIKE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BIKE]));
	static_objects[OBJ_BIKE].n_geom_instances = 1;

	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 100.0f, 0.0f));
	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::scale(static_objects[OBJ_BIKE].ModelMatrix[0], glm::vec3(10.0f, 10.0f, 10.0f));
	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BIKE].ModelMatrix[0],90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BIKE].ModelMatrix[0],90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	
	static_objects[OBJ_BIKE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BIKE].material[0].ambient = glm::vec4(0.13f, 0.26f, 0.1f, 1.0f);
	static_objects[OBJ_BIKE].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_BIKE].material[0].specular = glm::vec4(0.0992157f, 0.0941176f, 0.0807843f, 1.0f);
	static_objects[OBJ_BIKE].material[0].exponent = 0.21794872f*0.6f;
	
	//Godzilla
	strcpy(static_objects[OBJ_GODZILLA].filename, "Data/Godzilla.geom");
	static_objects[OBJ_GODZILLA].n_fields = 8;

	static_objects[OBJ_GODZILLA].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_GODZILLA]));
	static_objects[OBJ_GODZILLA].n_geom_instances = 1;

	static_objects[OBJ_GODZILLA].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(140.0f, 100.0f, 0.0f));
	static_objects[OBJ_GODZILLA].ModelMatrix[0] = glm::scale(static_objects[OBJ_GODZILLA].ModelMatrix[0],glm::vec3(0.1f, 0.1f, 0.1f));
	static_objects[OBJ_GODZILLA].ModelMatrix[0] = glm::rotate(static_objects[OBJ_GODZILLA].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_GODZILLA].ModelMatrix[0] = glm::rotate(static_objects[OBJ_GODZILLA].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_GODZILLA].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_GODZILLA].material[0].ambient = glm::vec4(0.29f, 0.15f, 0.14f, 1.0f);
	static_objects[OBJ_GODZILLA].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_GODZILLA].material[0].specular = glm::vec4(0.0992157f, 0.0941176f, 0.0807843f, 1.0f);
	static_objects[OBJ_GODZILLA].material[0].exponent = 0.21794872f*0.6f;


	strcpy(static_objects[OBJ_IRONMAN].filename, "Data/IronMan.geom");
	static_objects[OBJ_IRONMAN].n_fields = 8;

	static_objects[OBJ_IRONMAN].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_IRONMAN]));
	static_objects[OBJ_IRONMAN].n_geom_instances = 1;

	static_objects[OBJ_IRONMAN].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 120.0f, 60.0f));
	static_objects[OBJ_IRONMAN].ModelMatrix[0] = glm::scale(static_objects[OBJ_IRONMAN].ModelMatrix[0],
		glm::vec3(8.0f, 8.0f, 8.0f));
	static_objects[OBJ_IRONMAN].ModelMatrix[0] = glm::rotate(static_objects[OBJ_IRONMAN].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_IRONMAN].ModelMatrix[0] = glm::rotate(static_objects[OBJ_IRONMAN].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	static_objects[OBJ_IRONMAN].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_IRONMAN].material[0].ambient = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_IRONMAN].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_IRONMAN].material[0].specular = glm::vec4(0.0992157f, 0.0941176f, 0.0807843f, 1.0f);
	static_objects[OBJ_IRONMAN].material[0].exponent = 0.21794872f*0.6f;

	strcpy(static_objects[OBJ_TANK].filename, "Data/Tank.geom");
	static_objects[OBJ_TANK].n_fields = 8;

	static_objects[OBJ_TANK].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TANK]));
	static_objects[OBJ_TANK].n_geom_instances = 1;

	static_objects[OBJ_TANK].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 80.0f, 10.0f));
	static_objects[OBJ_TANK].ModelMatrix[0] = glm::scale(static_objects[OBJ_TANK].ModelMatrix[0],
		glm::vec3(3.0f, 3.0f, 3.0f));
	static_objects[OBJ_TANK].ModelMatrix[0] = glm::rotate(static_objects[OBJ_TANK].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_TANK].material[0].emission = glm::vec4(0.0f, 0.05f, 0.05f, 1.0f);
	static_objects[OBJ_TANK].material[0].ambient = glm::vec4(0.0f, 0.25f, 0.15f, 1.0f);
	static_objects[OBJ_TANK].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_TANK].material[0].specular = glm::vec4(0.0992157f, 0.0941176f, 0.0807843f, 1.0f);
	static_objects[OBJ_TANK].material[0].exponent = 0.21794872f*0.6f;


	n_static_objects =13;
}

GLuint VBO_axes, VAO_axes;
GLfloat vertices_axes[6][3] = {
	{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };

//멋찌다
void define_axes(void) {  
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define N_TIGER_FRAMES 12
Object tiger[N_TIGER_FRAMES];
struct {
	int time = 0;
	int cur_frame = 0;
	float rotation_angle = 0.0f;
} tiger_data;

struct {
	int time = 0;
	int cur_frame = 0;
	float rotation_angle = 0.0f;
} cctv_tiger_data;


// tiger object
#define N_TIGER_FRAMES 12
GLuint tiger_VBO, tiger_VAO;
int tiger_n_triangles[N_TIGER_FRAMES];
int tiger_vertex_offset[N_TIGER_FRAMES];
GLfloat *tiger_vertices[N_TIGER_FRAMES];

Material_Parameters material_tiger;
void prepare_tiger(void) { // vertices enumerated clockwise
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tiger_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		tiger_n_triangles[i] = read_geometry(&tiger_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		tiger_n_total_triangles += tiger_n_triangles[i];

		if (i == 0)
			tiger_vertex_offset[i] = 0;
		else
			tiger_vertex_offset[i] = tiger_vertex_offset[i - 1] + 3 * tiger_n_triangles[i - 1];
	}
	material_tiger.ambient_color[0] = 0.24725f;
	material_tiger.ambient_color[1] = 0.1995f;
	material_tiger.ambient_color[2] = 0.0745f;
	material_tiger.ambient_color[3] = 1.0f;

	material_tiger.diffuse_color[0] = 0.75164f;
	material_tiger.diffuse_color[1] = 0.60648f;
	material_tiger.diffuse_color[2] = 0.22648f;
	material_tiger.diffuse_color[3] = 1.0f;

	material_tiger.specular_color[0] = 0.628281f;
	material_tiger.specular_color[1] = 0.555802f;
	material_tiger.specular_color[2] = 0.366065f;
	material_tiger.specular_color[3] = 1.0f;

	material_tiger.specular_exponent = 51.2f;

	material_tiger.emissive_color[0] = 0.1f;
	material_tiger.emissive_color[1] = 0.1f;
	material_tiger.emissive_color[2] = 0.0f;
	material_tiger.emissive_color[3] = 1.0f;

	// initialize vertex buffer object
	glGenBuffers(1, &tiger_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glBufferData(GL_ARRAY_BUFFER, tiger_n_total_triangles*n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_TIGER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, tiger_vertex_offset[i] * n_bytes_per_vertex,
			tiger_n_triangles[i] * n_bytes_per_triangle, tiger_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_TIGER_FRAMES; i++)
		free(tiger_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &tiger_VAO);
	glBindVertexArray(tiger_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


}

void set_material_tiger(void) {
	
	glUniform4fv(loc_material.ambient_color, 1, material_tiger.ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material_tiger.diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material_tiger.specular_color);
	glUniform1f(loc_material.specular_exponent, material_tiger.specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material_tiger.emissive_color);
}

void draw_tiger(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(tiger_VAO);
	glDrawArrays(GL_TRIANGLES, tiger_vertex_offset[tiger_data.cur_frame], 3 * tiger_n_triangles[tiger_data.cur_frame]);
	glBindVertexArray(0);
}

// floor object
GLuint rectangle_VBO, rectangle_VAO;
GLfloat rectangle_vertices[12][3] = {  // vertices enumerated counterclockwise
	{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },
{ 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },
{ 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
};

Material_Parameters material_floor;
Material_Parameters material_screen;
void prepare_rectangle(void) { // Draw coordinate axes.
						   // Initialize vertex buffer object.
	glGenBuffers(1, &rectangle_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), &rectangle_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &rectangle_VAO);
	glBindVertexArray(rectangle_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_floor.ambient_color[0] = 0.3f;
	material_floor.ambient_color[1] = 0.2f;
	material_floor.ambient_color[2] = 0.1f;
	material_floor.ambient_color[3] = 1.0f;

	material_floor.diffuse_color[0] = 0.6f;
	material_floor.diffuse_color[1] = 0.3f;
	material_floor.diffuse_color[2] = 0.1f;
	material_floor.diffuse_color[3] = 1.0f;

	material_floor.specular_color[0] = 0.3f;
	material_floor.specular_color[1] = 0.2f;
	material_floor.specular_color[2] = 0.1f;
	material_floor.specular_color[3] = 1.0f;

	material_floor.specular_exponent = 2.5f;

	material_floor.emissive_color[0] = 0.0f;
	material_floor.emissive_color[1] = 0.0f;
	material_floor.emissive_color[2] = 0.0f;
	material_floor.emissive_color[3] = 1.0f;

	
	//material_screen
	material_screen.ambient_color[0] = 0.1745f;
	material_screen.ambient_color[1] = 0.0117f;
	material_screen.ambient_color[2] = 0.0117f;
	material_screen.ambient_color[3] = 1.0f;

	material_screen.diffuse_color[0] = 0.6142f;
	material_screen.diffuse_color[1] = 0.0413f;
	material_screen.diffuse_color[2] = 0.0413f;
	material_screen.diffuse_color[3] = 1.0f;

	material_screen.specular_color[0] = 0.7278f;
	material_screen.specular_color[1] = 0.6269f;
	material_screen.specular_color[2] = 0.6269f;
	material_screen.specular_color[3] = 1.0f;

	material_screen.specular_exponent = 20.5f;

	material_screen.emissive_color[0] = 0.0f;
	material_screen.emissive_color[1] = 0.0f;
	material_screen.emissive_color[2] = 0.0f;
	material_screen.emissive_color[3] = 1.0f;
}

void set_material_floor(void) {
	// assume ShaderProgram_PS is used
	glUniform4fv(loc_material.ambient_color, 1, material_floor.ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material_floor.diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material_floor.specular_color);
	glUniform1f(loc_material.specular_exponent, material_floor.specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material_floor.emissive_color);
}
void prepare_floor(){
	glGenBuffers(1, &rectangle_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), &rectangle_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &rectangle_VAO);
	glBindVertexArray(rectangle_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_floor.ambient_color[0] = 0.0f;
	material_floor.ambient_color[1] = 0.05f;
	material_floor.ambient_color[2] = 0.0f;
	material_floor.ambient_color[3] = 1.0f;

	material_floor.diffuse_color[0] = 0.4f;
	material_floor.diffuse_color[1] = 0.5f;
	material_floor.diffuse_color[2] = 0.4f;
	material_floor.diffuse_color[3] = 1.0f;

	material_floor.specular_color[0] = 0.04f;
	material_floor.specular_color[1] = 0.7f;
	material_floor.specular_color[2] = 0.04f;
	material_floor.specular_color[3] = 1.0f;

	material_floor.specular_exponent = 2.5f;

	material_floor.emissive_color[0] = 0.0f;
	material_floor.emissive_color[1] = 0.0f;
	material_floor.emissive_color[2] = 0.0f;
	material_floor.emissive_color[3] = 1.0f;

	material_floor.ambient_color[0] = 0.0f;
	material_floor.ambient_color[1] = 0.05f;
	material_floor.ambient_color[2] = 0.0f;
	material_floor.ambient_color[3] = 1.0f;

	material_floor.diffuse_color[0] = 0.4f;
	material_floor.diffuse_color[1] = 0.5f;
	material_floor.diffuse_color[2] = 0.4f;
	material_floor.diffuse_color[3] = 1.0f;

	material_floor.specular_color[0] = 0.04f;
	material_floor.specular_color[1] = 0.7f;
	material_floor.specular_color[2] = 0.04f;
	material_floor.specular_color[3] = 1.0f;

	material_floor.specular_exponent = 2.5f;

	material_floor.emissive_color[0] = 0.0f;
	material_floor.emissive_color[1] = 0.0f;
	material_floor.emissive_color[2] = 0.0f;
	material_floor.emissive_color[3] = 1.0f;
}
void draw_floor(void) {
	glFrontFace(GL_CCW);

	glBindVertexArray(rectangle_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}


void set_material_screen(void) {
	// assume ShaderProgram_PS is used
	glUniform4fv(loc_material.ambient_color, 1, material_screen.ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material_screen.diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material_screen.specular_color);
	glUniform1f(loc_material.specular_exponent, material_screen.specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material_screen.emissive_color);
}

void draw_screen(void) {
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(rectangle_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

//자동차 안그려지면 나는 행복해 그려지면 더 행복해 

#define WC_AXIS_LENGTH		60.0f
void cctv_draw_axes(int idx) {
	cctv_ModelViewMatrix[idx] = glm::scale(cctv_ViewMatrix[idx], glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	cctv_ModelViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ModelViewMatrix[idx];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}
#define rad 1.7f
#define ww 1.0f

void cctv_draw_static_object(int idx, Object *obj_ptr, int instance_ID) {

	glUseProgram(h_ShaderProgram_CUR);
	glFrontFace(obj_ptr->front_face_mode);

	cctv_ModelViewMatrix[idx] = cctv_ViewMatrix[idx] * obj_ptr->ModelMatrix[instance_ID];
	cctv_ModelViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ModelViewMatrix[idx];
	cctv_ModelViewMatrixInvTrans[idx] = glm::inverseTranspose(glm::mat3(cctv_ModelViewMatrix[idx]));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_cur, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_cur, 1, GL_FALSE, &cctv_ModelViewMatrix[idx][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_cur, 1, GL_FALSE, &cctv_ModelViewMatrixInvTrans[idx][0][0]);
	
	glUniform4f(loc_material.ambient_color, obj_ptr->material[instance_ID].ambient.r,
		obj_ptr->material[instance_ID].ambient.g, obj_ptr->material[instance_ID].ambient.b, obj_ptr->material[instance_ID].ambient.a);
	
	glUniform4f(loc_material.diffuse_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b, obj_ptr->material[instance_ID].diffuse.a);
	
	glUniform4f(loc_material.specular_color, obj_ptr->material[instance_ID].specular.r,
		obj_ptr->material[instance_ID].specular.g, obj_ptr->material[instance_ID].specular.b, obj_ptr->material[instance_ID].specular.a);
	
	glUniform4f(loc_material.emissive_color, obj_ptr->material[instance_ID].emission.r,
		obj_ptr->material[instance_ID].emission.g, obj_ptr->material[instance_ID].emission.b, obj_ptr->material[instance_ID].emission.a);
	
	glUniform1f(loc_material.specular_exponent, obj_ptr->material[instance_ID].exponent);
	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);

	glUseProgram(0);
}
float car_angle_z() {
	if (car_time >= 746) {
		return 0.0f;
	}

	glm::vec3 c, n;
	c = cpos[car_time];
	n = cpos[car_time + 5];

	float angle = 0.0f;

	if (n.y == c.y && c.x < n.x)
		angle = 0.0f;

	else if (n.y == c.y && c.x >= n.x)
		angle = 180 * TO_RADIAN;

	else if (c.x == n.x && c.y < n.y)
		angle = -90 * TO_RADIAN;

	else if (c.x == n.x && c.y >= n.y)
		angle = 90 * TO_RADIAN;

	else if (c.x > n.x)
		angle = atan((n.y - c.y) / (n.x - c.x)) + 180 * TO_RADIAN;

	else
		angle = atan((n.y - c.y) / (n.x - c.x));

	glm::vec3 tmp = glm::cross(c, n);
	if (tmp.y < 0)
		angle = -1 * angle;
	return angle;
}
float car_angle_y() {
	static float a = 0.0f;
	float dx;
	if (car_time < 10)
		return 0.0f;
	glm::vec3 tmp = cpos[car_time] - cpos[(car_time -10)];
	dx = sqrt(tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z);
	a = a + 0.8 * 180 * dx / (3.141592*rad);
	if (a >= 360.0f)
		a -= 360;
	return a;
}
void cctv_draw_wheel_and_nut(glm::mat4 ViewProjectionMatrix) {
	// angle is used in Hierarchical_Car_Correct later2
	int i;

	glUniform3f(loc_primitive_color, 0.000f, 0.808f, 0.820f); // color name: DarkTurquoise
	draw_geom_obj(GEOM_OBJ_ID_CAR_WHEEL); // draw wheel

	for (i = 0; i < 5; i++) {
		ModelMatrix_CAR_NUT = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*72.0f*i, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix_CAR_NUT = glm::translate(ModelMatrix_CAR_NUT, glm::vec3(rad - 0.5f, 0.0f, ww));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_NUT;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

		glUniform3f(loc_primitive_color, 0.690f, 0.769f, 0.871f); // color name: LightSteelBlue
		draw_geom_obj(GEOM_OBJ_ID_CAR_NUT); // draw i-th nut
	}
}

void cctv_draw_car_dummy(glm::mat4 ViewProjectionMatrix, int idx) {
	float zangle = car_angle_z();
	float yangle = car_angle_y();
	glm::vec3 x,y,z;

	if (car_time + 1 < 751) 
		z= glm::cross(cpos[car_time], cpos[car_time + 1]);

	if (car_time - 10 >= 0 && car_time + 10 < 751) {
		x = cpos[car_time] - cpos[car_time - 15];
		y = cpos[car_time + 15] - cpos[car_time];
		z = glm::cross(x, y);
	}

	glUniform3f(loc_primitive_color, 0.498f, 1.000f, 0.831f); // color name: Aquamarine
	draw_geom_obj(GEOM_OBJ_ID_CAR_BODY); // draw body

	glLineWidth(2.0f);
	cctv_draw_axes(idx); // draw MC axes of body
	glLineWidth(1.0f);

	ModelMatrix_CAR_DRIVER = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_DRIVER = glm::rotate(ModelMatrix_CAR_DRIVER, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_DRIVER;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(5.0f);
	cctv_draw_axes(idx); // draw camera frame at driver seat
	glLineWidth(1.0f);

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, 4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, z.z , glm::vec3(0, 1, 0));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, 90.0f*TO_RADIAN, glm::vec3(0, 0, 1));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, yangle, glm::vec3(0, 0, 1));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	
	if (z.z >= 0)
		glUniform3f(loc_primitive_color, 1.0f, 0.0f, 0.0f); // color name: Aquamarine
	else
		glUniform3f(loc_primitive_color, 0.0f, 0.0f, 1.0f); // color name: Aquamarine

	cctv_draw_wheel_and_nut(ViewProjectionMatrix);  // draw wheel 0


	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, 4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, yangle, glm::vec3(0, 0, 1));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	cctv_draw_wheel_and_nut(ViewProjectionMatrix);  // draw wheel 1

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, z.z, glm::vec3(0, -1, 0));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, yangle, glm::vec3(0, 0, 1));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	
	if( z.z >=0)
		glUniform3f(loc_primitive_color, 1.0f, 0.0f, 0.0f); // color name: Aquamarine
	else
		glUniform3f(loc_primitive_color, 0.0f, 0.0f, 1.0f); // color name: Aquamarine
	
	cctv_draw_wheel_and_nut(ViewProjectionMatrix);  // draw wheel 2


	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, yangle, glm::vec3(0, 0, 1));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	cctv_draw_wheel_and_nut(ViewProjectionMatrix);  // draw wheel 3
}

void cctv_draw_animated_tiger(int idx) {
	cctv_ModelViewMatrix[idx] = glm::translate(cctv_ViewMatrix[idx], tpos[tiger_data.time]);
	cctv_ModelViewMatrix[idx] = glm::rotate(cctv_ModelViewMatrix[idx], tangle[tiger_data.time], glm::vec3(0, 0, 1));
	cctv_ModelViewMatrix[idx] = glm::scale(cctv_ModelViewMatrix[idx], glm::vec3(0.21, 0.21, 0.21));
	cctv_ModelViewMatrix[idx] *= tiger[tiger_data.cur_frame].ModelMatrix[0];	
	tiger_light_mc = cctv_ModelViewMatrix[idx];

	//draw light 2
/*	if( light_timer%2==0)
		glUniform1i(loc_light[2].light_on,0);
	else*/
	glUniform1i(loc_light[2].light_on, light[2].light_on);
	
	tiger_light_mc = glm::translate(tiger_light_mc, glm::vec3(0, 0, 100));
	glm::vec4 position_EC = tiger_light_mc * glm::vec4(light[2].position[0], light[2].position[1], light[2].position[2],
		light[2].position[3]);
	glUniform4fv(loc_light[2].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light[2].specular_color, 1, light[2].specular_color);
	glm::vec3 direction_EC = glm::mat3(glm::inverseTranspose(tiger_light_mc))*glm::vec3(light[2].spot_direction[0], light[2].spot_direction[1],
		light[2].spot_direction[2]);
	glUniform3fv(loc_light[2].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light[2].spot_exponent, light[2].spot_exponent);

	//draw tiger
	cctv_ModelViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ModelViewMatrix[idx];
	cctv_ModelViewMatrixInvTrans[idx] = glm::inverseTranspose(glm::mat3(cctv_ModelViewMatrix[idx]));

	glUniformMatrix4fv(loc_ModelViewMatrix_cur, 1, GL_FALSE, &cctv_ModelViewMatrix[idx][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_cur, 1, GL_FALSE, &cctv_ModelViewMatrixInvTrans[idx][0][0]);
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_cur, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);

	glFrontFace(GL_CW);

	glBindVertexArray(tiger_VAO);
	glDrawArrays(GL_TRIANGLES, tiger_vertex_offset[cctv_tiger_data.cur_frame], 3 * tiger_n_triangles[cctv_tiger_data.cur_frame]);
	glBindVertexArray(0);
/*	
	cctv_ModelViewProjectionMatrix[idx] = glm::scale(cctv_ModelViewProjectionMatrix[idx], glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);
	cctv_draw_axes(idx);*/
}
void draw_line(glm::vec3 p1, glm::vec3 p2,int idx) {
	glm::vec3 x, y, z, X,Y,Z;
	GLuint VBO_lines, VAO_lines;
	GLfloat vertices_lines[2][3] = {
		{ p1.x, p1.y, p1.z } ,{ p2.x, p2.y, p2.z }
	};

	float lines_color[3] = { 1,0,0 };

	glUseProgram(h_ShaderProgram_simple);
	glGenBuffers(1, &VBO_lines);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_lines);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_lines), &vertices_lines[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_lines);
	glBindVertexArray(VAO_lines);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_lines);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cctv_ModelViewMatrix[idx] = cctv_ViewMatrix[idx];
	cctv_ModelViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ModelViewMatrix[idx];

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);
	glBindVertexArray(VAO_lines);
	glUniform3fv(loc_primitive_color, 1, lines_color);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	glUseProgram(0);

}

void draw_VV(glm::vec3 prp, glm::vec3 center, float fov, float as, float nc, float fc, int idx) {
	float w, h, w2, h2;

	glm::mat4 M = glm::affineInverse(glm::lookAt(cctv_eye, cctv_center, glm::vec3(0, 0, 1)));
	M = glm::scale(M, glm::vec3(-1, 1, 1));
	glm::mat3 M3 = glm::mat3(M);

	h = fabs(nc)*tan((fov / 2)*TO_RADIAN);
	w = h * as;
	draw_line(M3*glm::vec3(-w / 2, -h / 2, -nc)+cctv_eye , M3*glm::vec3(w / 2, -h / 2, -nc)+cctv_eye, idx);
	draw_line(M3*glm::vec3(-w / 2, -h / 2, -nc) + cctv_eye, M3*glm::vec3(-w / 2, h / 2, -nc) + cctv_eye, idx);
	draw_line(M3*glm::vec3(w / 2, h / 2, -nc) + cctv_eye, M3*glm::vec3(w / 2, -h / 2, -nc) + cctv_eye, idx);
	draw_line(M3*glm::vec3(-w / 2, h / 2, -nc) + cctv_eye, M3*glm::vec3(w / 2, h / 2, -nc) + cctv_eye, idx);

	h2 = fabs(fc)*tan((fov / 2)*TO_RADIAN);
	w2 = h2 * as;
	draw_line(M3*glm::vec3(-w2 / 2, -h2 / 2, -fc) + cctv_eye, M3*glm::vec3(w2 / 2, -h2 / 2, -fc) + cctv_eye, idx);
	draw_line(M3*glm::vec3(-w2 / 2, -h2 / 2, -fc) + cctv_eye, M3*glm::vec3(-w2 / 2, h2 / 2, -fc) + cctv_eye, idx);
	draw_line(M3*glm::vec3(w2 / 2, h2 / 2, -fc) + cctv_eye, M3*glm::vec3(w2 / 2, -h2 / 2, -fc) + cctv_eye, idx);
	draw_line(M3*glm::vec3(-w2 / 2, h2 / 2, -fc) + cctv_eye, M3*glm::vec3(w2 / 2, h2 / 2, -fc) + cctv_eye, idx);

	draw_line(M3*glm::vec3(w / 2, h / 2, -nc) + cctv_eye, M3*glm::vec3(w2 / 2, h2 / 2, -fc) + cctv_eye, idx);
	draw_line(M3*glm::vec3(-w / 2, h / 2, -nc) + cctv_eye, M3*glm::vec3(-w2 / 2, h2 / 2, -fc) + cctv_eye, idx);
	draw_line(M3*glm::vec3(-w / 2, -h / 2, -nc) + cctv_eye, M3*glm::vec3(-w2 / 2, -h2 / 2, -fc) + cctv_eye, idx);
	draw_line(M3*glm::vec3(w / 2, -h / 2, -nc)+ cctv_eye ,M3*glm::vec3(w2 / 2, -h2 / 2, -fc) + cctv_eye, idx);

	glLineWidth(1.0f);

}

void initialize_screen(void) {
	flag_draw_screen = flag_screen_effect = 0;
	screen_width = 0.125f;
}

void initialize_blind(void) {
	flag_blind_effect = 0;
	blind_width = 90.0f;
}

void initialize_cartoon(void) {
	flag_cartoon_effect = 0;
	cartoon_levels = 3.0f;
}

void initialize_ina(void) {
	flag_ina_effect = 0;
}
void initialize_ina_light(void) {
	flag_ina_light_effect = 0;
	ina_timer = 0;
}
void cctv_draw_light(int idx) {
	int i;
	glm::vec4 position_EC;
	glm::vec3 direction_EC;

	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glUniform4fv(loc_light[0].position, 1, light[0].position);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);


	for (i = 1; i <NUMBER_OF_LIGHT_SUPPORTED; i++) {
		if (i == 2)
			continue;
	
		if (i == 5) {
			glUniform1i(loc_light[i].light_on, light[i].light_on);
			position_EC = cctv_ViewMatrix[idx] * glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]);
			glUniform4fv(loc_light[i].position, 1, &position_EC[0]);
			glUniform4fv(loc_light[i].ambient_color, 1, color[light_timer%4]);
			glUniform4fv(loc_light[i].diffuse_color, 1, color[light_timer%4]);
			glUniform4fv(loc_light[i].specular_color, 1, color[light_timer%4]);
			direction_EC = glm::mat3(cctv_ViewMatrix[idx]) * glm::vec3(light[i].spot_direction[0], light[i].spot_direction[1], light[i].spot_direction[2]);
			glUniform3fv(loc_light[i].spot_direction, 1, &direction_EC[0]);
			glUniform1f(loc_light[i].spot_cutoff_angle, light[i].spot_cutoff_angle);
			glUniform1f(loc_light[i].spot_exponent, light[i].spot_exponent);
;
			glUniform1i(loc_blind_effect, flag_blind_effect);
			glUniform1f(loc_blind_width, blind_width);

			continue;
		}

		glUniform1i(loc_light[i].light_on, light[i].light_on);
		position_EC = cctv_ViewMatrix[idx] * glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]);

		if (i == 1) {
			glm::mat4 tmp = glm::mat4(1.0f);
			tmp = glm::rotate(tmp, (light_timer % 360)*TO_RADIAN, glm::vec3(0, 0, 1));
			tmp = glm::translate(tmp, glm::vec3(-1, 0, 0));
			tmp = tmp*cctv_ViewMatrix[idx];
			position_EC = tmp*( glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]));
		}
		glUniform4fv(loc_light[i].position, 1, &position_EC[0]);
		glUniform4fv(loc_light[i].ambient_color, 1, light[i].ambient_color);
		glUniform4fv(loc_light[i].diffuse_color, 1, light[i].diffuse_color);
		glUniform4fv(loc_light[i].specular_color, 1, light[i].specular_color);
		direction_EC = glm::mat3(cctv_ViewMatrix[idx]) * glm::vec3(light[i].spot_direction[0], light[i].spot_direction[1], light[i].spot_direction[2]);
		glUniform3fv(loc_light[i].spot_direction, 1, &direction_EC[0]);
		glUniform1f(loc_light[i].spot_cutoff_angle, light[i].spot_cutoff_angle);
		glUniform1f(loc_light[i].spot_exponent, light[i].spot_exponent);

		ina_timer = light_timer % 2;
		glUniform1i(loc_ina_timer, ina_timer);
		glUniform1i(loc_ina_light_effect, flag_ina_light_effect);
		
		glUniform1i(loc_blind_effect, flag_blind_effect);
		glUniform1f(loc_blind_width, blind_width);

	}

}
void draw_cctv(int idx) {
	glUseProgram(h_ShaderProgram_simple);
	glLineWidth(2.0f);
	cctv_draw_axes(idx);
	glLineWidth(1.0f);


	glUseProgram(h_ShaderProgram_CUR);
	cctv_draw_light(idx);

	//draw_tiger
	set_material_tiger();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	cctv_draw_animated_tiger(idx);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//draw_floor
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	set_material_floor();
	cctv_ModelViewMatrix[idx] = glm::translate(cctv_ViewMatrix[idx], glm::vec3(0.0f, 0.0f, 0.0f));
	cctv_ModelViewMatrix[idx] = glm::scale(cctv_ModelViewMatrix[idx], glm::vec3(230.0f, 160.0f, 160.0f));
	
	cctv_ModelViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ModelViewMatrix[idx];
	cctv_ModelViewMatrixInvTrans[idx] = glm::inverseTranspose(glm::mat3(cctv_ModelViewMatrix[idx]));

	glUniformMatrix4fv(loc_ModelViewMatrix_cur, 1, GL_FALSE, &cctv_ModelViewMatrix[idx][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_cur, 1, GL_FALSE, &cctv_ModelViewMatrixInvTrans[idx][0][0]);
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_cur, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);

	glFrontFace(GL_CCW);
	glBindVertexArray(rectangle_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//draw wall
	cctv_ModelViewMatrix[idx] = glm::translate(cctv_ViewMatrix[idx], glm::vec3(0.0f, 5.0f, 0.0f));
	cctv_ModelViewMatrix[idx] = glm::scale(cctv_ModelViewMatrix[idx], glm::vec3(230.0f, 200.0f, 50.0f));
	cctv_ModelViewMatrix[idx] = glm::rotate(cctv_ModelViewMatrix[idx], 90 * TO_RADIAN, glm::vec3(1, 0, 0));
	cctv_ModelViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ModelViewMatrix[idx];
	cctv_ModelViewMatrixInvTrans[idx] = glm::inverseTranspose(glm::mat3(cctv_ModelViewMatrix[idx]));
	cctv_ModelViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ModelViewMatrix[idx];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_cur, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_cur, 1, GL_FALSE, &cctv_ModelViewMatrix[idx][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_cur, 1, GL_FALSE, &cctv_ModelViewMatrixInvTrans[idx][0][0]);

	glFrontFace(GL_CCW);
	glBindVertexArray(rectangle_VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	

	//draw static obj
	cctv_draw_static_object(idx, &(static_objects[OBJ_BUILDING]), 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	cctv_draw_static_object(idx, &(static_objects[OBJ_TABLE]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_TABLE]), 1);

	cctv_draw_static_object(idx, &(static_objects[OBJ_LIGHT]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_LIGHT]), 1);
	cctv_draw_static_object(idx, &(static_objects[OBJ_LIGHT]), 2);
	cctv_draw_static_object(idx, &(static_objects[OBJ_LIGHT]), 3);
	cctv_draw_static_object(idx, &(static_objects[OBJ_LIGHT]), 4);

	cctv_draw_static_object(idx, &(static_objects[OBJ_TEAPOT]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_NEW_CHAIR]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_FRAME]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_NEW_PICTURE]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_COW]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_GODZILLA]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_IRONMAN]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_TANK]), 0);

	cctv_draw_static_object(idx, &(static_objects[OBJ_BUS]), 0);
	cctv_draw_static_object(idx, &(static_objects[OBJ_BIKE]), 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::mat4 M, MVP;
	M = translate(glm::mat4(1.0f), glm::vec3(25, 25, 0));
	M = scale(M, glm::vec3(3, 3, 3));
	MVP = cctv_ProjectionMatrix[idx] * cctv_ViewMatrix[idx] * M;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_cur, 1, GL_FALSE, &MVP[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_BIKE);

	glUseProgram(h_ShaderProgram_simple);

	cctv_ViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ViewMatrix[idx];
	cctv_ModelViewProjectionMatrix[idx] = cctv_ViewProjectionMatrix[idx] * ModelMatrix_PATH;
	cctv_ModelViewProjectionMatrix[idx] = glm::scale(cctv_ModelViewProjectionMatrix[idx], glm::vec3(3, 3, 3));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);
	//ModelViewProjectionMatrix = cctv_ModelViewProjectionMatrix[idx];
	draw_path();

	cctv_ViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ViewMatrix[idx];
	cctv_ModelViewProjectionMatrix[idx] = cctv_ViewProjectionMatrix[idx] * ModelMatrix_CAR_BODY;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);
	cctv_draw_car_dummy(cctv_ViewProjectionMatrix[idx], idx);
	
	glUseProgram(h_ShaderProgram_CUR);
	//screen
	if (flag_draw_screen) {
		set_material_screen();
		cctv_ModelViewMatrix[idx] = glm::translate(cctv_ViewMatrix[idx], glm::vec3(0, 0, 100));
		cctv_ModelViewMatrix[idx] = glm::scale(cctv_ModelViewMatrix[idx], glm::vec3(70, 160, 100));
		cctv_ModelViewMatrix[idx] = glm::rotate(cctv_ModelViewMatrix[idx], 90 * TO_RADIAN, glm::vec3(0, 1, 0));
		cctv_ModelViewMatrixInvTrans[idx] = glm::inverseTranspose(glm::mat3(cctv_ModelViewMatrix[idx]));

		cctv_ModelViewProjectionMatrix[idx] = cctv_ProjectionMatrix[idx] * cctv_ModelViewMatrix[idx];

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_cur, 1, GL_FALSE, &cctv_ModelViewProjectionMatrix[idx][0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_cur, 1, GL_FALSE, &cctv_ModelViewMatrix[idx][0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_cur, 1, GL_FALSE, &cctv_ModelViewMatrixInvTrans[idx][0][0]);

		glUniform1i(loc_screen_effect, flag_screen_effect);
		glUniform1f(loc_screen_width, screen_width);

		if (flag_screen_effect) {
			glUniform1f(loc_width_num, screen_width_num);
			glUniform1f(loc_height_num, screen_height_num);
		}
		draw_screen();

		glUniform1i(loc_screen_effect, 0);
	}
	glUseProgram(0);
}

void cleanup_OpenGL_stuffs(void) {
	for (int i = 0; i < n_static_objects; i++) {
		glDeleteVertexArrays(1, &(static_objects[i].VAO));
		glDeleteBuffers(1, &(static_objects[i].VBO));
	}

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		glDeleteVertexArrays(1, &(tiger[i].VAO));
		glDeleteBuffers(1, &(tiger[i].VBO));
	}

	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);
}