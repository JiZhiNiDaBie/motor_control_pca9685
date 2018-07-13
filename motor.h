#ifndef MOTOR_H
#define MOTOR_H

//#define unch unsigned char

class pca
{
public:
  pca(unsigned char inputAddr, int enPin) : addr(inputAddr), en(enPin), fd(init()) {}
  pca() : addr(0x40), en(7), fd(init()) {}
  pca(const pca &inputPca) : addr(inputPca.addr), en(inputPca.en), fd(init()) {}
  ~pca() {}
  void setmk(int num, int mk);
  void setDuty(int num, double duty);
  void enable();
  int init();
  unsigned char addr;
  int en;
  int fd;
};

class motor
{
public:
  motor(int pin1, int pin2, const pca &inputPca, int inputChannel);
  motor(int &pin1, int &pin2, const pca &inputPca, int &inputChannel);

  motor() {}
  void setup(int pin1, int pin2, const pca &inputPca, int inputChannel);
  void setSpeed(double duty);
  void stop();

  double currentSpeed = 0;

private:
  int pin_1, pin_2, channel;
  pca motorPca;
};

class servo
{
public:
  servo(const pca &inputPca, int inputChannel);
  servo() {}
  void write(double angle);

private:
  int channel;
  pca servoPca;
};

#endif
