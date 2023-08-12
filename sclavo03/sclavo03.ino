#include <AccelStepper.h> 
#include <Wire.h>

// Configuracion del stepper
AccelStepper stepperX(1, 5, 4);  

#define home_switch 12 // Define Pin 12 connectado como Switch FIN DE CARRERA (MicroSwitch)

//*Variables para el stepper

long initial_homing=-1;  // Usado para setear el final de carrera
long posicioncero = 2000;  //* DEFINE LA POSICION INICIAL CON RESPECTO AL TOPE siempre en positivo
long margen = 200;// Margen de seguridad para que el punto mas alto no toque el final de carrera
long MAX_up= margen - posicioncero;
long MAX_down= posicioncero;
const double v_set=100.0; //VELOCIDAD DE SETEO
const double a_set=100.0; //ACELERACION DE SETEO

const double vel=1000.0; //VELOCIDAD EN FUNCIONAMIENTO
const double acel=1000.0; //ACELERACION EN FUNCIONAMIENTO
long stop; 
long steps;
//*Variables para comunicacion I2C
const int i2c_address=8; // Direccion del slave en i2c
int listo=0; //variable contador de confirmacion
int bits=1; //pta funciona pe

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
  stepperX.setMaxSpeed(vel);      // Restauracion a velocidad normal
  stepperX.setAcceleration(acel);  // Restauracion a aceleracion normal

  //COMUNICACION i2C
  Wire.begin(i2c_address); //inicia comunicacion en i2c como slave
  Wire.onRequest(LaunchOk);
  Wire.onReceive(Master_says);
}


//*FUNCION DE CONFIRMACION
void LaunchOk(){
  while (listo==0)
  {
    listo=1;
    Wire.write(1);
    Serial.println("Launch enviado!");
  }
  if (listo==1)
      {
        delay(1000);//tiempo para que reaccione el stepper [PATCH]
        if (stepperX.currentPosition()!= steps)
        {
          Wire.write(3);// porsiacaso 3 para no confundir con 1 de launch
          Serial.println("aun no esta listo");
        }
        else
        {
          Wire.write(2);
          Serial.println("listo enviado");
        }
      }
}

void Master_says (int numBytes){
  int paso = Wire.read();
  Serial.println("Recibido: ");
  Serial.print(paso);
  steps = map(long(paso), -127,127,MAX_up,MAX_down);
  Serial.print(" transforma");
  Serial.print(steps);
  delay(1000);//delay para que el esclavo escriba los valores en el serial
  //stepperX.runToNewPosition(steps);
  
}

//*LOOP
void loop() {
  stepperX.moveTo(steps);
  //stop = steps - long(acel);
  while (stepperX.distanceToGo() != 0)
  {
    stepperX.run();
  }
  //steps = 0; reset de steps a 0, no es necesario 
  delay(500); //porsiacas
}