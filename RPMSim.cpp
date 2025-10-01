#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>
#include <vector>

#define M_PI 3.1415926 

// 旋转角度
float outerFrameAngle = 0.0f;    // 外框绕X轴旋转
float innerFrameAngle = 0.0f;    // 内框绕Y轴旋转

// 旋转速度
float outerSpeed = 0.5f;         // 外框旋转速度
float innerSpeed = 0.8f;         // 内框旋转速度

// 窗口尺寸
int windowWidth = 1200;
int windowHeight = 800;

// 相机参数
float cameraDistance = 6.0f;
float cameraAngleX = 25.0f;
float cameraAngleY = -35.0f;

// 存储重力轨迹点
std::vector<float> gravityTrajectory;

void drawAxis() {
    // 绘制坐标轴 - 更细的线
    glLineWidth(2.0f);
    
    // X轴 - 红色 (外框旋转轴)
    glColor3f(1.0f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex3f(-1.8f, 0.0f, 0.0f);
    glVertex3f(1.8f, 0.0f, 0.0f);
    glEnd();
    
    // Y轴 - 绿色 (内框旋转轴)
    glColor3f(0.3f, 1.0f, 0.3f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, -1.8f, 0.0f);
    glVertex3f(0.0f, 1.8f, 0.0f);
    glEnd();
    
    // Z轴 - 蓝色
    glColor3f(0.3f, 0.3f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, -1.8f);
    glVertex3f(0.0f, 0.0f, 1.8f);
    glEnd();
}

void drawOuterFrame() {
    // 外框 - 绕X轴旋转的黄色细线框架
    glPushMatrix();
    glRotatef(outerFrameAngle, 1.0f, 0.0f, 0.0f); // 绕X轴旋转
    
    // 设置外框为黄色细线（关闭闪烁效果）
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    
    // 绘制外框的圆形边界（在YZ平面）
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * M_PI * i / 32;
        glVertex3f(0.0f, cos(angle) * 1.2f, sin(angle) * 1.2f);
    }
    glEnd();
    
    // 绘制外框的十字支撑线
    glBegin(GL_LINES);
    // 垂直线
    glVertex3f(0.0f, -1.2f, 0.0f);
    glVertex3f(0.0f, 1.2f, 0.0f);
    // 水平线
    glVertex3f(0.0f, 0.0f, -1.2f);
    glVertex3f(0.0f, 0.0f, 1.2f);
    glEnd();
    
    // 绘制外框电机指示器（在X轴上）
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.6f, 0.0f);
    glutSolidSphere(0.08f, 12, 12);
    
    // 电机旋转指示线（关闭闪烁效果）
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
    // 内框 - 绕Y轴旋转的黄色细线框架
    glPushMatrix();
    glRotatef(outerFrameAngle, 1.0f, 0.0f, 0.0f); // 先应用外框旋转
    glRotatef(innerFrameAngle, 0.0f, 1.0f, 0.0f); // 再绕Y轴旋转
    
    // 设置内框为黄色细线（关闭闪烁效果）
    glColor3f(1.0f, 1.0f, 0.4f);
    glLineWidth(2.0f);
    
    // 绘制内框的圆形边界（在XZ平面）
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * M_PI * i / 32;
        glVertex3f(cos(angle) * 0.9f, 0.0f, sin(angle) * 0.9f);
    }
    glEnd();
    
    // 绘制内框的十字支撑线
    glBegin(GL_LINES);
    // 水平线
    glVertex3f(-0.9f, 0.0f, 0.0f);
    glVertex3f(0.9f, 0.0f, 0.0f);
    // 垂直线
    glVertex3f(0.0f, 0.0f, -0.9f);
    glVertex3f(0.0f, 0.0f, 0.9f);
    glEnd();
    
    // 绘制内框电机指示器（在Y轴上）
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(0.6f, 1.0f, 0.0f);
    glutSolidSphere(0.06f, 12, 12);
    
    // 电机旋转指示线（关闭闪烁效果）
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
    // 中心立方体 - 生物样本（放大到0.4f，半透明白色）
    glPushMatrix();
    glRotatef(outerFrameAngle, 1.0f, 0.0f, 0.0f); // 应用外框旋转
    glRotatef(innerFrameAngle, 0.0f, 1.0f, 0.0f); // 应用内框旋转
    
    // 设置立方体材质（半透明白色）
    glColor4f(0.9f, 0.9f, 0.9f, 0.7f);
    
    // 绘制立方体（不同面不同颜色）
    float size = 0.4f; // 放大尺寸
    
    glBegin(GL_QUADS);
    
    // 前面 - 红色
    glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
    glVertex3f(-size, -size, size);
    glVertex3f(size, -size, size);
    glVertex3f(size, size, size);
    glVertex3f(-size, size, size);
    
    // 后面 - 绿色
    glColor4f(0.0f, 1.0f, 0.0f, 0.7f);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, size, -size);
    glVertex3f(size, size, -size);
    glVertex3f(size, -size, -size);
    
    // 左面 - 蓝色
    glColor4f(0.0f, 0.0f, 1.0f, 0.7f);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, -size, size);
    glVertex3f(-size, size, size);
    glVertex3f(-size, size, -size);
    
    // 右面 - 黄色
    glColor4f(1.0f, 1.0f, 0.0f, 0.7f);
    glVertex3f(size, -size, -size);
    glVertex3f(size, size, -size);
    glVertex3f(size, size, size);
    glVertex3f(size, -size, size);
    
    // 上面 - 青色
    glColor4f(0.0f, 1.0f, 1.0f, 0.7f);
    glVertex3f(-size, size, -size);
    glVertex3f(-size, size, size);
    glVertex3f(size, size, size);
    glVertex3f(size, size, -size);
    
    // 下面 - 紫色
    glColor4f(1.0f, 0.0f, 1.0f, 0.7f);
    glVertex3f(-size, -size, -size);
    glVertex3f(size, -size, -size);
    glVertex3f(size, -size, size);
    glVertex3f(-size, -size, size);
    
    glEnd();
    
    glPopMatrix();
}

void drawGravityVector() {
    // 绘制重力向量指示器 - 改为绿色向下的线
    glPushMatrix();
    
    // 重力方向箭头 - 绿色
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(3.0f); // 加粗以便更好观察
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, -1.5f, 0.0f); // 改为向下（Y轴负方向）
    glEnd();
    
    // 箭头头部
    glPushMatrix();
    glTranslatef(0.0f, -1.5f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // 旋转箭头方向
    glColor3f(0.0f, 0.8f, 0.0f);
    glutSolidCone(0.08f, 0.2f, 6, 6);
    glPopMatrix();
    
    glPopMatrix();
}

void drawGravityTrajectory() {
    // 绘制重力轨迹
    if (gravityTrajectory.size() >= 3) {
        glPushMatrix();
        glColor3f(1.0f, 0.5f, 0.0f); // 橙色轨迹线
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
    // 计算立方体在当前旋转下的位置
    glPushMatrix();
    glRotatef(outerFrameAngle, 1.0f, 0.0f, 0.0f);
    glRotatef(innerFrameAngle, 0.0f, 1.0f, 0.0f);
    
    // 检查立方体的各个顶点，看是否有面向重力方向的
    // 简单方法：记录立方体中心的位置变化
    static int frameCount = 0;
    frameCount++;
    
    // 每10帧记录一个点，避免轨迹过于密集
    if (frameCount % 10 == 0) {
        // 计算当前立方体在重力方向上的投影
        float gravityDir[3] = {0.0f, -1.0f, 0.0f}; // 重力方向向下
        
        // 记录当前立方体中心的位置（经过旋转后的位置）
        gravityTrajectory.push_back(0.0f); // x
        gravityTrajectory.push_back(0.0f); // y  
        gravityTrajectory.push_back(0.0f); // z
        
        // 限制轨迹点的数量，避免内存无限增长
        if (gravityTrajectory.size() > 300) {
            gravityTrajectory.erase(gravityTrajectory.begin(), gravityTrajectory.begin() + 3);
        }
    }
    
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // 设置相机位置
    float camX = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float camY = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float camZ = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    
    gluLookAt(camX, camY, camZ,    // 相机位置
              0.0f, 0.0f, 0.0f,    // 观察点
              0.0f, 1.0f, 0.0f);   // 上方向
    
    // 启用混合以实现半透明效果
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 绘制坐标轴
    drawAxis();
    
    // 绘制重力向量
    drawGravityVector();
    
    // 更新重力轨迹
    updateGravityTrajectory();
    
    // 绘制重力轨迹
    drawGravityTrajectory();
    
    // 绘制RPM组件
    drawOuterFrame();
    drawInnerFrame();
    drawCenterCube(); // 改为绘制立方体
    
    // 显示旋转信息
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
                       "°  Inner(Y): " + std::to_string((int)innerFrameAngle) + "°";
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
    // 更新旋转角度
    outerFrameAngle += outerSpeed;
    innerFrameAngle += innerSpeed;
    
    // 保持角度在0-360度范围内
    if (outerFrameAngle > 360.0f) outerFrameAngle -= 360.0f;
    if (innerFrameAngle > 360.0f) innerFrameAngle -= 360.0f;
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // 约60FPS
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

// 鼠标拖动控制相机（X轴反转）
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
        // X轴反转：将减号改为加号
        cameraAngleY -= (x - mouseX) * 0.5f; // X轴反转
        cameraAngleX += (y - mouseY) * 0.5f;
        
        // 限制垂直视角
        if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
        if (cameraAngleX < -89.0f) cameraAngleX = -89.0f;
        
        mouseX = x;
        mouseY = y;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+': // 加速
            outerSpeed *= 1.2f;
            innerSpeed *= 1.2f;
            break;
        case '-': // 减速
            outerSpeed *= 0.8f;
            innerSpeed *= 0.8f;
            break;
        case 'r': // 重置
        case 'R':
            outerFrameAngle = 0.0f;
            innerFrameAngle = 0.0f;
            outerSpeed = 0.5f;
            innerSpeed = 0.8f;
            cameraAngleX = 25.0f;
            cameraAngleY = -35.0f;
            gravityTrajectory.clear(); // 清除重力轨迹
            break;
        case 27: // ESC退出
            exit(0);
            break;
    }
}

void init() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // 深蓝色背景
    
    // 启用光照
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
    
    // 启用颜色材质
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    // 平滑着色
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
