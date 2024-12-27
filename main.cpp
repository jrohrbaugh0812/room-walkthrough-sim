// CS370 Final Project
// Fall 2024

#define STB_IMAGE_IMPLEMENTATION
#include "./common/stb_image.h"	// Sean Barrett's image loader - http://nothings.org/
#include <stdio.h>
#include <vector>
#include "./common/vgl.h"
#include "./common/objloader.h"
#include "./common/tangentspace.h"
#include "./common/utils.h"
#include "./common/vmath.h"
#include "lighting.h"
#include "house.h"
#include "./include/SDL/SDL.h"
#include "./include/SDL/SDL_mixer.h"
#define DEG2RAD (M_PI/180.0)

using namespace vmath;
using namespace std;

// Vertex array and buffer names
enum VAO_IDs {Cube, Cylinder, Triangle, Table, DiningChair, Drawer, Boombox, Door, CeilingFan, SwitchPlate, LightSwitch,
    Mirror, Frame, Lamp, CubeUV, CanNotCrushed, Cup, Bowl, Apple, Carpet, PictureFrame, PictureCanvas, SkySphere, Ground,
    GrassSmallBlob, GrassSmallBlobSparse, GrassMediumBlob, GrassMediumBlobSparse, TreeSmallBark, TreeSmallLeaves,
    TreeSmallBranches, TreeMediumBark, TreeMediumLeaves, TreeMediumBranches, WindowBlindHolder, WindowBlind, SpiderMan,
    Outlet, NumVAOs};
enum ObjBuffer_IDs {PosBuffer, NormBuffer, TexBuffer, TangBuffer, BiTangBuffer, NumObjBuffers};
enum Color_Buffer_IDs {RedCube, NumColorBuffers};
enum LightBuffer_IDs {LightBuffer, NumLightBuffers};
enum LightNames {SunLight, CeilingLight, LampLight};
enum MaterialBuffer_IDs {MaterialBuffer, NumMaterialBuffers};
enum MaterialNames {Wood, RedPlastic, Glass, Soda, WhitePlastic};
enum Textures {Blank, ShadowTex, CeilingFanTex, CeilingFanTexNorm, MirrorTex, DinnerTableTex, DinnerTableTexNorm,
    DiningChairTex, DiningChairTexNorm, DrawerTex, DrawerTexNorm, WallSurfaceTex, WallSurfaceTexNorm, FloorSurfaceTex,
    FloorSurfaceTexNorm, RoofSurfaceTex, RoofSurfaceTexNorm, DoorSurfaceTex, DoorSurfaceTexNorm, CanTex, BowlTex,
    BowlTexNorm, AppleTex, CarpetTex, CarpetTexNorm, PictureFrameTex, PictureCanvasTex, LampTex, SkyTex, GrassTex,
    GrassTexNorm, HGrassTex, HGrassTexNorm, GrassBlobTex, BarkTex, LeavesTex, BranchesTex, WindowBlindTex,
    WindowBlindFabricTex, SpiderManTex, OutletTex, OutletTexNorm, NumTextures};

// Vertex array and buffer objects
GLuint VAOs[NumVAOs];
GLuint ObjBuffers[NumVAOs][NumObjBuffers];
GLuint ColorBuffers[NumColorBuffers];
GLuint LightBuffers[NumLightBuffers];
GLuint MaterialBuffers[NumMaterialBuffers];
GLuint TextureIDs[NumTextures];
GLuint ShadowBuffer;

// Number of vertices in each object
GLint numVertices[NumVAOs];

// Number of component coordinates
GLint posCoords = 4;
GLint normCoords = 3;
GLint texCoords = 2;
GLint colCoords = 4;
GLint tangCoords = 3;
GLint bitangCoords = 3;

// Model files
const char * cubeFile = "../models/unitcube(oldnew).obj";
const char * cylinderFile = "../models/cylinder.obj";
const char * triangleFile = "../models/triangle.obj";
const char * tableFile = "../models/inside/floor_items/table.obj";
const char * diningChairFile = "../models/inside/floor_items/dining_chair.obj";
const char * drawerFile = "../models/inside/floor_items/drawer.obj";
const char * boomboxFile = "../models/inside/misc/boombox.obj";
const char * doorFile = "../models/inside/wall_items/door.obj";
const char * ceilingFanFile = "../models/inside/ceiling_items/ceiling_fan.obj";
const char * switchPlateFile = "../models/inside/wall_items/switch_plate.obj";
const char * lightSwitchFile = "../models/inside/wall_items/light_switch.obj";
const char * planeFile = "../models/outside/plane.obj";
const char * lampFile = "../models/inside/misc/lamp.obj";
const char * canNotCrushedFile = "../models/inside/table_items/can_not_crushed.obj";
const char * cupFile = "../models/inside/table_items/cup.obj";
const char * bowlFile = "../models/inside/table_items/bowl.obj";
const char * appleFile = "../models/inside/table_items/apple.obj";
const char * carpetFile = "../models/inside/floor_items/carpet.obj";
const char * pictureFrameFile = "../models/inside/wall_items/picture_frame.obj";
const char * pictureCanvasFile = "../models/inside/wall_items/picture_canvas.obj";
const char * skySphereFile = "../models/outside/sky_sphere.obj";
const char * groundFile = "../models/outside/ground.obj";
const char * grassSmallBlobFile = "../models/outside/grass/grass_small_blob.obj";
const char * grassSmallBlobSparseFile = "../models/outside/grass/grass_small_blob_sparse.obj";
const char * grassMediumBlobFile = "../models/outside/grass/grass_medium_blob.obj";
const char * grassMediumBlobSparseFile = "../models/outside/grass/grass_medium_blob_sparse.obj";
const char * treeSmallBarkFile = "../models/outside/trees/tree_small_bark.obj";
const char * treeSmallLeavesFile = "../models/outside/trees/tree_small_leaves.obj";
const char * treeSmallBranchesFile = "../models/outside/trees/tree_small_branches.obj";
const char * treeMediumBarkFile = "../models/outside/trees/tree_medium_bark.obj";
const char * treeMediumLeavesFile = "../models/outside/trees/tree_medium_leaves.obj";
const char * treeMediumBranchesFile = "../models/outside/trees/tree_medium_branches.obj";
const char * windowBlindHolderFile = "../models/inside/wall_items/window.obj";
const char * windowBlindFile = "../models/inside/wall_items/window_blind.obj";
const char * spiderManFile = "../models/spiderman.obj";
const char * outletFile = "../models/inside/wall_items/outlet.obj";

// Texture files
const char * ceilingFanTexFile = "../textures/inside/ceiling_items/ceiling_fan_texture.jpeg";
const char * ceilingFanTexNormFile = "../textures/inside/ceiling_items/ceiling_fan_texture_normal.png";
const char * dinnerTableTexFile = "../textures/inside/floor_items/stylized_table_texture.png";
const char * dinnerTableTexNormFile = "../textures/inside/floor_items/stylized_table_texture_normal.png";
const char * diningChairTexFile = "../textures/inside/floor_items/dining_chair_texture.jpg";
const char * diningChairTexNormFile = "../textures/inside/floor_items/dining_chair_texture_normal.jpg";
const char * drawerTexFile = "../textures/inside/floor_items/drawer_texture.jpg";
const char * drawerTexNormFile = "../textures/inside/floor_items/drawer_texture_normal.jpg";
const char * wallSurfaceTexFile = "../textures/inside/wall_items/wall-surface-texture.jpg";
const char * wallSurfaceTexNormFile = "../textures/inside/wall_items/wall-surface-texture-norm.jpg";
const char * floorSurfaceTexFile = "../textures/inside/floor_items/floor_surface_texture.jpg";
const char * floorSurfaceTexNormFile = "../textures/inside/floor_items/floor_surface_texture_norm.png";
const char * roof_surface_texture = "../textures/inside/ceiling_items/roof_surface_texture.jpg";
const char * roof_surface_texture_normal = "../textures/inside/ceiling_items/roof_surface_texture_normal.jpg";
const char * doorSurfaceTexFile = "../textures/inside/wall_items/door_surface_texture.jpeg";
const char * doorSurfaceTexNormFile = "../textures/inside/wall_items/door_surface_texture_norm.png";
const char * canTexFile = "../textures/inside/table_items/can_texture.png";
const char * bowlTexFile = "../textures/inside/table_items/bowl_texture.jpg";
const char * bowlTexNormFile = "../textures/inside/table_items/bowl_texture_normal.jpg";
const char * appleTexFile = "../textures/inside/table_items/apple_texture.jpg";
const char * carpetTexFile = "../textures/inside/floor_items/carpet_texture.jpeg";
const char * carpetTexNormFile = "../textures/inside/floor_items/carpet_texture_normal.png";
const char * pictureFrameTexFile = "../textures/inside/wall_items/picture_frame_texture.jpg";
const char * pictureCanvasTexFile = "../textures/inside/wall_items/picture_canvas_texture.png";
const char * lampTexFile = "../textures/inside/misc/lamp_texture.png";
const char * skyTexFile = "../textures/outside/sky_texture.hdr";
const char * grassTexFile = "../textures/outside/grass_texture.jpg";
const char * grassTexNormFile = "../textures/outside/grass_texture_normal.jpg";
const char * hGrassTexFile = "../textures/outside/grass_texture.png";
const char * hGrassTexNormFile = "../textures/outside/grass_texture_normal.png";
const char * grassBlobTexFile = "../textures/outside/grass/grass_blob_texture.jpg";
const char * treeBarkTexFile = "../textures/outside/trees/bark_texture.png";
const char * treeLeavesTexFile = "../textures/outside/trees/leaves_texture.png";
const char * treeBranchesTexFile = "../textures/outside/trees/branches_texture.png";
const char * windowBlindTexFile = "../textures/inside/wall_items/window_blind_texture.png";
const char * windowBlindFabricTexFile = "../textures/inside/wall_items/window_blind_fabric_texture.png";
const char * spiderManTexFile = "../textures/spiderman_texture.png";
const char * outletTexFile = "../textures/inside/wall_items/outlet_texture.png";
const char * outletTexNormFile = "../textures/inside/wall_items/outlet_texture_normal.png";

// Camera
vec3 eye = {3.0f, 0.0f, 0.0f};
vec3 center = {0.0f, 0.0f, 0.0f};
vec3 up = {0.0f, 1.0f, 0.0f};
vec3 mirror_eye = {0.0f, FLOOR_HEIGHT / 2 + 4.0f, FLOOR_LENGTH / 2 - WALL_WIDTH};
vec3 mirror_center = {0.0f, FLOOR_HEIGHT / 2 + 1.5f, 0.0f};
vec3 mirror_up = {0.0f, 1.0f, 0.0f};
GLfloat azimuth = 0.0f;
GLfloat daz = 2.0f;
GLfloat elevation = 90.0f;
GLfloat del = 2.0f;
GLfloat radius = 2.0f;
GLfloat dr = 0.1f;

// Shader variables
// Light shader program with shadows reference
GLuint phong_shadow_program;
GLuint phong_shadow_vPos;
GLuint phong_shadow_vNorm;
GLuint phong_shadow_proj_mat_loc;
GLuint phong_shadow_camera_mat_loc;
GLuint phong_shadow_norm_mat_loc;
GLuint phong_shadow_model_mat_loc;
GLuint phong_shadow_shad_proj_mat_loc;
GLuint phong_shadow_shad_cam_mat_loc;
GLuint phong_shadow_lights_block_idx;
GLuint phong_shadow_materials_block_idx;
GLuint phong_shadow_material_loc;
GLuint phong_shadow_num_lights_loc;
GLuint phong_shadow_light_on_loc;
GLuint phong_shadow_eye_loc;
const char *phong_shadow_vertex_shader = "../phongShadow.vert";
const char *phong_shadow_frag_shader = "../phongShadow.frag";

// Shader variables
// Light shader program reference
GLuint lighting_program;
GLuint lighting_vPos;
GLuint lighting_vNorm;
GLuint lighting_camera_mat_loc;
GLuint lighting_model_mat_loc;
GLuint lighting_proj_mat_loc;
GLuint lighting_norm_mat_loc;
GLuint lighting_lights_block_idx;
GLuint lighting_materials_block_idx;
GLuint lighting_material_loc;
GLuint lighting_num_lights_loc;
GLuint lighting_light_on_loc;
GLuint lighting_eye_loc;
const char *lighting_vertex_shader = "../lighting.vert";
const char *lighting_frag_shader = "../lighting.frag";

// Shadow shader program reference
GLuint shadow_program;
GLuint shadow_vPos;
GLuint shadow_proj_mat_loc;
GLuint shadow_camera_mat_loc;
GLuint shadow_model_mat_loc;
const char *shadow_vertex_shader = "../shadow.vert";
const char *shadow_frag_shader = "../shadow.frag";

// Multi-texture shader program reference
GLuint multi_tex_program;
// Multi-texture shader component references
GLuint multi_tex_vPos;
GLuint multi_tex_vTex;
GLuint multi_tex_proj_mat_loc;
GLuint multi_tex_camera_mat_loc;
GLuint multi_tex_model_mat_loc;
GLuint multi_tex_base_loc;
GLuint multi_tex_dirt_loc;
const char *multi_tex_vertex_shader = "../multiTex.vert";
const char *multi_tex_frag_shader = "../multiTex.frag";

// Bumpmapping shader program reference
GLuint bump_program;
GLuint bump_proj_mat_loc;
GLuint bump_camera_mat_loc;
GLuint bump_norm_mat_loc;
GLuint bump_model_mat_loc;
GLuint bump_vPos;
GLuint bump_vNorm;
GLuint bump_vTex;
GLuint bump_vTang;
GLuint bump_vBiTang;
GLuint bump_lights_block_idx;
GLuint bump_num_lights_loc;
GLuint bump_light_on_loc;
GLuint bump_eye_loc;
GLuint bump_base_loc;
GLuint bump_norm_loc;
const char *bump_vertex_shader = "../bumpTex.vert";
const char *bump_frag_shader = "../bumpTex.frag";

// Bump normal variant
GLboolean bump = false;

// Texture shader program reference
GLuint texture_program;
GLuint texture_vPos;
GLuint texture_vTex;
GLuint texture_proj_mat_loc;
GLuint texture_camera_mat_loc;
GLuint texture_model_mat_loc;
const char *texture_vertex_shader = "../texture.vert";
const char *texture_frag_shader = "../texture.frag";

// // Debug mirror shader
// GLuint debug_mirror_program;
// const char *debug_mirror_vertex_shader = "../debugMirror.vert";
// const char *debug_mirror_frag_shader = "../debugMirror.frag";

// Mirror flag
GLboolean mirror = false;

// Global state
mat4 proj_matrix;
mat4 camera_matrix;
mat4 normal_matrix;
mat4 model_matrix;
mat4 shadow_proj_matrix;
mat4 shadow_camera_matrix;

vector<LightProperties> Lights;
vector<MaterialProperties> Materials;
GLuint numLights = 0;
GLint lightOn[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// Global variables
#define ORTHOGRAPHIC 0
#define PERSPECTIVE 1
int proj = PERSPECTIVE;
vec3 eye_perspective = {3.0f, 5.0f, 0.0f};
vec3 center_perspective = {0.0f, 0.0f, 0.0f};
vec3 up_perspective = {0.0f, 1.0f, 0.0f};
GLfloat theta = 0.0f;
GLfloat phi = 0.0f;
GLfloat step_size = 0.2f;
float pitch = 0.0f;
float yaw = -90.0f;  // Initial yaw pointing forward
bool firstMouse = true;
float lastX, lastY;  // Initialize with the center of the screen
float sensitivity = 0.0001f;  // Adjust sensitivity as needed
GLboolean mouse_control_mode = false;
GLdouble elTime;
GLfloat fan_angle;
GLfloat fan_speed = 20.0f;
GLfloat max_fan_speed = 100.0f;
GLboolean fan_spin = false;
GLboolean is_outside = false;
GLboolean window_open = true;

// Shadow flag
GLuint shadow = false;

// Generic shader variables references
GLuint vPos;
GLuint vNorm;
GLuint model_mat_loc;

// Global screen dimensions
GLint ww,hh;

// Declare sound and music variables
Mix_Music* bg_music;
Mix_Chunk* floor_sound;
Mix_Chunk* grass_sound;

void display();
void render_scene();
void create_shadows( );
void create_mirror( );
void build_geometry();
void build_solid_color_buffer(GLuint num_vertices, vec4 color, GLuint buffer);
void build_materials( );
void build_lights( );
void build_shadows( );
void build_textures();
void build_mirror(GLuint m_texid);
void load_model(const char * filename, GLuint obj);
void load_bump_model(const char * filename, GLuint obj);
void load_texture(const char * filename, GLuint texID, GLint magFilter, GLint minFilter, GLint sWrap, GLint tWrap, bool mipMap, bool invert);
void draw_color_obj(GLuint obj, GLuint color);
void draw_mat_object(GLuint obj, GLuint material);
void draw_multi_tex_object(GLuint obj, GLuint texture1, GLuint texture2);
void draw_bump_object(GLuint obj, GLuint base_texture, GLuint normal_map, GLfloat tilingFactor);
void draw_mat_shadow_object(GLuint obj, GLuint material);
void draw_tex_object(GLuint obj, GLuint texture, GLfloat tilingFactor);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void update_first_person_perspective();
void initialize_sdl();
Mix_Music* load_music(const char* music_file);
Mix_Chunk* load_sound(const char* sound_file);
void play_footsteps_sound();
void renderQuad(GLuint shader, GLuint tex);
void draw_frame(GLuint obj);
void generate_grass_field(int numBlobs);
void populate_grass_field();
void generate_tree_field(int numTrees);
void populate_tree_field();

// Flag for ground
GLboolean high_quality_ground = false;

int main(int argc, char**argv)
{

    // Initialize SDL
    initialize_sdl();

    // Load sounds, music, etc.
    bg_music = load_music("./bin/sounds/background.mp3");
    floor_sound = load_sound("./bin/sounds/wooden_floor_sound.mp3");
    grass_sound = load_sound("./bin/sounds/grass_sound.mp3");

    // Just making sure that if the sounds can't be found in the /bin folder, the program can still run.
    if (bg_music == nullptr) printf("Background music not loaded...\n");
    if (floor_sound == nullptr) printf("Floor sound not loaded...\n");
    if (grass_sound == nullptr) printf("Grass sound not loaded...\n");

    // Play background music on loop (-1 for infinite loop)
    if (bg_music != nullptr) {
        Mix_PlayMusic(bg_music, -1);
    }

	// Create OpenGL window
	GLFWwindow* window = CreateWindow("Think Inside The Box");
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    } else {
        printf("OpenGL window successfully created\n");
    }

    // Store initial window size
    glfwGetFramebufferSize(window, &ww, &hh);

    // Register callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // Hide and capture cursor

    // Create geometry buffers
    build_geometry();
    // Create material buffers
    build_materials();
    // Create light buffers
    build_lights();
    // Create textures
    build_textures();
    // Create shadow buffer
    build_shadows();
    // Create mirror texture
    build_mirror(MirrorTex);

    // Load shaders
    // Load light shader with shadows
	ShaderInfo phong_shadow_shaders[] = { {GL_VERTEX_SHADER, phong_shadow_vertex_shader},{GL_FRAGMENT_SHADER, phong_shadow_frag_shader},{GL_NONE, NULL} };
    phong_shadow_program = LoadShaders(phong_shadow_shaders);
    phong_shadow_vPos = glGetAttribLocation(phong_shadow_program, "vPosition");
    phong_shadow_vNorm = glGetAttribLocation(phong_shadow_program, "vNormal");
    phong_shadow_camera_mat_loc = glGetUniformLocation(phong_shadow_program, "camera_matrix");
    phong_shadow_proj_mat_loc = glGetUniformLocation(phong_shadow_program, "proj_matrix");
    phong_shadow_norm_mat_loc = glGetUniformLocation(phong_shadow_program, "normal_matrix");
    phong_shadow_model_mat_loc = glGetUniformLocation(phong_shadow_program, "model_matrix");
    phong_shadow_shad_proj_mat_loc = glGetUniformLocation(phong_shadow_program, "light_proj_matrix");
    phong_shadow_shad_cam_mat_loc = glGetUniformLocation(phong_shadow_program, "light_cam_matrix");
    phong_shadow_lights_block_idx = glGetUniformBlockIndex(phong_shadow_program, "LightBuffer");
    phong_shadow_materials_block_idx = glGetUniformBlockIndex(phong_shadow_program, "MaterialBuffer");
    phong_shadow_material_loc = glGetUniformLocation(phong_shadow_program, "Material");
    phong_shadow_num_lights_loc = glGetUniformLocation(phong_shadow_program, "NumLights");
    phong_shadow_light_on_loc = glGetUniformLocation(phong_shadow_program, "LightOn");
    phong_shadow_eye_loc = glGetUniformLocation(phong_shadow_program, "EyePosition");

    // Load shadow shader
    ShaderInfo shadow_shaders[] = { {GL_VERTEX_SHADER, shadow_vertex_shader},{GL_FRAGMENT_SHADER, shadow_frag_shader},{GL_NONE, NULL} };
    shadow_program = LoadShaders(shadow_shaders);
    shadow_vPos = glGetAttribLocation(shadow_program, "vPosition");
    shadow_proj_mat_loc = glGetUniformLocation(shadow_program, "light_proj_matrix");
    shadow_camera_mat_loc = glGetUniformLocation(shadow_program, "light_cam_matrix");
    shadow_model_mat_loc = glGetUniformLocation(shadow_program, "model_matrix");

    // Load texture shaders
    ShaderInfo multi_tex_shaders[] = { {GL_VERTEX_SHADER, multi_tex_vertex_shader},{GL_FRAGMENT_SHADER, multi_tex_frag_shader},{GL_NONE, NULL} };
    multi_tex_program = LoadShaders(multi_tex_shaders);
    multi_tex_vPos = glGetAttribLocation(multi_tex_program, "vPosition");
    multi_tex_vTex = glGetAttribLocation(multi_tex_program, "vTexCoord");
    multi_tex_proj_mat_loc = glGetUniformLocation(multi_tex_program, "proj_matrix");
    multi_tex_camera_mat_loc = glGetUniformLocation(multi_tex_program, "camera_matrix");
    multi_tex_model_mat_loc = glGetUniformLocation(multi_tex_program, "model_matrix");
    multi_tex_base_loc = glGetUniformLocation(multi_tex_program, "baseMap");
    multi_tex_dirt_loc = glGetUniformLocation(multi_tex_program, "dirtMap");

    // Load bump shader
    ShaderInfo bump_shaders[] = { {GL_VERTEX_SHADER, bump_vertex_shader},{GL_FRAGMENT_SHADER, bump_frag_shader},{GL_NONE, NULL} };
    bump_program = LoadShaders(bump_shaders);
    bump_vPos = glGetAttribLocation(bump_program, "vPosition");
    bump_vNorm = glGetAttribLocation(bump_program, "vNormal");
    bump_vTex = glGetAttribLocation(bump_program, "vTexCoord");
    bump_vTang = glGetAttribLocation(bump_program, "vTangent");
    bump_vBiTang = glGetAttribLocation(bump_program, "vBiTangent");
    bump_proj_mat_loc = glGetUniformLocation(bump_program, "proj_matrix");
    bump_camera_mat_loc = glGetUniformLocation(bump_program, "camera_matrix");
    bump_norm_mat_loc = glGetUniformLocation(bump_program, "normal_matrix");
    bump_model_mat_loc = glGetUniformLocation(bump_program, "model_matrix");
    bump_lights_block_idx = glGetUniformBlockIndex(bump_program, "LightBuffer");
    bump_num_lights_loc = glGetUniformLocation(bump_program, "NumLights");
    bump_light_on_loc = glGetUniformLocation(bump_program, "LightOn");
    bump_eye_loc = glGetUniformLocation(bump_program, "EyePosition");
    bump_base_loc = glGetUniformLocation(bump_program, "baseMap");
    bump_norm_loc = glGetUniformLocation(bump_program, "normalMap");

    // Load texture shaders
    ShaderInfo texture_shaders[] = { {GL_VERTEX_SHADER, texture_vertex_shader},{GL_FRAGMENT_SHADER, texture_frag_shader},{GL_NONE, NULL} };
    texture_program = LoadShaders(texture_shaders);
    texture_vPos = glGetAttribLocation(texture_program, "vPosition");
    texture_vTex = glGetAttribLocation(texture_program, "vTexCoord");
    texture_proj_mat_loc = glGetUniformLocation(texture_program, "proj_matrix");
    texture_camera_mat_loc = glGetUniformLocation(texture_program, "camera_matrix");
    texture_model_mat_loc = glGetUniformLocation(texture_program, "model_matrix");

    // Load shaders
    // Load light shader
    ShaderInfo lighting_shaders[] = { {GL_VERTEX_SHADER, lighting_vertex_shader},{GL_FRAGMENT_SHADER, lighting_frag_shader},{GL_NONE, NULL} };
    lighting_program = LoadShaders(lighting_shaders);
    lighting_vPos = glGetAttribLocation(lighting_program, "vPosition");
    lighting_vNorm = glGetAttribLocation(lighting_program, "vNormal");
    lighting_proj_mat_loc = glGetUniformLocation(lighting_program, "proj_matrix");
    lighting_camera_mat_loc = glGetUniformLocation(lighting_program, "camera_matrix");
    lighting_norm_mat_loc = glGetUniformLocation(lighting_program, "normal_matrix");
    lighting_model_mat_loc = glGetUniformLocation(lighting_program, "model_matrix");
    lighting_lights_block_idx = glGetUniformBlockIndex(lighting_program, "LightBuffer");
    lighting_materials_block_idx = glGetUniformBlockIndex(lighting_program, "MaterialBuffer");
    lighting_material_loc = glGetUniformLocation(lighting_program, "Material");
    lighting_num_lights_loc = glGetUniformLocation(lighting_program, "NumLights");
    lighting_light_on_loc = glGetUniformLocation(lighting_program, "LightOn");
    lighting_eye_loc = glGetUniformLocation(lighting_program, "EyePosition");

    // // Load debug mirror shader
    // ShaderInfo debug_mirror_shaders[] = { {GL_VERTEX_SHADER, debug_mirror_vertex_shader},{GL_FRAGMENT_SHADER, debug_mirror_frag_shader},{GL_NONE, NULL} };
    // debug_mirror_program = LoadShaders(debug_mirror_shaders);

    // Enable depth test
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set Initial camera position
    GLfloat x, y, z;
    x = (GLfloat)(radius*sin(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
    y = (GLfloat)(radius*cos(elevation*DEG2RAD));
    z = (GLfloat)(radius*cos(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
    eye = vec3(x, y, z);

    elTime = glfwGetTime();

    // Generates the positions for all the grass blobs and trees. These values being passed can be lowered to improve performance.
    generate_grass_field(450);
    generate_tree_field(70);

    // Start loop
    while ( !glfwWindowShouldClose( window ) ) {
        glCullFace(GL_FRONT);
        create_shadows();
        glCullFace(GL_BACK);

        // Load environment map
        create_mirror();
        // Uncomment instead of display() to view mirror map for debugging
        // renderQuad(debug_mirror_program, MirrorTex);
        // Draw graphics
        display();

        double curTime = glfwGetTime();

        if (fan_spin) fan_angle += fan_speed * (360.0f/ 60.0f) * (curTime - elTime);

        update_first_person_perspective();

        elTime = curTime;

        // Update other events like input handling
        glfwPollEvents();
        // Swap buffer onto screen
        glfwSwapBuffers( window );
    }

    // Clean up sounds, music, etc.
    if (bg_music != nullptr) Mix_FreeMusic(bg_music);
    if (floor_sound != nullptr) Mix_FreeChunk(floor_sound);
    if (grass_sound != nullptr) Mix_FreeChunk(grass_sound);

    // Quit SDL_mixer and SDL
    Mix_CloseAudio();
    SDL_Quit();

    // Close window
    glfwTerminate();
    return 0;

}
GLfloat xratio = 1.0f;

void display( )
{
    // Declare projection and camera matrices
    proj_matrix = mat4().identity();
    camera_matrix = mat4().identity();

	// Clear window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute anisotropic scaling
    xratio = 1.0f;
    GLfloat yratio = 1.0f;
    // If taller than wide adjust y
    if (ww <= hh)
    {
        yratio = (GLfloat)hh / (GLfloat)ww;
    }
        // If wider than tall adjust x
    else if (hh <= ww)
    {
        xratio = (GLfloat)ww / (GLfloat)hh;
    }

    // DEFAULT ORTHOGRAPHIC PROJECTION
    if (proj == ORTHOGRAPHIC) {
        GLfloat scale = 10.0f * radius; // Adjust zoom with radius
        proj_matrix = ortho(-scale*xratio, scale*xratio, -scale*yratio, scale*yratio, -100.0f, 100.0f);

        // Set camera matrix
        camera_matrix = lookat(eye, center, up);
    } else if (proj == PERSPECTIVE) {
        proj_matrix = perspective(radians(-90.0f), xratio, 0.5f, 500.0f);
        // proj_matrix = frustum(-1.0f * xratio, 1.0f * xratio, -1.0f * yratio, 1.0f * yratio, 0.5f, 50.0f);
        camera_matrix = lookat(eye_perspective, center_perspective, up_perspective);
    }


    // Render objects
	render_scene();

	// Flush pipeline
	glFlush();
}

void render_scene( )
{
    // Declare transformation matrices
    model_matrix = mat4().identity();
    mat4 scale_matrix = mat4().identity();
    mat4 rot_matrix = mat4().identity();
    mat4 trans_matrix = mat4().identity();

    // _____Outside_____

    // Draw sky
    glEnable(GL_CULL_FACE);     // Enable face culling
    glCullFace(GL_FRONT);       // Cull front faces to render the inside
    glDepthMask(GL_FALSE);      // Disable depth writing

    model_matrix = translate(eye_perspective[0], WALL_HEIGHT / 2, eye_perspective[2]) * rotate(180.0f, 1.0f, 0.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_tex_object(SkySphere, SkyTex, 1.0f);

    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    // Draws the ground outside, depending on what the flag for the ground is set to.
    if (high_quality_ground) {
        model_matrix = translate(0.0f, -1.0f, 0.0f) * scale(SKY_DOME_RADIUS, 1.0f, SKY_DOME_RADIUS);
        normal_matrix = model_matrix.inverse().transpose();
        draw_bump_object(Ground, HGrassTex, HGrassTexNorm, 50.0f);
    } else {
        model_matrix = translate(0.0f, -1.0f, 0.0f) * scale(SKY_DOME_RADIUS, 0.1f, SKY_DOME_RADIUS);
        normal_matrix = model_matrix.inverse().transpose();
        draw_bump_object(CubeUV, GrassTex, GrassTexNorm, 15.0f);
    }

    // Draw trees
    populate_tree_field();

    // Draw grass blobs
    populate_grass_field();

    // _____Inside_____

    // Floor
    model_matrix = scale(FLOOR_WIDTH, FLOOR_HEIGHT, FLOOR_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, FloorSurfaceTex, FloorSurfaceTexNorm, 1.0f);

    // Window

    // Draw border for window

    // Bottom
    model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 3, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2 - WALL_HEIGHT / 4, 0.0f) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * scale(8.0f, 0.25f, 0.5f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(Cube, WhitePlastic);
    // Top
    model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 3, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2 + WALL_HEIGHT / 4, 0.0f) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * scale(8.0f, 0.25f, 0.5f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(Cube, WhitePlastic);
    // Left
    model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 3, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, SHORT_WALL_LENGTH / 6) * rotate(90.0f, 0.0f, 0.0f, 1.0f) * scale(4.0f, 0.25f, 0.5f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(Cube, WhitePlastic);
    // Right
    model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 3, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, -SHORT_WALL_LENGTH / 6) * rotate(90.0f, 0.0f, 0.0f, 1.0f) * scale(4.0f, 0.25f, 0.5f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(Cube, WhitePlastic);

    // Everything regarding the stencil buffer below is for the window, as using this allows us to "cut out" a hole in the wall.
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);  // Clear the stencil buffer at the start of each frame

    // Configure stencil test for window
    glStencilFunc(GL_ALWAYS, 1, 0xFF);  // Always pass stencil test, set value to 1 where window is drawn
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  // Replace stencil value where the window is drawn
    glStencilMask(0xFF);  // Enable writing to the stencil buffer
    glDepthMask(GL_FALSE);  // Disable depth writing (we only care about the stencil buffer for now)

    // Render the window geometry (this will write to the stencil buffer)
    model_matrix = translate(-FLOOR_WIDTH / 2, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, 0.0f) * scale(WALL_WIDTH, WALL_HEIGHT / 2, SHORT_WALL_LENGTH / 3);
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(Cube, Glass);  // Render the window with the glass material

    glDepthMask(GL_TRUE);  // Re-enable depth writing after rendering the window

    // Set up stencil test to only pass where stencil value is 0
    glStencilFunc(GL_EQUAL, 0, 0xFF);  // Only render where the stencil value is 0
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);  // Don't change stencil buffer during this pass
    glDepthMask(GL_TRUE);  // Enable depth writing to affect depth buffer

    // Walls

    // Back wall
    model_matrix = translate(-FLOOR_WIDTH / 2, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, 0.0f) * scale(WALL_WIDTH, WALL_HEIGHT, SHORT_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, WallSurfaceTex, WallSurfaceTexNorm, 1.0f);
    model_matrix = translate(-FLOOR_WIDTH / 2 - WALL_WIDTH, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH, WALL_HEIGHT, SHORT_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, WallSurfaceTex, WallSurfaceTexNorm, 1.0f);
    // Disables the stencil test since the window and walls have been rendered.
    glDisable(GL_STENCIL_TEST);

    // Front wall
    model_matrix = translate(FLOOR_WIDTH / 2, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH, WALL_HEIGHT, SHORT_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, WallSurfaceTex, WallSurfaceTexNorm, 1.0f);
    model_matrix = translate(FLOOR_WIDTH / 2 + WALL_WIDTH, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, 0.0f) * scale(WALL_WIDTH, WALL_HEIGHT, SHORT_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, WallSurfaceTex, WallSurfaceTexNorm, 1.0f);

    // Right wall
    model_matrix = translate(0.0f, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, FLOOR_LENGTH / 2) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * scale(WALL_WIDTH, WALL_HEIGHT, LONG_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, WallSurfaceTex, WallSurfaceTexNorm, 1.0f);
    model_matrix = translate(0.0f, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, FLOOR_LENGTH / 2 + WALL_WIDTH) * rotate(180.0f, 0.0f, 1.0f, 0.0f) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * scale(WALL_WIDTH, WALL_HEIGHT, LONG_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, WallSurfaceTex, WallSurfaceTexNorm, 1.0f);

    // Left wall
    model_matrix = translate(0.0f, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, -FLOOR_LENGTH / 2) * rotate(180.0f, 1.0f, 0.0f, 0.0f) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * scale(WALL_WIDTH, WALL_HEIGHT, LONG_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, WallSurfaceTex, WallSurfaceTexNorm, 1.0f);
    model_matrix = translate(0.0f, WALL_HEIGHT / 2 + FLOOR_HEIGHT / 2, -FLOOR_LENGTH / 2 - WALL_WIDTH) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * scale(WALL_WIDTH, WALL_HEIGHT, LONG_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, WallSurfaceTex, WallSurfaceTexNorm, 1.0f);

    // Window frame

    model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 1.5f, WALL_HEIGHT / 2 + WALL_HEIGHT / 3, 0.0f) * scale(0.25f, 0.25f, 0.6f) * rotate(90.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_tex_object(WindowBlindHolder, WindowBlindTex, 1.0f);
    if (window_open) {
        model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 1.5f, WALL_HEIGHT / 2 + WALL_HEIGHT / 4, 0.0f) * scale(0.3f, 0.2f, 0.7f) * rotate(90.0f, 0.0f, 1.0f, 0.0f);
        normal_matrix = model_matrix.inverse().transpose();
        draw_tex_object(WindowBlind, WindowBlindFabricTex, 1.0f);
    } else {
        model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 1.5f, WALL_HEIGHT / 2 + 0.3f, 0.0f) * scale(0.3f, 1.0f, 0.7f) * rotate(90.0f, 0.0f, 1.0f, 0.0f);
        normal_matrix = model_matrix.inverse().transpose();
        draw_tex_object(WindowBlind, WindowBlindFabricTex, 1.0f);
    }

    // Roof

    float roof_height =  ((FLOOR_LENGTH / 2) / cos(DEG2RAD * (90.0f - ROOF_ANGLE)));
    float roof_vertical_distance_from_wall = ((FLOOR_LENGTH / 2) * tan(DEG2RAD * (90.0f - ROOF_ANGLE)));
    // Long roof panels
    model_matrix = translate(0.0f, WALL_HEIGHT + FLOOR_HEIGHT / 2 + roof_vertical_distance_from_wall / 2, -FLOOR_LENGTH / 4) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * rotate(ROOF_ANGLE, 0.0f, 0.0f, 1.0f) * rotate(180.0f, 0.0f, 1.0f, 0.0f) * scale(ROOF_WIDTH, roof_height, LONG_ROOF_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, RoofSurfaceTex, RoofSurfaceTexNorm, 1.0f);
    model_matrix = translate(0.0f, WALL_HEIGHT + FLOOR_HEIGHT / 2 + roof_vertical_distance_from_wall / 2, FLOOR_LENGTH / 4) * rotate(-90.0f, 0.0f, 1.0f, 0.0f) * rotate(ROOF_ANGLE, 0.0f, 0.0f, 1.0f) * rotate(180.0f, 0.0f, 1.0f, 0.0f) *  scale(ROOF_WIDTH, roof_height, LONG_ROOF_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, RoofSurfaceTex, RoofSurfaceTexNorm, 1.0f);
    // Short roof panels
    model_matrix = translate(FLOOR_WIDTH / 4 + 2.0f, WALL_HEIGHT + FLOOR_HEIGHT / 2 + roof_vertical_distance_from_wall / 2, 0.0f) * rotate(ROOF_ANGLE, 0.0f, 0.0f, 1.0f) * rotate(180.0f, 0.0f, 1.0f, 0.0f) * scale(ROOF_WIDTH, roof_height, SHORT_ROOF_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, RoofSurfaceTex, RoofSurfaceTexNorm, 1.0f);
    model_matrix = translate(-FLOOR_WIDTH / 4 - 2.0f, WALL_HEIGHT + FLOOR_HEIGHT / 2 + roof_vertical_distance_from_wall / 2, 0.0f) * rotate(180.0f, 0.0f, 1.0f, 0.0f) * rotate(ROOF_ANGLE, 0.0f, 0.0f, 1.0f) * rotate(180.0f, 0.0f, 1.0f, 0.0f) *  scale(ROOF_WIDTH, roof_height, SHORT_ROOF_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, RoofSurfaceTex, RoofSurfaceTexNorm, 1.0f);

    // Support
    model_matrix = translate(0.0f, WALL_HEIGHT + roof_vertical_distance_from_wall / 1.7f, 0.0f) * rotate(180.0f, 0.0f, 1.0f, 0.0f) * scale(FLOOR_WIDTH / 1.75f, 1.0f, 1.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, RoofSurfaceTex, RoofSurfaceTexNorm, 1.0f);

    // Trim

    // Floor

    // Left Wall (Art)
    model_matrix = translate(FLOOR_WIDTH / 2 - WALL_WIDTH / 2, FLOOR_HEIGHT / 2, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH / 2, 1.0f, SHORT_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, FloorSurfaceTex, FloorSurfaceTexNorm, 1.0f);
    // Back Wall (Mirror)
    model_matrix = translate(0.0f, FLOOR_HEIGHT / 2, FLOOR_LENGTH / 2 - WALL_WIDTH / 2) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH / 2, 1.0f, LONG_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, FloorSurfaceTex, FloorSurfaceTexNorm, 1.0f);
    // Right Wall (Window)
    model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 2, FLOOR_HEIGHT / 2, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH / 2, 1.0f, SHORT_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, FloorSurfaceTex, FloorSurfaceTexNorm, 1.0f);
    // Front Wall (Door)
    // Left of Door
    model_matrix = translate(-FLOOR_WIDTH / 3.6f, FLOOR_HEIGHT / 2, -FLOOR_LENGTH / 2 + WALL_WIDTH / 2) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH / 2, 1.0f, LONG_WALL_LENGTH / 2.3f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, FloorSurfaceTex, FloorSurfaceTexNorm, 1.0f);
    // Right of Door
    model_matrix = translate(FLOOR_WIDTH / 3.6f, FLOOR_HEIGHT / 2, -FLOOR_LENGTH / 2 + WALL_WIDTH / 2) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH / 2, 1.0f, LONG_WALL_LENGTH / 2.3f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, FloorSurfaceTex, FloorSurfaceTexNorm, 1.0f);

    // Roof

    // Left Wall (Art)
    model_matrix = translate(FLOOR_WIDTH / 2 - WALL_WIDTH / 2, WALL_HEIGHT + FLOOR_HEIGHT / 2, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH / 2, 1.0f, SHORT_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, RoofSurfaceTex, RoofSurfaceTexNorm, 1.0f);
    // Back Wall (Mirror)
    model_matrix = translate(0.0f, WALL_HEIGHT + FLOOR_HEIGHT / 2, FLOOR_LENGTH / 2 - WALL_WIDTH / 2) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH / 2, 1.0f, LONG_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, RoofSurfaceTex, RoofSurfaceTexNorm, 1.0f);
    // Right Wall (Window)
    model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 2, WALL_HEIGHT + FLOOR_HEIGHT / 2, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH / 2, 1.0f, SHORT_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, RoofSurfaceTex, RoofSurfaceTexNorm, 1.0f);
    // Front Wall (Door)
    model_matrix = translate(0.0f, WALL_HEIGHT + FLOOR_HEIGHT / 2, -FLOOR_LENGTH / 2 + WALL_WIDTH / 2) * rotate(90.0f, 0.0f, 1.0f, 0.0f) * rotate(180.0f, 0.0f, 0.0f, 1.0f) * scale(WALL_WIDTH / 2, 1.0f, LONG_WALL_LENGTH);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CubeUV, RoofSurfaceTex, RoofSurfaceTexNorm, 1.0f);

    // Objects

    // Fan and Light
    model_matrix = translate(0.0f, FLOOR_HEIGHT / 2 + WALL_HEIGHT + roof_vertical_distance_from_wall / 2 - 0.5f, 0.0f) * rotate(fan_angle, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(CeilingFan, CeilingFanTex, CeilingFanTexNorm, 1.0f);

    // Fan and Light Switches
    model_matrix = translate(3.0f, FLOOR_HEIGHT / 2 + 4.0f, -FLOOR_LENGTH / 2 + 0.5f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(SwitchPlate, DoorSurfaceTex, DoorSurfaceTexNorm, 1.0f);
    if (fan_spin) {
        model_matrix = translate(3.0f, FLOOR_HEIGHT / 2 + 4.2f, -FLOOR_LENGTH / 2 + 0.5f) * rotate(-45.0f, 1.0f, 0.0f, 0.0f);
    } else {
        model_matrix = translate(3.0f, FLOOR_HEIGHT / 2 + 4.2f, -FLOOR_LENGTH / 2 + 0.5f) * rotate(45.0f, 1.0f, 0.0f, 0.0f);
    }
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(LightSwitch,  DoorSurfaceTex, DoorSurfaceTexNorm, 1.0f);

    if (lightOn[CeilingLight] == 1) {
        model_matrix = translate(3.0f, FLOOR_HEIGHT / 2 + 3.8f, -FLOOR_LENGTH / 2 + 0.5f) * rotate(-45.0f, 1.0f, 0.0f, 0.0f);
    } else {
        model_matrix = translate(3.0f, FLOOR_HEIGHT / 2 + 3.8f, -FLOOR_LENGTH / 2 + 0.5f) * rotate(45.0f, 1.0f, 0.0f, 0.0f);
    }
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(LightSwitch, DoorSurfaceTex, DoorSurfaceTexNorm, 1.0f);

    // Door

    // Inside
    model_matrix = translate(0.0f, FLOOR_HEIGHT / 2 + 3.5f, -FLOOR_LENGTH / 2 + 0.1f) * rotate(90.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(Door, DoorSurfaceTex, DoorSurfaceTexNorm, 1.0f);
    // Outside
    model_matrix = translate(0.0f, FLOOR_HEIGHT / 2 + 3.5f, -FLOOR_LENGTH / 2 - WALL_WIDTH * 1.25f) * rotate(270.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(Door, DoorSurfaceTex, DoorSurfaceTexNorm, 1.0f);

    // Main Furniture

    // Table
    model_matrix = translate(FLOOR_WIDTH / 4, FLOOR_HEIGHT / 2 + WALL_HEIGHT / 2 - 0.8f, FLOOR_LENGTH / 4);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(Table, DinnerTableTex, DinnerTableTexNorm, 1.0f);
    // Chairs
    GLfloat chair_offset_x = 2.0f;
    GLfloat chair_offset_z = 1.0f;
    model_matrix = translate(FLOOR_WIDTH / 4 - chair_offset_x, FLOOR_HEIGHT / 2 + WALL_HEIGHT / 2 - 1.5f, FLOOR_LENGTH / 4) * rotate(90.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(DiningChair, DiningChairTex, DiningChairTexNorm, 1.0f);
    model_matrix = translate(FLOOR_WIDTH / 4, FLOOR_HEIGHT / 2 + WALL_HEIGHT / 2 - 1.5f, FLOOR_LENGTH / 4 + chair_offset_z * 3.0f) * rotate(180.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(DiningChair, DiningChairTex, DiningChairTexNorm, 1.0f);
    model_matrix = translate(FLOOR_WIDTH / 4 + chair_offset_x, FLOOR_HEIGHT / 2 + WALL_HEIGHT / 2 - 1.5f, FLOOR_LENGTH / 4) * rotate(270.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(DiningChair, DiningChairTex, DiningChairTexNorm, 1.0f);
    model_matrix = translate(FLOOR_WIDTH / 4, FLOOR_HEIGHT / 2 + WALL_HEIGHT / 2 - 1.5f, FLOOR_LENGTH / 4 - chair_offset_z * 3.0f) * rotate(0.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(DiningChair, DiningChairTex, DiningChairTexNorm, 1.0f);
    // Drawer
    model_matrix = translate(-FLOOR_WIDTH / 2 + 3.0f, FLOOR_HEIGHT / 2 + 1.0f, -FLOOR_LENGTH / 2 + 2.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(Drawer, DrawerTex, DrawerTexNorm, 1.0f);
    // Lamp
    model_matrix = translate(-FLOOR_WIDTH / 2 + 3.0f, FLOOR_HEIGHT / 2 + 1.0f + 3.0f, -FLOOR_LENGTH / 2 + 1.5f) * rotate(90.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_tex_object(Lamp, LampTex, 1.0f);

    // Table items

    // Can
    model_matrix = translate(FLOOR_WIDTH / 4, FLOOR_HEIGHT / 2 + 3.7f, FLOOR_LENGTH / 3);
    draw_tex_object(CanNotCrushed, CanTex, 1.0f);
    // Cup
    glDepthMask(GL_FALSE);
    model_matrix = translate(FLOOR_WIDTH / 4 + 0.5f, FLOOR_HEIGHT / 2 + 3.7f, FLOOR_LENGTH / 3);
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(Cup, Glass);
    glDepthMask(GL_TRUE);
    // Soda
    glDepthMask(GL_FALSE);
    model_matrix = translate(FLOOR_WIDTH / 4 + 0.5f, FLOOR_HEIGHT / 2 + 3.7f, FLOOR_LENGTH / 3) * scale(0.2f, 0.3f, 0.2f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(Cylinder, Soda);
    glDepthMask(GL_TRUE);
    // Bowl
    model_matrix = translate(FLOOR_WIDTH / 4, FLOOR_HEIGHT / 2 + 3.5f, FLOOR_LENGTH / 4);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(Bowl, BowlTex, BowlTexNorm, 1.0f);
    // Apples
    model_matrix = translate(FLOOR_WIDTH / 4, FLOOR_HEIGHT / 2 + 3.6f, FLOOR_LENGTH / 4);
    draw_tex_object(Apple, AppleTex, 1.0f);
    model_matrix = translate(FLOOR_WIDTH / 4 + 0.35f, FLOOR_HEIGHT / 2 + 3.65f, FLOOR_LENGTH / 4) * rotate(45.0f, 0.0f, 1.0f, 0.0f);
    draw_tex_object(Apple, AppleTex, 1.0f);
    model_matrix = translate(FLOOR_WIDTH / 4 - 0.35f, FLOOR_HEIGHT / 2 + 3.65f, FLOOR_LENGTH / 4) * rotate(90.0f, 0.0f, 1.0f, 0.0f);
    draw_tex_object(Apple, AppleTex, 1.0f);
    model_matrix = translate(FLOOR_WIDTH / 4, FLOOR_HEIGHT / 2 + 3.65f, FLOOR_LENGTH / 4 + 0.35f) * rotate(135.0f, 0.0f, 1.0f, 0.0f);
    draw_tex_object(Apple, AppleTex, 1.0f);
    model_matrix = translate(FLOOR_WIDTH / 4, FLOOR_HEIGHT / 2 + 3.65f, FLOOR_LENGTH / 4 - 0.35f) * rotate(180.0f, 0.0f, 1.0f, 0.0f);
    draw_tex_object(Apple, AppleTex, 1.0f);

    // Picture Frame
    model_matrix = translate(FLOOR_WIDTH / 2 - WALL_WIDTH / 2, FLOOR_HEIGHT / 2 + WALL_HEIGHT / 2, 0.0f) * rotate(-90.0f, 0.0f, 1.0f, 0.0f);
    draw_tex_object(PictureFrame, PictureFrameTex, 1.0f);
    draw_tex_object(PictureCanvas, PictureCanvasTex, 1.0f);

    // Carpet
    model_matrix = translate(0.0f, FLOOR_HEIGHT, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(Carpet, CarpetTex, CarpetTexNorm, 1.0f);

    // Outlets
    model_matrix = translate(FLOOR_WIDTH / 2 - WALL_WIDTH / 2,  FLOOR_HEIGHT * 4, -FLOOR_LENGTH / 2 + WALL_WIDTH * 4)  * rotate(90.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(Outlet, OutletTex, OutletTexNorm, 1.0f);
    model_matrix = translate(-FLOOR_WIDTH / 2 + WALL_WIDTH / 2,  FLOOR_HEIGHT * 4, FLOOR_LENGTH / 2 - WALL_WIDTH * 4)  * rotate(-90.0f, 0.0f, 1.0f, 0.0f);
    normal_matrix = model_matrix.inverse().transpose();
    draw_bump_object(Outlet, OutletTex, OutletTexNorm, 1.0f);

    // // Player model
    // model_matrix = translate(eye_perspective[0], FLOOR_HEIGHT / 2, eye_perspective[2] - 0.7f) * rotate(pitch, 1.0f, 0.0f, 0.0f);
    // normal_matrix = model_matrix.inverse().transpose();
    // draw_tex_object(SpiderMan, SpiderManTex, 1.0f);

    // Draw mirror
    if (!mirror) {
        // Render mirror in scene
        // Set mirror transformation
        model_matrix = translate(mirror_eye) * rotate(-90.0f, 1.0f, 0.0f, 0.0f) * scale(2.0f, 1.0f, 2.0f);
        // Draw textured mirror
        draw_tex_object(Mirror, MirrorTex, 1.0f);

        // Draw border for mirror

        // Bottom
        model_matrix = translate(mirror_eye[0], mirror_eye[1] - 2.0f, mirror_eye[2]) * scale(4.0f, 0.25f, 0.25f);
        normal_matrix = model_matrix.inverse().transpose();
        draw_mat_object(Cube, WhitePlastic);
        // Top
        model_matrix = translate(mirror_eye[0], mirror_eye[1] + 2.0f, mirror_eye[2]) * scale(4.0f, 0.25f, 0.25f);
        normal_matrix = model_matrix.inverse().transpose();
        draw_mat_object(Cube, WhitePlastic);
        // Left
        model_matrix = translate(mirror_eye[0] + 2.0f, mirror_eye[1], mirror_eye[2]) * rotate(90.0f, 0.0f, 0.0f, 1.0f) * scale(4.0f, 0.25f, 0.25f);
        normal_matrix = model_matrix.inverse().transpose();
        draw_mat_object(Cube, WhitePlastic);
        // Right
        model_matrix = translate(mirror_eye[0] - 2.0f, mirror_eye[1], mirror_eye[2]) * rotate(90.0f, 0.0f, 0.0f, 1.0f) * scale(4.0f, 0.25f, 0.25f);
        normal_matrix = model_matrix.inverse().transpose();
        draw_mat_object(Cube, WhitePlastic);
    }
}

void create_shadows( ){
    // TODO: Set shadow projection matrix
    shadow_proj_matrix = frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 20.0);

    // TODO: Set shadow camera matrix based on light position and direction
    vec3 leye = {Lights[0].position[0], Lights[0].position[1], Lights[0].position[2]};
    vec3 ldir = {Lights[0].direction[0], Lights[0].direction[1], Lights[0].direction[2]};
    vec3 lup = {0.0f, 1.0f, 0.0f};
    vec3 lcenter = leye + ldir;
    shadow_camera_matrix = lookat(leye, lcenter, lup);

    // Change viewport to match shadow framebuffer size
    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, ShadowBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    // TODO: Render shadow scene
    shadow = true;
    render_scene();
    shadow = false;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport
    glViewport(0, 0, ww, hh);
}

void create_mirror( ) {
    // Clear framebuffer for mirror rendering pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set mirror projection matrix
    // proj_matrix = frustum(-0.2f, 0.2f, -0.2f, 0.2f, 0.2f, 100.0f);
    proj_matrix = perspective(radians(-90.0f), xratio, 0.5f, 500.0f);

    // Set mirror camera matrix

    // mirrors normal
    vec3 mirror_normal = normalize(mirror_eye - mirror_center);

    // Reflect the eye and center positions across the mirror plane
    vec3 reflected_eye = eye_perspective - 2.0f * dot(eye - mirror_center, mirror_normal) * mirror_normal;
    vec3 reflected_center = center_perspective - 2.0f * dot(center_perspective - mirror_center, mirror_normal) * mirror_normal;

    // Set up the mirror view matrix
    camera_matrix = lookat(reflected_eye, reflected_center, mirror_up);

    // Render mirror scene (without mirror)
    mirror = true;
    render_scene();
    glFlush();
    mirror = false;

    // Activate texture unit 0
    glActiveTexture(GL_TEXTURE0);
    // Bind mirror texture
    glBindTexture(GL_TEXTURE_2D, TextureIDs[MirrorTex]);
    // Copy framebuffer into mirror texture
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, ww, hh, 0);
}

void build_geometry( )
{
    // Generate vertex arrays and buffers
    glGenVertexArrays(NumVAOs, VAOs);

    // Load models
    load_model(cubeFile, Cube);
    load_model(cylinderFile, Cylinder);
    load_bump_model(triangleFile, Triangle);
    load_bump_model(tableFile, Table);
    load_bump_model(diningChairFile, DiningChair);
    load_bump_model(drawerFile, Drawer);
    load_model(boomboxFile, Boombox);
    load_bump_model(doorFile, Door);
    load_bump_model(ceilingFanFile, CeilingFan);
    load_bump_model(switchPlateFile, SwitchPlate);
    load_bump_model(lightSwitchFile, LightSwitch);
    load_model(planeFile, Mirror);
    load_model(lampFile, Lamp);
    load_bump_model(cubeFile, CubeUV);
    load_model(canNotCrushedFile, CanNotCrushed);
    load_model(cupFile, Cup);
    load_bump_model(bowlFile, Bowl);
    load_model(appleFile, Apple);
    load_bump_model(carpetFile, Carpet);
    load_model(pictureFrameFile, PictureFrame);
    load_model(pictureCanvasFile, PictureCanvas);
    load_model(skySphereFile, SkySphere);
    if (high_quality_ground) load_bump_model(groundFile, Ground);
    load_model(grassSmallBlobFile, GrassSmallBlob);
    load_model(grassSmallBlobSparseFile, GrassSmallBlobSparse);
    load_model(grassMediumBlobFile, GrassMediumBlob);
    load_model(grassMediumBlobSparseFile, GrassMediumBlobSparse);
    load_model(treeSmallBarkFile, TreeSmallBark);
    load_model(treeSmallLeavesFile, TreeSmallLeaves);
    load_model(treeSmallBranchesFile, TreeSmallBranches);
    load_model(treeMediumBarkFile, TreeMediumBark);
    load_model(treeMediumLeavesFile, TreeMediumLeaves);
    load_model(treeMediumBranchesFile, TreeMediumBranches);
    load_model(windowBlindHolderFile, WindowBlindHolder);
    load_model(windowBlindFile, WindowBlind);
    // load_model(spiderManFile, SpiderMan);
    load_bump_model(outletFile, Outlet);

    // Generate color buffers
    // glGenBuffers(NumColorBuffers, ColorBuffers);

    // Build color buffers
    // Define cube vertex colors (red)
    // build_solid_color_buffer(numVertices[Cube], vec4(1.0f, 0.0f, 0.0f, 1.0f), RedCube);
}

void build_materials( ) {
    // Add materials to Materials vector
    MaterialProperties wood = {
        vec4(0.3f, 0.2f, 0.1f, 1.0f),
        vec4(0.6f, 0.4f, 0.2f, 1.0f),
        vec4(0.0f, 0.0f, 0.0f, 1.0f),
        0.0f,
        {0.0f, 0.0f, 0.0f}
    };
    // Red plastic
    MaterialProperties redPlastic = {
        vec4(0.3f, 0.0f, 0.0f, 1.0f),
        vec4(0.6f, 0.0f, 0.0f, 1.0f),
        vec4(0.8f, 0.6f, 0.6f, 1.0f),
        32.0f,
        {0.0f, 0.0f, 0.0f}
    };

    MaterialProperties glass = {
        vec4(0.1f, 0.1f, 0.1f, 0.2f), // Ambient: Minimal ambient light reflection
        vec4(0.1f, 0.1f, 0.1f, 0.2f), // Diffuse: Almost no diffuse color
        vec4(0.9f, 0.9f, 0.9f, 0.2f), // Specular: Strong highlights
        96.0f,                         // Shininess: High for a smooth, glassy surface
        {0.0f, 0.0f, 0.0f}             // Padding
    };

    // Soda Material
    MaterialProperties soda = {
        vec4(0.1f, 0.05f, 0.0f, 0.8f), // Ambient color (dark brown, translucent)
        vec4(0.8f, 0.4f, 0.1f, 0.8f), // Diffuse color (orangish-brown for soda appearance)
        vec4(1.0f, 1.0f, 1.0f, 0.8f), // Specular color (bright highlights)
        50.0f,                        // Shininess (high for strong reflections)
        {0.0f, 0.0f, 0.0f}            // Padding
    };

    // White plastic
    MaterialProperties whitePlastic = {
        vec4(0.7f, 0.7f, 0.7f, 1.0f),
        vec4(0.8f, 0.8f, 0.8f, 1.0f),
        vec4(0.4f, 0.4f, 0.4f, 1.0f),
        32.0f,
        {0.0f, 0.0f, 0.0f}
    };


    Materials.push_back(wood);
    Materials.push_back(redPlastic);
    Materials.push_back(glass);
    Materials.push_back(soda);
    Materials.push_back(whitePlastic);
    glGenBuffers(NumMaterialBuffers, MaterialBuffers);
    glBindBuffer(GL_UNIFORM_BUFFER, MaterialBuffers[MaterialBuffer]);
    glBufferData(GL_UNIFORM_BUFFER, Materials.size()*sizeof(MaterialProperties), Materials.data(), GL_STATIC_DRAW);
}

void build_lights( ) {
    // Add lights to Lights vector

    LightProperties sunLight = {
        SPOT,  // Type of light
        {0.0f, 0.0f, 0.0f}, // Padding
        vec4(0.3f, 0.3f, 0.3f, 1.0f), // Dim ambient light
        vec4(1.0f, 0.95f, 0.9f, 1.0f), // Diffuse color
        vec4(1.0f, 1.0f, 1.0f, 1.0f), // Specular color
        vec4(0.0f, 200.0f, 100.0f, 1.0f), // Position (further down the x-axis)
        vec4(-0.5f, -1.0f, -0.25f, 0.0f), // Direction (angled toward the scene from new position)
        60.0f, // SpotCutoff (angle in degrees, defines cone width)
        2.0f  // SpotExponent (focuses the light; higher value narrows the beam)
    };

    // Spot light for ceiling light.
    LightProperties ceilingLight = {
        SPOT, //type
        {0.0f, 0.0f, 0.0f}, //pad
        vec4(0.2f, 0.2f, 0.2f, 1.0f), //ambient
        vec4(1.0f, 1.0f, 1.0f, 1.0f), //diffuse
        vec4(1.0f, 1.0f, 1.0f, 1.0f), //specular
        vec4(0.0f, FLOOR_HEIGHT / 2 + WALL_HEIGHT + 2.0f, 0.0f, 1.0f),  //position
        vec4(0.0f, -1.0f, 0.0f, 0.0f), //direction
        120.0f,   //cutoff
        2.0f,  //exponent
        {0.0f, 0.0f}  //pad2
    };

    // Spotlight for desk lamp.
    LightProperties lampLight = {
        POINT, // type
        {0.0f, 0.0f, 0.0f}, // pad
        vec4(0.0f, 0.0f, 0.0f, 1.0f),  // Ambient color (soft, warm color)
        vec4(1.0f, 0.8f, 0.6f, 1.0f),  // Diffuse color (stronger, warm light)
        vec4(1.0f, 0.8f, 0.6f, 1.0f),  // Specular color (sharp, warm light for highlights)
        vec4(-FLOOR_WIDTH / 2 + 3.0f, FLOOR_HEIGHT / 2 + 1.0f + 2.25f, -FLOOR_LENGTH / 2 + 1.5f, 1.0f), // Position of the light
        vec4(0.0f, 0.0f, 0.0f, 0.0f),  // Direction of the spotlight
        0.0f,    //cutoff
        0.0f,  //exponent
        {0.0f, 0.0f} // Padding or any additional parameters
    };

    LightProperties candleLight = {
        POINT, // Type: Candlelight is better represented as a point light
        {0.0f, 0.0f, 0.0f}, // Padding
        vec4(0.1333f, 0.1f, 0.05f, 1.0f), // Ambient: Warm, subtle light
        vec4(1.0f, 0.9f, 0.7f, 1.0f), // Diffuse: Soft yellow glow
        vec4(0.5f, 0.4f, 0.3f, 1.0f), // Specular: Adds a slight highlight
        vec4(FLOOR_WIDTH / 4, FLOOR_HEIGHT / 2 + WALL_HEIGHT / 2 + 3.5f, FLOOR_LENGTH / 4, 1.0f), // Position: On the table
        vec4(0.0f, 0.0f, 0.0f, 0.0f), // Direction: Not needed for point light
        0.0f,  // Cutoff: Not applicable to point lights
        0.0f,  // Exponent: Not applicable to point lights
        {0.0f, 0.0f} // Padding
    };


    Lights.push_back(sunLight);
    Lights.push_back(ceilingLight);
    Lights.push_back(lampLight);
    // Lights.push_back(candleLight);

    // Set numLights
    numLights = Lights.size();

    // Turn all lights on
    for (int i = 0; i < numLights; i++) {
        lightOn[i] = 1;
    }

    lightOn[LampLight] = 0;
    lightOn[CeilingLight] = 0;

    // Create uniform buffer for lights
    glGenBuffers(NumLightBuffers, LightBuffers);
    glBindBuffer(GL_UNIFORM_BUFFER, LightBuffers[LightBuffer]);
    glBufferData(GL_UNIFORM_BUFFER, Lights.size()*sizeof(LightProperties), Lights.data(), GL_STATIC_DRAW);
}

void build_shadows( ) {
    // Generate new framebuffer and corresponding texture for storing shadow distances
    glGenFramebuffers(1, &ShadowBuffer);
    glGenTextures(1, &TextureIDs[ShadowTex]);
    // Bind shadow texture and only store depth value
    glBindTexture(GL_TEXTURE_2D, TextureIDs[ShadowTex]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindFramebuffer(GL_FRAMEBUFFER, ShadowBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TextureIDs[ShadowTex], 0);
    // Buffer is not actually drawn into since only for creating shadow texture
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void build_mirror(GLuint m_texid ) {
    // Generate mirror texture
    glGenTextures(1, &TextureIDs[m_texid]);
    // Bind mirror texture
    glBindTexture(GL_TEXTURE_2D, TextureIDs[m_texid]);
    // Create empty mirror texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ww, hh, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void build_textures( )
{
    // Create textures and activate unit 0
    glGenTextures( NumTextures,  TextureIDs);
    glActiveTexture( GL_TEXTURE0 );

    load_texture(ceilingFanTexFile, CeilingFanTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(ceilingFanTexNormFile, CeilingFanTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(dinnerTableTexFile, DinnerTableTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(dinnerTableTexNormFile, DinnerTableTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(diningChairTexFile, DiningChairTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(diningChairTexNormFile, DiningChairTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(drawerTexFile, DrawerTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(drawerTexNormFile, DrawerTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(wallSurfaceTexFile, WallSurfaceTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(floorSurfaceTexFile, FloorSurfaceTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(floorSurfaceTexNormFile, FloorSurfaceTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(roof_surface_texture, RoofSurfaceTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(roof_surface_texture_normal, RoofSurfaceTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(wallSurfaceTexNormFile, WallSurfaceTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(doorSurfaceTexFile, DoorSurfaceTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(doorSurfaceTexNormFile, DoorSurfaceTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(canTexFile, CanTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(bowlTexFile, BowlTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(bowlTexNormFile, BowlTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(appleTexFile, AppleTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(carpetTexFile, CarpetTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(carpetTexNormFile, CarpetTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(pictureFrameTexFile, PictureFrameTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(pictureCanvasTexFile, PictureCanvasTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(lampTexFile, LampTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(skyTexFile, SkyTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(grassTexFile, GrassTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(grassTexNormFile, GrassTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(hGrassTexFile, HGrassTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(hGrassTexNormFile, HGrassTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(grassBlobTexFile, GrassBlobTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(treeBarkTexFile, BarkTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(treeLeavesTexFile, LeavesTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(treeBranchesTexFile, BranchesTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(windowBlindTexFile, WindowBlindTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(windowBlindFabricTexFile, WindowBlindFabricTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(spiderManTexFile, SpiderManTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(outletTexFile, OutletTex, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
    load_texture(outletTexNormFile, OutletTexNorm, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT, true, false);
}

void load_bump_model(const char * filename, GLuint obj) {
    vector<vec4> vertices;
    vector<vec2> uvCoords;
    vector<vec3> normals;
    vector<vec3> tangents;
    vector<vec3> bitangents;

    // Load model and set number of vertices
    loadOBJ(filename, vertices, uvCoords, normals);
    numVertices[obj] = vertices.size();

    // TODO: Compute tangents and bitangents
    computeTangentBasis(vertices, uvCoords, normals, tangents, bitangents);

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], uvCoords.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TangBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*tangCoords*numVertices[obj], tangents.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][BiTangBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*bitangCoords*numVertices[obj], bitangents.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_mat_shadow_object(GLuint obj, GLuint material){
    // Reference appropriate shader variables
    if (shadow) {
        // Use shadow shader
        glUseProgram(shadow_program);
        // Pass shadow projection and camera matrices to shader
        glUniformMatrix4fv(shadow_proj_mat_loc, 1, GL_FALSE, shadow_proj_matrix);
        glUniformMatrix4fv(shadow_camera_mat_loc, 1, GL_FALSE, shadow_camera_matrix);

        // Set object attributes to shadow shader
        vPos = shadow_vPos;
        model_mat_loc = shadow_model_mat_loc;
    } else {
        // Use lighting shader with shadows
        glUseProgram(phong_shadow_program);

        // Pass object projection and camera matrices to shader
        glUniformMatrix4fv(phong_shadow_proj_mat_loc, 1, GL_FALSE, proj_matrix);
        glUniformMatrix4fv(phong_shadow_camera_mat_loc, 1, GL_FALSE, camera_matrix);

        // Bind lights
        glUniformBlockBinding(phong_shadow_program, phong_shadow_lights_block_idx, 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, LightBuffers[LightBuffer], 0, Lights.size() * sizeof(LightProperties));

        // Bind materials
        glUniformBlockBinding(phong_shadow_program, phong_shadow_materials_block_idx, 1);
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, MaterialBuffers[MaterialBuffer], 0,
                          Materials.size() * sizeof(MaterialProperties));

        // Set camera position
        glUniform3fv(phong_shadow_eye_loc, 1, eye);

        // Set num lights and lightOn
        glUniform1i(phong_shadow_num_lights_loc, Lights.size());
        glUniform1iv(phong_shadow_light_on_loc, numLights, lightOn);

        // Pass normal matrix to shader
        glUniformMatrix4fv(phong_shadow_norm_mat_loc, 1, GL_FALSE, normal_matrix);

        // Pass material index to shader
        glUniform1i(phong_shadow_material_loc, material);

        // TODO: Pass shadow projection and camera matrices
        glUniformMatrix4fv(phong_shadow_shad_proj_mat_loc, 1, GL_FALSE, shadow_proj_matrix);
        glUniformMatrix4fv(phong_shadow_shad_cam_mat_loc, 1, GL_FALSE, shadow_camera_matrix);

        // TODO: Bind shadow texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureIDs[ShadowTex]);

        // Set object attributes for phong shadow shader
        vPos = phong_shadow_vPos;
        vNorm = phong_shadow_vNorm;
        model_mat_loc = phong_shadow_model_mat_loc;
    }

    // Pass model matrix to shader
    glUniformMatrix4fv(model_mat_loc, 1, GL_FALSE, model_matrix);

    // Bind vertex array
    glBindVertexArray(VAOs[obj]);

    // Bind position object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glVertexAttribPointer(vPos, posCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(vPos);

    if (!shadow) {
        // Bind object normal buffer if using phong shadow shader
        glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
        glVertexAttribPointer(vNorm, normCoords, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(vNorm);
    }

    // Draw object
    glDrawArrays(GL_TRIANGLES, 0, numVertices[obj]);
}

void draw_multi_tex_object(GLuint obj, GLuint texture1, GLuint texture2){
    // Select shader program
    glUseProgram(multi_tex_program);

    // Pass projection matrix to shader
    glUniformMatrix4fv(multi_tex_proj_mat_loc, 1, GL_FALSE, proj_matrix);

    // Pass camera matrix to shader
    glUniformMatrix4fv(multi_tex_camera_mat_loc, 1, GL_FALSE, camera_matrix);

    // Pass model matrix to shader
    glUniformMatrix4fv(multi_tex_model_mat_loc, 1, GL_FALSE, model_matrix);

    // Set base texture to texture unit 0 and make it active
    glUniform1i(multi_tex_base_loc, 0);
    glActiveTexture(GL_TEXTURE0);
    // Bind base texture (to unit 0)
    glBindTexture(GL_TEXTURE_2D, TextureIDs[texture1]);

    // Set second texture to texture unit 1 and make it active
    glUniform1i(multi_tex_dirt_loc, 1);
    glActiveTexture(GL_TEXTURE1);
    // Bind second texture (to unit 1)
    glBindTexture(GL_TEXTURE_2D, TextureIDs[texture2]);

    // Bind vertex array
    glBindVertexArray(VAOs[obj]);

    // Bind position object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glVertexAttribPointer(multi_tex_vPos, posCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(multi_tex_vPos);

    // Bind texture object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glVertexAttribPointer(multi_tex_vTex, texCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(multi_tex_vTex);

    // Draw object
    glDrawArrays(GL_TRIANGLES, 0, numVertices[obj]);
}

void draw_bump_object(GLuint obj, GLuint base_texture, GLuint normal_map, GLfloat tilingFactor) {
    // Select shader program
    glUseProgram(bump_program);

    // Pass projection and camera matrices to shader
    glUniformMatrix4fv(bump_proj_mat_loc, 1, GL_FALSE, proj_matrix);
    glUniformMatrix4fv(bump_camera_mat_loc, 1, GL_FALSE, camera_matrix);

    // Pass tiling factor to shader
    GLint tilingFactorLoc = glGetUniformLocation(bump_program, "tilingFactor");
    glUniform1f(tilingFactorLoc, tilingFactor);

    // Bind lights
    glUniformBlockBinding(bump_program, bump_lights_block_idx, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, LightBuffers[LightBuffer], 0, Lights.size() * sizeof(LightProperties));

    // Set camera position
    glUniform3fv(bump_eye_loc, 1, eye);

    // Set num lights and lightOn
    glUniform1i(bump_num_lights_loc, numLights);
    glUniform1iv(bump_light_on_loc, numLights, lightOn);

    // Pass model matrix and normal matrix to shader
    glUniformMatrix4fv(bump_model_mat_loc, 1, GL_FALSE, model_matrix);
    glUniformMatrix4fv(bump_norm_mat_loc, 1, GL_FALSE, normal_matrix);

    // Set base texture to texture unit 0 and make it active
    glUniform1i(bump_base_loc, 0);
    glActiveTexture(GL_TEXTURE0);
    // Bind base texture (to unit 0)
    glBindTexture(GL_TEXTURE_2D, TextureIDs[base_texture]);

    // Set normal map texture to texture unit 1 and make it active
    glUniform1i(bump_norm_loc, 1);
    glActiveTexture(GL_TEXTURE1);
    // Bind normal map texture (to unit 1)
    glBindTexture(GL_TEXTURE_2D, TextureIDs[normal_map]);

    // Bind vertex array
    glBindVertexArray(VAOs[obj]);

    // Bind position object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glVertexAttribPointer(bump_vPos, posCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(bump_vPos);

    // Bind normal object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glVertexAttribPointer(bump_vNorm, normCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(bump_vNorm);

    // Bind texture object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glVertexAttribPointer(bump_vTex, texCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(bump_vTex);

    // Bind tangent object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TangBuffer]);
    glVertexAttribPointer(bump_vTang, tangCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(bump_vTang);

    // Bind bitangent object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][BiTangBuffer]);
    glVertexAttribPointer(bump_vBiTang, bitangCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(bump_vBiTang);

    // Draw object
    glDrawArrays(GL_TRIANGLES, 0, numVertices[obj]);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // ESC to quit
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }

    // Toggle projection mode
    if (key == GLFW_KEY_O)
    {
        proj = ORTHOGRAPHIC;
    }
    else if (key == GLFW_KEY_P)
    {
        proj = PERSPECTIVE;
    }

    // Orthographic Perspective
    if (proj == ORTHOGRAPHIC) {
        // Adjust elevation angle
        if (key == GLFW_KEY_W)
        {
            elevation += del;
            if (elevation > 179.0)
            {
                elevation = 179.0;
            }
        }
        else if (key == GLFW_KEY_S)
        {
            elevation -= del;
            if (elevation < 1.0)
            {
                elevation = 1.0;
            }
        }

        // Adjust azimuth
        if (key == GLFW_KEY_A) {
            azimuth += daz;
            if (azimuth > 360.0) {
                azimuth -= 360.0;
            }
        } else if (key == GLFW_KEY_D) {
            azimuth -= daz;
            if (azimuth < 0.0)
            {
                azimuth += 360.0;
            }
        }

        GLfloat min_radius = 0.5f;
        GLfloat max_radius = 6.0f;
        // Adjust radius (zoom)
        if (key == GLFW_KEY_X)
        {
            radius += dr;
            if (radius > max_radius)
            {
                radius = max_radius;
            }
        }
        else if (key == GLFW_KEY_Z)
        {
            radius -= dr;
            if (radius < min_radius)
            {
                radius = min_radius;
            }
        }

        // Compute updated camera position
        GLfloat x, y, z;
        x = (GLfloat)(radius*sin(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
        y = (GLfloat)(radius*cos(elevation*DEG2RAD));
        z = (GLfloat)(radius*cos(azimuth*DEG2RAD)*sin(elevation*DEG2RAD));
        eye = vec3(x,y,z);
    }

    // First-person Perspective
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        mouse_control_mode = !mouse_control_mode;
        center_perspective[1] = 0.0f;
    }

    if (mouse_control_mode) {
        // Key callback for movement and strafing
        if (key == GLFW_KEY_W && mods == GLFW_MOD_SHIFT) {  // Move forward faster with Shift + W
            vec3 dir = center_perspective - eye_perspective;
            dir[1] = 0.0f;  // Ignore vertical movement
            dir = normalize(dir);
            eye_perspective += dir * step_size * 2;
            play_footsteps_sound();
        } else if (key == GLFW_KEY_W) {  // Move forward
            vec3 dir = center_perspective - eye_perspective;
            dir[1] = 0.0f;  // Ignore vertical movement
            dir = normalize(dir);
            eye_perspective += dir * step_size;
            play_footsteps_sound();
        } else if (key == GLFW_KEY_S) {  // Move backward
            vec3 dir = center_perspective - eye_perspective;
            dir[1] = 0.0f;  // Ignore vertical movement
            dir = normalize(dir);
            eye_perspective -= dir * step_size;
            play_footsteps_sound();
        } else if (key == GLFW_KEY_A) {  // Strafe left
            vec3 right = normalize(cross(center_perspective - eye_perspective, up_perspective));
            eye_perspective -= right * step_size;
            play_footsteps_sound();
        } else if (key == GLFW_KEY_D) {  // Strafe right
            vec3 right = normalize(cross(center_perspective - eye_perspective, up_perspective));
            eye_perspective += right * step_size;
            play_footsteps_sound();
        }
    } else {
        if (key == GLFW_KEY_A) {
            theta -= 2.0f;
        } else if (key == GLFW_KEY_D) {
            theta += 2.0f;
        }

        vec3 dir = {static_cast<float>(cos(DEG2RAD * theta)), 0.0f, static_cast<float>(sin(DEG2RAD * theta))};

        if (key == GLFW_KEY_W && mods == GLFW_MOD_SHIFT) {
            eye_perspective[0] += step_size * dir[0];
            eye_perspective[2] += 2.0f * step_size * dir[2];
            play_footsteps_sound();
        } else if (key == GLFW_KEY_W) {
            eye_perspective[0] += step_size * dir[0];
            eye_perspective[2] += step_size * dir[2];
            play_footsteps_sound();
        } else if (key == GLFW_KEY_S) {
            eye_perspective[0] -= step_size * dir[0];
            eye_perspective[2] -= step_size * dir[2];
            play_footsteps_sound();
        } else if (key == GLFW_KEY_Z) {
            phi += 2.0f;
            if (phi > 60.0f) phi = 60.0f;
        } else if (key == GLFW_KEY_X) {
            phi -= 2.0f;
            if (phi < -60.0f) phi = -60.0f;
        }

        center_perspective[0] = eye_perspective[0] + cos(DEG2RAD * theta);       // Horizontal direction
        center_perspective[1] = eye_perspective[1] + sin(DEG2RAD * phi);         // Vertical direction (for elevation)
        center_perspective[2] = eye_perspective[2] + sin(DEG2RAD * theta);       // Horizontal direction
    }

    // Fan/Light
    if (eye_perspective[0] > -2.0f && eye_perspective[0] < 4.0f && eye_perspective[2] < -FLOOR_LENGTH / 2 + 3.0f) {
        if (key == GLFW_KEY_F && action == GLFW_PRESS && mods == GLFW_MOD_SHIFT && fan_spin) {
            fan_speed += 20.0f;
            if (fan_speed > max_fan_speed) {
                fan_speed = 20.0f;
            }
        } else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
            fan_spin = !fan_spin;
        }
        if (key == GLFW_KEY_L && action == GLFW_PRESS) {
            lightOn[CeilingLight] = (lightOn[CeilingLight] == 0) ? 1 : 0;
        }
        if (key == GLFW_KEY_N && action == GLFW_PRESS) {
            if (is_outside) {
                eye_perspective[2] += 5.0f;
            } else {
                eye_perspective[2] -= 5.0f;
            }
            is_outside = !is_outside;
        }
    }

    // Desk lamp light
    if (eye_perspective[0] < -FLOOR_WIDTH / 2 + 5.0f && eye_perspective[2] < -FLOOR_LENGTH / 2 + 8.0f) {
        if (key == GLFW_KEY_L && action == GLFW_PRESS) {
            lightOn[LampLight] = (lightOn[LampLight] == 0) ? 1 : 0;
        } else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
            window_open = !window_open;
        }
    }
}

void mouse_callback(GLFWwindow *window, int button, int action, int mods){

}

void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouse_control_mode) {
        if (firstMouse) {  // Initialize lastX and lastY on first use
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xOffset = xpos - lastX;
        float yOffset = lastY - ypos;  // Reversed since y-coordinates range bottom to top
        lastX = xpos;
        lastY = ypos;

        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        // Clamp pitch to avoid flipping
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
}


void update_first_person_perspective() {
    // Collision detection for walls
    if (!is_outside) {
        if (eye_perspective[0] < -(FLOOR_WIDTH / 2 - WALL_WIDTH * 3)) {
            eye_perspective[0] += step_size / 2;
        } else if (eye_perspective[0] > FLOOR_WIDTH / 2 - WALL_WIDTH * 3) {
            eye_perspective[0] -= step_size / 2;
        }
        if (eye_perspective[2] < -(FLOOR_LENGTH / 2 - WALL_WIDTH * 3)) {
            eye_perspective[2] += step_size / 2;
        } else if (eye_perspective[2] > FLOOR_LENGTH / 2 - WALL_WIDTH * 3) {
            eye_perspective[2] -= step_size / 2;
        }
    } else {
        if (eye_perspective[0] > -(FLOOR_WIDTH / 2 + WALL_WIDTH * 3) && eye_perspective[0] < FLOOR_WIDTH / 2 + WALL_WIDTH * 3 && eye_perspective[2] > -(FLOOR_LENGTH / 2 + WALL_WIDTH * 3) && eye_perspective[2] < FLOOR_LENGTH / 2 + WALL_WIDTH * 3) {
            eye_perspective[2] -= step_size / 1.5;
        }
    }

    // Camera

    if (mouse_control_mode) {
        vec3 front;
        front[0] = cos(radians(yaw)) * cos(radians(pitch));
        front[1] = sin(radians(pitch));
        front[2] = sin(radians(yaw)) * cos(radians(pitch));
        center_perspective = eye_perspective + normalize(front);
    }
}

// The next three functions deal with setting up SDL and loading in the music/sounds.
void initialize_sdl() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

Mix_Music* load_music(const char* music_file) {
    // Load background music
    Mix_Music* bg_music = Mix_LoadMUS(music_file);
    if (bg_music == nullptr) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        return nullptr;
    }

    return bg_music;
}


Mix_Chunk* load_sound(const char* sound_file) {
    // Load sound effects
    Mix_Chunk* sound = Mix_LoadWAV(sound_file);
    if (sound == nullptr) {
        printf("Failed to load fan sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        Mix_FreeChunk(sound);
        return nullptr;
    }

    return sound;
}

void play_footsteps_sound() {
    if (is_outside) {
        if (!Mix_Playing(-1) && grass_sound != nullptr) {
            Mix_PlayChannel(-1, grass_sound, 0);
        }
    } else {
        if (!Mix_Playing(-1) && floor_sound != nullptr) {
            Mix_PlayChannel(-1, floor_sound, 0);
        }
    }
}

void build_frame(GLuint obj) {
    vector<vec4> vertices;
    vector<vec3> normals;

    // Create wireframe for mirror
    vertices = {
        vec4(1.0f, 0.0f, -1.0f, 1.0f),
        vec4(1.0f, 0.0f, 1.0f, 1.0f),
        vec4(-1.0f, 0.0f, 1.0f, 1.0f),
        vec4(-1.0f, 0.0f, -1.0f, 1.0f)
};

    normals = {
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f)
};

    numVertices[obj] = vertices.size();

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Debug mirror renderer
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad(GLuint shader, GLuint tex)
{
    // reset viewport
    glViewport(0, 0, ww, hh);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureIDs[tex]);
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

struct GrassInstance {
    vec3 position;
    float scale;
    bool isMediumGrass; // true for medium tree, false for small tree
    bool isSparse;
};

std::vector<GrassInstance> grassInstances;

void generate_grass_field(int numBlobs) {
    grassInstances.clear(); // Clear any existing data
    for (int i = 0; i < numBlobs; i++) {
        GrassInstance instance;
        do {
            // Generate a random position within the larger range
            instance.position = vec3(
                ((rand() % 1000) / 1000.0f - 0.5f) * 400.0f, // Range of [-200, 200]
                0.0f,                                       // y at ground level
                ((rand() % 1000) / 1000.0f - 0.5f) * 400.0f  // Range of [-200, 200]
            );
        } while (abs(instance.position[0]) < 20.0f && abs(instance.position[2]) < 20.0f);

        // Set scale and type
        instance.scale = ((rand() % 100) / 100.0f * 0.5f) + 0.75f; // Random scale [0.75, 1.25]
        instance.isMediumGrass = rand() % 2 == 0; // Randomly choose medium or small grass blob
        instance.isSparse = rand() % 2 == 0; // Randomly choose if the grass blob is sparse or not

        // Add the instance to the list
        grassInstances.push_back(instance);
    }
}

void populate_grass_field() {
    for (const auto& instance : grassInstances) {
        model_matrix = translate(instance.position[0], instance.position[1], instance.position[2]) *
                       scale(instance.scale, instance.scale, instance.scale);
        normal_matrix = model_matrix.inverse().transpose();

        if (instance.isMediumGrass) {
            if (instance.isSparse) {
                draw_tex_object(GrassMediumBlobSparse, GrassBlobTex, 1.0f);
            } else {
                draw_tex_object(GrassMediumBlob, GrassBlobTex, 1.0f);
            }
        } else {
            if (instance.isSparse) {
                draw_tex_object(GrassSmallBlobSparse, GrassBlobTex, 1.0f);
            } else {
                draw_tex_object(GrassSmallBlob, GrassBlobTex, 1.0f);

            }
        }
    }
}

struct TreeInstance {
    vec3 position;
    float scale;
    bool isMediumTree; // true for medium tree, false for small tree
};

std::vector<TreeInstance> treeInstances;

void generate_tree_field(int numTrees) {
    treeInstances.clear(); // Clear any existing data
    for (int i = 0; i < numTrees; i++) {
        TreeInstance instance;
        do {
            // Generate a random position within the larger range
            instance.position = vec3(
                ((rand() % 1000) / 1000.0f - 0.5f) * 400.0f, // Range of [-200, 200]
                0.0f,                                       // y at ground level
                ((rand() % 1000) / 1000.0f - 0.5f) * 400.0f  // Range of [-200, 200]
            );
        } while (abs(instance.position[0]) < 20.0f && abs(instance.position[2]) < 20.0f);

        // Set scale and type
        instance.scale = ((rand() % 100) / 100.0f * 0.5f) + 0.75f; // Random scale [0.75, 1.25]
        instance.isMediumTree = rand() % 2 == 0; // Randomly choose medium or small tree

        // Add the instance to the list
        treeInstances.push_back(instance);
    }
}


void populate_tree_field() {
    for (const auto& instance : treeInstances) {
        model_matrix = translate(instance.position[0], instance.position[1], instance.position[2]) *
                       scale(instance.scale, instance.scale, instance.scale);
        normal_matrix = model_matrix.inverse().transpose();

        if (instance.isMediumTree) {
            // Draw medium tree components
            draw_tex_object(TreeMediumBark, BarkTex, 1.0f);
            draw_tex_object(TreeMediumLeaves, LeavesTex, 1.0f);
            draw_tex_object(TreeMediumBranches, BranchesTex, 1.0f);
        } else {
            // Draw small tree components
            draw_tex_object(TreeSmallBark, BarkTex, 1.0f);
            draw_tex_object(TreeSmallLeaves, LeavesTex, 1.0f);
            draw_tex_object(TreeSmallBranches, BranchesTex, 1.0f);
        }
    }
}


#include "utilfuncs.cpp"