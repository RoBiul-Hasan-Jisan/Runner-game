#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// ==================== GAME CONSTANTS ====================
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 550;
const int GROUND_Y = WINDOW_HEIGHT - 68;
const int PLAYER_WIDTH = 34;
const int PLAYER_HEIGHT = 44;
const int PLAYER_X = 120;

// ==================== GAME STRUCTURES ====================
struct Obstacle {
    float x, y, w, h;
    std::string variant;
    bool counted;
    int colorVibe;
};

struct Powerup {
    float x, y, w, h;
    std::string type;
    bool collected;
};

struct Particle {
    float x, y, vx, vy;
    int life;
    float size;
    float r, g, b;
};

// ==================== GLOBAL VARIABLES ====================
bool gameRunning = true;
float score = 0;
float distance = 0;
int bestScore = 0;
int frame = 0;
float baseSpeed = 5.2f;
float currentSpeed = 5.2f;

// Power-up system
bool hasShield = false;
int shieldFramesLeft = 0;
int invincibilityFlash = 0;
float screenShake = 0;

// Player physics
float playerY = GROUND_Y - PLAYER_HEIGHT;
float yVelocity = 0;
bool isOnGround = true;
int coyoteTimer = 0;
int jumpBufferTimer = 0;
const float GRAVITY = 0.85f;
const float JUMP_POWER = -11.8f;
const int COYOTE_TIME = 6;
const int JUMP_BUFFER = 8;

// Game objects
std::vector<Obstacle> obstacles;
std::vector<Powerup> powerups;
std::vector<Particle> particles;

int spawnCounter = 0;
int powerupSpawnCounter = 0;

// ==================== UTILITY FUNCTIONS ====================
void drawRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawRectAlpha(float x, float y, float w, float h, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawCircle(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        float x = cx + radius * cos(angle);
        float y = cy + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawCircleAlpha(float cx, float cy, float radius, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        float x = cx + radius * cos(angle);
        float y = cy + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawEllipse(float cx, float cy, float rx, float ry, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i += 15) {
        float angle = i * 3.14159f / 180.0f;
        float x = cx + rx * cos(angle);
        float y = cy + ry * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawRoundedRect(float x, float y, float w, float h, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x + radius, y);
    glVertex2f(x + w - radius, y);
    glVertex2f(x + w - radius, y + h);
    glVertex2f(x + radius, y + h);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x, y + radius);
    glVertex2f(x + w, y + radius);
    glVertex2f(x + w, y + h - radius);
    glVertex2f(x, y + h - radius);
    glEnd();

    drawCircle(x + radius, y + radius, radius, r, g, b);
    drawCircle(x + w - radius, y + radius, radius, r, g, b);
    drawCircle(x + radius, y + h - radius, radius, r, g, b);
    drawCircle(x + w - radius, y + h - radius, radius, r, g, b);
}

void drawText(float x, float y, std::string text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// ==================== GAME LOGIC ====================
void updateBest() {
    int intScore = (int)score;
    if (intScore > bestScore) {
        bestScore = intScore;
    }
}

void restartGame() {
    gameRunning = true;
    score = 0;
    distance = 0;
    currentSpeed = baseSpeed;
    obstacles.clear();
    powerups.clear();
    particles.clear();
    hasShield = false;
    shieldFramesLeft = 0;
    invincibilityFlash = 0;
    screenShake = 0;
    playerY = GROUND_Y - PLAYER_HEIGHT;
    yVelocity = 0;
    isOnGround = true;
    coyoteTimer = 0;
    jumpBufferTimer = 0;
    frame = 0;
    spawnCounter = 6;
    powerupSpawnCounter = 45;
}

void spawnObstacle() {
    float obsHeight = 32 + (rand() % 17);
    float obsY = GROUND_Y - obsHeight;
    std::string variant = (rand() % 100 > 70) ? "spike" : "block";

    Obstacle obs;
    obs.x = WINDOW_WIDTH;
    obs.y = obsY;
    obs.w = 28;
    obs.h = obsHeight;
    obs.variant = variant;
    obs.counted = false;
    obs.colorVibe = 30 + (rand() % 50);
    obstacles.push_back(obs);
}

void spawnPowerup() {
    Powerup p;
    p.x = WINDOW_WIDTH;
    p.y = GROUND_Y - 28;
    p.w = 22;
    p.h = 22;
    p.type = "shield";
    p.collected = false;
    powerups.push_back(p);
}

void addParticles(float x, float y, float r, float g, float b) {
    for (int i = 0; i < 8; i++) {
        Particle p;
        p.x = x + ((rand() % 30) - 15);
        p.y = y + ((rand() % 24) - 12);
        p.vx = ((rand() % 80) - 40) / 10.0f;
        p.vy = ((rand() % 80) - 40) / 10.0f - 2;
        p.life = 14 + (rand() % 8);
        p.size = 3 + (rand() % 5);
        p.r = r;
        p.g = g;
        p.b = b;
        particles.push_back(p);
    }
}

bool rectCollide(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return !(x2 > x1 + w1 || x2 + w2 < x1 || y2 > y1 + h1 || y2 + h2 < y1);
}

void updateGame() {
    if (!gameRunning) return;

    // Dynamic difficulty
    float targetSpeed = baseSpeed + (distance / 850.0f);
    if (targetSpeed > 12.8f) targetSpeed = 12.8f;
    currentSpeed = currentSpeed * 0.97f + targetSpeed * 0.03f;
    if (currentSpeed < 4.5f) currentSpeed = 4.5f;

    // Update distance
    distance += currentSpeed * 0.25f;

    // Player physics
    yVelocity += GRAVITY;
    playerY += yVelocity;
    float floorLimit = GROUND_Y - PLAYER_HEIGHT;

    if (playerY >= floorLimit) {
        playerY = floorLimit;
        yVelocity = 0;
        isOnGround = true;
        coyoteTimer = COYOTE_TIME;
    } else {
        if (isOnGround) isOnGround = false;
        if (coyoteTimer > 0) coyoteTimer--;
    }

    if (playerY < 0) {
        playerY = 0;
        if (yVelocity < 0) yVelocity = 0;
    }

    // Jump buffer
    if (jumpBufferTimer > 0) jumpBufferTimer--;
    if (jumpBufferTimer > 0 && (isOnGround || coyoteTimer > 0)) {
        yVelocity = JUMP_POWER;
        isOnGround = false;
        coyoteTimer = 0;
        jumpBufferTimer = 0;
        addParticles(PLAYER_X + 16, GROUND_Y - 8, 1.0f, 0.7f, 0.3f);
    }

    // Spawn obstacles
    if (spawnCounter <= 0) {
        int delay = std::max(32, 58 - (int)(currentSpeed * 1.5f));
        spawnCounter = delay;
        spawnObstacle();
    } else {
        spawnCounter--;
    }

    // Spawn powerups
    if (powerupSpawnCounter <= 0 && gameRunning) {
        powerupSpawnCounter = 180 + (rand() % 150);
        spawnPowerup();
    } else {
        powerupSpawnCounter--;
    }

    // Update obstacles and scoring
    for (auto& obs : obstacles) {
        obs.x -= currentSpeed;
        if (!obs.counted && obs.x + obs.w < PLAYER_X) {
            obs.counted = true;
            score += 12;
            updateBest();
            for (int p = 0; p < 4; p++) {
                addParticles(obs.x + obs.w / 2, obs.y - 5, 1.0f, 0.88f, 0.54f);
            }
        }
    }

    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(),
        [](const Obstacle& o) { return o.x + o.w < -30; }), obstacles.end());

    // Update powerups
    for (auto& p : powerups) {
        p.x -= currentSpeed;
    }

    powerups.erase(std::remove_if(powerups.begin(), powerups.end(),
        [](const Powerup& p) { return p.x + p.w < 0 || p.collected; }), powerups.end());

    // Powerup collection
    for (auto& p : powerups) {
        if (!p.collected && rectCollide(PLAYER_X, playerY, PLAYER_WIDTH, PLAYER_HEIGHT, p.x, p.y, p.w, p.h)) {
            p.collected = true;
            hasShield = true;
            shieldFramesLeft = 450;
            addParticles(p.x + 10, p.y + 10, 1.0f, 0.85f, 0.4f);
        }
    }

    // Shield timer
    if (hasShield && shieldFramesLeft > 0) {
        shieldFramesLeft--;
        if (shieldFramesLeft <= 0) hasShield = false;
    }

    // Collision detection
    for (auto& obs : obstacles) {
        if (rectCollide(PLAYER_X, playerY, PLAYER_WIDTH, PLAYER_HEIGHT, obs.x, obs.y, obs.w, obs.h)) {
            if (hasShield && shieldFramesLeft > 0) {
                hasShield = false;
                shieldFramesLeft = 0;
                screenShake = 8;
                invincibilityFlash = 12;
                addParticles(PLAYER_X + 16, playerY + 20, 0.85f, 0.7f, 0.3f);
                obs.counted = true;
            } else {
                gameRunning = false;
                screenShake = 12;
                addParticles(PLAYER_X + 16, playerY + 20, 0.9f, 0.3f, 0.2f);
                break;
            }
        }
    }

    // Update particles
    for (auto& p : particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.life--;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.life <= 0 || p.y > WINDOW_HEIGHT + 50; }), particles.end());

    if (screenShake > 0) screenShake -= 0.5f;
    if (invincibilityFlash > 0) invincibilityFlash--;
    frame++;
}

// ==================== RENDERING ====================
void renderSky() {
    // Gradient sky
    for (int i = 0; i <= WINDOW_HEIGHT; i++) {
        float t = (float)i / WINDOW_HEIGHT;
        float r = 0.06f + t * 0.1f;
        float g = 0.11f + t * 0.15f;
        float b = 0.17f + t * 0.12f;
        glColor3f(r, g, b);
        glBegin(GL_QUADS);
        glVertex2f(0, i);
        glVertex2f(WINDOW_WIDTH, i);
        glVertex2f(WINDOW_WIDTH, i + 1);
        glVertex2f(0, i + 1);
        glEnd();
    }

    // Stars
    for (int i = 0; i < 80; i++) {
        float brightness = 0.3f + sin(frame * 0.02f + i) * 0.1f;
        drawCircle((i * 131) % WINDOW_WIDTH, (i * 57) % (WINDOW_HEIGHT / 2), 1.2f, brightness, brightness * 0.9f, brightness * 0.7f);
    }

    // Moon
    drawCircle(WINDOW_WIDTH - 80, 70, 35, 0.98f, 0.88f, 0.63f);
    drawCircle(WINDOW_WIDTH - 70, 60, 6, 0.96f, 0.8f, 0.5f);
}

void renderGround() {
    // Ground layers
    drawRect(0, GROUND_Y - 5, WINDOW_WIDTH, 8, 0.31f, 0.22f, 0.16f);
    drawRect(0, GROUND_Y - 2, WINDOW_WIDTH, 5, 0.51f, 0.43f, 0.29f);
    drawRect(0, GROUND_Y, WINDOW_WIDTH, WINDOW_HEIGHT - GROUND_Y + 5, 0.17f, 0.14f, 0.09f);

    // Ground lines
    for (int i = 0; i < 20; i++) {
        float lineX = fmod(frame * 2.8f + i * 62, WINDOW_WIDTH + 80) - 40;
        drawRect(lineX, GROUND_Y - 4, 22, 4, 0.9f, 0.74f, 0.49f);
    }

    for (int i = 0; i < 12; i++) {
        float markX = fmod(frame * 1.2f + i * 103, WINDOW_WIDTH + 150) - 50;
        drawRect(markX, GROUND_Y + 3, 12, 4, 0.67f, 0.54f, 0.33f);
    }
}

void renderPowerups() {
    for (const auto& p : powerups) {
        drawCircle(p.x + 11, p.y + 11, 12, 1.0f, 0.85f, 0.4f);
        drawCircle(p.x + 11, p.y + 11, 8, 0.96f, 0.64f, 0.38f);
        drawCircle(p.x + 11, p.y + 11, 4, 1.0f, 0.89f, 0.52f);

        glColor3f(1.0f, 0.7f, 0.28f);
        glRasterPos2f(p.x + 4, p.y + 18);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '*');
    }
}

void renderObstacles() {
    for (const auto& obs : obstacles) {
        drawRect(obs.x, obs.y, obs.w, obs.h, 0.55f, 0.24f, 0.17f);
        drawRect(obs.x + 4, obs.y + obs.h - 12, obs.w - 8, 6, 0.17f, 0.09f, 0.06f);

        if (obs.variant == "spike") {
            glColor3f(0.79f, 0.47f, 0.29f);
            for (int s = 0; s < 3; s++) {
                glBegin(GL_TRIANGLES);
                glVertex2f(obs.x + 5 + s * 8, obs.y - 6);
                glVertex2f(obs.x + 2 + s * 8, obs.y + 2);
                glVertex2f(obs.x + 8 + s * 8, obs.y + 2);
                glEnd();
            }
        } else {
            drawRect(obs.x + 6, obs.y + 8, 6, 8, 0.94f, 0.74f, 0.48f);
            drawRect(obs.x + 16, obs.y + 8, 6, 8, 0.94f, 0.74f, 0.48f);
        }
    }
}

void renderPlayer() {
    bool draw = true;
    if (invincibilityFlash > 0 && (frame % 6 < 3)) draw = false;

    if (draw) {
        // Body
        drawRoundedRect(PLAYER_X + 4, playerY + 10, PLAYER_WIDTH - 8, PLAYER_HEIGHT - 12, 8, 0.25f, 0.64f, 0.63f);
        drawRect(PLAYER_X + 8, playerY + 22, 6, 12, 0.15f, 0.42f, 0.41f);
        drawRect(PLAYER_X + 20, playerY + 22, 6, 12, 0.15f, 0.42f, 0.41f);

        // Head
        drawEllipse(PLAYER_X + 17, playerY + 12, 14, 16, 0.98f, 0.84f, 0.64f);

        // Eyes
        drawCircle(PLAYER_X + 11, playerY + 10, 2.8f, 0.09f, 0.11f, 0.12f);
        drawCircle(PLAYER_X + 23, playerY + 10, 2.8f, 0.09f, 0.11f, 0.12f);
        drawCircle(PLAYER_X + 10, playerY + 9, 1.2f, 0.96f, 0.94f, 0.87f);
        drawCircle(PLAYER_X + 22, playerY + 9, 1.2f, 0.96f, 0.94f, 0.87f);

        // Scarf
        drawRect(PLAYER_X - 8, playerY + 14, 9, 14, 0.92f, 0.42f, 0.28f);

        // Shield aura
        if (hasShield && shieldFramesLeft > 0) {
            float alpha = 0.6f + sin(frame * 0.094f) * 0.2f;
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            drawCircleAlpha(PLAYER_X + 17, playerY + 22, 24, 1.0f, 0.78f, 0.34f, alpha);
            glDisable(GL_BLEND);
        }
    }
}

void renderParticles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (const auto& p : particles) {
        float alpha = p.life / 15.0f;
        glColor4f(p.r, p.g, p.b, alpha);
        glBegin(GL_QUADS);
        glVertex2f(p.x, p.y);
        glVertex2f(p.x + p.size, p.y);
        glVertex2f(p.x + p.size, p.y + p.size);
        glVertex2f(p.x, p.y + p.size);
        glEnd();
    }
    glDisable(GL_BLEND);
}

void renderUI() {
    std::stringstream ss;
    ss << (int)score;
    glColor3f(1.0f, 0.9f, 0.7f);
    drawText(20, 45, "SCORE: " + ss.str());

    ss.str("");
    ss << bestScore;
    drawText(20, 75, "BEST: " + ss.str());

    ss.str("");
    ss << (int)distance;
    drawText(20, 105, "DIST: " + ss.str());

    // Speed indicator
    ss.str("");
    ss.precision(1);
    ss << std::fixed << currentSpeed;
    drawText(WINDOW_WIDTH - 100, 45, "SPEED: " + ss.str());

    // Shield status
    if (hasShield && shieldFramesLeft > 0) {
        drawText(WINDOW_WIDTH - 150, 75, "SHIELD ACTIVE");
    } else {
        drawText(WINDOW_WIDTH - 150, 75, "NO SHIELD");
    }

    // Game over overlay
    if (!gameRunning) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawRectAlpha(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f);
        glDisable(GL_BLEND);

        glColor3f(1.0f, 0.81f, 0.49f);
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 30, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
        drawText(WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT / 2 + 20, "Press R to Restart", GLUT_BITMAP_HELVETICA_18);
    }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Screen shake effect
    if (screenShake > 0) {
        float shakeX = ((rand() % 10) - 5) * (screenShake / 10.0f);
        float shakeY = ((rand() % 10) - 5) * (screenShake / 10.0f);
        glTranslatef(shakeX, shakeY, 0);
    }

    renderSky();
    renderGround();
    renderPowerups();
    renderObstacles();
    renderPlayer();
    renderParticles();
    renderUI();

    glutSwapBuffers();
}

// ==================== INPUT HANDLING ====================
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':
        case 'w':
        case 'W':
            if (gameRunning) {
                jumpBufferTimer = JUMP_BUFFER;
                if (isOnGround || coyoteTimer > 0) {
                    yVelocity = JUMP_POWER;
                    isOnGround = false;
                    coyoteTimer = 0;
                    jumpBufferTimer = 0;
                    addParticles(PLAYER_X + 16, GROUND_Y - 6, 1.0f, 0.7f, 0.3f);
                }
            }
            break;
        case 'r':
        case 'R':
            restartGame();
            break;
        case 27: // ESC
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        if (gameRunning) {
            jumpBufferTimer = JUMP_BUFFER;
            if (isOnGround || coyoteTimer > 0) {
                yVelocity = JUMP_POWER;
                isOnGround = false;
                coyoteTimer = 0;
                jumpBufferTimer = 0;
                addParticles(PLAYER_X + 16, GROUND_Y - 6, 1.0f, 0.7f, 0.3f);
            }
        }
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (gameRunning) {
            jumpBufferTimer = JUMP_BUFFER;
            if (isOnGround || coyoteTimer > 0) {
                yVelocity = JUMP_POWER;
                isOnGround = false;
                coyoteTimer = 0;
                jumpBufferTimer = 0;
                addParticles(PLAYER_X + 16, GROUND_Y - 6, 1.0f, 0.7f, 0.3f);
            }
        }
    }
    glutPostRedisplay();
}

void update(int value) {
    updateGame();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

void initGL() {
    glClearColor(0.05f, 0.08f, 0.12f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ==================== MAIN ====================
int main(int argc, char** argv) {
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("TEMPLATE RUN - OpenGL Edition");

    initGL();
    restartGame();

    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
