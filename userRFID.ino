#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above
byte tarjetaescribirUID[4];
char uidSerial[16];
char uidSerial2[8];
byte myHex[7];
byte myHex2[3];
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // DIR,E, RW, RS, D4,D5,D6,D7

struct MyStruct{
  byte UID[4];
  char name[10];
};
char opcion = 'k';
byte dato;
int cont = 0;
byte memoryRecord[4];
byte memo[4];
MyStruct info,memo2;

//Funcion formateo de memoria
void formateo() {
  for ( int index = 0 ; index < EEPROM.length() ; index++ )
  {
    EEPROM[ index ] = 0;
  }
}
//Funcion para verificar el acceso
boolean acceso(byte uid[]) {
  
  for (int i = 0; i < 1024; i++) {
    EEPROM.get(i, memo2);
    if (compareArray(uid, memo2.UID)) {
      return true;
    }
    i+=13;
  }
  return false;
}
//funcion eliminar usuario
boolean deleteUser(byte uid[]) {
  
  for (int i = 0; i < 1024;i++) {
    EEPROM.get(i, memo2);
    if (compareArray(uid, memo2.UID)) {
      i--;
      for(int j = 0 ; j <14;j++){
        EEPROM.put(j+i, 0);
      }
      return true;
    }else
      i+=13;
  }
  return false;
}

//Función para comparar dos vectores
boolean compareArray(byte array1[], byte array2[])
{
  if (array1[0] != array2[0])return (false);
  if (array1[1] != array2[1])return (false);
  if (array1[2] != array2[2])return (false);
  if (array1[3] != array2[3])return (false);
  return (true);
}
//Funcion para guardar nuevo usuario
void saveUser(MyStruct user){
  
  for (int i = 0; i < 1024; i++) {
    byte address = EEPROM.read(i);
    if (address == 0) {
      EEPROM.put(i, user);
      return;
    }else{
      i +=13;
           
    }
  }
}

MyStruct viewUsers(byte num){
    MyStruct deleteUser;
    byte address = EEPROM.read(num);
    if (address != 0) {
      deleteUser = EEPROM.get(num, deleteUser);
      return deleteUser;
    } 
}


void mostrarMemoria(){
  for (int direccion=0;direccion<1024;direccion++)
    {
      for(int columna=0;columna<10;columna++)
      { 
        // escribe hileras de 10 valores por línea
        // lee la posición EEPROM indicada por 'direccion'
        dato = EEPROM.read(direccion);
        Serial.print(direccion); // muestra la direccion del valor
        Serial.print(" ="); // separador
        Serial.print("\t"); // tabulacion
        Serial.print(dato); // muestra el valor en direccion
        Serial.print("\t"); // tabulacion
        direccion++; // incrementa direccion
        if(direccion==1024) break; // solo muestra hasta la direccion 1023
      }
      Serial.println(); // salto a la sig línea
      direccion--; // decrementa direccion para la siguiente linea
    }  
}

void setup() {
  Serial.begin(9600); // inicializar puerto serie
  while (!Serial) {} 
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // iniciacion del lector
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16,2);
  lcd.clear();
}

void loop() { 
    if(opcion != '\n'){
      
      Serial.println("ºººººººººººººººººººººººººººººººººººººººººººººººººººººººººº\nIngrese la opcion");
      Serial.println("a. Validar Usuario");
      Serial.println("b. Ingresar Nuevo Usuario");
      Serial.println("c. Eliminar Usuario");
      Serial.println("d. Mostrar Memoria");
      Serial.println("e. Vaciar Memoria");
      Serial.println(" ");
    }
    while(Serial.available() == 0) {
    }
    opcion = Serial.read();
    boolean r = false;
    
      switch (opcion) {
/*---------------------------------------------------------------------------------------------*/      
        case 'a':
          Serial.println("Validación de usuario, acerque la tarjeta al lector RFID");
          while (r == false) {
            if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) 
              delay(500);
             else 
              r = true;  
          }
          Serial.print("UID");
            for(byte i=0;i<mfrc522.uid.size;i++){//vamos a ver el valor de UID de las tarjetas
              if(mfrc522.uid.uidByte[i]<0X10){
                Serial.print(" 0");
              }else{
                Serial.print(" ");
              }
              Serial.print(mfrc522.uid.uidByte[i],HEX);   
              info.UID[i] = mfrc522.uid.uidByte[i];
            }
          
          Serial.println("");
          
          if (acceso(info.UID)){
            Serial.println("Acceso concedido");
            Serial.print("Bienvenido: ");
            Serial.println(memo2.name);}
          else
            Serial.println("Acceso denegado");
          break;
/*---------------------------------------------------------------------------------------------*/       
        case 'b':
    
          Serial.println("ººººººººººººººººIngreso de nuevo usuarioºººººººººººººººººº");
          Serial.println("-----------Por favor acerque su tarjeta");
          while (r == false) {
            if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) 
              delay(500);
             else 
              r = true;  
          }
          Serial.print("UID");
            for(byte i=0;i<mfrc522.uid.size;i++){//vamos a ver el valor de UID de las tarjetas
              if(mfrc522.uid.uidByte[i]<0X10){
                Serial.print(" 0");
              }else{
                Serial.print(" ");
              }
              Serial.print(mfrc522.uid.uidByte[i],HEX);   
              info.UID[i] = mfrc522.uid.uidByte[i];
            }
            
          if (acceso(info.UID)){
            Serial.println("\nUsuario ya existente \n");
            break;
           }
          
          //Ingreso de nombre
          for(int i = 0; i<10;i++)
            info.name[i]=0;
          Serial.println("Ingrese su nombre finalizando con # Ejemplo: Julian# ");
          for(int i = 0; i<10;i++){
            while(Serial.available()==0){}
            char var = Serial.read();
            if (var=='#')
              i=10;
            else if(var != '\n')
              info.name[i]=var;
            else
              i--;
          }
          for(int i = 0; i<10;i++)
            Serial.print(info.name[i]);
          Serial.println(" ");
          saveUser(info);
          break;
/*---------------------------------------------------------------------------------------------*/                
        case 'c':
          Serial.println("ºººººººººººººººººººººEliminar Usuarioººººººººººººººººººººº");
          Serial.println("-----------Por favor acerque su tarjeta");.

          
          while (r == false) {
            if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) 
              delay(500);
             else 
              r = true;  
          }
          Serial.print("UID");
            for(byte i=0;i<mfrc522.uid.size;i++){//vamos a ver el valor de UID de las tarjetas
              if(mfrc522.uid.uidByte[i]<0X10){
                Serial.print(" 0");
              }else{
                Serial.print(" ");
              }
              Serial.print(mfrc522.uid.uidByte[i],HEX);   
              info.UID[i] = mfrc522.uid.uidByte[i];
            }
          
          Serial.println("");
          if(deleteUser(info.UID))
            Serial.println("Usuario eliminado ");
          else
            Serial.println("Usuario no encontrado");
   
        break;
        
        case 'd':
          mostrarMemoria();   
          break;
        case 'e':
          formateo();
        default:
          break;
    
      }
       mfrc522.PICC_HaltA(); 
    
}









