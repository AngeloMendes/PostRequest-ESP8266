#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "RTClib.h"
#include <String.h>

#define TERMOMETRO 5
#define DEBUG true
#define id 1

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
RTC_DS1307 rtc;

typedef struct {
  int sensor;
  double valor;
  String horario;
  String data;
} Sensor;


int i = 0;
String horario;
String data;
double valorTermometro;
Sensor leitura[10];

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);// se for usar arduino uno, leia sobre a lib Software Serial
  delay(150);
  sendData("AT+RST\r\n","ready", 2000); 
  while(Serial.find("ready")){
    sendData("AT+RST\r\n","ready", 2000);
  }
  sendData("AT+CWMODE=1\r\n","no change", 2000); 
  while(Serial.find("no change")){
  sendData("AT+CWMODE=1\r\n","no change", 2000); 
  }  
  sendData("AT+CIPMODE=0\r\n","OK",1000);  
  while(Serial.find("OK")){
  sendData("AT+CIPMODE=0\r\n","OK",1000);  
  }  
  sendData("AT+CIPMUX=0\r\n","OK",1000); 
  while(Serial.find("OK")){
  sendData("AT+CIPMUX=0\r\n","OK",1000); 
  }  
  sendData("AT+CWJAP=\"SSID\",\"PASSWORD\"\r\n","OK",10000); 
  while(Serial.find("OK")){
  sendData("AT+CWJAP=\"SSID\",\"PASSWORD\"\r\n","OK",10000); 
  }
  mlx.begin();  
}

void loop() {
 for(i=0;i<1;i++){
  lerTermometro();
  }   
   geraXML();
   i=0; 
}


void enviarLeitura(char* xml){
  sendData("AT+CIPSTART=\"TCP\",\"192.168.0.104\",80\r\n","Linked",10000); // abrir conexão com o servidor
  char registro[2500] = "POST \/meteo\/insere.php HTTP\/1.1\r\nHost: 192.168.0.104\r\nContent-Type: application\/x-www-form-urlencoded\r\nContent-Length: "; //montagem da string com os dados
  char complemento[2500];
  String tamanhoXML = (String) strlen(xml);
  tamanhoXML.toCharArray(complemento, sizeof(complemento));
  concatenar(registro,complemento, sizeof(complemento));
  strcpy(complemento,"\r\n\r\n");
  concatenar(registro,complemento, sizeof(complemento));
  strcpy(complemento,"xml=");
  concatenar(registro,complemento, sizeof(complemento));
  strcpy(complemento,xml);
  concatenar(registro,complemento, sizeof(complemento));
  strcpy(complemento,"\r\n\r\n");
  concatenar(registro,complemento, sizeof(complemento));
  char sendCIP[50] = "AT+CIPSEND=";
  String tamanhoRegistro = (String) strlen(registro);
  tamanhoRegistro.toCharArray(complemento,sizeof(complemento));
  concatenar(sendCIP,complemento, sizeof(complemento));
  strcpy(complemento,"\r\n");
  concatenar(sendCIP,complemento, sizeof(complemento));
  sendData(sendCIP,">",1000);   //  vou mandar xx bytes...
  sendData(registro,"OKOKOK",100000);
  
}

void gravar(int codSensor, double valor, String horario, String data) {
  leitura[i].sensor = codSensor;
  leitura[i].valor = valor;
  leitura[i].horario = horario;
  leitura[i].data = data;
}

void lerTermometro(){ //leitura da temperatura
    valorTermometro=mlx.readAmbientTempC();
    horario = lerHora(); 
    data = lerData();
    gravar(TERMOMETRO,valorTermometro,horario,data);    
}
String lerHora(){ //leitura do horario
  DateTime now = rtc.now();
  leitura[i].horario=(String) now.hour();
  leitura[i].horario+=":";
  leitura[i].horario+=(String) now.minute();
  leitura[i].horario+=":";    
  leitura[i].horario+=(String) now.second();
  delay(1000); 
  return leitura[i].horario;
    
}  
String lerData(){// leitura da data
 DateTime now = rtc.now();
 leitura[i].data=(String) now.year();
 leitura[i].data+="-";  
 leitura[i].data+=(String) now.month();
 leitura[i].data+="-";
 leitura[i].data+=(String) now.day();
 return leitura[i].data;
  
}

String sendData(String command, String fimResposta, const int timeout){// função genenérica para escrita dos comandos ATs na ESP
                                                                       // disponivel em http://allaboutee.com/2014/12/30/esp8266-and-arduino-webserver/
    if(DEBUG){
      Serial.print("\r\nEnviado: ");
      Serial.println(command);
      Serial.print("Resposta: ");
    }
    String response = "";
    Serial1.print(command); 
    long int time = millis();
    while( (time+timeout) > millis() && (response.indexOf(fimResposta)==-1)){
        while(Serial1.available()) { 
        char c = Serial1.read();
        response+=c;
        if(DEBUG)Serial.print(c);
      } 
    }
    return response;
}

void geraXML (){ // montagem do xml com os dados 
 
  char xml[3500]="";
  char complemento[2500]="";
 
  
   strcpy(xml,"\n\<'?xml' version=\'1.0\' encoding=\'UTF-8\'\?\>\n");
   strcpy(complemento,"<estacao>\n");
   concatenar(xml,complemento, sizeof(complemento));
  
   for(i=0;i<1;i++){
    strcpy(complemento,"<registro>\n");
    concatenar(xml,complemento, sizeof(complemento));
     strcpy(complemento,"<id>\n");
    concatenar(xml,complemento, sizeof(complemento));
    String auxID = (String) id;
    auxID.toCharArray(complemento,sizeof(complemento));
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"\n<\\id>\n");
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"<sensor>\n");
    concatenar(xml,complemento, sizeof(complemento));
    String auxSensor = (String) leitura[i].sensor;
    auxSensor.toCharArray(complemento,sizeof(complemento));
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"\n<\\sensor>\n");
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"<data>\n");
    concatenar(xml,complemento, sizeof(complemento));
    leitura[i].data.toCharArray(complemento,sizeof(complemento));
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"\n<\\data>\n");
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"<hora>\n");
    concatenar(xml,complemento, sizeof(complemento));
    leitura[i].horario.toCharArray(complemento,sizeof(complemento));
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"\n<\\hora>\n");
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"<valorLido>\n");
    concatenar(xml,complemento, sizeof(complemento));
    String auxValor = (String) leitura[i].valor;
    auxValor.toCharArray(complemento,sizeof(complemento));
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"\n<\\valorLido>\n");
    concatenar(xml,complemento, sizeof(complemento));
    strcpy(complemento,"<\\registro>\n");
    concatenar(xml,complemento, sizeof(complemento));
       
  }
   strcpy(complemento,"<\\estacao>\n");
   concatenar(xml,complemento, sizeof(complemento));
   strcpy(complemento,"fim\n");// não entendi pq, mas os ultimos 3 caracteres são ignorados na string xml
                               // independente de do tamanho do xml, sempre os 3 ultimos são ignorados, estou estudando o porquê
   concatenar(xml,complemento, sizeof(complemento));
   
   enviarLeitura(xml);

}

void concatenar(char* base, char* acrescimo, int tamAcres){// funcao para concatenar os dados sem ocorrer overflow ou fragmentacao
                                                           // strcat e atribuicao simples (char = char + complemento) deram problemas
  int i;
  int tamBase = 0;
  while(base[tamBase]!='\0')tamBase++;
  for(i=0; i < tamAcres; i++){
     base[tamBase+i] =  acrescimo[i];  
  }
}
