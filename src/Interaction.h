#pragma once

//定义涉及交互动作的常量以及函数
enum LEFT_BUTTON_ACTION
{
	LBA_FREE,
	LBA_DIGGING,
	LBA_PLACE,
	LBA_ATTACK
};

const GLint TOUCH_MAX = 100;
const GLfloat TOUCH_STEP = CUBESIZE / 10.0f;

LEFT_BUTTON_ACTION left_button_state=LBA_FREE;
GLfloat LBA_hold_time = 0.0f;
GLint last_x,last_y,last_z;
CubeType active_item = stone;

void UpdateDynamicItem(CubeType item);
GLint touch_cube(GLint *x, GLint *y, GLint *z);
void digged_cube(GLint x, GLint y, GLint z);
void LBA_handle();