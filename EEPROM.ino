#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // DIR,E, RW, RS, D4,D5,D6,D7

struct MyStruct {
  byte UID[4];
  char name[20];
};
const int ledRed = 6;
const int ledGreen = 5;
char name1[100];
char opcion = 'k';
char newUser;
char deleteUs;
char deleteIndex;
byte dato;
int cont = 0;
byte memoryRecord[4];
byte memo[4];
MyStruct info, memo2;
MyStruct userDelete;
String opcionesArray[] = {"a. ValidarUser","b. IngresarUser","c. EliminarUser","d. MostrarUsers","e. VaciarMemoria"," "};
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
    i += 23;
  }
  return false;
}
//funcion eliminar usuario Contacto
boolean deleteUser(byte uid[]) {

  for (int i = 0; i < 1024; i++) {
    EEPROM.get(i, memo2);
    if (compareArray(uid, memo2.UID)) {
      i--;
      for (int j = 0 ; j < 24; j++) {
        EEPROM.put(j + i, 0);
      }
      return true;
    } else
      i += 23;
  }
  return false;
}
//funcion eliminar usuario Teclado
void deleteUserTec(byte index) {
  int lim = (index * 24) + 24;
  int pos = (index) * 24;
  for (int j = pos ; j < lim; j++) {
    EEPROM.put(j, 0);//Elimina un usuario segun el indice recibido
  }
  
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

//Funcion save name
void saveName() {
  //Ingreso de nombre
  for (int i = 0; i < 20; i++)
    info.name[i] = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ingrese Nombre");
  lcd.setCursor(0,1);
  lcd.print("por favor");  
  Serial.println("\nIngrese su nombre finalizando con # Ejemplo: Julian Marin# ");
  for (int i = 0; i < 100; i++) {
    while (Serial.available() == 0) {}
    char var = Serial.read();
    if (var == '#')
      i = 100;
    else if (var != '\n')
      name1[i] = var;
    else
      i--;
  }
  for (int i =0; i<16;i++){
    info.name[i] = name1[i];}
  
  for (int i = 0; i < 20; i++)
    Serial.print(info.name[i]);
  Serial.println(" registrado en el Sistema.");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(info.name);
  lcd.setCursor(0,1);
  lcd.print("Registrado...");
  digitalWrite(ledGreen , HIGH);
  delay(3000);
  digitalWrite(ledGreen , LOW);
  
}
//Funcion para guardar nuevo usuario

void saveUser(MyStruct user) {
  for (int i = 0; i < 1024; i++) {
    byte address = EEPROM.read(i);
    if (address == 0) {
      EEPROM.put(i, user);
      return;
    } else {
      i += 23;

    }
  }
}

boolean viewUsers(byte num) {//Verifica la posicion del usuario a mostrar
  byte address = EEPROM.read(num);
  if (address != 0) {
    EEPROM.get(num, userDelete);
    return true;
  }
  return false;
}

void listaUsers(){//Imprime la lista de Usuarios registrados
  int cont = 1;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("UID: ");
      
  for (int i = 0; i < 1024; i++) {
    if (viewUsers(i)) {
      
      lcd.setCursor(0,0);
      lcd.print(userDelete.name);
      
      i--;
      Serial.print(cont); Serial.print(". ");
      Serial.print(" UID: ");
      for (int j = 0; j < 4; j++) {
        Serial.print(userDelete.UID[j],HEX);
        Serial.print(" ");   
      }
      lcd.setCursor(0,1);
      lcd.print(userDelete.UID[0],HEX);
      lcd.setCursor(3,1);
      lcd.print(userDelete.UID[1],HEX);
      lcd.setCursor(6,1);
      lcd.print(userDelete.UID[2],HEX);
      lcd.setCursor(9,1);
      lcd.print(userDelete.UID[3],HEX);
      
      Serial.print("Usuario: ");
      Serial.println(userDelete.name);
      cont++;
      i += 24;
      delay(3000);
    }
    
    else
      break;
  }  
}


void mostrarMemoria() {//Codigo extraido de: https://robots-argentina.com.ar/didactica/uso-de-la-eeprom-de-arduino/
  for (int direccion = 0; direccion < 1024; direccion++)
  {
    for (int columna = 0; columna < 10; columna++)
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
      if (direccion == 1024) break; // solo muestra hasta la direccion 1023
    }
    Serial.println(); // salto a la sig línea
    direccion--; // decrementa direccion para la siguiente linea
  }
}

void leerTarjeta() {

  delay(2500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Acerque");
  lcd.setCursor(0,1);
  lcd.print("La tarjeta...");
  boolean r = false;
  while (r == false) {
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() )
      delay(500);
    else
      r = true;
  }
  Serial.print("UID");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) { //vamos a ver el valor de UID de las tarjetas
    if (mfrc522.uid.uidByte[i] < 0X10) {
      Serial.print(" 0");
    } else {
      Serial.print(" ");
    }
    lcd.setCursor(0,1);
    lcd.print(mfrc522.uid.uidByte[0],HEX);
    lcd.setCursor(3,1);
    lcd.print(mfrc522.uid.uidByte[1],HEX);
    lcd.setCursor(6,1);
    lcd.print(mfrc522.uid.uidByte[2],HEX);
    lcd.setCursor(9,1);
    lcd.print(mfrc522.uid.uidByte[3],HEX);
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    info.UID[i] = mfrc522.uid.uidByte[i];
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
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(ledRed , OUTPUT);  //definir pin como salida
  pinMode(ledGreen , OUTPUT);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sistema control");
  lcd.setCursor(0,1);
  lcd.print(" De usuarios");
}

void loop() {
    
    if (opcion != '\n') {
    
    Serial.println("ºººººººººººººººººººººººººººººººººººººººººººººººººººººººººº\nIngrese la opcion");
    Serial.println("a. Validar Usuario");
    Serial.println("b. Ingresar Nuevo Usuario");
    Serial.println("c. Eliminar Usuario");
    Serial.println("d. Mostrar Usuarios");
    Serial.println("e. Vaciar Memoria");
    Serial.println(" ");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ingrese opcion:");
    delay(2000);
      for(int i = 0;i<6;i+=2){
        lcd.setCursor(0,0);
        lcd.print(opcionesArray[i%6]);
        lcd.setCursor(0,1);
        lcd.print(opcionesArray[i%6+1]);
        delay(500);
        lcd.clear();
      }
    }
    lcd.setCursor(0,0);
    lcd.print("Ingrese opcion:");
  while (Serial.available() == 0) {
  }
  opcion = Serial.read();
  boolean r = false;

  switch (opcion) {
    /*----------------------------------------------Validacion de usuarios-------------------------------------------*/
    case 'a':
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Validando");
      lcd.setCursor(0,1);
      lcd.print("Usuario...");
      
      Serial.println("Validación de usuario, acerque la tarjeta al lector RFID");
      leerTarjeta();
      delay(2000);
      Serial.println("");

      if (acceso(info.UID)) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Acceso");
        lcd.setCursor(0,1);
        lcd.print("Concedido...");
        digitalWrite(ledGreen , HIGH);  
        
        Serial.println("Acceso concedido");
        Serial.print("Bienvenido: ");
        Serial.println(memo2.name);
        delay(3000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Bienvenido");
        lcd.setCursor(0,1);
        lcd.print(memo2.name);
        delay(2000);
        digitalWrite(ledGreen , LOW);
      }
      else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Acceso");
        lcd.setCursor(0,1);
        lcd.print("Denegado...");
        Serial.println("Acceso denegado");
        digitalWrite(ledRed , HIGH);
        Serial.println("Desea registra un nuevo Usuario?");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Registrar User?");
        lcd.setCursor(0,1);
        lcd.print("a. Si    b. No");
        delay(500);
        Serial.println("a. Registrar nuevo Usuario");
        Serial.println("b. Continuar");
        Serial.println(" ");
        delay(1500);
        digitalWrite(ledRed , LOW);
        while (Serial.available() == 0) {}
        newUser = Serial.read();
        switch (newUser) {
          case 'a':
            Serial.println("ººººººººººººººººIngreso de nuevo usuarioºººººººººººººººººº");
            Serial.println("-----------Por favor acerque su tarjeta");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Acerque");
            lcd.setCursor(0,1);
            lcd.print(" la tarjeta...");
            leerTarjeta();
            delay(1500);
            saveName();
            saveUser(info);
            break;
          default:
            break;
        }
      }
      break;
    /*-----------------------------------------------Registro de usuarios----------------------------------------------*/
    case 'b':

      Serial.println("ººººººººººººººººIngreso de nuevo usuarioºººººººººººººººººº");
      Serial.println("-----------Por favor acerque su tarjeta");
      leerTarjeta();
      delay(1500);
      if (acceso(info.UID)) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Usuario ya");
        lcd.setCursor(0,1);
        lcd.print("Existente");
        digitalWrite(ledRed , HIGH);
        Serial.println("\nUsuario ya existente \n");
        delay(2000);
        digitalWrite(ledRed , LOW);
        break;
      }
      saveName();
      saveUser(info);
      break;
    /*----------------------------------------------Eliminacion de usuarios---------------------------------------------*/
    case 'c':
      Serial.println("ºººººººººººººººººººººEliminar Usuarioººººººººººººººººººººº");
      Serial.println("a. Por seleccion");
      Serial.println("b. Por contacto");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Eliminando");
      lcd.setCursor(0,1);
      lcd.print("Usuario...");
      while (Serial.available() == 0) {}
      deleteUs = Serial.read();
      switch (deleteUs) {
        /*-------------Eliminacion de usuarios por contacto----------------*/
        case 'b':
          Serial.println("Acerque la tarjeta ");
          
          leerTarjeta();
          delay(2000);
          Serial.println("");
          if (deleteUser(info.UID)){
            Serial.println("Usuario eliminado ");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Usuario");
            lcd.setCursor(0,1);
            lcd.print("Eliminado");
            digitalWrite(ledGreen , HIGH);
            delay(3000);
            digitalWrite(ledGreen , LOW);}
          else{
            Serial.println("Error al eliminar");
            digitalWrite(ledRed , HIGH);
            delay(3000);
            digitalWrite(ledRed , LOW);}
            
          break;
        /*--------------------------------------Eliminacion de usuarios por listas-------------------------*/
        case 'a':
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Seleccione");
          lcd.setCursor(0,1);
          lcd.print("el usuario...");
          delay(1000);
          int cont = 1;
          for (int i = 0; i < 1024; i++) {
            if (viewUsers(i)) {//Verifica la posicion del usuario y lo agrega a una estructura User Delete
              i--;
              Serial.print(cont); Serial.print(". ");
              Serial.print(" UID: ");
              for (int j = 0; j < 4; j++) {
                Serial.print(userDelete.UID[j]);
                Serial.print(" ");
              }
              Serial.print("Usuario: ");
              Serial.println(userDelete.name);
              cont++;
              i += 24;
              lcd.clear(); 
              lcd.setCursor(0,0);
              lcd.print(userDelete.name);
              
              delay(3000);
            }
        
            else
              break;
          }
          delay(1000);

          Serial.println("Ingrese el indice del usuario a Eliminar: ");
          while (Serial.available() == 0) {}
          deleteIndex = Serial.read();
          if (deleteIndex > 0){
            deleteUserTec(deleteIndex - 49); //Envia la posicion convirtiendo de ASCII a INT
            Serial.println("Usuario eliminado ");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Usuario");
            lcd.setCursor(0,1);
            lcd.print("Eliminado");
            digitalWrite(ledGreen , HIGH);
            delay(3000);
            digitalWrite(ledGreen , LOW);
            }
               
          else{
            Serial.println("Opcion invalida, intentelo de nuevo");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Opcion");
            lcd.setCursor(0,1);
            lcd.print("Invalida...");
            digitalWrite(ledRed , HIGH);
            delay(1500);
            digitalWrite(ledRed , LOW);
          }
          break;

        default:
          break;
      }


      break;
    /*----------------------------------------Lista de Usuarios ---------------------------------------*/
    case 'd':
    
      Serial.println("Lista de Usuarios del sistema: \n");
      
      listaUsers();
      delay(2000);
      break;
    /*----------------------------------------Formateo de memoria------------------------------------------*/
    case 'e':
      Serial.println("SEGURO QUE DESEA FORMATEAR LA MEMORIA? \n");
      Serial.println("y: Si");
      Serial.println("n: No");
      while (Serial.available() == 0) {}
      deleteUs = Serial.read();
      switch (deleteUs) {
        case 'y':
        Serial.println("FORMTEANDO MEMORIA");
        formateo();
        for(int i = 0; i<20;i++){
          delay(100);
          Serial.print("-");}
        Serial.println("100%");
        Serial.println("Memoria formateada correctamente");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Memoria");
        lcd.setCursor(0,1);
        lcd.print("Formateada...");
        digitalWrite(ledGreen , HIGH);
        delay(3000);
        digitalWrite(ledGreen , LOW);
        break;
      }
    default:
        
        Serial.println("Opcion invalida");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Opcion");
        lcd.setCursor(0,1);
        lcd.print("Invalida...");
        digitalWrite(ledRed , HIGH);
        delay(2000);
        digitalWrite(ledRed , LOW);
      break;

  }
  mfrc522.PICC_HaltA();

}
