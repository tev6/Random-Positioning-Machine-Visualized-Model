# Random-Positioning Machine Microgravity Visualize Simulator

A simluator to visualized the process of random-positioning machine (RPM).

![RPM Simulation](https://img.shields.io/badge/Simulation-OpenGL%2FGLUT-blue)
![Research Grade](https://img.shields.io/badge/Research-Academic%20Grade-green)

## 📖 Overview

This project provides an interactive 3D simulation of Random Positioning Machines (RPM) used in ground-based microgravity research. Based on the comprehensive academic review in `RPM_Academic_Review.pdf`, the simulator demonstrates how RPMs achieve functional weightlessness through gravity-vector averaging across two rotational axes.

### Key Features
- **Dual-Axis Rotation**: Realistic simulation of outer (X-axis) and inner (Y-axis) rotational frames
- **Gravity Vector Visualization**: Real-time display of gravitational direction and averaging
- **Sample Tracking**: Multi-colored semi-transparent cube representing biological samples
- **Trajectory Analysis**: Orange trajectory line showing gravity exposure history
- **Interactive Controls**: Adjustable rotation speeds and camera perspectives

## 🎯 Scientific Background

Random Positioning Machines simulate microgravity by continuously reorienting samples along two perpendicular axes, effectively averaging out the gravitational vector over time. When reorientation occurs faster than the biological system's response time, the net experienced gravity approximates zero, functionally mimicking weightlessness.

**Key Principles:**
- Gravity vector averaging through 3D rotation
- Temporal integration of gravitational effects
- Biological response time considerations
- Quasi-random rotation patterns for optimal coverage

## 🛠️ Installation & Requirements

### Prerequisites
- C++ Compiler (GCC, Clang, or MSVC)
- OpenGL libraries
- GLUT (OpenGL Utility Toolkit)
- GLU (OpenGL Utilities)

### Linux Installation
```bash
# Ubuntu/Debian
sudo apt-get install freeglut3-dev libglu1-mesa-dev

# Compilation
g++ -o rpm_simulator RPMSim.cpp -lGL -lGLU -lglut -lm
