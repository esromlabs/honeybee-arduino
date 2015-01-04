#define HB_VERSION "HoneyBee Arduino interpreter version 0.0.1"
// outline of HoneyBee UltraMini TransmissionFormat version 0.0.1 
#define START_HBG "1_hbumtf_v0"
#define NODES_HBG "nodes:"
/*
  0|env|||
  1|high||1|
  2|led|13|| */
#define EDGES_HBG "edges:" 
/*
  0|0|1|msg|graph_init|
  1|1|2|set|| */
#define END_HBG "0_hbumtf"
/*
nodes follow this sequence
  id|name|io|value|process
edges follow this sequence
  id|from|to|type|name|alias/guard

*/

//#define DUBUG_BUILD true
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

typedef struct
{
  int from;
  int to;
  char type[4];
  char name[NAME_SIZE];
  char guard[PROCESS_SIZE];
} Edge;
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // serial inputString is complete

boolean intComplete = false;
int node_index = 0;
Node nodes[10];
int edge_index = 0;
Edge edges[10];

// hbumtf parser vars.
// state 0=outside_cmd, 1=loading, 2=nodes, 3=edges 
int state = 0;


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
      #ifdef DUBUG_BUILD
      debug("raw|"+inputString+"|state|"+state);
      #endif
    }
   else {
      // add it to the inputString:
      inputString += inChar;
   } 
  }
  if (!stringComplete) {return;}
  if (state == 0 && inputString == START_HBG) {
    next_state = 1;
    #ifdef DUBUG_BUILD
    debug("load|"+inputString);
    #endif
  }
  else if (state == 0) {
    #ifdef DUBUG_BUILD
    debug("outside_com|"+inputString);
    #endif
    if (inputString == "rfi") {
      com_rfi();
    }
  }
  if (state == 1 && inputString == NODES_HBG) {
    next_state = 2;
    state = -1;
  }
  if (state == 2 && inputString == EDGES_HBG) {
    next_state = 3;
    state = -1;
  }
  if (state == 3 && inputString == END_HBG) {
    next_state = 0;
    state = -1;
    com_graph();
  }
  // processing for states 2 and 3
  if (state == 2) {
    #ifdef DUBUG_BUILD
    debug("parsing node|"+inputString);
    #endif
    parse_node(inputString);
  }
  if (state == 3) {
    #ifdef DUBUG_BUILD
    debug("edge|"+inputString);
    #endif
    parse_edge(inputString);
  }
  if (next_state >= 0) {
    state = next_state;
  }
  inputString = "";
}

int crawlStr(char *result, int i, String inputString, char stopChar) {
  const unsigned int len = inputString.length();
  int j = 0;
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

void parse_node(String in_str) {
    int i = 0;
    int node_i;
    char temp[10];
    i = crawlStr(temp, i, in_str, '|');
    node_i = atoi(temp);
    
    i = crawlStr(nodes[node_i].name, i, in_str, '|');
    
    temp[0] = '\0';
    i = crawlStr(temp, i, in_str, '|');
    if (temp == "") {
      nodes[node_i].io = UNDEF_INT;
    }
    else {
      nodes[node_i].io = atoi(temp);
    }
    #ifdef DUBUG_BUILD
    Serial.print("io = ");
    Serial.print(nodes[node_i].io, DEC);
    Serial.print("|");
    #endif
    
    temp[0] = '\0';
    i = crawlStr(temp, i, in_str, '|');
    if (temp == "") {
      nodes[node_i].value = UNDEF_INT;
    }
    else {
      nodes[node_i].value = atoi(temp);
    }
    #ifdef DUBUG_BUILD
    Serial.print("value = ");
    Serial.print(nodes[node_i].value, DEC);
    Serial.println("|");
    #endif
    
    i = crawlStr(nodes[node_i].process, i, in_str, '|');
}

void parse_edge(String in_str) {
    int i = 0;
    int edge_i;
    char temp[10];
    i = crawlStr(temp, i, in_str, '|');
    edge_i = atoi(temp);
    
    temp[0] = '\0';
    i = crawlStr(temp, i, in_str, '|');
    if (temp == "") {
      edges[edge_i].from = UNDEF_INT;
    }
    else {
      edges[edge_i].from = atoi(temp);
    }
    #ifdef DUBUG_BUILD
    Serial.print("from = ");
    Serial.print(edges[edge_i].from, DEC);
    Serial.print("|");
    #endif
    
    temp[0] = '\0';
    i = crawlStr(temp, i, in_str, '|');
    if (temp == "") {
      edges[edge_i].to = UNDEF_INT;
    }
    else {
      edges[edge_i].to = atoi(temp);
    }
    #ifdef DUBUG_BUILD
    Serial.print("to = ");
    Serial.print(edges[edge_i].to, DEC);
    Serial.println("|");
    #endif

    i = crawlStr(edges[edge_i].type, i, in_str, '|');    
    i = crawlStr(edges[edge_i].name, i, in_str, '|');    
    i = crawlStr(edges[edge_i].guard, i, in_str, '|');
}


#ifdef DUBUG_BUILD
void debug(String out) {
  Serial.print("Debug: ");
  Serial.println(out);
}
#endif

void com_rfi() {
  Serial.println(HB_VERSION);
  Serial.print("Serial state: ");
  Serial.println(state);
  com_graph();
}

void com_graph() {
  int i;
  Serial.println(START_HBG);
  Serial.println(NODES_HBG);
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
  Serial.println(EDGES_HBG);
  for (i = 0; i < 10; i++) {
    if (edges[i].type[0]) {
      Serial.print(i);
      Serial.print("|");
      Serial.print(edges[i].from);
      Serial.print("|");
      Serial.print(edges[i].to);
      Serial.print("|");
      Serial.print(edges[i].type);
      Serial.print("|");
      Serial.print(edges[i].name);
      Serial.print("|");
      Serial.print(edges[i].guard);
      Serial.println("");
    }
  }
  Serial.println(END_HBG);
}
