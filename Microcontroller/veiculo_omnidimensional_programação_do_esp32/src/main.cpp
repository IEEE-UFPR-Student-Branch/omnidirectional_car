#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include "BluetoothSerial.h"
BluetoothSerial bt;

int stage= 0 ;
int front1v_left= 23;
int front2v_left = 24;
int front1v_right = 25;
int front2v_right = 26;
int back1v_left = 27;
int back2v_left = 28;
int back1v_right = 29 ;
int back2v_right = 30 ;
int frontleftengine = 10;
int frontrightengine = 11;
int backleftengine = 12;
int backrightengine= 13;
char valor_lido ; 



void setup() {
  Serial.begin(115200);
  bt.begin("veiculo_ominidimentional");
  
  
  // put your setup code here, to run once:
  pinMode(front1v_left,OUTPUT);
  pinMode(front1v_right,OUTPUT);
  pinMode(front2v_left,OUTPUT);
  pinMode(front2v_right,OUTPUT);
  pinMode(back1v_left,OUTPUT);
  pinMode(back2v_left,OUTPUT);
  pinMode(back1v_right,OUTPUT);
  pinMode(back2v_right,OUTPUT);
  pinMode(frontleftengine,OUTPUT);
  pinMode(frontrightengine,OUTPUT);
  pinMode(backleftengine,OUTPUT);
  pinMode(backrightengine,OUTPUT);
}

void loop() {

  valor_lido = (char)bt.read();
if(bt.available()){
  if(valor_lido=='1'){
    stage= 1 ;

  } else if(valor_lido=='2'){
    stage= 2 ;
  }else if(valor_lido== '3'){
    stage= 3 ;
    
  }else if(valor_lido=='4'){
    stage= 4 ;
    
  }else if(valor_lido== '5'){
    stage= 5 ;
    
  }
}
    
  while(1){
    if(stage==0){
      //Veiculo parado 
      girar_motor_frente(front1v_left,front2v_left,front1v_right,front2v_right, frontleftengine,frontrightengine, 0);
      girar_motor_frente(back1v_left,back2v_left,back1v_right,back2v_right, backleftengine, back1v_right , 0 );
    delay(1);

    } else if(stage==1){
      //Veiculo andando para frente 
      girar_motor_frente(front1v_left,front2v_left,front1v_right,front2v_right, frontleftengine,frontrightengine, 255);
      girar_motor_frente(back1v_left,back2v_left,back1v_right,back2v_right, backleftengine, back1v_right , 255 );
      delay(1);


    } else if(stage==2){
      //Veiculo andando para tras
      girar_motor_tras(front1v_left,front2v_left,front1v_right,front2v_right, frontleftengine,frontrightengine, 255);
      girar_motor_tras(back1v_left,back2v_left,back1v_right,back2v_right, backleftengine, back1v_right , 255);
      delay(1);
    } else if(stage==3){
      //Veiculo andando para esqueda
       girar_motor_frente(front1v_left,front2v_left,front1v_right,front2v_right, frontleftengine,frontrightengine, 255);
       girar_motor_tras(back1v_left,back2v_left,back1v_right,back2v_right, backleftengine, back1v_right , 255);
       delay(1);
    } else if(stage==4){
      //Veiculo andando para direita 
      girar_motor_tras(front1v_left,front2v_left,front1v_right,front2v_right, frontleftengine,frontrightengine, 255);
      girar_motor_frente(back1v_left,back2v_left,back1v_right,back2v_right, backleftengine, back1v_right , 255 );
      delay(1);
      
    }
  }
}

void girar_motor_frente(int variavel1, int variavel2,int variavel3, int variavel4, int pino_velocidade1,int pino_velocidade2 ,int velocidade){

  digitalWrite(variavel1,LOW );
  digitalWrite(variavel2,HIGH);
  digitalWrite(variavel3,LOW );
  digitalWrite(variavel4,HIGH);
  analogWrite(pino_velocidade1,velocidade);
  analogWrite(pino_velocidade2,velocidade);
  delay(1);
}
void girar_motor_tras(int variavel11, int variavel22, int variavel33, int variavel44, int pino_velocidade1 , int pino_velocidade2,int velocidade1){

  digitalWrite(variavel11,HIGH );
  digitalWrite(variavel22,LOW);
  digitalWrite(variavel33,HIGH );
  digitalWrite(variavel44,LOW);
  analogWrite(pino_velocidade1,velocidade1);
  analogWrite(pino_velocidade2,velocidade1);
  delay(1);
}


