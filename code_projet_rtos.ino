#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

SemaphoreHandle_t  MutexSerial = NULL;

QueueHandle_t Queue1;
QueueHandle_t Queue2;
QueueHandle_t Queue3;
QueueHandle_t Queue4;

void Tache1(void*); void Tache2(void*); void Tache3(void*); void Tache4(void*); void Tache5(void*);
void printer(String);

struct val_capt 
{
  int analog;
  int num;
  unsigned long t_ms;
};

val_capt vals;
int tmp1, tmp2;
int val_num, val_analog;
int PIN_1 = 4;
int PIN_2 = 5;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_1, INPUT);
  pinMode(PIN_2, INPUT);

  MutexSerial = xSemaphoreCreateMutex();

  Queue1 = xQueueCreate(10, sizeof(int));
  Queue2 = xQueueCreate(10, sizeof(int));
  Queue3 = xQueueCreate(10, sizeof(val_capt));
  Queue4 = xQueueCreate(10, sizeof(val_capt));

  if(xTaskCreate(Tache1, "tache 1", 100,  NULL,  3  ,  NULL ) != pdPASS); 
  if(xTaskCreate(Tache2, "tache 2", 100,  NULL,  3  ,  NULL ) != pdPASS);
  if(xTaskCreate(Tache3, "tache 3", 100,  NULL,  3  ,  NULL ) != pdPASS); 
  if(xTaskCreate(Tache4, "tache 4", 100,  NULL,  3  ,  NULL ) != pdPASS); 
  if(xTaskCreate(Tache5, "tache 5", 200,  NULL,  3  ,  NULL ) != pdPASS); 
}

void loop() 
{
  // put your main code here, to run repeatedly:
}

void Tache1(void* p) 
{
  while (1) 
  {
    val_analog = analogRead(A0);
    xQueueSendToBack(Queue1, &val_analog, portMAX_DELAY );
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void Tache2(void* p) 
{
  while (1) 
  {
    if(digitalRead(PIN_1)==HIGH) tmp1 = 1; else tmp1 = 0;
    if(digitalRead(PIN_2)==HIGH) tmp2 = 1; else tmp2 = 0;
    val_num =  tmp1 + tmp2;
    xQueueSendToBack(Queue2, &val_num, portMAX_DELAY );
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void Tache3(void* p) 
{
  while (1) 
  {
    if ((xQueueReceive( Queue1, &vals.analog, portMAX_DELAY ) == pdTRUE) && (xQueueReceive( Queue2, &vals.num, portMAX_DELAY ) == pdTRUE)) 
    {
      vals.t_ms = millis();
      xQueueSendToBack(Queue3, &vals, portMAX_DELAY );
    }
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void Tache4(void* p) 
{
  val_capt val;
  while (1) 
  {
    if (xQueueReceive( Queue3, &val, portMAX_DELAY ) == pdTRUE) 
    {
      xQueueSendToBack(Queue4, &val, portMAX_DELAY );
      printer((String)("analogique : ") + val.analog + (String)("\nnumerique : ") + val.num + (String)("\ntemps : ") + val.t_ms + (String)("\n"));
    }
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void Tache5(void* p) 
{
  while (1) 
  {
    if ((xQueueReceive( Queue4, &vals, portMAX_DELAY ) == pdTRUE)) 
    {
      printer((String)("temps en minutes: ") + (vals.t_ms/60000)+ (String)(" min\n"));
    }
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void printer(String s) 
{
  if( xSemaphoreTake(MutexSerial, portMAX_DELAY) == pdTRUE)
  {
    Serial.print(s);
    xSemaphoreGive(MutexSerial); // release mutex
  } 
}
