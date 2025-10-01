#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>
#include <vector>

#define M_PI 3.1415926 

// ��ת�Ƕ�
float outerFrameAngle = 0.0f;    // �����X����ת
float innerFrameAngle = 0.0f;    // �ڿ���Y����ת

// ��ת�ٶ�
float outerSpeed = 0.5f;         // �����ת�ٶ�
float innerSpeed = 0.8f;         // �ڿ���ת�ٶ�

// ���ڳߴ�
int windowWidth = 1200;
int windowHeight = 800;

// �������
float cameraDistance = 6.0f;
float cameraAngleX = 25.0f;
float cameraAngleY = -35.0f;

// �洢�����켣��
std::vector<float> gravityTrajectory;

void drawAxis() {
    // ���������� - ��ϸ����
    glLineWidth(2.0f);
    
    // X�� - ��ɫ (�����ת��)
    glColor3f(1.0f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex3f(-1.8f, 0.0f, 0.0f);
    glVertex3f(1.8f, 0.0f, 0.0f);
    glEnd();
    
    // Y�� - ��ɫ (�ڿ���ת��)
    glColor3f(0.3f, 1.0f, 0.3f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, -1.8f, 0.0f);
    glVertex3f(0.0f, 1.8f, 0.0f);
    glEnd();
    
    // Z�� - ��ɫ
    glColor3f(0.3f, 0.3f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, -1.8f);
    glVertex3f(0.0f, 0.0f, 1.8f);
    glEnd();
}

void drawOuterFrame() {
    // ��� - ��X����ת�Ļ�ɫϸ�߿��
    glPushMatrix();
    glRotatef(outerFrameAngle, 1.0f, 0.0f, 0.0f); // ��X����ת
    
    // �������Ϊ��ɫϸ�ߣ��ر���˸Ч����
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    
    // ��������Բ�α߽磨��YZƽ�棩
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * M_PI * i / 32;
        glVertex3f(0.0f, cos(angle) * 1.2f, sin(angle) * 1.2f);
    }
    glEnd();
    
    // ��������ʮ��֧����
    glBegin(GL_LINES);
    // ��ֱ��
    glVertex3f(0.0f, -1.2f, 0.0f);
    glVertex3f(0.0f, 1.2f, 0.0f);
    // ˮƽ��
    glVertex3f(0.0f, 0.0f, -1.2f);
    glVertex3f(0.0f, 0.0f, 1.2f);
    glEnd();
    
    // ���������ָʾ������X���ϣ�
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.6f, 0.0f);
    glutSolidSphere(0.08f, 12, 12);
    
    // �����תָʾ�ߣ��ر���˸Ч����
    glColor3f(1.0f, 0.8f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    float indicatorLength = 0.3f;
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(indicatorLength * cos(outerFrameAngle * M_PI / 180.0f), 
               0.0f, 
               indicatorLength * sin(outerFrameAngle * M_PI / 180.0f));
    glEnd();
    glPopMatrix();
    
    glPopMatrix();
}

void drawInnerFrame() {
    // �ڿ� - ��Y����ת�Ļ�ɫϸ�߿��
    glPushMatrix();
    glRotatef(outerFrameAngle, 1.0f, 0.0f, 0.0f); // ��Ӧ�������ת
    glRotatef(innerFrameAngle, 0.0f, 1.0f, 0.0f); // ����Y����ת
    
    // �����ڿ�Ϊ��ɫϸ�ߣ��ر���˸Ч����
    glColor3f(1.0f, 1.0f, 0.4f);
    glLineWidth(2.0f);
    
    // �����ڿ��Բ�α߽磨��XZƽ�棩
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * M_PI * i / 32;
        glVertex3f(cos(angle) * 0.9f, 0.0f, sin(angle) * 0.9f);
    }
    glEnd();
    
    // �����ڿ��ʮ��֧����
    glBegin(GL_LINES);
    // ˮƽ��
    glVertex3f(-0.9f, 0.0f, 0.0f);
    glVertex3f(0.9f, 0.0f, 0.0f);
    // ��ֱ��
    glVertex3f(0.0f, 0.0f, -0.9f);
    glVertex3f(0.0f, 0.0f, 0.9f);
    glEnd();
    
    // �����ڿ���ָʾ������Y���ϣ�
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(0.6f, 1.0f, 0.0f);
    glutSolidSphere(0.06f, 12, 12);
    
    // �����תָʾ�ߣ��ر���˸Ч����
    glColor3f(0.8f, 1.0f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    float indicatorLength = 0.25f;
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 
               indicatorLength * cos(innerFrameAngle * M_PI / 180.0f),
               indicatorLength * sin(innerFrameAngle * M_PI / 180.0f));
    glEnd();
    glPopMatrix();
    
    glPopMatrix();
}

void drawCenterCube() {
    // ���������� - �����������Ŵ�0.4f����͸����ɫ��
    glPushMatrix();
    glRotatef(outerFrameAngle, 1.0f, 0.0f, 0.0f); // Ӧ�������ת
    glRotatef(innerFrameAngle, 0.0f, 1.0f, 0.0f); // Ӧ���ڿ���ת
    
    // ������������ʣ���͸����ɫ��
    glColor4f(0.9f, 0.9f, 0.9f, 0.7f);
    
    // ���������壨��ͬ�治ͬ��ɫ��
    float size = 0.4f; // �Ŵ�ߴ�
    
    glBegin(GL_QUADS);
    
    // ǰ�� - ��ɫ
    glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
    glVertex3f(-size, -size, size);
    glVertex3f(size, -size, size);
    glVertex3f(size, size, size);
    glVertex3f(-size, size, size);
    
    // ���� - ��ɫ
    glColor4f(0.0f, 1.0f, 0.0f, 0.7f);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, size, -size);
    glVertex3f(size, size, -size);
    glVertex3f(size, -size, -size);
    
    // ���� - ��ɫ
    glColor4f(0.0f, 0.0f, 1.0f, 0.7f);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, -size, size);
    glVertex3f(-size, size, size);
    glVertex3f(-size, size, -size);
    
    // ���� - ��ɫ
    glColor4f(1.0f, 1.0f, 0.0f, 0.7f);
    glVertex3f(size, -size, -size);
    glVertex3f(size, size, -size);
    glVertex3f(size, size, size);
    glVertex3f(size, -size, size);
    
    // ���� - ��ɫ
    glColor4f(0.0f, 1.0f, 1.0f, 0.7f);
    glVertex3f(-size, size, -size);
    glVertex3f(-size, size, size);
    glVertex3f(size, size, size);
    glVertex3f(size, size, -size);
    
    // ���� - ��ɫ
    glColor4f(1.0f, 0.0f, 1.0f, 0.7f);
    glVertex3f(-size, -size, -size);
    glVertex3f(size, -size, -size);
    glVertex3f(size, -size, size);
    glVertex3f(-size, -size, size);
    
    glEnd();
    
    glPopMatrix();
}

void drawGravityVector() {
    // ������������ָʾ�� - ��Ϊ��ɫ���µ���
    glPushMatrix();
    
    // ���������ͷ - ��ɫ
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(3.0f); // �Ӵ��Ա���ù۲�
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, -1.5f, 0.0f); // ��Ϊ���£�Y�Ḻ����
    glEnd();
    
    // ��ͷͷ��
    glPushMatrix();
    glTranslatef(0.0f, -1.5f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // ��ת��ͷ����
    glColor3f(0.0f, 0.8f, 0.0f);
    glutSolidCone(0.08f, 0.2f, 6, 6);
    glPopMatrix();
    
    glPopMatrix();
}

void drawGravityTrajectory() {
    // ���������켣
    if (gravityTrajectory.size() >= 3) {
        glPushMatrix();
        glColor3f(1.0f, 0.5f, 0.0f); // ��ɫ�켣��
        glLineWidth(2.0f);
        
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < gravityTrajectory.size(); i += 3) {
            glVertex3f(gravityTrajectory[i], gravityTrajectory[i+1], gravityTrajectory[i+2]);
        }
        glEnd();
        
        glPopMatrix();
    }
}

void updateGravityTrajectory() {
    // �����������ڵ�ǰ��ת�µ�λ��
    glPushMatrix();
    glRotatef(outerFrameAngle, 1.0f, 0.0f, 0.0f);
    glRotatef(innerFrameAngle, 0.0f, 1.0f, 0.0f);
    
    // ���������ĸ������㣬���Ƿ����������������
    // �򵥷�������¼���������ĵ�λ�ñ仯
    static int frameCount = 0;
    frameCount++;
    
    // ÿ10֡��¼һ���㣬����켣�����ܼ�
    if (frameCount % 10 == 0) {
        // ���㵱ǰ�����������������ϵ�ͶӰ
        float gravityDir[3] = {0.0f, -1.0f, 0.0f}; // ������������
        
        // ��¼��ǰ���������ĵ�λ�ã�������ת���λ�ã�
        gravityTrajectory.push_back(0.0f); // x
        gravityTrajectory.push_back(0.0f); // y  
        gravityTrajectory.push_back(0.0f); // z
        
        // ���ƹ켣��������������ڴ���������
        if (gravityTrajectory.size() > 300) {
            gravityTrajectory.erase(gravityTrajectory.begin(), gravityTrajectory.begin() + 3);
        }
    }
    
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // �������λ��
    float camX = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float camY = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float camZ = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    
    gluLookAt(camX, camY, camZ,    // ���λ��
              0.0f, 0.0f, 0.0f,    // �۲��
              0.0f, 1.0f, 0.0f);   // �Ϸ���
    
    // ���û����ʵ�ְ�͸��Ч��
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);
    
    // ����������
    drawAxis();
    
    // ������������
    drawGravityVector();
    
    // ���������켣
    updateGravityTrajectory();
    
    // ���������켣
    drawGravityTrajectory();
    
    // ����RPM���
    drawOuterFrame();
    drawInnerFrame();
    drawCenterCube(); // ��Ϊ����������
    
    // ��ʾ��ת��Ϣ
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor3f(0.9f, 0.9f, 0.9f);
    glRasterPos2f(10, windowHeight - 30);
    std::string info = "RPM Microgravity Simulator - Outer(X): " + std::to_string((int)outerFrameAngle) + 
                       "��  Inner(Y): " + std::to_string((int)innerFrameAngle) + "��";
    for (char c : info) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    
    glRasterPos2f(10, windowHeight - 60);
    std::string info2 = "Speeds - Outer: " + std::to_string(outerSpeed).substr(0,4) + 
                        "  Inner: " + std::to_string(innerSpeed).substr(0,4);
    for (char c : info2) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    
    glRasterPos2f(10, windowHeight - 90);
    std::string info3 = "Gravity Direction: -Y (Green arrow)";
    for (char c : info3) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glRasterPos2f(10, windowHeight - 110);
    std::string info4 = "Controls: Mouse drag - Rotate camera  +/- - Change speed  R - Reset";
    for (char c : info4) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glRasterPos2f(10, windowHeight - 130);
    std::string info5 = "Gravity Trajectory: Orange line shows gravity exposure history";
    for (char c : info5) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    
    glutSwapBuffers();
}

void update(int value) {
    // ������ת�Ƕ�
    outerFrameAngle += outerSpeed;
    innerFrameAngle += innerSpeed;
    
    // ���ֽǶ���0-360�ȷ�Χ��
    if (outerFrameAngle > 360.0f) outerFrameAngle -= 360.0f;
    if (innerFrameAngle > 360.0f) innerFrameAngle -= 360.0f;
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // Լ60FPS
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// ����϶����������X�ᷴת��
int mouseX = 0, mouseY = 0;
bool mouseLeftDown = false;

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseLeftDown = true;
            mouseX = x;
            mouseY = y;
        } else {
            mouseLeftDown = false;
        }
    }
}

void motion(int x, int y) {
    if (mouseLeftDown) {
        // X�ᷴת�������Ÿ�Ϊ�Ӻ�
        cameraAngleY -= (x - mouseX) * 0.5f; // X�ᷴת
        cameraAngleX += (y - mouseY) * 0.5f;
        
        // ���ƴ�ֱ�ӽ�
        if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
        if (cameraAngleX < -89.0f) cameraAngleX = -89.0f;
        
        mouseX = x;
        mouseY = y;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+': // ����
            outerSpeed *= 1.2f;
            innerSpeed *= 1.2f;
            break;
        case '-': // ����
            outerSpeed *= 0.8f;
            innerSpeed *= 0.8f;
            break;
        case 'r': // ����
        case 'R':
            outerFrameAngle = 0.0f;
            innerFrameAngle = 0.0f;
            outerSpeed = 0.5f;
            innerSpeed = 0.8f;
            cameraAngleX = 25.0f;
            cameraAngleY = -35.0f;
            gravityTrajectory.clear(); // ��������켣
            break;
        case 27: // ESC�˳�
            exit(0);
            break;
    }
}

void init() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // ����ɫ����
    
    // ���ù���
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat lightPos[] = {3.0f, 3.0f, 3.0f, 1.0f};
    GLfloat lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat lightDiffuse[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
    // ������ɫ����
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    // ƽ����ɫ
    glShadeModel(GL_SMOOTH);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("RPM Microgravity Simulator - Enhanced Visualization");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);
    
    std::cout << "RPM Microgravity Simulator Started" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Enhanced Features:" << std::endl;
    std::cout << "- Gravity direction: Green downward arrow (-Y axis)" << std::endl;
    std::cout << "- Center sample: Multi-colored semi-transparent cube" << std::endl;
    std::cout << "- Gravity trajectory: Orange line showing exposure history" << std::endl;
    std::cout << "- Mouse control: X-axis inverted for more intuitive camera control" << std::endl;
    std::cout << "- No flashing effects: Stable yellow frame display" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- Mouse drag: Rotate camera (X-axis inverted)" << std::endl;
    std::cout << "- +/-: Change rotation speed" << std::endl;
    std::cout << "- R: Reset simulation and clear trajectory" << std::endl;
    std::cout << "- ESC: Exit" << std::endl;
    
    glutMainLoop();
    
    return 0;
}
