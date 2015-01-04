#define HB_VERSION "HoneyBee Arduino interpreter version 0.0.1"
/*
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
#define UNDEF_INT -32768
#define NAME_SIZE  12
#define PROCESS_SIZE  12

typedef struct
{
  char name[NAME_SIZE];
  int io;
  int value;
  char process[PROCESS_SIZE];
} Node;
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // serial inputString is complete

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
    if (inputString == "rfi") {
      Serial.println(HB_VERSION);
      Serial.println(state);
      debugAllNodes();
    }
  }
  if (state == 1 && inputString == "nodes:") {
    node_i = 0;
    next_state = 2;
    state = -1;
    //debug("state from|" + state + "|to|" + next_state);
  }
  if (state == 2 && inputString == "edges:") {
    edge_i = 0;
    next_state = 3;
    state = -1;
  }
  if (state == 3 && inputString == "0_hbumtf") {
    next_state = 0;
    state = -1;
    debugAllNodes();
  }
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
    if (temp == "") {
      nodes[node_i].io = UNDEF_INT;
    }
    else {
      nodes[node_i].io = atoi(temp);
    }
    Serial.print("io = ");
    Serial.print(nodes[node_i].io, DEC);
    Serial.print("|");
    
    temp[0] = '\0';
    i = crawlStr(temp, i, inputString, '|');
    if (temp == "") {
      nodes[node_i].value = UNDEF_INT;
    }
    else {
      nodes[node_i].value = atoi(temp);
    }
    Serial.print("value = ");
    Serial.print(nodes[node_i].value, DEC);
    Serial.println("|");
    
    i = crawlStr(nodes[node_i].process, i, inputString, '|');
  
    //debug("process = " + nodes[node_i].process);
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
      result[j] = '\0';
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

void debugAllNodes() {
  int i;
  Serial.println("all nodes:");
  for (i = 0; i < 10; i++) {
    if (nodes[i].name[0]) {
      Serial.print(i);
      Serial.print("|");
      Serial.print(nodes[i].name);
      Serial.print("|");
      Serial.print(nodes[i].io);
      Serial.print("|");
      Serial.print(nodes[i].value);
      Serial.println("");
    }
  }
  

}
