#include "../include/mini_serial/mini_serial_node.h"
#include "mini_serial_node.cpp"

int main(int argc,char ** argv){
  node n(argc,argv);
  n.init();
  n.run();
}
