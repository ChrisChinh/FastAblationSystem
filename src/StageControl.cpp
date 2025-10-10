#pragma once
#include "StageControl.h"
#define IP_ADDRESS "10.10.10.100"
#include "../include/ACSC.h"
#include <iostream>

StageControl::StageControl() {
  int port = 701;
  handle = (HANDLE)-1;
  handle = acsc_OpenCommEthernetTCP(_strdup(IP_ADDRESS), port);
  if (handle == ACSC_INVALID) {
    std::cout << "Failed to connect to stage controller" << std::endl;
    acsc_GetLastError();
  }
}

void StageControl::moveAbsolute(double x, double y, double z, int flags, bool async_mode) {
  auto wait = async_mode ? ACSC_IGNORE : ACSC_SYNCHRONOUS;
  int axes[] = {ACSC_AXIS_0, ACSC_AXIS_1, ACSC_AXIS_2, -1};
  double positions[] = {x, y, z};
  if (!acsc_ToPointM(handle, 0, axes, positions, wait)) {
    std::cout << "Failed to move stage to absolute position" << std::endl;
    std::cout << acsc_GetLastError();
  }
  if (!async_mode) {
      acsc_WaitMotionEnd(handle, 0, 10000);
      acsc_WaitMotionEnd(handle, 1, 10000);
      acsc_WaitMotionEnd(handle, 2, 10000);
  }
}

double *StageControl::getStagePosition() {
  double x_pos, y_pos, z_pos;
  int result = 0;
  result += acsc_GetFPosition(handle, ACSC_AXIS_0, &x_pos, ACSC_SYNCHRONOUS);
  result += acsc_GetFPosition(handle, ACSC_AXIS_1, &y_pos, ACSC_SYNCHRONOUS);
  result += acsc_GetFPosition(handle, ACSC_AXIS_2, &z_pos, ACSC_SYNCHRONOUS);
  if (result == 0) {
    std::cout << "Failed to get stage position" << std::endl;
    acsc_GetLastError();
  }
  double *positions = new double[3];
  positions[0] = x_pos;
  positions[1] = y_pos;
  positions[2] = z_pos;
  return positions;
}

void StageControl::moveRelative(double x, double y, double z, bool async_mode) {
  double* pos = this->getStagePosition();
  x += pos[0];
  y += pos[1];
  z += pos[2];
  StageControl::moveAbsolute(x, y, z, 0, async_mode);
}

void StageControl::reconnect() {
  int port = 703;
  handle = (HANDLE)-1;
  handle = acsc_OpenCommEthernetTCP(_strdup(IP_ADDRESS), port);
  if (handle == ACSC_INVALID) {
    std::cout << "Failed to reconnect to stage controller" << std::endl;
    acsc_GetLastError();
  }
}

void StageControl::jogMotion(int axis, double velocity) {
  this->setVelocity(velocity);
  acsc_Jog(handle, 0, axis, velocity, ACSC_SYNCHRONOUS);
}

void StageControl::stopJogMotion(int axis) {
    acsc_Kill(handle, axis, ACSC_SYNCHRONOUS);
}

void StageControl::setVelocity(double velocity) {
    acsc_SetVelocity(handle, 0, velocity, ACSC_SYNCHRONOUS);
    acsc_SetVelocity(handle, 1, velocity, ACSC_SYNCHRONOUS);
    acsc_SetVelocity(handle, 2, velocity / 8.0, ACSC_SYNCHRONOUS);
}

double StageControl::getVelocity() {
    double velocity;
    acsc_GetVelocity(handle, 0, &velocity, ACSC_SYNCHRONOUS);
    return velocity;
}

bool StageControl::isMoving() {
    int state_x, state_y, state_z;
    acsc_GetMotorState(handle, 0, &state_x, ACSC_SYNCHRONOUS);
    acsc_GetMotorState(handle, 1, &state_y, ACSC_SYNCHRONOUS);
    acsc_GetMotorState(handle, 2, &state_z, ACSC_SYNCHRONOUS);

    return state_x == ACSC_MST_MOVE || state_y == ACSC_MST_MOVE || state_z == ACSC_MST_MOVE;

}

StageControl::~StageControl() { acsc_CloseComm(handle); }