#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#define M_PI 3.1415926 

// ===== 可配置的随机化参数 =====
const float GOLDEN_RATIO = 1.61803398875f;      // 黄金比例 φ
const float SQRT_2 = 1.41421356237f;            // 2的平方根
const float MIN_RATIO = 1.4f;                   // 最小速度比例 (inner/outer)
const float MAX_RATIO = 1.6f;                   // 最大速度比例 (inner/outer)
const float MIN_SPEED = 0.3f;                   // 最小角速度 (rad/s)
const float MAX_SPEED = 0.7f;                   // 最大角速度 (rad/s)
const float MAX_ACCELERATION = 0.01f;           // 最大角加速度 (rad/s2)
const float TARGET_CHANGE_INTERVAL = 3.0f;      // 目标速度变化间隔 (秒)
const bool USE_RANDOM_RATIO = true;             // 是否在比例范围内随机取值
const bool USE_FIXED_RATIO = false;             // 是否使用固定比例
const float FIXED_RATIO = GOLDEN_RATIO;               // 固定比例值 (当USE_FIXED_RATIO=true时使用)
// =============================

// 旋转角度
float outerFrameAngle = 0.0f;    // 外框绕X轴旋转
float innerFrameAngle = 0.0f;    // 内框绕Y轴旋转

// 当前速度和目标速度
float outerSpeed = 0.5f;         // 外框当前速度
float innerSpeed = 0.8f;         // 内框当前速度
float outerTargetSpeed = 0.5f;   // 外框目标速度
float innerTargetSpeed = 0.8f;   // 内框目标速度
float currentRatio = 1.618f;     // 当前速度比例

// 窗口尺寸
int windowWidth = 1200;
int windowHeight = 800;

// 相机参数
float cameraDistance = 6.0f;
float cameraAngleX = 25.0f;
float cameraAngleY = -35.0f;

// 存储重力轨迹点
std::vector<float> gravityTrajectory;

// 随机数生成器
std::mt19937 rng;
std::uniform_real_distribution<float> speedDist;
std::uniform_real_distribution<float> ratioDist;
std::uniform_real_distribution<float> timeDist;

// 计时和状态变量
float timeSinceLastChange = 0.0f;
float currentTime = 0.0f;
bool isTransitioning = false;
float transitionStartTime = 0.0f;

void initRandom() {
    // 初始化随机数生成器
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(seed);
    speedDist = std::uniform_real_distribution<float>(MIN_SPEED, MAX_SPEED);
    ratioDist = std::uniform_real_distribution<float>(MIN_RATIO, MAX_RATIO);
    timeDist = std::uniform_real_distribution<float>(TARGET_CHANGE_INTERVAL * 0.8f, TARGET_CHANGE_INTERVAL * 1.2f);
    
    // 设置初始目标速度
    if (USE_FIXED_RATIO) {
        // 使用固定比例
        currentRatio = FIXED_RATIO;
        outerTargetSpeed = speedDist(rng);
        innerTargetSpeed = outerTargetSpeed * currentRatio;
    } else if (USE_RANDOM_RATIO) {
        // 在比例范围内随机选择
        currentRatio = ratioDist(rng);
        outerTargetSpeed = speedDist(rng);
        innerTargetSpeed = outerTargetSpeed * currentRatio;
    } else {
        // 交替使用几个关键的无理数
        static int ratioIndex = 0;
        float ratios[] = {SQRT_2, GOLDEN_RATIO, 1.5f, 1.732f}; // √2, φ, 3/2, √3
        currentRatio = ratios[ratioIndex];
        ratioIndex = (ratioIndex + 1) % 4;
        
        outerTargetSpeed = speedDist(rng);
        innerTargetSpeed = outerTargetSpeed * currentRatio;
    }
    
    // 确保速度在合理范围内
    while (innerTargetSpeed > MAX_SPEED || innerTargetSpeed < MIN_SPEED) {
        if (innerTargetSpeed > MAX_SPEED) {
            outerTargetSpeed *= 0.8f;
            innerTargetSpeed = outerTargetSpeed * currentRatio;
        } else {
            outerTargetSpeed *= 1.2f;
            innerTargetSpeed = outerTargetSpeed * currentRatio;
        }
    }
    
    outerSpeed = outerTargetSpeed;
    innerSpeed = innerTargetSpeed;
    isTransitioning = false;
    timeSinceLastChange = 0.0f;
}

void updateSpeeds(float deltaTime) {
    currentTime += deltaTime;
    
    if (!isTransitioning) {
        // 不在过渡期，正常计时
        timeSinceLastChange += deltaTime;
        
        // 检查是否需要设置新的目标速度
        if (timeSinceLastChange >= TARGET_CHANGE_INTERVAL) {
            // 开始新的速度过渡
            isTransitioning = true;
            transitionStartTime = currentTime;
            timeSinceLastChange = 0.0f;
            
            // 保存当前目标速度为起始速度
            float startOuterSpeed = outerTargetSpeed;
            float startInnerSpeed = innerTargetSpeed;
            
            // 设置新的目标速度
            if (USE_FIXED_RATIO) {
                // 使用固定比例
                currentRatio = FIXED_RATIO;
                outerTargetSpeed = speedDist(rng);
                innerTargetSpeed = outerTargetSpeed * currentRatio;
            } else if (USE_RANDOM_RATIO) {
                // 在比例范围内随机选择
                currentRatio = ratioDist(rng);
                outerTargetSpeed = speedDist(rng);
                innerTargetSpeed = outerTargetSpeed * currentRatio;
            } else {
                // 交替使用几个关键的无理数
                static int ratioIndex = 0;
                float ratios[] = {SQRT_2, GOLDEN_RATIO, 1.5f, 1.732f}; // √2, φ, 3/2, √3
                currentRatio = ratios[ratioIndex];
                ratioIndex = (ratioIndex + 1) % 4;
                
                outerTargetSpeed = speedDist(rng);
                innerTargetSpeed = outerTargetSpeed * currentRatio;
            }
            
            // 确保速度在合理范围内
            while (innerTargetSpeed > MAX_SPEED || innerTargetSpeed < MIN_SPEED || 
                   outerTargetSpeed > MAX_SPEED || outerTargetSpeed < MIN_SPEED) {
                if (innerTargetSpeed > MAX_SPEED || outerTargetSpeed > MAX_SPEED) {
                    outerTargetSpeed *= 0.8f;
                    innerTargetSpeed = outerTargetSpeed * currentRatio;
                } else {
                    outerTargetSpeed *= 1.2f;
                    innerTargetSpeed = outerTargetSpeed * currentRatio;
                }
            }
        }
    } else {
        // 在过渡期内，平滑过渡到目标速度
        float transitionProgress = (currentTime - transitionStartTime) / 2.0f; // 2秒过渡时间
        
        if (transitionProgress >= 1.0f) {
            // 过渡完成
            outerSpeed = outerTargetSpeed;
            innerSpeed = innerTargetSpeed;
            isTransitioning = false;
            timeSinceLastChange = 0.0f;
        } else {
            // 平滑过渡
            float smoothProgress = 0.5f - 0.5f * cos(transitionProgress * M_PI);
            
            // 计算当前速度（从当前值平滑过渡到目标值）
            float startOuterSpeed = outerSpeed;
            float startInnerSpeed = innerSpeed;
            
            outerSpeed = startOuterSpeed + (outerTargetSpeed - startOuterSpeed) * smoothProgress;
            innerSpeed = startInnerSpeed + (innerTargetSpeed - startInnerSpeed) * smoothProgress;
        }
    }
}

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
    
    // 根据速度变化颜色：低速蓝色，中速绿色，高速红色
    float speedRatio = (outerSpeed - MIN_SPEED) / (MAX_SPEED - MIN_SPEED);
    if (speedRatio < 0.5f) {
        glColor3f(0.0f, speedRatio * 2.0f, 1.0f - speedRatio * 2.0f); // 蓝到青
    } else {
        glColor3f((speedRatio - 0.5f) * 2.0f, 1.0f - (speedRatio - 0.5f) * 2.0f, 0.0f); // 青到红
    }
    glutSolidSphere(0.08f, 12, 12);
    
    // 电机旋转指示线
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
    
    // 根据速度变化颜色：低速蓝色，中速绿色，高速红色
    float speedRatio = (innerSpeed - MIN_SPEED) / (MAX_SPEED - MIN_SPEED);
    if (speedRatio < 0.5f) {
        glColor3f(0.0f, speedRatio * 2.0f, 1.0f - speedRatio * 2.0f); // 蓝到青
    } else {
        glColor3f((speedRatio - 0.5f) * 2.0f, 1.0f - (speedRatio - 0.5f) * 2.0f, 0.0f); // 青到红
    }
    glutSolidSphere(0.06f, 12, 12);
    
    // 电机旋转指示线
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
                        "  Inner: " + std::to_string(innerSpeed).substr(0,4) +
                        "  Ratio: " + std::to_string(currentRatio).substr(0,6);
    for (char c : info2) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    
    glRasterPos2f(10, windowHeight - 90);
    std::string mode;
    if (USE_FIXED_RATIO) {
        mode = "Mode: Fixed Ratio (" + std::to_string(FIXED_RATIO).substr(0,6) + ")";
    } else if (USE_RANDOM_RATIO) {
        mode = "Mode: Random Ratio [" + std::to_string(MIN_RATIO).substr(0,4) + 
               "-" + std::to_string(MAX_RATIO).substr(0,4) + "]";
    } else {
        mode = "Mode: Alternating Irrational Ratios";
    }
    for (char c : mode) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glRasterPos2f(10, windowHeight - 110);
    std::string state = isTransitioning ? "State: Transitioning to new speeds" : 
                                       "State: Stable (Next change in: " + 
                                       std::to_string((int)(TARGET_CHANGE_INTERVAL - timeSinceLastChange)) + "s)";
    for (char c : state) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glRasterPos2f(10, windowHeight - 130);
    std::string info3 = "Gravity Direction: -Y (Green arrow)";
    for (char c : info3) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glRasterPos2f(10, windowHeight - 150);
    std::string info4 = "Controls: Mouse drag - Rotate camera  +/- - Re-randomize  R - Reset";
    for (char c : info4) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glRasterPos2f(10, windowHeight - 170);
    std::string info5 = "Gravity Trajectory: Orange line shows gravity exposure history";
    for (char c : info5) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    glRasterPos2f(10, windowHeight - 190);
    std::string info6 = "Motor Colors: Blue(low) -> Green(medium) -> Red(high speed)";
    for (char c : info6) {
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
    // 更新时间 (deltaTime 约为 0.016秒 = 16ms)
    float deltaTime = 0.016f;
    
    // 更新速度（带平滑过渡）
    updateSpeeds(deltaTime);
    
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
        case '+': // 重新随机化速度
        case '-': 
            initRandom();
            break;
        case 'r': // 重置
        case 'R':
            outerFrameAngle = 0.0f;
            innerFrameAngle = 0.0f;
            initRandom(); // 使用新的随机速度
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
    
    // 初始化随机化系统
    initRandom();
    
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
    glutCreateWindow("RPM Microgravity Simulator - Advanced Ratio Control");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);
    
    std::cout << "RPM Microgravity Simulator with Advanced Ratio Control" << std::endl;
    std::cout << "======================================================" << std::endl;
    std::cout << "Configuration:" << std::endl;
    if (USE_FIXED_RATIO) {
        std::cout << "- Mode: Fixed Ratio (" << FIXED_RATIO << ")" << std::endl;
    } else if (USE_RANDOM_RATIO) {
        std::cout << "- Mode: Random Ratio in range [" << MIN_RATIO << ", " << MAX_RATIO << "]" << std::endl;
    } else {
        std::cout << "- Mode: Alternating between √2, φ, 3/2, √3" << std::endl;
    }
    std::cout << "- Speed Range: [" << MIN_SPEED << ", " << MAX_SPEED << "] rad/s" << std::endl;
    std::cout << "- Change Interval: " << TARGET_CHANGE_INTERVAL << " seconds (after transition)" << std::endl;
    std::cout << "- Transition Time: 2 seconds" << std::endl;
    std::cout << "Enhanced Features:" << std::endl;
    std::cout << "- Ratio control within specified bounds" << std::endl;
    std::cout << "- Smooth speed transitions" << std::cout << "- Improved timing logic (starts after transition)" << std::endl;
    std::cout << "- Real-time state display" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- Mouse drag: Rotate camera" << std::endl;
    std::cout << "- +/-: Re-randomize speeds immediately" << std::endl;
    std::cout << "- R: Reset simulation and clear trajectory" << std::endl;
    std::cout << "- ESC: Exit" << std::endl;
    
    glutMainLoop();
    
    return 0;
}
