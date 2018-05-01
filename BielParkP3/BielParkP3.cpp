/*
Warbird Simulation - Phase 2
MEMBER 1 : Dino Biel
EMAIL: dino.biel.934@my.csun.edu
MEMBER 2: Inkyu Park
EMAIL: inkyu.park.12@my.csun.edu
*/

// define your target operating system: __Windows__  __Linux__  __Mac__
//Changes this to windows and change the include to work for windows
# define __Windows__
# include "../includes465/include465.hpp"
# include "../includes465/texture.hpp"

const int X = 0, Y = 1, Z = 2, START = 0, STOP = 1;
// constants for models:  file names, vertex count, model display 
//this tells the loops and loaders how many models will be loaded
const int nModels = 11;  // number of models in this scene
//this is used to open the model files. replace these names with your names if you want
char * modelFile[nModels] = { "ruber.tri", "unum.tri", "duo.tri", "primus.tri", "secondus.tri",
"ship.tri", "turret.tri", "turret.tri", "rocket.tri", "rocket.tri", "rocket.tri" };
float modelBR[nModels];       // model's bounding radius
float scaleValue[nModels];    // model's scaling "size" value
const int nVertices[nModels] = { 264 * 3, 264 * 3, 264 * 3, 264 * 3, 264 * 3, 940 * 3,
456 * 3, 456 * 3, 324 * 3, 324 * 3, 324 * 3 };
char * vertexShaderFile = "textureModelVertex.glsl";
char * fragmentShaderFile = "textureModelFragment.glsl";

GLuint shaderProgram;
GLuint VAO[nModels + 1];      // Vertex Array Objects
GLuint buffer[nModels + 1];   // Vertex Buffer Objects

// Shader handles, matrices, etc
GLuint MVP;  // Model View Projection matrix's handle
// vPosition, vColor, vNormal handles for models
GLuint vPosition[nModels + 1], vColor[nModels + 1], vNormal[nModels + 1];

//model locations
glm::vec3 translate[8] = {
	glm::vec3(0.0f, 0.0f, 0.0f), //ruber
	glm::vec3(4000.0f, 0.0f, 0.0f), //unum
	glm::vec3(9000.0f, 0.0f, 0.0f), //duo
	glm::vec3(2000.0f, 0.0f, 0.0f), //primus 9000 + 2000 = 11K
	glm::vec3(4000.0f, 0.0f, 0.0f), //secondus 9000 + 4000 = 13K
	glm::vec3(15000.0f, 0.0f, 0.0f), //ship
	glm::vec3(4000.0f, 250.0f, 0.0f),//turret#1
	glm::vec3(9000.0f, 500.0f, 0.0f),//turret#2
};
glm::mat4 model[nModels];  // set in display()
glm::mat4 view;            // set in init()
glm::mat4 projection;      // set in reshape()
glm::mat4 modelViewProjectionMatrix; // set in display()
float boundingRadius[nModels]; //used to get the bounding radius for calculations

//Window Title Variables
char baseStr[75] = "Warbird {next:V,prev:X,speed:T,warp:w,gravity:G,missle:F,exit:Q}: ";
char viewStr[20] = " front_view";
char fpsStr[20] = "";
char upsStr[20] = "";
char timerStr[20] = " pilot_timer";
char titleStr[225];

// rotation variables  -- the modelMatrix
//will be used for each objects rotation
GLfloat unumRotateRadian = 0.0f;
GLfloat duoRotateRadian = 0.0f;
GLfloat primusRotateRadian = 0.0f;
GLfloat secondusRotateRadian = 0.0f;
GLfloat shipRotateRadian = 0.02f;

//rotations -- stores matrix for rotations
//Main rotations occur in update
glm::mat4 identity(1.0f);
glm::mat4 unumRotation;
glm::mat4 duoRotation;
glm::mat4 primusRotation;
glm::mat4 secondusRotation;
glm::mat4 shipRotation;
glm::mat4 turretUnumRotation;
glm::mat4 turretDuoRotation;
glm::mat4 shipRocketRotation;
glm::mat4 unumTurretRocketRotation;
glm::mat4 duoTurretRocketRotation;

//scale -- holds scale matrix set in init and used in display
glm::mat4 ruberScale;
glm::mat4 unumScale;
glm::mat4 duoScale;
glm::mat4 primusScale;
glm::mat4 secondusScale;
glm::mat4 shipScale;
glm::mat4 turretScale;
glm::mat4 rocketScale;

//translations--holds translation matrix set in init and used in display
glm::mat4 ruberTrans;
glm::mat4 unumTrans;
glm::mat4 duoTrans;
glm::mat4 primusTrans;
glm::mat4 secondusTrans;
glm::mat4 shipTrans;
glm::mat4 turretTransUnum;
glm::mat4 turretTransDuo;
glm::mat4 shipRocketTrans;
glm::mat4 unumTurretRocketTrans;
glm::mat4 duoTurretRocketTrans;
glm::mat4 outOfBounds;

//used for animation
int timerDelay = 40, frameCount, updateCount = 0;
double currentTime, lastTime, timeInterval;
int fastTimerCounter = 1;  // interval or idle timer start on pilot
int count_ship_view = 0;

// viewpoint
const int num_views = 5;
glm::mat4 frontViewMat, topViewMat, shipViewMat, unumViewMat, duoViewMat;
int view_index = 0;
char * view_titles[num_views] = { "front_view", "top_view", "ship_view", "unum_view", "duo_view" };

// ship movement
GLfloat shipSpeed = 10.0f; // slow(default value): 10.0f normal: 50.0f, fast: 200.0f
int shipSpeedIndex = 0; // to control ship movement speed
glm::vec3 shipMoveVector; // to get the correct vector movement for ship

//warp variables
int warpCount = 0; //changes between unum and duo warp

/*
Eye, At, Up vectors are relative fixed vectors of Camera from the model
Position, Target, Head vectors are individual dynamic vectors of camera in world coordinates
*/
// ship camera
glm::vec3 shipCameraEye = glm::vec3(0.0f, 300.0f, 1000.0f);
glm::vec3 shipCameraAt = glm::vec3(0.0f, 300.0f, 0.0f);
glm::vec3 shipCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 shipCameraPosition = shipCameraEye + translate[5];
glm::vec3 shipCameraTarget = shipCameraAt + translate[5];
glm::vec3 shipCameraHead = shipCameraUp;

// unum camera
glm::vec3 unumCameraEye = glm::vec3(-4000.0f, 0.0f, -4000.0f);
glm::vec3 unumCameraAt = glm::vec3(4000.0f, 0.0f, 0.0f);
glm::vec3 unumCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 unumCameraPosition = glm::vec3(0.0f, 0.0f, -4000.0f);
glm::vec3 unumCameraTarget = glm::vec3(4000.0f, 0.0f, 0.0f);
glm::vec3 unumCameraHead = glm::vec3(0.0f, 1.0f, 0.0f);

// duo camera
glm::vec3 duoCameraEye = glm::vec3(-4000.0f, 0.0f, -4000.0f);
glm::vec3 duoCameraAt = glm::vec3(9000.0f, 0.0f, 0.0f);
glm::vec3 duoCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 duoCameraPosition = glm::vec3(5000.0f, 0.0f, -4000.0f);
glm::vec3 duoCameraTarget = glm::vec3(9000.0f, 0.0f, 0.0f);
glm::vec3 duoCameraHead = glm::vec3(0.0f, 1.0f, 0.0f);

// ship rocket variables
bool shipRocketActive = false;
int shipFireCount = 0;
int shipRocketFrameCount = 0;
const int shipFireMax = 9;
const int RocketFrameMax = 2000;
const float shipRocketSpeed = -50.0f;					//must be 10
int targetPlanetIndicator = 0; // when it has not been set up, it is 0

// turret rocket variables
bool unumTurretRocketActive = false;
int unumTurretRocketFrameCount = 0;
int unumTurretFireCount = 0;
bool duoTurretRocketActive = false;
int duoTurretRocketFrameCount = 0;
int duoTurretFireCount = 0;
const float turretRocketSpeed = -5.0f;
const int turretFireMax = 5;
const float turretRocketRange = 3000.0f;

//gravity variables
const float gravityConst = 90000000.0f;
float gravityForce = 0.0f;
bool gravity = false;
glm::vec3 shipGravityForward;
glm::vec3 shipGravityBackward;
glm::vec3 shipGravity;
bool gravityCheck = false;
bool backwardPress = false;	//checks if forward was pressed for forward and backward gravity

bool shipAlive = true;
bool duoTurretAlive = true;
bool unumTurretAlive = true;
int unumTurretHitCount = 0;
int duoTurretHitCount = 0;
int shipHitCount = 0;
const int turretHitCountMax = 3;
const int shipHitCountMax = 3;

// texture back ground variables
int window_w = 640, window_h = 640;
int texture_w = 640, texture_h = 480;
float vanishing_point = 100000.0f;
char * textureFile = "star.raw";
GLuint NormalMatrix;
glm::mat4 textureTrans, textureRotation, textureModel, textureModelView;
glm::mat3 normalMatrix;
const GLuint textureVertices = 6; // two triangle = plane
static const float tex_scale = vanishing_point * glm::tan(glm::radians(30.0f));
//static const float tex_scale = 5000.0f * glm::tan(glm::radians(30.0f));
GLuint ibo;
GLuint texture, showTexture;
GLuint vTexCoord[1];

static const GLfloat tex_point[] = {
	-tex_scale, tex_scale, -0.0f, 1.0f, // left top
	tex_scale, tex_scale, -0.0f, 1.0f, // right top
	tex_scale, -tex_scale, -0.0f, 1.0f, // right bottom
	-tex_scale, -tex_scale, -0.0f, 1.0f // left bottom
};

static const unsigned int tex_indices[] = {
	0, 1, 2,
	0, 2, 3
};

static const GLfloat texCoords[] = {
	0.0f, 0.0f, // 0
	1.0f, 0.0f, // 1
	1.0f, 1.0f, // 2
	0.0f, 1.0f  // 3
};

// Light
glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lightPosEC;
GLuint light, ModelView;
glm::mat4 mv, ModelViewMat;

/***** Additional Function Declaration *****/
void shipRocketInActive();
void shipRocketUpdate(glm::vec3 unumTurretPos, glm::vec3 duoTurretPos, glm::vec3 shipRocketPos);
bool shipRocketTurretCollision(int indicator);
bool shipCollisionCheck(glm::mat4 ruber, glm::mat4 unum, glm::mat4 duo, glm::mat4 primus, glm::mat4 secondus, glm::mat4 ship, glm::mat4 turretU, glm::mat4 turretD);
int findTargetTurret(glm::vec3 unumTurretPos, glm::vec3 duoTurretPos, glm::vec3 rocketPos);
void smartRocket(glm::vec3 targetPosition, glm::mat4 &rocketTrans, glm::mat4 &rocketRotation, float rocketSpeed);
void updateRotationMatrix(glm::mat4 &m, glm::vec4 R, glm::vec4 U, glm::vec4 A);
glm::vec3 calculateGravityForward(glm::mat4 ruber, glm::mat4 shipT, glm::mat4 shipR, float gravConst, float speed);
glm::vec3 calculateGravityBackward(glm::mat4 ruber, glm::mat4 shipT, glm::mat4 shipR, float gravConst, float speed);
glm::vec3 calculateGravity(glm::mat4 ruber, glm::mat4 shipT, glm::mat4 shipR, float gravConst);
void unumTurretRocketUpdate(glm::vec3 unumTurretPos, glm::vec3 shipPos);
void unumTurretRocketInActive();
bool unumTurretRocketCollision();
void duoTurretRocketUpdate(glm::vec3 duoTurretPos, glm::vec3 shipPos);
void duoTurretRocketInActive();
bool duoTurretRocketCollision();

// update and display animation state in window title
void updateTitle() {
	strcpy(titleStr, baseStr);
	strcat(titleStr, viewStr);
	strcat(titleStr, timerStr);
	strcat(titleStr, fpsStr);
	strcat(titleStr, upsStr);
	if (gravityCheck == true) {
		strcat(titleStr, " gravity: on");
	}
	else {
		strcat(titleStr, " gravity: off");
	}
	if (unumTurretAlive == false && duoTurretAlive == false) {
		strcat(titleStr, " MISSION: PASSED");
	}
	else if (shipAlive == false) {
		strcat(titleStr, " MISSION: RESIGNED");
	}
	else {
		strcat(titleStr, " MISSION: ACTIVE");
	}

	// printf("title string = %s \n", titleStr);
	glutSetWindowTitle(titleStr);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//ruber
	model[0] = ruberTrans * ruberScale;
	//unum
	model[1] = unumRotation * unumTrans * unumScale;
	//duo
	model[2] = duoRotation * duoTrans * duoScale;
	//primus
	model[3] = primusRotation * primusTrans * primusScale;
	//secondus
	model[4] = secondusRotation * secondusTrans * secondusScale;
	//ship
	if (shipAlive == true) {
		model[5] = shipTrans * shipRotation * shipScale;
	}
	else {
		model[5] = outOfBounds;
	}

	//turret unum
	if (unumTurretAlive == true) {
		model[6] = turretUnumRotation * turretTransUnum * turretScale;
	}
	else {
		model[6] = outOfBounds;
	}
	//turret duo
	if (duoTurretAlive == true) {
		model[7] = turretDuoRotation * turretTransDuo * turretScale;
	}
	else {
		model[7] = outOfBounds;
	}
	// ship rocket
	model[8] = shipRocketTrans * shipRocketRotation * rocketScale;
	// unum turret rocket
	model[9] = unumTurretRocketTrans * unumTurretRocketRotation * rocketScale;
	// duo turret rocket
	model[10] = duoTurretRocketTrans * duoTurretRocketRotation * rocketScale;

	// set up ship's camera position, target and direction of head
	if (shipAlive == true) {
		shipCameraPosition = getPosition((shipTrans*shipRotation) * glm::translate(identity, shipCameraEye));
		shipCameraTarget = getPosition((shipTrans*shipRotation) * glm::translate(identity, shipCameraAt));
		shipCameraHead = getUp((shipTrans*shipRotation));

		// make the ship view matrix
		shipViewMat = glm::lookAt(
			shipCameraPosition,
			shipCameraTarget,
			shipCameraHead
			);
	}
	else {//ship dead
		shipViewMat = identity;
	}

	//set up unum's camera position, target and direction of head
	unumCameraPosition = getPosition((unumRotation * unumTrans) * glm::translate(identity, unumCameraEye));
	unumCameraTarget = getPosition(unumRotation * unumTrans);
	unumCameraHead = getUp(unumRotation * unumTrans);

	// make unum view matrix
	unumViewMat = glm::lookAt(
		unumCameraPosition,
		unumCameraTarget,
		unumCameraHead
		);

	//set up duo's camera position, target and direction of head
	duoCameraPosition = getPosition((duoRotation * duoTrans) * glm::translate(identity, duoCameraEye));
	duoCameraTarget = getPosition(duoRotation * duoTrans);
	duoCameraHead = getUp(duoRotation * duoTrans);

	// make duo view matrix
	duoViewMat = glm::lookAt(
		duoCameraPosition,
		duoCameraTarget,
		duoCameraHead
		);

	if (view_index == 0) { // front view
		view = frontViewMat;

		// calculate rotation and translation for texture background and make model mat
		textureRotation = glm::rotate(identity, -glm::atan(1.0f / 2.0f), glm::vec3(1, 0, 0));
		textureTrans = glm::translate(identity, glm::vec3(0.0f, 0.0f, -(vanishing_point - 30000.0f)));
		textureModel = textureRotation * textureTrans;

	}
	else if (view_index == 1) { // top view
		view = topViewMat;

		// calculate rotation and translation for texture background and make model mat
		textureRotation = glm::rotate(identity, -(PI / 2), glm::vec3(1, 0, 0));
		textureTrans = glm::translate(identity, glm::vec3(0.0f, 0.0f, -(vanishing_point - 28000.0f)));
		textureModel = textureRotation * textureTrans;
	}
	else if (view_index == 2) { // ship view
		view = shipViewMat;

		// calculate rotation and translation for texture background and make model mat
		textureRotation = shipRotation;
		textureTrans = glm::translate(identity, getPosition(shipTrans));
		textureModel = glm::translate((textureTrans * textureRotation), glm::vec3(0.0f, 0.0f, -(vanishing_point - 2000.0f)));

	}
	else if (view_index == 3) { // unum view
		view = unumViewMat;

		// calculate rotation and translation for texture background and make model mat
		glm::vec3 targetVector = getPosition(unumRotation * unumTrans) - unumCameraPosition;
		targetVector = glm::normalize(targetVector);

		float radian = glm::acos(glm::dot(targetVector, glm::vec3(0.0f, 0.0f, -1.0f)));
		textureTrans = glm::translate(identity, getPosition(unumRotation * unumTrans));

		if (targetVector.x >= 0.0f) {
			textureRotation = glm::rotate(identity, (-1.0f)*radian, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			textureRotation = glm::rotate(identity, radian, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		textureModel = glm::translate((textureTrans * textureRotation), glm::vec3(0, 0, -(vanishing_point - 7000.0f)));
	}
	else if (view_index == 4) { // duo view
		view = duoViewMat;

		// calculate rotation and translation for texture background and make model mat
		glm::vec3 targetVector = getPosition(duoRotation * duoTrans) - duoCameraPosition;
		targetVector = glm::normalize(targetVector);

		float radian = glm::acos(glm::dot(targetVector, glm::vec3(0.0f, 0.0f, -1.0f)));
		textureTrans = glm::translate(identity, getPosition(duoRotation * duoTrans));

		if (targetVector.x >= 0.0f) {
			textureRotation = glm::rotate(identity, (-1.0f)*radian, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			textureRotation = glm::rotate(identity, radian, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		textureModel = glm::translate((textureTrans * textureRotation), glm::vec3(0, 0, -(vanishing_point - 7000.0f)));
	}

	// get light position
	lightPosEC = glm::vec3(view * glm::vec4(lightPos, 1.0f));

	// update model matrix, bind model and draw
	for (int m = 0; m < nModels; m++) {
		glUniform1f(showTexture, 0);

		// calculate view * model to put light in the correct space
		if (m != 5) {
			mv = view * model[m];
		}
		else { // ship has to calculated opposite of planets model for ship
			mv = -1.0f * (view * model[m]);
		}

		ModelViewMat = mv;
		normalMatrix = mv;

		//prepares the view to be loaded
		modelViewProjectionMatrix = projection * view * model[m];
		glUniformMatrix3fv(NormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(ModelViewMat));
		glUniform3fv(light, 1, glm::value_ptr(glm::vec3(lightPosEC)));		

		//binds the arrays so everything can be displayed on screen
		glBindVertexArray(VAO[m]);

		//draws VAO buffer to screen
		glDrawArrays(GL_TRIANGLES, 0, nVertices[m]);
	}

	// draw texture on background
	glUniform1f(showTexture, 1);
	textureModelView = view * textureModel;
	normalMatrix = glm::mat3(textureModelView);
	modelViewProjectionMatrix = projection * textureModelView;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
	glUniformMatrix3fv(NormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glBindVertexArray(VAO[11]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, textureVertices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	//prepares to load a new buffer
	glutSwapBuffers();
	//count frames used for FPS
	frameCount++;

	//used for animation
	currentTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time
	timeInterval = currentTime - lastTime;
	if (timeInterval >= 1000) {
		sprintf(fpsStr, " fps %4d", (int)(frameCount / (timeInterval / 1000.0f)));
		sprintf(upsStr, " ups %3d", (int)(updateCount / (timeInterval / 1000.0f)));
		lastTime = currentTime;
		frameCount = 0;
		updateCount = 0;
	}
	updateTitle();
}

// for use with Idle and intervalTimer functions
// to set rotation
void update(void) {
	if (shipAlive == true) {//while the ship is allive check for collision
		shipAlive = shipCollisionCheck(model[0], model[1], model[2], model[3], model[4], model[5], model[6], model[7]);
	}
	//update variables for collision check

	// These are the roation speeds for each of the objects in the scene
	unumRotateRadian += 0.004f;
	duoRotateRadian += 0.002f;
	primusRotateRadian += 0.002f;
	secondusRotateRadian += 0.004f;

	//reset rotations for all models
	if (unumRotateRadian > 2 * PI) unumRotateRadian = 0.0f;
	if (duoRotateRadian > 2 * PI) duoRotateRadian = 0.0f;
	if (primusRotateRadian > 2 * PI) primusRotateRadian = 0.0f;
	if (secondusRotateRadian > 2 * PI) secondusRotateRadian = 0.0f;

	//set rotations
	unumRotation = glm::rotate(identity, unumRotateRadian, glm::vec3(0, 1, 0));
	duoRotation = glm::rotate(identity, duoRotateRadian, glm::vec3(0, 1, 0));
	turretUnumRotation = glm::rotate(identity, unumRotateRadian, glm::vec3(0, 1, 0));
	turretDuoRotation = glm::rotate(identity, duoRotateRadian, glm::vec3(0, 1, 0));

	//these two moons are rotating around duo, so duo is the origin for moon 1 and moon 2.
	primusRotation = glm::rotate(glm::translate(duoRotation, translate[2]), primusRotateRadian, glm::vec3(0, 1, 0));
	secondusRotation = glm::rotate(glm::translate(duoRotation, translate[2]), secondusRotateRadian, glm::vec3(0, 1, 0));

	// ship rocket update
	glm::vec3 shipRocketPosition = getPosition(shipRocketTrans);
	glm::vec3 unumTurretPosition = getPosition(turretUnumRotation * turretTransUnum);
	glm::vec3 duoTurretPosition = getPosition(turretDuoRotation * turretTransDuo);

	if (unumTurretAlive || duoTurretAlive) {
		shipRocketUpdate(unumTurretPosition, duoTurretPosition, shipRocketPosition);
	}

	// unum turret update
	glm::vec3 shipPosition = getPosition(shipTrans);

	if (shipAlive) {
		unumTurretRocketUpdate(unumTurretPosition, shipPosition);
		duoTurretRocketUpdate(duoTurretPosition, shipPosition);
	}

	// gravity update
	if (gravity == true) {
		//apply to ship movement
		shipGravityForward = calculateGravityForward(model[0], shipTrans, shipRotation, gravityConst, shipSpeed);
		shipGravityBackward = calculateGravityBackward(model[0], shipTrans, shipRotation, gravityConst, shipSpeed);
		shipGravity = calculateGravity(model[0], shipTrans, shipRotation, gravityConst);

		//apply to ship
		if (2092.0f < distance(getPosition(model[0]), getPosition(shipTrans * shipRotation))) {
			shipTrans = shipTrans * glm::translate(identity, shipGravity);
			gravityCheck = true; // turn gravity on if ship is not inside planet
		}
		else {// if inside planet temporarily turn off gravity to allow ship to escape gravity
			shipHitCount = 3;
			shipAlive = false;
		}
	}

	// update count for UPS
	updateCount++;
}

/******************* EXTRA FUNCTIONS **********************/

// rocket inactive functions
void shipRocketInActive() {
	shipRocketActive = false;
	shipRocketFrameCount = 0;
}

void unumTurretRocketInActive() {
	unumTurretRocketActive = false;
	unumTurretRocketFrameCount = 0;
}

void duoTurretRocketInActive() {
	duoTurretRocketActive = false;
	duoTurretRocketFrameCount = 0;
}

// update duo turret rocket
void duoTurretRocketUpdate(glm::vec3 duoTurretPos, glm::vec3 shipPos) {
	// calculate the distance
	float dist = distance(duoTurretPos, shipPos);
	// when duo turret rocket is able to shoot rocket, it become active and move the rocket to current turret position
	if (dist <= turretRocketRange && !duoTurretRocketActive && (duoTurretFireCount < turretFireMax)) {
		duoTurretRocketActive = true;
		duoTurretRocketTrans = glm::translate(identity, duoTurretPos);
		duoTurretFireCount++;
	} // now the rocket is following the ship until it hits the target or used all rockets
	else if (duoTurretRocketActive &&
		(duoTurretFireCount < turretFireMax + 1) && !duoTurretRocketCollision()) {

		if (duoTurretRocketFrameCount < RocketFrameMax) {
			smartRocket(shipPos, duoTurretRocketTrans, duoTurretRocketRotation, turretRocketSpeed);
			duoTurretRocketFrameCount++;
		} // rocket inactive
		else if (duoTurretRocketFrameCount == RocketFrameMax) {
			duoTurretRocketInActive();
		}
	}
	else { // move the rocket to out of bounds
		duoTurretRocketTrans = outOfBounds;
		duoTurretRocketRotation = identity;
	}
}

// check duo turret's rocket and ship collision
bool duoTurretRocketCollision() {

	bool result = false;
	float dist;
	float shipBR = modelBR[5] * 49.751243781f;			//multiply by scale to create bounding sphere
	float rocketBR = modelBR[8] * 10.683760684f;		//multiply by scale to create bounding sphere
	// calculate distance
	dist = distance(getPosition(shipTrans), getPosition(duoTurretRocketTrans));

	if ((shipBR + rocketBR) >= dist) {
		result = true;
		duoTurretRocketInActive();
		printf("duo turret rocket hit the ship *COLLISION* \n");
		shipHitCount++;
		if (shipHitCount == shipHitCountMax) {
			shipAlive = false; // ship dead
		}
	}

	return result;
}

// update unum turret rocket
void unumTurretRocketUpdate(glm::vec3 unumTurretPos, glm::vec3 shipPos) {
	// calculate the distance
	float dist = distance(unumTurretPos, shipPos);
	// when unum turret rocket is able to shoot rocket, it become active and move the rocket to current turret position
	if (dist <= turretRocketRange && !unumTurretRocketActive && (unumTurretFireCount < turretFireMax)) {
		unumTurretRocketActive = true;
		unumTurretRocketTrans = glm::translate(identity, unumTurretPos);
		unumTurretFireCount++;
	} // now the rocket is following the ship until it hits the target or used all rockets
	else if (unumTurretRocketActive &&
		(unumTurretFireCount < turretFireMax + 1) && !unumTurretRocketCollision()) {

		if (unumTurretRocketFrameCount < RocketFrameMax) {
			smartRocket(shipPos, unumTurretRocketTrans, unumTurretRocketRotation, turretRocketSpeed);
			unumTurretRocketFrameCount++;
		} // rocket inactive
		else if (unumTurretRocketFrameCount == RocketFrameMax) {
			unumTurretRocketInActive();
		}
	}
	else { // move the rocket to out of bounds
		unumTurretRocketTrans = outOfBounds;
		unumTurretRocketRotation = identity;
	}
}

// check if ship's rocket hit the planet
bool unumTurretRocketCollision() {

	bool result = false;
	float dist;
	float shipBR = modelBR[5] * 49.751243781f;			//multiply by scale to create bounding sphere
	float rocketBR = modelBR[8] * 10.683760684f;		//multiply by scale to create bounding sphere

	// calculate distance
	dist = distance(getPosition(shipTrans), getPosition(unumTurretRocketTrans));

	if ((shipBR + rocketBR) >= dist) {
		result = true;
		unumTurretRocketInActive();
		printf("unum turret rocket hit the ship *COLLISION* \n");
		shipHitCount++;		// how many times ship has been hit
		if (shipHitCount == shipHitCountMax) {
			shipAlive = false; // ship dead
		}
	}

	return result;
}

// this is ship's rocket update controller function
// based on the ship rocket time frame count
void shipRocketUpdate(glm::vec3 unumTurretPos, glm::vec3 duoTurretPos, glm::vec3 shipRocketPos) {

	// if ship rocket is active and update all the ship's rockets movement
	if (shipRocketActive && (shipFireCount < shipFireMax + 1) && !shipRocketTurretCollision(targetPlanetIndicator)) {
		// ship's rocket stupid
		if (shipRocketFrameCount < 200) {
			// get ship's rocket move vector
			glm::vec3 rocketMoveVector = getPosition(shipRotation * glm::translate(identity, glm::vec3(0.0f, 0.0f, shipRocketSpeed)));
			shipRocketTrans = shipRocketTrans * glm::translate(identity, rocketMoveVector);
			shipRocketFrameCount++;
		}
		// ship is smart
		else if (shipRocketFrameCount >= 200 && shipRocketFrameCount < RocketFrameMax) {

			// set target turret when rocket become smart, it doesn't change target turret later
			if (shipRocketFrameCount == 200) {
				targetPlanetIndicator = findTargetTurret(unumTurretPos, duoTurretPos, shipRocketPos);
			}
			// find target turret position
			glm::vec3 targetPosition;

			if (targetPlanetIndicator == 1) {
				targetPosition = unumTurretPos;
			}
			else if (targetPlanetIndicator == 2) {
				targetPosition = duoTurretPos;
			}
			// make rocket follow the target, and change the rocket's translation, rotation matrix
			smartRocket(targetPosition, shipRocketTrans, shipRocketRotation, shipRocketSpeed);
			shipRocketFrameCount++;
		}
		// if ship's rocket time frame over 2000, it become inactive
		else if (shipRocketFrameCount == RocketFrameMax) {
			shipRocketInActive();
		}
	}
	// when the rocket is not active or spend all the rockets or rocket hit the planet
	// it goes to Ruber's center
	else {
		shipRocketTrans = outOfBounds;
		shipRocketRotation = identity;
	}
}

// check if ship's rocket hit the planet
bool shipRocketTurretCollision(int indicator) {

	bool result = false;
	glm::mat4 unumTurretRotTran = turretUnumRotation * turretTransUnum;
	glm::mat4 duoTurretRotTran = turretDuoRotation * turretTransDuo;
	float unumTurretDist = distance(getPosition(unumTurretRotTran), getPosition(shipRocketTrans));;
	float duoTurretDist = distance(getPosition(duoTurretRotTran), getPosition(shipRocketTrans));;
	float unumTurretBR = modelBR[6] * 11.0f;		//multiply by scale to create bounding sphere
	float duoTurretBR = modelBR[7] * 11.0f;			//multiply by scale to create bounding sphere
	float rocketBR = modelBR[8] * 10.683760684f;	//multiply by scale to create bounding sphere

	// target is unum turret
	if (indicator == 1) {
		//printf("distance %f \n", unumTurretDist);
		if (unumTurretBR + rocketBR >= unumTurretDist) {
			result = true;
			shipRocketInActive();
			printf("rocket hit the unum turret *COLLISION* \n");
			unumTurretHitCount++;
			if (unumTurretHitCount == turretHitCountMax) {
				printf("unum turret has been destroyed\n");
				unumTurretAlive = false; // unum turret dead
			}
		}
	} // target is duo turret
	else if (indicator == 2) {
		//printf("distance %f \n", duoTurretDist);
		if (duoTurretBR + rocketBR >= duoTurretDist) {
			result = true;
			shipRocketInActive();
			printf("rocket hit the duo turret *COLLISION* \n");
			duoTurretHitCount++;
			if (duoTurretHitCount == turretHitCountMax) {
				printf("duo turret has been destroyed\n");
				duoTurretAlive = false; // duo turret dead
			}
		}
	}
	else if (indicator == 0) { // target is not set up
		if (unumTurretBR + rocketBR >= unumTurretDist) {
			result = true;
			shipRocketInActive();
			printf("rocket hit the unum turret *COLLISION*\n");
			unumTurretHitCount++;
			if (unumTurretHitCount == turretHitCountMax) {
				printf("unum turret has been destroyed\n");
				unumTurretAlive = false; // unum turret dead
			}
		}
		if (duoTurretBR + rocketBR >= duoTurretDist) {
			result = true;
			shipRocketInActive();
			duoTurretHitCount++;
			printf("rocket hit the duo turret *COLLISION*\n");
			if (duoTurretHitCount == turretHitCountMax) {
				printf("duo turret has been destroyed\n");
				duoTurretAlive = false; // duo turret dead
			}
		}
	}

	return result;
}

// find target planet from ship between unum and duo
// if it returns 1, unum is target
// it it returns 2, duo is target
int findTargetTurret(glm::vec3 unumTurretPos, glm::vec3 duoTurretPos, glm::vec3 rocketPos) {

	int result;

	//get distances
	float unumTurretDistance = distance(unumTurretPos, rocketPos);
	float duoTurretDistance = distance(duoTurretPos, rocketPos);
	if (duoTurretAlive && unumTurretAlive) {
		if (unumTurretDistance < duoTurretDistance) {
			printf("turret_unum is target\n");
			result = 1;
		}
		else {
			printf("turret_duo is target\n");
			result = 2;
		}
	}
	else if (unumTurretAlive && !duoTurretAlive) {
		printf("turret_unum is only target\n");
		result = 1;
	}
	else {
		printf("turret_duo is only target\n");
		result = 2;
	}


	return result;
}

// when rocket become smart it follow the target
void smartRocket(glm::vec3 targetPosition, glm::mat4 &rocketTrans, glm::mat4 &rocketRotation, float rocketSpeed) {

	glm::vec3 rocketPosition = getPosition(rocketTrans);
	glm::vec3 rocketAt, rocketTarget, rocketAxis;
	glm::vec4 rocketV4Right, rocketV4Up, rocketV4At;
	float rocketACos, rocketRadian;
	glm::quat aQuat;

	// vec3 and Vec4 for rocket's right, up and at vector and target vector
	rocketAt = glm::normalize(getIn(rocketRotation));
	rocketTarget = glm::normalize(targetPosition - rocketPosition);
	rocketV4Right = glm::vec4(rocketRotation[0][0], rocketRotation[0][1], rocketRotation[0][2], 0);
	rocketV4Up = glm::vec4(shipRocketRotation[1][0], rocketRotation[1][1], rocketRotation[1][2], 0);
	rocketV4At = glm::vec4(rocketAt, 0);

	// if rocket target vector and rocket At vector are not colinear
	if (!colinear(rocketTarget, rocketAt, 0.1)) {
		// get aCos, axis of rotation and radian
		rocketACos = glm::dot(rocketTarget, rocketAt);

		// to take right direction of axis we use this order of parameters for cross product
		rocketAxis = glm::normalize(glm::cross(rocketAt, rocketTarget));

		// get radian
		rocketRadian = glm::acos(rocketACos);
		// get rotation quaternion
		aQuat = glm::angleAxis(rocketRadian, rocketAxis);

		// achieve correct ship rocket rotation's right, at, up vector
		rocketV4Right = aQuat * rocketV4Right;
		rocketV4Up = aQuat * rocketV4Up;
		rocketV4At = aQuat * rocketV4At;

		// replace ship rocket rotation's right, at, up vector
		updateRotationMatrix(rocketRotation, rocketV4Right, rocketV4Up, rocketV4At);
	} // they are colinear
	else {
		// check if they are oppoisite direction colinear vectors
		if (glm::dot(rocketTarget, rocketAt) < 0) {
			// get axis and rotation quaternion
			rocketAxis = glm::normalize(glm::cross(rocketAt, rocketTarget));
			aQuat = glm::angleAxis(PI, rocketAxis);

			// achieve correct ship rocket rotation's right, at, up vector
			rocketV4Right = aQuat * rocketV4Right;
			rocketV4Up = aQuat * rocketV4Up;
			rocketV4At = aQuat * rocketV4At;

			// replace ship rocket rotation's right, at, up vector
			updateRotationMatrix(rocketRotation, rocketV4Right, rocketV4Up, rocketV4At);
		}
	}
	// make new translation matrix
	glm::vec3 rocketMoveVector = rocketSpeed * glm::vec3(-rocketV4At[0], -rocketV4At[1], -rocketV4At[2]);
	rocketTrans = rocketTrans * glm::translate(identity, rocketMoveVector);
}

// replace rotation matrix with Right, Up, At vectors
void updateRotationMatrix(glm::mat4 &m, glm::vec4 R, glm::vec4 U, glm::vec4 A) {
	m[0][0] = R[0];
	m[0][1] = R[1];
	m[0][2] = R[2];
	m[1][0] = U[0];
	m[1][1] = U[1];
	m[1][2] = U[2];
	m[2][0] = -A[0];
	m[2][1] = -A[1];
	m[2][2] = -A[2];
}


//calculate gravity for ships movement forward
glm::vec3 calculateGravityForward(glm::mat4 ruber, glm::mat4 shipT, glm::mat4 shipR, float gravConst, float speed) {
	glm::vec3 result;
	glm::vec3 dv;		//contains the ships movement information
	glm::vec3 gv;		//contains the gravitational force
	float gForce;		//gives the force of gravity as a float value to be applied to gv vector
	float dist = distance(getPosition(ruber), getPosition(shipT * shipR));

	gForce = gravConst / (dist * dist); //gravity = gravityConst/distance^2

	dv = getIn(shipT * shipR) * speed;	//LAT vector of ship and make it step forward with speed

	gv = glm::normalize(glm::vec3(0, 0, 0) - getPosition(shipT * shipR)); //normalize values
	gv *= gForce; //calculate gravity vector

	result = dv + gv; //Add ships step vector with gravity vector to apply gravity to ships movement

	return result;	//result is returned (in update) then used (in keyboardSpecial)
}

//calculate gravity for ships movement back
glm::vec3 calculateGravityBackward(glm::mat4 ruber, glm::mat4 shipT, glm::mat4 shipR, float gravConst, float speed) {
	glm::vec3 result;
	glm::vec3 dv;		//contains the ships movement information
	glm::vec3 gv;		//contains the gravitational force
	float gForce;		//gives the force of gravity as a float value to be applied to gv vector
	float dist = distance(getPosition(ruber), getPosition(shipT * shipR));

	gForce = gravConst / (dist * dist); //gravity = gravityConst/distance^2

	dv = getOut(shipT * shipR) *  speed;	//LAT vector of ship and make it step forward with speed

	gv = glm::normalize(glm::vec3(0, 0, 0) - getPosition(shipT * shipR)); //normalize values
	gv *= gForce; //calculate gravity vector

	result = dv + gv; //Add ships step vector with gravity vector to apply gravity to ships movement

	return result;	//result is returned (in update) then used (in keyboardSpecial)
}

//calculate gravity for ships movement forward
glm::vec3 calculateGravity(glm::mat4 ruber, glm::mat4 shipT, glm::mat4 shipR, float gravConst) {
	glm::vec3 result;
	glm::vec3 dv;		//contains the ships movement information
	glm::vec3 gv;		//contains the gravitational force
	float gForce;		//gives the force of gravity as a float value to be applied to gv vector
	float dist = distance(getPosition(ruber), getPosition(shipT * shipR));

	gForce = gravConst / (dist * dist); //gravity = gravityConst/distance^2

	dv = getIn(shipT * shipR);	//LAT vector of ship and make it step forward with speed

	gv = glm::normalize(glm::vec3(0, 0, 0) - getPosition(shipT * shipR)); //normalize values
	gv *= gForce; //calculate gravity vector

	result = dv + gv; //Add ships step vector with gravity vector to apply gravity to ships movement

	return result;	//result is returned (in update) then used (in keyboardSpecial)
}
bool shipCollisionCheck(glm::mat4 ruber, glm::mat4 unum, glm::mat4 duo, glm::mat4 primus, glm::mat4 secondus, glm::mat4 ship, glm::mat4 turretU, glm::mat4 turretD) {
	bool result;

	//get distances for calculations
	float ruberDist = distance(getPosition(ruber), getPosition(ship));
	float unumDist = distance(getPosition(unum), getPosition(ship));
	float duoDist = distance(getPosition(duo), getPosition(ship));
	float primusDist = distance(getPosition(primus), getPosition(ship));
	float secondusDist = distance(getPosition(secondus), getPosition(ship));
	float turretUDist = distance(getPosition(turretU), getPosition(ship));
	float turretDDist = distance(getPosition(turretD), getPosition(ship));

	//get bounding radius for each and add together
	float ruberShipBoundingRad = (modelBR[0] * 400) + (modelBR[5] * 50);
	float unumShipBoundingRad = (modelBR[1] * 40) + (modelBR[5] * 50);
	float duoShipBoundingRad = (modelBR[2] * 80) + (modelBR[5] * 50);
	float primusShipBoundingRad = (modelBR[3] * 20) + (modelBR[5] * 50);
	float secondusShipBoundingRad = (modelBR[4] * 30) + (modelBR[5] * 50);
	float turretUShipBoundingRad = (modelBR[6] * 20) + (modelBR[5] * 50);
	float turretDShipBoundingRad = (modelBR[7] * 20) + (modelBR[5] * 50);

	//perform collision checks
	if (ruberShipBoundingRad > ruberDist) {//collision with ruber
		result = false;
	}
	else if (unumShipBoundingRad > unumDist) {//collision with unum
		result = false;
	}
	else if (duoShipBoundingRad > duoDist) {//collision with duo
		result = false;
	}
	else if (primusShipBoundingRad > primusDist) {//collision with primus
		result = false;
	}
	else if (secondusShipBoundingRad > secondusDist) {//collision with secondus
		result = false;
	}
	else if (turretUShipBoundingRad > turretUDist) {//collision with unums turret
		result = false;
	}
	else if (turretDShipBoundingRad > turretDDist) {//collision with duos turret
		result = false;
	}
	else {//no collision
		result = true;
	}

	//return result
	return result;
}

/******************* EXTRA FUNCTIONS END **********************/

void reshape(int width, int height) {
	float aspectRatio = (float)window_w / (float)window_h;
	float FOVY = glm::radians(60.0f);
	glViewport(0, 0, width, height);
	printf("reshape: FOVY = %5.2f, width = %4d height = %4d aspect = %5.2f \n",
		FOVY, width, height, aspectRatio);
	projection = glm::perspective(FOVY, aspectRatio, 1.0f, vanishing_point);
}


// Estimate FPS, use for fixed interval timer driven animation
//this is used for animation
void intervalTimer(int i) {
	glutTimerFunc(timerDelay, intervalTimer, 1);
	update();
}

// load the shader programs, vertex data from model files, create the solids, set initial view
void init() {
	// WHEN YOU USE loadBufferModel:
	// These two commands must come before loadBufferModel or it will not work. The Shader' must be active in order for the
	// buffer to work
	shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
	glUseProgram(shaderProgram);

	// generate VAOs and VBOs
	glGenVertexArrays(nModels + 1, VAO);
	glGenBuffers(nModels + 1, buffer);

	// load the buffers from the model files
	// this loop loads all information from each file into buffer to be passed to shader
	for (int i = 0; i < nModels; i++) {
		modelBR[i] = loadModelBuffer(modelFile[i], nVertices[i], VAO[i], buffer[i], shaderProgram,
			vPosition[i], vColor[i], vNormal[i], "vPosition", "vColor", "vNormal");
	}

	// texture vao and buffers	
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tex_indices), tex_indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO[11]);
	glEnableVertexAttribArray(buffer[11]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[11]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_point) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tex_point), tex_point);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(tex_point), sizeof(texCoords), texCoords);

	vPosition[11] = glGetAttribLocation(shaderProgram, "vPosition");
	glVertexAttribPointer(vPosition[11], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition[11]);

	vTexCoord[0] = glGetAttribLocation(shaderProgram, "vTexCoord");
	glVertexAttribPointer(vTexCoord[0], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(tex_point)));
	glEnableVertexAttribArray(vTexCoord[0]);

	// uniform
	light = glGetUniformLocation(shaderProgram, "light_position");
	ModelView = glGetUniformLocation(shaderProgram, "ModelView");	
	showTexture = glGetUniformLocation(shaderProgram, "IsTexture");
	NormalMatrix = glGetUniformLocation(shaderProgram, "NormalMatrix");
	MVP = glGetUniformLocation(shaderProgram, "ModelViewProjection");

	// texture load
	texture = loadRawTexture(texture, textureFile, texture_w, texture_h);
	if (texture != 0) {
		printf("texture file, read, texture generated and bound  \n");
	}
	else { // texture file loaded
		printf("Texture in file %s NOT LOADED !!! \n");
	}

	// init each viewpoint matrix
	frontViewMat = glm::lookAt(
		glm::vec3(0.0f, 10000.0f, 20000.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	topViewMat = glm::lookAt(
		glm::vec3(0.0f, 20000.0f, 0.0000001f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	shipViewMat = glm::lookAt(
		shipCameraPosition,
		shipCameraTarget,
		shipCameraHead
	);

	unumViewMat = glm::lookAt(
		unumCameraEye,
		unumCameraAt,
		unumCameraUp
	);

	duoViewMat = glm::lookAt(
		duoCameraEye,
		duoCameraAt,
		duoCameraUp
	);

	// set view as front view as a default
	view = frontViewMat;

	//set scales
	ruberScale = glm::scale(identity, glm::vec3(400.0f, 400.0f, 400.0f));
	unumScale = glm::scale(identity, glm::vec3(40.0f, 40.0f, 40.0f));
	duoScale = glm::scale(identity, glm::vec3(80.0f, 80.0f, 80.0f));
	primusScale = glm::scale(identity, glm::vec3(20.0f, 20.0f, 20.0f));
	secondusScale = glm::scale(identity, glm::vec3(30.0f, 30.0f, 30.0f));
	shipScale = glm::scale(identity, glm::vec3(49.751243781f, 49.751243781f, 49.751243781f));
	turretScale = glm::scale(identity, glm::vec3(20.0f, 20.0f, 20.0f));
	rocketScale = glm::scale(identity, glm::vec3(10.683760684f, 10.683760684f, 10.683760684f));

	//set translations--save to
	ruberTrans = glm::translate(identity, translate[0]);
	unumTrans = glm::translate(identity, translate[1]);
	duoTrans = glm::translate(identity, translate[2]);
	primusTrans = glm::translate(identity, translate[3]);
	secondusTrans = glm::translate(identity, translate[4]);
	shipTrans = glm::translate(identity, translate[5]);
	turretTransUnum = glm::translate(identity, translate[6]); //turret on unum
	turretTransDuo = glm::translate(identity, translate[7]); //turret on unum
	shipRocketTrans = ruberTrans;
	unumTurretRocketTrans = ruberTrans;
	duoTurretRocketTrans = ruberTrans;
	outOfBounds = glm::translate(identity, glm::vec3(0.0f, 0.0f, 200000.0f));

	// set render state values
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033: case 'q':  case 'Q': exit(EXIT_SUCCESS); break;
	case 's': case 'S':
		if (shipSpeedIndex == 0) {
			shipSpeedIndex = 1;
			shipSpeed = 50.0f;
		}
		else if (shipSpeedIndex == 1) {
			shipSpeedIndex = 2;
			shipSpeed = 200.0f;
		}
		else if (shipSpeedIndex == 2) {
			shipSpeedIndex = 0;
			shipSpeed = 10.0f;
		}
		break;
	case 'v': case 'V': // change the camera position (next)
		view_index = view_index + 1;
		if (view_index == num_views) {
			view_index = 0;
		}
		strcpy(viewStr, view_titles[view_index]);
		break;
	case 'x': case 'X': // change the camera position (previous)
		view_index = view_index - 1;
		if (view_index < 0) {
			view_index = 4;
		}
		strcpy(viewStr, view_titles[view_index]);

		break;
	case 't': case 'T': // change the animation timer
		if (fastTimerCounter == 0) {
			timerDelay = 40;
			strcpy(timerStr, " pilot_timer");
			fastTimerCounter = 1;
		}
		else if (fastTimerCounter == 1) {
			timerDelay = 100;
			strcpy(timerStr, " trainee_timer");
			fastTimerCounter = 2;
		}
		else if (fastTimerCounter == 2) {
			timerDelay = 500;
			strcpy(timerStr, " debug_timer");
			fastTimerCounter = 3;
		}
		else if (fastTimerCounter == 3) {
			timerDelay = 5;
			strcpy(timerStr, " ace_timer");
			fastTimerCounter = 0;
		}
		break;
	case 'w': case 'W':
		// warp ship to unum or duo
		if (warpCount == 0) { // unum warp

			// get target vector and normalize
			glm::vec3 targetVector = getPosition(unumRotation * unumTrans) - unumCameraPosition;
			targetVector = glm::normalize(targetVector);

			// calculate radian between negative z axis and target vector
			float warpRadian = glm::acos(glm::dot(targetVector, glm::vec3(0.0f, 0.0f, -1.0f)));

			// make new ship's tranRot matrix
			if (targetVector.x >= 0.0f) { // take negative angle
				shipTrans = glm::translate(identity, unumCameraPosition);
				shipRotation = glm::rotate(identity, (-1.0f)*warpRadian, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else { // take positive angle
				shipTrans = glm::translate(identity, unumCameraPosition);
				shipRotation = glm::rotate(identity, warpRadian, glm::vec3(0.0f, 1.0f, 0.0f));
			}

			warpCount++;
		}
		else if (warpCount == 1) { // duo warp

			// get target vector and normalize
			glm::vec3 targetVector = getPosition(duoRotation * duoTrans) - duoCameraPosition;
			targetVector = glm::normalize(targetVector);

			// calculate radian between negative z axis and target vector
			float warpRadian = glm::acos(glm::dot(targetVector, glm::vec3(0.0f, 0.0f, -1.0f)));

			// make new ship's tranRot matrix
			if (targetVector.x >= 0.0f) { // take negative angle
				shipTrans = glm::translate(identity, duoCameraPosition);
				shipRotation = glm::rotate(identity, (-1.0f)*warpRadian, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else { // take positive angle
				shipTrans = glm::translate(identity, duoCameraPosition);
				shipRotation = glm::rotate(identity, warpRadian, glm::vec3(0.0f, 1.0f, 0.0f));
			}

			warpCount--;
		}
		break;
	case 'f': case 'F': //ship fire missle
		if (!shipRocketActive && (shipFireCount < shipFireMax) && (shipHitCount < shipHitCountMax)) {
			shipFireCount++; // restrict the number of ship fires
			shipRocketActive = true; // ship can fire when it become true
			shipRocketTrans = shipTrans; // when ship fire it starts from ship's center
			shipRocketRotation = shipRotation; // ship rocket have same rotation matrix with ship
		}
		else if (shipHitCount == shipHitCountMax) {
			printf("ship has been destroyed, so ship can't fire anymore\n");
		}
		break;
	case 'g': case 'G': // gravity on and off
		if (gravity == false) {
			gravity = true; //turn gravity on
			gravityCheck = true; //used for display
		}
		else {
			gravity = false; //turn gravity off
			gravityCheck = false; //used for display
		}
		break;
	}
}

void keyboardSpecial(int key, int x, int y) {
	int mod = glutGetModifiers();
	if (shipAlive == true) {
		switch (key) {
		case GLUT_KEY_UP:
			if (mod == GLUT_ACTIVE_CTRL) { // pitch up
				shipRotation = shipRotation * glm::rotate(identity, shipRotateRadian, glm::vec3(1, 0, 0));
			}
			else { // move ship forward
				backwardPress = false; // forward was pressed so this should be false (cause false = forward true=back)
				if (gravity == false) {// if gravity is off simply move forward without gravity
					shipMoveVector = getPosition(shipRotation * glm::translate(identity, glm::vec3(0.0f, 0.0f, (-1.0f)*shipSpeed)));
					shipTrans = shipTrans * glm::translate(identity, shipMoveVector);
				}
				else {	 // gravity is on (check to see if inside planet)
					// if not inside planet apply gravity to ship
					if (2005.0f < distance(getPosition(model[0]), getPosition(shipTrans * shipRotation))) {
						shipTrans = shipTrans * glm::translate(identity, shipGravityForward);
						gravityCheck = true; // turn gravity back in ship is not inside planet
					}
					else {
						// if inside planet temporarily turn off gravity to allow ship to escape gravity
						shipHitCount = 3;
						shipAlive = false;
					}
				}
			}
			break;
		case GLUT_KEY_LEFT:
			if (mod == GLUT_ACTIVE_CTRL) { // roll to left
				shipRotation = shipRotation * glm::rotate(identity, shipRotateRadian, glm::vec3(0, 0, 1));
			}
			else { //rotate ship left
				// update ship rotation matrix
				shipRotation = shipRotation * glm::rotate(identity, shipRotateRadian, glm::vec3(0, 1, 0));
			}
			break;
		case GLUT_KEY_DOWN:
			if (mod == GLUT_ACTIVE_CTRL) { // pitch down
				shipRotation = shipRotation * glm::rotate(identity, (-1.0f) * shipRotateRadian, glm::vec3(1, 0, 0));
			}
			else { //move ship backward
				backwardPress = true; // back was pressed so this is true
				if (gravity == false) {// if gravity is off simply move forward without gravity
					shipMoveVector = getPosition(shipRotation * glm::translate(identity, glm::vec3(0.0f, 0.0f, shipSpeed)));
					shipTrans = shipTrans * glm::translate(identity, shipMoveVector);
				}
				else {  // gravity is on (check to see if inside planet)
					// if not inside planet apply gravity to ship
					if (2005.0f < distance(getPosition(model[0]), getPosition(shipTrans * shipRotation))) {
						shipTrans = shipTrans * glm::translate(identity, shipGravityBackward);
						gravityCheck = true; // turn gravity on if ship is not inside planet
					}
					else {// if inside planet temporarily turn off gravity to allow ship to escape gravity
						shipHitCount = 3;
						shipAlive = false;
					}
				}
			}
			break;
		case GLUT_KEY_RIGHT:
			if (mod == GLUT_ACTIVE_CTRL) { // roll to right
				shipRotation = shipRotation * glm::rotate(identity, (-1.0f) * shipRotateRadian, glm::vec3(0, 0, 1));
			}
			else { //rotate ship right
				// update ship rotation matrix
				shipRotation = shipRotation * glm::rotate(identity, (-1.0f) * shipRotateRadian, glm::vec3(0, 1, 0));
			}
			break;
		}
		//end if ship alive
	}
	else {//ship is dead here

	}

}

void cleanUp(void) {
	glDeleteBuffers(1, &buffer[11]);
	glDeleteBuffers(1, &ibo);
	freeTexture(texture);
	printf("texture clean up\n");
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
# ifdef __Mac__
	// Can't change the version in the GLUT_3_2_CORE_PROFILE
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
# endif
# ifndef __Mac__
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
# endif
	glutInitWindowSize(window_w, window_h);
	// set OpenGL and GLSL versions to 3.3 for Comp 465/L, comment to see highest versions
# ifndef __Mac__
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
# endif
	glutCreateWindow("465 manyModelsStatic Example");
	// initialize and verify glew
	glewExperimental = GL_TRUE;  // needed my home system
	GLenum err = glewInit();
	if (GLEW_OK != err)
		printf("GLEW Error: %s \n", glewGetErrorString(err));
	else {
		printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
		printf("OpenGL %s, GLSL %s\n",
			glGetString(GL_VERSION),
			glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
	// initialize scene
	init();

	// set glut callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecial);
	// animation of rotation
	glutIdleFunc(display);
	glutTimerFunc(timerDelay, intervalTimer, 1);
	glutCloseFunc(cleanUp);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
	printf("done\n");
	return 0;
}