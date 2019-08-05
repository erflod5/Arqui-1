#define led 3
#define btn1 A0
bool opcion = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT); 
  pinMode(btn1,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(btn1)){
    delay(50);
    if(digitalRead(btn1))
      opcion = !opcion;  
  }
  if(opcion){
    digitalWrite(led,HIGH);
  }
  else{
    digitalWrite(led,LOW); 
  }
}
