#include "../include/mini_serial/mini_serial_node.h"

node::node(int _argc,char ** _argv):argc(_argc),
  argv(_argv)
{

}
node::~node(){
  if(ros::isStarted()){
    ros::shutdown();
    ros::waitForShutdown();
  }
}
void node::init(){
  ros::init(argc,argv,"mini_serial");
  ros::start(); // explicitly needed since our nodehandle is going out of scope.
  ros::NodeHandle n;
  // Add your ros communications here.
  encoder_state_pub=n.advertise<mini_serial::encoder_msg>("/encoder_state",1);
  control_motor_sub=n.subscribe("/motor_val",1,&node::control_motor_callback,this);
  
  s.setPort("/dev/ttyU2D2");
  s.setBaudrate(115200);

  try
  {
    s.open();
  }
  catch (serial::IOException e)   // 퍼미션, 포트 없음 등으로 열수 없을 때 예외 출력하고 리턴
  {
    cerr << "Port open failed." <<e.what()<<endl;
    return ;
  }
    transe.header1=0xFF;
    transe.header2=0xFF;
    transe.id=1;
    transe.instruct=0x03;
    transe.length=0x04;
    transe.left_rpm=0;
    transe.right_rpm=0;
}

void node::run(){
  thread th(&node::serial_recive,this);
  ros::Rate looprate(10);
  while (ros::ok()) {
    bool serial(false),rpm(false);
    unsigned char tx_buffer[20];
    while(!serial||!rpm){
      if(rpm_m.try_lock()){
        rpm=true;
        memcpy(tx_buffer,&transe,20);
        rpm_m.unlock();
      }
      if(serial_m.try_lock()){
        serial=true;
        encoder_state_msg.encoder_L=recive.encoder[0];
        encoder_state_msg.encoder_R=recive.encoder[1];
        serial_m.unlock();
      }
    }
    encoder_state_pub.publish(encoder_state_msg);

    s.write(tx_buffer,20);
    ros::spinOnce();
    looprate.sleep();
  }
  th.join();
}

void node::serial_recive(){
  while (ros::ok()) {
    
  try{
    size_t size=s.available();
    if(size){
      unsigned char readbuffer[100];
      s.read(readbuffer,size);
      //for(int i = 0 ; i < size ; i++) cout << i << " : " <<(int)readbuffer[i] << endl;
      if(readbuffer[0] == 255 && readbuffer[1] == 255){
         serial_m.lock();
         memcpy(&recive,readbuffer,20);
         serial_m.unlock();
      }
    }
  }catch (IOException e)   // 예외 발생시 메시지 띄우고 포트 닫는다.
    {
      cerr << "Port disconnected. closing port(" << e.what() << ")." << endl;
      s.close();
    }

    this_thread::sleep_for(chrono::milliseconds(1));    // cpu 점유율 낮추기 위해 잠깐 sleep
  }
}

void node::control_motor_callback(const mini_serial::motor_msgPtr &msg){
  rpm_m.lock();
  transe.left_rpm=msg->motor_L;
  transe.right_rpm=msg->motor_R;
  transe.checksum=~(transe.id+transe.length+transe.instruct+transe.left_rpm+transe.right_rpm);
  rpm_m.unlock();
}
