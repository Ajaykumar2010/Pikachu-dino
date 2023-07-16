//g++ map.cpp -lglfw -lGL -lGLU -lglut -lSOIL -lGLEW -lX11 -lpthread -lXi

#include "GL/include/GL/glew.h"
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include "SOIL.h"

// Window dimensions
int windowWidth = 440;
int windowHeight = 320;
int lives = 3;

// Game state
enum GameState {
    PLAYING,
    GAME_OVER
};

GameState gameState = PLAYING;

// Dinosaur properties
float dinosaurX = 100.0f;
float dinosaurY = 0.0f;
float dinosaurSize = 100.0f;
float dinosaurJumpHeight = 150.0f;
float dinosaurJumpSpeed = 9.0f;
bool isJumping = false;

// Obstacle properties
struct Obstacle {
    float x;
    float width;
    bool counted;
};
float obstacleHeight = 10.0f;

std::vector<Obstacle> obstacles;

// Game over message properties
std::string gameOverMessage = "You lose! Press any key to restart.";

// Score
int score = 0;

// Texture properties
GLuint dinosaurTexture;
GLuint obstacleTexture;

// Function declaration
GLuint LoadTexture(const char* filename);

// Initialize the game
void initialize() 
{
    // Set up initial obstacle
    Obstacle obstacle;
    obstacle.x = 0;
    obstacle.width = 50.0f;
    obstacle.counted = false;
    obstacles.push_back(obstacle);

    // Reset score
    score = 0;
    lives = 3;

    // Load textures
    dinosaurTexture = LoadTexture("pikachu.png");
    obstacleTexture = LoadTexture("obstacle_texture.png");
}

// Load texture from file
GLuint LoadTexture(const char* filename) 
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load texture image
    int width, height;
    unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);

    // Set texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    // glGenerateMipmap(GL_TEXTURE_2D);

    // Free image data
    SOIL_free_image_data(image);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D,0);

    return textureID;
}

// Draw the dinosaur
void drawDinosaur()
 {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, dinosaurTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(dinosaurX, dinosaurY);
    glTexCoord2f(1, 1);
    glVertex2f(dinosaurX + dinosaurSize, dinosaurY);
    glTexCoord2f(1, 0);
    glVertex2f(dinosaurX + dinosaurSize, dinosaurY + dinosaurSize);
    glTexCoord2f(0, 0);
    glVertex2f(dinosaurX, dinosaurY + dinosaurSize);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawObstacles() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, obstacleTexture);
    for (const auto& obstacle : obstacles) 
    {
        // Draw the filled obstacle
        glBegin(GL_QUADS);
        glTexCoord2f(1, 1);
        glVertex2f(obstacle.x, 0.0f);
        glTexCoord2f(0, 1);
        glVertex2f(obstacle.x + obstacle.width, 0.0f);
        glTexCoord2f(0, 0);
        glVertex2f(obstacle.x + obstacle.width, dinosaurSize);
        glTexCoord2f(1, 0);
        glVertex2f(obstacle.x, dinosaurSize);
        glEnd();

        // Draw the outline around the obstacle
        glLineWidth(20.0f);
 
        // white color for outline 
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_LINE_LOOP);
        glVertex2f(obstacle.x, 0.0f);
        glVertex2f(obstacle.x + obstacle.width, 0.0f);
        glVertex2f(obstacle.x + obstacle.width, dinosaurSize);
        glVertex2f(obstacle.x, dinosaurSize);
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(obstacle.x, 0.0f);
        glVertex2f(obstacle.x, dinosaurSize);

        glVertex2f(obstacle.x + obstacle.width, 0.0f);
        glVertex2f(obstacle.x + obstacle.width, dinosaurSize);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

// Draw the game over message
void drawGameOverMessage() 
{
    glPushMatrix();
    glTranslatef(windowWidth / 2 - 100, windowHeight / 2, 0.0f);
    glColor3f(1.0f, 1.0f, 0.0f); // Red color for game over message
    glRasterPos2f(0.0f, 0.0f);
    for (char c : gameOverMessage) 
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    glPopMatrix();
}

// Draw the score
void drawScore() 
{
    glPushMatrix();
    glTranslatef(10.0f, windowHeight - 30.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // White color for score
    std::string scoreString = "Score: " + std::to_string(score);
    glRasterPos2f(0.0f, 0.0f);
    for (char c : scoreString)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    glPopMatrix();
}

// Draw the remaining lives
void drawLives() 
{
    glPushMatrix();
    glTranslatef(windowWidth - 100.0f, windowHeight - 30.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(0.0f, 0.0f);
    std::string livesText = "Lives: " + std::to_string(lives);
    for (char c : livesText) 
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    glPopMatrix();
}
void update() 
{
    printf("Update Log:\n");
    printf("Dinosaur Position: (%.2f, %.2f)\n", dinosaurX, dinosaurY);
    printf("Score: %d\n", score);
    printf("Lives: %d\n", lives);
    printf("---------------------------\n");
    if (gameState == PLAYING) 
    {
        // Update dinosaur position
        if (isJumping) 
        {
            dinosaurY += dinosaurJumpSpeed;
            dinosaurJumpSpeed -= 0.5f;
            if (dinosaurY <= 0.0f) 
            {
                dinosaurY = 0.0f;
                dinosaurJumpSpeed = 9.0f;
                isJumping = false;
            }
        }

        // Update obstacle positions
        for (auto& obstacle : obstacles)
         {
            obstacle.x -= 5.0f;
            if (obstacle.x < -obstacle.width) 
            {
                obstacle.x = windowWidth;
                obstacle.counted = false;
            }

            // Check for collision
            if (obstacle.x <= dinosaurX + dinosaurSize &&
                obstacle.x + obstacle.width >= dinosaurX &&
                dinosaurY <= obstacleHeight + dinosaurSize) 
                {
                if (!obstacle.counted) 
                {
                    lives--;
                    obstacle.counted = true;
                    if (lives <= 0) 
                    {
                        gameState = GAME_OVER;
                    }
                }
            }

            // Update score
            if (obstacle.x + obstacle.width <= dinosaurX && !obstacle.counted) 
            {
                score++;
                obstacle.counted = true;
            }
            if(score==5)
            {
                    lives++;
                    score=0;
            }
            }
        }
    }

// Keyboard event handler
void onKeyPress(unsigned char key, int x, int y) {
    if (gameState == GAME_OVER) 
    {
        // Restart the game
        obstacles.clear();
        initialize();
        gameState = PLAYING;
    }
     else if (key == ' ' || key==GLUT_KEY_UP) 
     {
        if (!isJumping) 
        {
            isJumping = true;
            dinosaurJumpSpeed = 12.0f; // Adjust the jump speed for long jumps
        }
         else 
         {
            dinosaurJumpSpeed = 12.0f; // Adjust the jump speed for long jumps
         }
    }
}

void catchKey(int key, int x, int y)
{
         if(key == GLUT_KEY_LEFT)    
         {
             printf("Left key is pressed\n");
             dinosaurX-=20;
         }
        else if(key == GLUT_KEY_RIGHT)
        {
            dinosaurX+=20;
            printf("Right key is pressed\n");
        }
        else if(key == GLUT_KEY_DOWN)
        {
            dinosaurJumpSpeed=-12.0;
            printf("Down key is pressed\n");
        }
        else if(key == GLUT_KEY_UP)
        {
        if (!isJumping) 
        {
            isJumping = true;
            dinosaurJumpSpeed = 12.0f; // Adjust the jump speed for long jumps
        }
        else 
        {
            dinosaurJumpSpeed = 12.0f; // Adjust the jump speed for long jumps
        }
            printf("Up key is pressed\n");
        }
}

// Mouse event handler
void onMouseClick(int button, int state, int x, int y) 
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
    {
        if (!isJumping) 
        {
            isJumping = true;
            printf("GLUT_LEFT_BUTTON is pressed\n");
            dinosaurJumpSpeed = 12.0f; // Adjust the jump speed for long jumps
        }
         else 
        {
            printf("GLUT_LEFT_BUTTON is pressed\n");
            dinosaurJumpSpeed = 12.0f; // Adjust the jump speed for long jumps
        }
    }
}

// Display callback function
void display() 
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glLoadIdentity();
    drawDinosaur();
    drawObstacles();
    drawScore();    
    drawLives();

    
    if ((gameState == GAME_OVER) && (lives==0)==1)
    {
        drawGameOverMessage();
    }
    glutSwapBuffers();
}

// Timer callback function
void timer(int) 
{
    update();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// Reshape callback function
void reshape(int width, int height) 
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    windowWidth = width;
    windowHeight = height;
}

int main(int argc, char** argv) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Pikachu Dino Game");
    glClear(0.0); 
    initialize();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(catchKey);
    glutKeyboardFunc(onKeyPress);
    glutMouseFunc(onMouseClick);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
