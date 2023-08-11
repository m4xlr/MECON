// * CODIGO MASTER 06
#include <Wire.h>

const int I2C_SLAVE_ADDR_1 = 8;  //Direccion del esclavo1 MANO DER
const int I2C_SLAVE_ADDR_2 = 9;  //Direccion del esclavo2 PIE DER
const int I2C_SLAVE_ADDR_3 = 10; //Direccion del esclavo3 PIE IZQ
const int I2C_SLAVE_ADDR_4 = 11; //Direccion del esclavo4 MANO IZQ
const int I2C_SLAVE_ADDR_5 = 12; //Direccion del esclavo5 CUERPO

// * SECUENCIA DE BAILE
// Tiempo de baile 3min
// tiempo aprox por paso 6s
// total de pasos resultante 30
// cada paso se representa como un int con valores entre -127 y 127
// donde 0 es la posicion inicial del hilo, -127 es la mas alta y 127 la mas baja
const int motores=5; //cantidad de motores, constante necesaria para deconstruir los pasos luego
// para demostracion pasos 16
const int pasos= 16; //constante necesaria para deconstruir los pasos luego
int baile [pasos][motores] = {
    //test manos
    {-30,0,0,-30,0},
    {0,0,0,0,0},
    //test piernas
    {0,-30,-30,0,0},
    {0,0,0,0,0},
    //test cuerpo
    {0,0,0,0,-30},
    {0,0,0,0,0},
    //la ola
    {-20,0,0,0,0},
    {-40,-20,0,0,0},
    {-60,-40,0,0,-20},
    {-40,-60,-20,0,-40},
    {-20,-40,-40,-20,-60},
    {0,-20,-60,-40,-40},
    {0,0,-40,-60,-20},
    {0,0,-20,-40,0},
    {0,0,0,-20,0},
    {0,0,0,0,0},
    };

    

//* VOID DE ORDENES
void mandarPasos(const int a[][motores]){
    int ok = 0;
    int done=0;
    for (int i = 0; i < pasos; i++)//revisa la matriz de baile paso por paso
    {
       for (int j = 0; j < motores; j++) //revisa el paso motor por motor
       {
        Serial.print("Valor a enviar: ");
        Serial.print(a[i][j]);
        Wire.beginTransmission(j+8);//inicia transmision al esclavo reemplazando la direcion con la variable
        int pass = a[i][j]+127; //reemplaza el valor del baile agregando 127 para transmitir el byte. 
        Wire.write(pass);//escribe los valores para mandar al buffer
        Wire.endTransmission();//transmite los valores
       };

       //* CONFIRMACION
       for (int y = 0; y < motores; y++)//revisa confirmacion motor por motor
       {
        ok = 0;//Reset ok
        done=0;//Reset done
        while (done != 1)
        {
        Wire.requestFrom(y+8,1);//solicita confirmacion por parte de los slaves como 1 byte
        while (Wire.available())//espera disponibilidad de respuesta
        {
            ok = Wire.read(); //reemplaza el valor de lectura en la variable ok
            Serial.println("recibiendo..."); //notificacion en serial[DEV]
            Serial.print(ok);//debe ser 2
        };
        if (ok==2) //corroboracion de respuesta = 2
            {
            Serial.println("motor"+String(y+1)+"is done"); //confirmacion a serial [DEV]
            done=1; //rompe el while de "done" para continuar al siguiente motor
            };

        delay(200);//Espera para volver a solicitar
        }
       };
    };
    
}

//* SETUP
void setup(){
    Wire.begin();
    Serial.begin(9600); //inicia monitor serial [DEV]
    int listo = 0;
    for (int i = 0; i < motores; i++) //inicia verificacion para cada motor
    {
        while (listo != 1)
        {
            int address_slave = i+8;
            Wire.requestFrom(address_slave,1);
            delay(500);//espera a que responda el slave
            while (Wire.available() >= 1)
            {
                listo = int(Wire.read());
                Serial.print("valor recibido: ");
                Serial.println(listo);
            }
            delay(500); //Tiempo de espera para volver a solicitar
        }

        Serial.println("Motor"); //NOTIFICACION A SERIAL [DEV]
        Serial.print(i+1);
        Serial.print("listo!");

        listo = 0; //reset de variable listo
    }
    Serial.println("TODOS LOS MOTORES LISTOS!");
    delay(3000); //Cuenta regresiva 3..2..1
} 


//* LOOP
void loop(){
    mandarPasos(baile);
    delay(1800000);//no hace nada por media hora
}
