#include <stdint.h>

typedef struct
{ 
  uint8_t header1;
  uint8_t header2;
  uint8_t id;
  uint8_t length;
  uint8_t instruct;
  int right_rpm;
  int left_rpm;
  uint8_t checksum;
}Stm_motor_packet;

typedef struct
{
  uint8_t header1;
  uint8_t header2;
  uint8_t id;
  uint8_t length;
  uint8_t instruct;
  int encoder[2];
  uint8_t checksum;
}Stm_encoder_packet;
