/*
 HB interpretor

outline of HoneyBee UltraMini TransmitionFormat version 0.0.1 
1_hbumtf_v0
nodes:
  0|env|||
  1|high||1|
  2|led|13||
edges:
  0|1|msg|graph_init|
  1|2|set||
0_hbumtf

nodes follow this sequence
  id|name|io|value|process
edges follow this sequence
  from|to|type|name|alias/guard

*/
#define NAME_SIZE  12
#define PROCESS_SIZE  12

typedef struct
{
  char name[NAME_SIZE];
  int io;
  int value;
  char process[PROCESS_SIZE];
} Node;
// Node n;
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

boolean intComplete = false;
int node_index = 0;
Node nodes[10];

// hbumtf parser vars.
// state 0=outside_cmd, 1=loading, 2=nodes, 3=edges 
int state = 0;
int node_i, edge_i;


void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  pinMode(led, OUTPUT);     

}

void loop() {
  hbumtfSerial();
}

/*
  hbumtfSerial
*/
void hbumtfSerial() {
  int next_state = -1;
  stringComplete = false;
  while (!stringComplete && Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      debug("raw|"+inputString+"|state|"+state);
    }
   else {
      // add it to the inputString:
      inputString += inChar;
   } 
  }
  if (!stringComplete) {return;}
  if (state == 0 && inputString == "1_hbumtf_v0") {
    next_state = 1;
    debug("load|"+inputString);
  }
  else if (state == 0) {
    debug("outside_cmd|"+inputString);
  }
  if (state == 1 && inputString == "nodes:") {
    node_i = 0;
    next_state = 2;
    //debug("state from|" + state + "|to|" + next_state);
  }
  if (state == 2 && inputString == "edges:") {
    edge_i = 0;
    next_state = 3;
  }
  if (state == 3 && inputString == "0_hbumtf") { next_state = 0;}
  // processing for states 2 and 3
  if (state == 2) {
    debug("parsing node|"+inputString);
    int i = 0;
    char temp[10];
    i = crawlStr(temp, i, inputString, '|');
    node_i = atoi(temp);
    
    i = crawlStr(nodes[node_i].name, i, inputString, '|');
    
    temp[0] = '\0';
    i = crawlStr(temp, i, inputString, '|');
    nodes[node_i].io = atoi(temp);
    
    temp[0] = '\0';
    i = crawlStr(temp, i, inputString, '|');
    nodes[node_i].value = atoi(temp);
    
    i = crawlStr(nodes[node_i].process, i, inputString, '|');

    debug(nodes[node_i].process);
  }
  if (state == 3) {
    debug("edge|"+inputString);
  }
  if (next_state >= 0) {
    state = next_state;
  }
  inputString = "";
}

int crawlStr(char *result, int i, String inputString, char stopChar) {
  const unsigned int len = inputString.length();
  int j = 0;
  //Serial.println(len, DEC);
  //Serial.println(i, DEC);
  while (i < len) {
    if (inputString[i] == stopChar) {
      return i+1;
    }
    result[j] = inputString[i];
    i++;
    j++;
  }
}

void debug(String out) {
  Serial.println(out);
}
