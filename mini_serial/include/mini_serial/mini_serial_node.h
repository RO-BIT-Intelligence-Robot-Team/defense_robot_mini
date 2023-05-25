#ifndef MINI_SERIAL_H
#define MINI_SERIAL_H
#include "mini_serial_struct.h"
#include <serial/serial.h>
#include <thread>
#include <mutex>
#include <ros/ros.h>
#include <mini_serial/encoder_msg.h>
#include <mini_serial/motor_msg.h>
#include <cstring>
#include <chrono>


using namespace serial;
using namespace std;

class node{
public:
  node(int _argc,char ** _argv);
  ~node();
  void init();
  void run();
  void serial_recive();

private:
  void control_motor_callback(const mini_serial::motor_msgPtr &msg);

  ros::Publisher encoder_state_pub;
  ros::Subscriber control_motor_sub;

  mini_serial::encoder_msg encoder_state_msg;

  int argc;
  char ** argv;
  Serial s;
  mutex rpm_m;
  mutex serial_m;
  int encoder_L;
  int encoder_R;

  Stm_encoder_packet recive;
  Stm_motor_packet transe;
};



#endif
