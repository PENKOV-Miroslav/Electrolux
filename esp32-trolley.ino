#include <SPI.h> //permet de communiquer avec des périphériques SPI, avec la carte Arduino/ESP32 en tant que composant "maître"
#include <GxEPD.h> //import de la bibliotheque qui gere e-paper
//import des polices d'ecriture d'Adafruit_GFX permetant 
//#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
// sélection de la classe d'affichage de l'écran utiliser
#include <GxGDEW042T2/GxGDEW042T2.h>      // 4.2" b/w  

#include <GxIO/GxIO_SPI/GxIO_SPI.h> //import des classe permetants de gerer l'e-paper
#include <GxIO/GxIO.h>

// constructeur pour ESP32-WROOM-32 Dev KitC V4 et V2
GxIO_Class io(SPI, /*CS=*/ SS, /*DC=*/ 22, /*RST=*/ 16); 
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); 



//définition pour le deep sleep
#define uS_TO_S_FACTOR 1000000  /* Facteur de conversion des microsecondes en secondes */
#define TIME_TO_SLEEP 1800        /* Le temps où l'ESP32 s'endormira (en secondes) 1800 sec => 30minutes */

RTC_DATA_ATTR int bootCount = 0; //init compteur reboot deep sleep

#include "defines.h" //fichier definition des module WiFi
#include "Credentials.h" //fichier definition SSID et compte MySQL
#include <MySQL_Generic.h> //bibliotheque MySQL

#define USING_HOST_NAME     true

#if USING_HOST_NAME
  // A l'aide du nom d'hôte et de la recherche DNS intégrée on se connectera au serveur
  char server[] = "FRWS3005.epr.electroluxprofessional.com"; // changer le nom d'hôte/l'URL de votre serveur si besoin
#else
  IPAddress server(10, 16, 16, 14); //ardresse ip du serveur frws3005 si il n'arrive pas à joindre le serveur via l'URL
#endif

uint16_t server_port = 3306; //le port sur lequel les esp32 communique avec la bdd, envoient des requetes et reception des resultats

char default_database[] = "trolley";              
char default_table[]    = "trolleysensors";              

//variable de verification
unsigned long QUERY_VERIF = 800000;
//requete SQL faut changer le numero du id arduino pour chaque module ainsi on obtiens la bonne ligne
const char QUERY_TROLLEY[] = "SELECT ID_ARDUINO,NAME_TROLLEY AS TROLLEY,LIGNE,DATE_PROD AS DATE,LIVRE,CONSO,MAP,STATUT FROM trolley.trolleysensors WHERE ID_ARDUINO = '3';";

char query[128];
//initialisation objet de connexion
MySQL_Connection conn((Client *)&client);

void setup()
{
  display.init(); //initialisation du display
  Serial.begin(115200);
  while (!Serial && millis() < 5000); // attendre que le port série se connecte

  MYSQL_DISPLAY1("\nLancement du programme ESP32-trolley", BOARD_NAME);
  MYSQL_DISPLAY(MYSQL_MARIADB_GENERIC_VERSION);

  // Pensez à initialiser votre module WiFi
#if ( USING_WIFI_ESP8266_AT  || USING_WIFIESPAT_LIB ) 
  #if ( USING_WIFI_ESP8266_AT )
    MYSQL_DISPLAY("En utilisant la Bibliotheque ESP8266_AT/ESP8266_AT_WebServer");
  #elif ( USING_WIFIESPAT_LIB )
    MYSQL_DISPLAY("En utilisant la Bibliotheque WiFiEspAT");
  #endif
  
  // initialiser la sérial pour le module ESP
  EspSerial.begin(115200);
  // initialiser le module ESP WiFi
  WiFi.init(&EspSerial);

  MYSQL_DISPLAY(F("Bouclier WiFi init terminé"));

  // vérifier la présence du bouclier
  if (WiFi.status() == WL_NO_SHIELD)
  {
    MYSQL_DISPLAY(F("Bouclier Wi-Fi non présent"));
    // ne continuez pas
    while (true);
  }
#endif

  // Commencer la section Wi-Fi
  MYSQL_DISPLAY1("Connexion a : ", ssid);
  //Connexion au reseaux WiFi indiquer plus haut
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    MYSQL_DISPLAY0(".");
  }

  // imprimer les informations sur la connexion dans le Moniteur Serial
  MYSQL_DISPLAY1("Connecte au reseau. Mon adresse IP est :", WiFi.localIP());

  MYSQL_DISPLAY3("Connexion a SQL Server @", server, ", Port =", server_port);
  MYSQL_DISPLAY5("User =", user, ", PassWord =", password, ", DataBase =", default_database);
}




//Méthode qui va afficher les resultats de la requete SQL 
void runQuery(){

//init affichage e-paper et format
  display.update(); //on rafraichie le display pour ecrire la reponse de la requete
  display.setRotation(0);// la rotation du contenu et horizontale lorsque la valeur est de 0 pui vertical lorsque c'est 1
  display.setFont(&FreeMonoBold18pt7b); //utilisation d'un format de police qui gére l'utf-8
  display.setTextColor(GxEPD_BLACK); //display.setTextColor(GxEPD_WHITE); 
  int16_t tbx, tby; uint16_t tbw, tbh; //variable qui vont me permet de délimiter l'écran et structurer le contenu
  uint16_t x; //initialisation de l'abscisse du display
  uint16_t y; //initialisation de l'ordonnée du display
  // boîte englobante centrale par transposition de l'origine
   x =((display.width()-tbw)*0)+ tbx; //calcule le début suivant la largeur de l'ecran
   y =((display.height() -tbh)*0.1)+ tby; //calcule le début suivant la hauteur de l'ecran
  display.fillScreen(GxEPD_WHITE); //display.fillScreen(GxEPD_BLACK); 
  display.setCursor(x, y); //lance l'ecriture suivant x et y

  //variable nécessaire à la verification de la requête et à la bonne execution de celle-ci
  sprintf(query, QUERY_TROLLEY, QUERY_VERIF + (( millis() % 100000 ) * 10) );
  //affiche notre requête
  MYSQL_DISPLAY(query);
    // Lancer l'instance de  la classe de requête
  MySQL_Query query_mem = MySQL_Query(&conn);
  // Exécuter la requête et
  // vérifie si elle est valide avant de récupérer les informations
  if ( !query_mem.execute(query) )
  {
    MYSQL_DISPLAY("Erreur dans la requete");
    return;
  }

  // Lisez les lignes et imprimez-les
  row_values *row = NULL;
  // Récupérer les colonnes et les imprimer
  column_names *cols = query_mem.get_columns();
  //tant qu'il y a des colonnes et des lignes à affiche il va les stocké puis va les print sur le e-paper
  do 
  {
    row = query_mem.get_next_row();
    
    if (row != NULL) 
    {
      for (int f = 0; f < cols->num_fields; f++) 
      {
        //MYSQL_DISPLAY0(row->values[f]); //test affiche dans le serial le resultat
        display.print(cols->fields[f]->name);
        display.print(":");
        display.print(row->values[f]);
        
        if (f < cols->num_fields - 1) 
        {
          display.print("               ");

          //MYSQL_DISPLAY0(",");  //test affiche dans le serial le resultat
        }
      }

      display.println();
      //MYSQL_DISPLAY();  //test affiche dans le serial le resultat
    }
  } while (row != NULL);

}
//Méthode affichage error sur l'écran si n'arrive pas à ce connecter au serveur
void error() {
  const char errorSQL [] ="\nEchec de connexion. Réessayer à la prochaine itération. Si cela persiste contacter IT";
  display.update();
  display.setRotation(0);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  // boîte englobante centrale par transposition de l'origine:
  uint16_t x =((display.width()-tbw)*0)+ tbx; 
  uint16_t y =((display.height() -tbh)*0.1)+ tby;  
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(x, y);
  display.print(errorSQL);
}

/*
Méthode pour imprimer la raison pour laquelle ESP32
a été réveillé du sommeil suivant les cas suivant
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Reveil cause par un signal externe utilisant RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Reveil cause par un signal externe utilisant RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Reveil cause par la minuterie"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Reveil cause par le pavé tactile"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Reveil cause par le programme ULP"); break;
    default : Serial.printf("Le reveil n'a pas ete cause par un sommeil profond: %d\n",wakeup_reason); break;
  }
}


void loop() {
  //if (conn.connect(server, server_port, user, password)) //essayez si connectionNonBlocking ne marche pas
  if (conn.connectNonBlocking(server, server_port, user, password) != RESULT_FAIL)
  {
    //Serial.begin(115200); //init serial
    //delay(1000); //attendre 1 sec

    //runQuery(); ////test affiche dans le serial le resultat
    display.eraseDisplay();   //efface le contenu du e-paper 
    delay(1000);               //pour que par la suite il imprime les nouvelles informations
    display.drawPaged(runQuery);      //ecrit le resultat de la requete
    conn.close();                     // fermer la connexion à la BDD
    display.powerDown();              //eteind l'e-paper de maniere sécuriser 
    
  } 
  else 
  {
    display.drawPaged(error); //ecrit le message d'érreur
    delay(5000);              //va l'afficher pendant 5sec
    display.eraseDisplay();   //puis efface le contenu du display
    delay(1000);              //attend 1sec
    display.powerDown();      //va eteindre l'e-paper de maniere sécuriser
    //MYSQL_DISPLAY("\nEchec de connexion. Réessayer à la prochaine itération."); //test dans le terminal
  }

  //MYSQL_DISPLAY("\nDormant..."); //test dans le terminal
 // MYSQL_DISPLAY("================================================"); //test dans le terminal
 
  delay(1000);
  //Incrémentez le nombre de démarrage et l'imprime à chaque redémarrage dans le serial
  ++bootCount;
  Serial.println("nombre de demarrage: " + String(bootCount));

  //Imprimer la raison de réveil pour ESP32 dans le serial
  print_wakeup_reason();

  /*
  Nous configurons d'abord la source de réveil puis
  nous avons configuré notre ESP32 pour qu'il se réveille toutes les X minutes
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Configuration du ESP32 pour dormir " + String(TIME_TO_SLEEP/60) +
  " Minutes");


  Serial.println("Je vais aller dormir maintenant...");
  delay(1000); //attendre 1sec
  Serial.flush(); //vider le serial
  delay(500);   //attendre 0.5sec
  esp_deep_sleep_start(); //methode qui eteind l'esp pour economise de l'energie
}
