// OpenGL Jet program.
// ECE8893, Georgia Tech, Fall 2012

#include <iostream>

//#ifdef LINUX
//Linux Headers
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//#endif

#ifdef OSX
// MAC Headers
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif

#ifdef WINDOWS
//Windows Headers
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <fstream>
#include <vector>
#include <inttypes.h>
#include "jet.h"
#include <string.h>
using namespace std;

/************************/
/*	Global Variables 	*/
/************************/
unsigned int LightSourceNo= 0;
GLfloat updateRate = 20.0; // Change this later
float angle =0.0f;
bool rotating = false, rotatingX=false, rotatingY=false, rotatingZ=false;
GLfloat angleX=0.0f, angleY=0.0f, angleZ=0.0f;
GLfloat scalingX=1.2f, scalingY=1.2f, scalingZ=1.2f;
/*	End	*/

/************************/
/*	Global Vectors	 	*/
/************************/
vector<material_t> materials;
vector<vertex_t>verticies;
vector<vertex_t>normals;
vector<face_t>faces;
vector<texture_t>texCords;
/*	END	*/


// Code to read material file
void loadMtl()
{
  ifstream ifs("jet.mtl");
  int currentid=-1;
  if (!ifs) return;
  string line;
  
  if(ifs.is_open()){
	  while(ifs.good()){
		  getline(ifs, line);

		  if(line[0] == 'n'){
			  currentid++;
			  material_t temp_material;
			  memset( &temp_material, 0, sizeof(material_t));
			  materials.push_back(temp_material);
		  }
		  else if(line[0] == '\t' && line[1] == 'N'){
			  sscanf(line.c_str(), "%*s %f", &materials[currentid].ns);
		  }
		  else if(line[0] == '\t' && line[1] == 'T' && line[2] == 'r'){
			  sscanf(line.c_str(), "%*s %f", &materials[currentid].Tr);
		  }
		  else if( line[0] == '\t' && line[1] == 'K' && line[2] == 'a'){
			  sscanf(line.c_str(), "%*s %f %f %f", &materials[currentid].ar,
					  &materials[currentid].ag, &materials[currentid].ab);
		  }
		  else if(line[0] == '\t' && line[1] == 'K' && line[2] == 'd'){
			  sscanf(line.c_str(), "%*s %f %f %f", &materials[currentid].dr,
					  &materials[currentid].dg, &materials[currentid].db);
		  }
		  else if(line[0] == '\t' && line[1] == 'K' && line[2] == 's'){
			  sscanf(line.c_str(), "%*s %f %f %f", &materials[currentid].sr,
					  &materials[currentid].sg, &materials[currentid].sb);
		  }
		  else if(line[0] == '\t' && line[1] == 'K' && line[2] == 'e'){
			  sscanf(line.c_str(), "%*s %f %f %f", &materials[currentid].er,
					  &materials[currentid].eg, &materials[currentid].eb);
		  }
		  else{}
	  }
  }
}

//Code to load from obj file
void loadModel()
{
	//open file
	ifstream inputFile ("jet.obj");

	string line;
	int currMaterial = 0;

	if(inputFile.is_open())
    {
		while(inputFile.good())
		{
			getline(inputFile, line);

			if(line[0] == 'u')
			{
				sscanf(line.c_str(), "usemtl %d", &currMaterial);
			}

			//process vertex
			else if(line[0] == 'v' && line[1] == ' ')
			{
				vertex_t vert;
				sscanf(line.c_str(), "%*s %f %f %f", &vert.x, &vert.y, &vert.z);
				verticies.push_back(vert);
				//cout << "Vertex! ";
			}

			//process normal
			else if(line[0] == 'v' && line[1] == 'n')
			{
				vertex_t normal;
				sscanf(line.c_str(), "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
				normals.push_back(normal);
			}

			//process texture
			else if(line[0] == 'v' && line[1] == 't')
			{
				texture_t tex;
				sscanf(line.c_str(), "%*s %f %f", &tex.u, &tex.v);
				texCords.push_back(tex);
			}

			//process face
			else if(line[0] == 'f')
			{
				size_t first;
				size_t second;

				face_t temp_face;

				temp_face.material = currMaterial;

				first = line.find("/");
				second = line.find("/", first+1);

				if(second - first == 1) //then there isn't texture data
				{
					temp_face.istexture = false;
					sscanf(line.c_str(), "%*s %d//%d %d//%d %d//%d" , &temp_face.a,
							&temp_face.normalIdxA, &temp_face.b, &temp_face.normalIdxB,
							&temp_face.c, &temp_face.normalIdxC);
				}
				else //there is texture data
				{
				  temp_face.istexture = true;
				  sscanf(line.c_str(), "%*s %d/%d/%d %d/%d/%d %d/%d/%d" , &temp_face.a,
						  &temp_face.texA, &temp_face.normalIdxA, &temp_face.b, &temp_face.texB,
						  &temp_face.normalIdxB, &temp_face.c, &temp_face.texC, &temp_face.normalIdxC);
				}
				faces.push_back(temp_face);
			}
		}
	}
}

void setMaterial(int materialId)
{
  float specular[4];
  float ambient[4];
  float diffuse[4];
  float emmisive[4];
  
  // Ugly but works
  
  specular[0] = materials[materialId].sr;
  specular[1] = materials[materialId].sg;
  specular[2] = materials[materialId].sb;
  specular[3] = 1 - materials[materialId].Tr;
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  
  ambient[0] = materials[materialId].ar;
  ambient[1] = materials[materialId].ag;
  ambient[2] = materials[materialId].ab;
  ambient[3] = 1 - materials[materialId].Tr;
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  
  diffuse[0] = materials[materialId].dr;
  diffuse[1] = materials[materialId].dg;
  diffuse[2] = materials[materialId].db;
  diffuse[3] = 1 - materials[materialId].Tr;
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  
  emmisive[0] = materials[materialId].er;
  emmisive[1] = materials[materialId].eg;
  emmisive[2] = materials[materialId].eb;
  emmisive[3] = 1 - materials[materialId].Tr;
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emmisive);
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &materials[materialId].ns);
}

void drawModel()
{
  // Your code to draw the model here
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLES);
		for(unsigned int i =0 ; i<faces.size();i++){
			setMaterial(faces[i].material);

			glNormal3f(normals[faces[i].normalIdxA-1].x, normals[faces[i].normalIdxA-1].y, normals[faces[i].normalIdxA-1].z);
			glVertex3f(verticies[faces[i].a-1].x, verticies[faces[i].a-1].y, verticies[faces[i].a-1].z);

			glNormal3f(normals[faces[i].normalIdxB-1].x, normals[faces[i].normalIdxB-1].y, normals[faces[i].normalIdxB-1].z);
			glVertex3f(verticies[faces[i].b-1].x, verticies[faces[i].b-1].y, verticies[faces[i].b-1].z);

			glNormal3f(normals[faces[i].normalIdxC-1].x, normals[faces[i].normalIdxC-1].y, normals[faces[i].normalIdxC-1].z);
			glVertex3f(verticies[faces[i].c-1].x, verticies[faces[i].c-1].y, verticies[faces[i].c-1].z);

		}
	glEnd();
}


void init(void)
{ // Called from main
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}


//double updateRate = 20;

void timer(int)
{
  // Adjust rotation angles as needed here
	float deltaAngle = 360/5/updateRate;
	if(rotatingX){
		angleX = angleX + deltaAngle;
		if(angleX>=360.0f){
			angleX =0;
		}
	}

	if(rotatingY){
			angleY = angleY + deltaAngle;
			if(angleY>=360.0f){
				angleY =0;
			}
	}

	if(rotatingZ){
			angleZ = angleZ + deltaAngle;
			if(angleZ>=360.0f){
				angleZ =0;
			}
	}

	else{
		//angle =0;
	}

  // Then tell glut to redisplay
  glutPostRedisplay();
  // And reset tht timer
  glutTimerFunc(1000.0 / updateRate, timer, 0);
}

void display(void)
{
	//glClearColor(0.0, 0.0, 0.0, 1.0); // black background
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 15.0, -25.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    // rotations and scaling here
    glRotatef(angleX, 1.0, 0.0f, 0.0f);
    glRotatef(angleY, 0.0, 1.0f, 0.0f);
    glRotatef(angleZ, 0.0, 0.0f, 1.0f);

    glScalef(scalingX, scalingY, scalingZ);
    // Draw the model
    drawModel();
    // Swap the double buffers
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    //height = h;
    //width = w;
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}



void keyboard (unsigned char key, int x, int y) 
{
  // Keystroke processing here
	GLfloat position1[] = {2.0f, 2.0f, 2.0f};
	GLfloat position2[] = {-2.0f, -2.0f, -2.0f};
	GLfloat position4[] = {-1.0f, 1.0f, 1.0f};
	GLfloat position5[] = {1.0f, -1.0f, 1.0f};
	GLfloat position6[] = {1.0f, -1.0f, -1.0f};

	GLfloat ambientLight1[] = {1.0f,0.0f,3.0f, -1.0f};
	GLfloat ambientLight2[] = {0.0f,3.0f,.0f,-1.0f};
	GLfloat ambientLight3[] = {-1.5f,3.8f,.03f,-1.0f};
	GLfloat ambientLight4[] = {3.0f,.21f,1.33f,-1.0f};
	GLfloat ambientLight5[] = {.0f,.1f,-0.5f,-1.0f};
	GLfloat ambientLight6[] = {2.03f,-0.19f,.07f,-1.0f};




	GLfloat diffuseLight1[] = {3.0f,1.9f,3.9f,1.0f};
	GLfloat diffuseLight2[] = {-1.0f,1.9f,0.3f,1.0f};
	GLfloat diffuseLight3[] = {-0.5f,-2.9f,1.5f,1.0f};
	GLfloat diffuseLight4[] = {.33f,1.66f,-2.33f,1.0f};
	GLfloat diffuseLight5[] = {-0.77f,1.9f,-2.55f,1.0f};
	GLfloat diffuseLight6[] = {.89f,-2.45f,0.6f,1.0f};

	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f};

	switch(key){
		case 'x':
			if(rotatingX){
				rotatingX = false;
				break;
			}
			else{
				rotatingX = true;
				break;
			}
			break;

		case 'y':
			if(rotatingY){
				rotatingY = false;
				break;
			}
			else{
				rotatingY = true;
				break;
			}
			break;

		case 'z':
				if(rotatingZ){
					rotatingZ = false;
					break;
				}
				else{
					rotatingZ = true;
					break;
				}
				break;

		case 'S':
				scalingX*=1.01f;
				scalingY*=1.01f;
				scalingZ*=1.01f;
				break;

		case 's':
				if(scalingX>0.0){
					scalingX *= 0.99f;
				}
				if(scalingX>0.0){
					scalingY *= 0.99f;
				}
				if(scalingX>0.0){
					scalingZ *= 0.99f;
				}
				break;

		case 'L':
				switch(LightSourceNo){
				case(0):
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT1);
					glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
					glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
					glLightfv(GL_LIGHT1, GL_POSITION, position1);
					LightSourceNo++;
					break;

				case(1):
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT2);
					glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight2);
					glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight2);
					glLightfv(GL_LIGHT2, GL_SPECULAR, specularLight);
					glLightfv(GL_LIGHT2, GL_POSITION, position2);
					LightSourceNo++;
					break;

				case(2):
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT3);
					glLightfv(GL_LIGHT3, GL_AMBIENT, ambientLight3);
					glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuseLight3);
					glLightfv(GL_LIGHT3, GL_SPECULAR, specularLight);
					LightSourceNo++;
					break;

				case(3):
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT4);
					glLightfv(GL_LIGHT4, GL_AMBIENT, ambientLight4);
					glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuseLight4);
					glLightfv(GL_LIGHT4, GL_SPECULAR, specularLight);
					LightSourceNo++;
					break;

				case(4):
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT5);
					glLightfv(GL_LIGHT5, GL_AMBIENT, ambientLight5);
					glLightfv(GL_LIGHT5, GL_DIFFUSE, diffuseLight5);
					glLightfv(GL_LIGHT5, GL_SPECULAR, specularLight);
					glLightfv(GL_LIGHT5, GL_POSITION, position5);
					LightSourceNo++;
					break;

				case(5):
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT6);
					glLightfv(GL_LIGHT6, GL_AMBIENT, ambientLight6);
					glLightfv(GL_LIGHT6, GL_DIFFUSE, diffuseLight6);
					glLightfv(GL_LIGHT6, GL_SPECULAR, specularLight);
					glLightfv(GL_LIGHT6, GL_POSITION, position6);
					LightSourceNo++;
					break;

				default:
					break;
				}
				break;
		case 'l':
			switch(LightSourceNo){
				case(1):
					glDisable(GL_LIGHT1);
					LightSourceNo--;
					break;

				case(2):
					glDisable(GL_LIGHT2);
					LightSourceNo--;
					break;

				case(3):
					glDisable(GL_LIGHT3);
					LightSourceNo--;
				break;

				case(4):
					glDisable(GL_LIGHT4);
					LightSourceNo--;
				break;

				case(5):
					glDisable(GL_LIGHT5);
					LightSourceNo--;
					break;

				case(6):
					glDisable(GL_LIGHT6);
					LightSourceNo--;
					break;

				default:
					break;
			}
				break;

		case 'q':
			exit(0);


		default:
			break;
	}
}


int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Jet Display");
    loadModel(); // Uncomment when the model reader is coded.
    loadMtl();  // Uncomment when the materials reader is coded
    init();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc (keyboard);
    glutReshapeFunc(reshape);
    // Compute the update rate here...
    glutTimerFunc(1000.0 / updateRate, timer, 0);
    glutMainLoop();
    return 0;
}

