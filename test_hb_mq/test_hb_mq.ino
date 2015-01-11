
// test of the message queue 
#define HB_VERSION "HoneyBee Arduino interpreter version 0.0.1"
#define DUBUG_BUILD  true

// message queue
#define MQ_SIZE  5
short message_count = 0;
short message_in = 0;
short message_out = 0;
String messages[MQ_SIZE];

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
//  inputString.reserve(200);
  //pinMode(led, OUTPUT);     
  
}

void loop() {

  delay(10000);
  test_mq();

}

void test_mq() {
  enq_message("msg:graph_init 1");
  enq_message("msg:graph_init 2");
  enq_message("msg:graph_init 3");
  enq_message("msg:graph_init 4");
  enq_message("msg:graph_init 5");
  enq_message("msg:graph_init 6");
  enq_message("msg:graph_init 7");
  Serial.println("mq test: ");
  Serial.println(deq_message());
  Serial.println(deq_message());
  Serial.println(deq_message());
  Serial.println(deq_message());
  Serial.println(deq_message());
  Serial.println(deq_message());
  Serial.println(deq_message());
  Serial.println(deq_message());
  Serial.println(deq_message());
  Serial.println(deq_message());
}

void enq_message(String msg) {
  if (message_count < MQ_SIZE) {
    message_in = (message_in + 1) % MQ_SIZE;
    message_count++;
    messages[message_in] = msg;
  }
  #ifdef DUBUG_BUILD
  else {
    Serial.print("enq_message count error");
    Serial.println(msg);
  }
  #endif
}
String deq_message() {
  if (message_count > 0) {
    message_count--;
    message_out = (message_out + 1) % MQ_SIZE;
    return messages[message_out];
  }
  #ifdef DUBUG_BUILD
  else {
    Serial.print("deq_message count error");
    Serial.println(message_count);
  }
  #endif
  return "";
}


