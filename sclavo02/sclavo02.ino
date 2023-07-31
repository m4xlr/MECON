#include <AccelStepper.h> 
#include <Wire.h>

// Configuracion del stepper
AccelStepper stepperX(1, 5, 4);  

#define home_switch 12 // Define Pin 12 connectado como Switch FIN DE CARRERA (MicroSwitch)

//*Variables para el stepper

long initial_homing=-1;  // Usado para setear el final de carrera
long posicioncero = 200;  //* DEFINE LA POSICION INICIAL CON RESPECTO AL TOPE siempre en positivo
long margen = 30;
long MAX_up= -posicioncero+margen;
long MAX_down= posicioncero;
const double v_set=100.0; //VELOCIDAD DE SETEO
const double a_set=100.0; //ACELERACION DE SETEO

const double vel=1000.0; //VELOCIDAD EN FUNCIONAMIENTO
const double acel=1000.0; //ACELERACION EN FUNCIONAMIENTO

//*Variables para comunicacion I2C
const int i2c_address=8; // Direccion del slave en i2c
int listo=0; //variable contador de confirmacion

//SETUP
void setup() {
   Serial.begin(9600);  // Inicia monitor serial [DEV]
   
   pinMode(home_switch, INPUT_PULLUP); //Configuracion pin FIN DE CARRERA
   
   delay(1000);  // *tiempo de espera para encender drivers y liberar reles

   //  Configuracion inicial para seteo
  stepperX.setMaxSpeed(v_set);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperX.setAcceleration(a_set);  // Set Acceleration of Stepper
 

// Start Homing procedure of Stepper Motor at startup

  Serial.print("Stepper is Homing . . . . . . . . . . . "); //notificacion a serial [DEV]

  while (digitalRead(home_switch)) {  // Make the Stepper move CCW until the switch is activated   
    stepperX.moveTo(initial_homing);  // Set the position to move to
    initial_homing--;  // Decrease by 1 for next move if needed
    stepperX.run();  // Start moving the stepper
    delay(5);
}

  stepperX.setCurrentPosition(0);  // Set the current position as zero for now
  stepperX.setMaxSpeed(v_set);      // Velocidad de seteo
  stepperX.setAcceleration(a_set);  // Aceleracion seteo
  initial_homing=1;

  while (!digitalRead(home_switch)) { // Make the Stepper move CW until the switch is deactivated
    stepperX.moveTo(initial_homing);  
    stepperX.run();
    initial_homing++;
    delay(5);
  }
  //SECUENCIA DE SETEO A POSICION CERO
  stepperX.setCurrentPosition(0);
  stepperX.runToNewPosition(posicioncero);
  stepperX.setCurrentPosition(0);
  Serial.println("Homing Completed"); //Notificacion a serial [DEV]
  Serial.println("");
  stepperX.setMaxSpeed(vel);      // Restauracion a velocidad normal
  stepperX.setAcceleration(acel);  // Restauracion a aceleracion normal

  //COMUNICACION i2C
  Wire.begin(i2c_address); //inicia comunicacion en i2c como slave
  Wire.onRequest(LaunchOk);
  Wire.onReceive(Master_says);
}


//*FUNCION DE CONFIRMACION
void LaunchOk(){
  if (listo==0)
  {
    listo=1;
    Wire.write(listo);
    Serial.println("Launch enviado!");
  }
  else{
    if (listo==1)
      {
        while (stepperX.isRunning())
        {
        delay(200);
        }
        listo=2;
        Wire.write(listo);
        Serial.println("listo enviado");
      }
    listo=1;
  }
}

void Master_says (int numBytes){
  int paso = Wire.read();
  long steps = map(paso, -127,127,MAX_up,MAX_down);
  Serial.println("Recibido: ");
  Serial.print(paso);
  Serial.print(" ->");
  Serial.print(steps);
  stepperX.runToNewPosition(steps);
}

//*LOOP
void loop() {
  delay(200); //porsiacas
}