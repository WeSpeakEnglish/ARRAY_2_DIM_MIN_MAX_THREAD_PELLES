#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdint.h>
#include <math.h>
#include "nortos.h"

#define CHANNELS_N      2   //channels of ADC collecting
#define BUFFER_SIZE     16// points for each ADC channel (collected in total)
#define STEP_AMP_MEAN   8 // how many points to average (steep control) (attack ) for each the channels
#define THRESHOLD_VALUE 500  //hihger apmlithude = increade Attenuation, lower = decrease ATT
#define MIDDLE_VALUE    2048  //cold be autocalibrated with muted input signal

volatile uint16_t bufferADC[BUFFER_SIZE][CHANNELS_N];    //buffer of ADC
volatile uint16_t position;                              //position inside the buffer

 int counterThread = 0;
 int counterMain = 0 ;
 int counterArray = 0;

 int amp1sin = 200;
 int amp2sin = 2000;

int module (int A){
 return (A < 0)? -A: A;
}
 int ApmControl(struct fParams * Cont){
     int amp1ch = 0;
     int amp2ch = 0;
     int amp1max = 0;
     int amp2max = 0;

     for (int i =0; i< STEP_AMP_MEAN; i++){
        if(Cont->IntVar){
            amp1ch = module(bufferADC[STEP_AMP_MEAN+i][0] - MIDDLE_VALUE);
            amp2ch = module(bufferADC[STEP_AMP_MEAN+i][1] - MIDDLE_VALUE);
        }
        else{
            amp1ch = module(bufferADC[i][0] - MIDDLE_VALUE);
            amp2ch = module(bufferADC[i][1] - MIDDLE_VALUE);
        }
    if (amp1ch > amp1max)amp1max = amp1ch;
    if (amp2ch > amp2max)amp2max = amp2ch;
     }
 if (amp1max > THRESHOLD_VALUE) amp1sin--;
 else amp1sin++;
 if (amp2max > THRESHOLD_VALUE) amp2sin--;
 else amp2sin++;

 printf ("\n amp1max: %d, amp1sin: %d;   amp2max: %d, amp2sin: %d\n", amp1max, amp1sin, amp2max, amp2sin);
 }

DWORD WINAPI DoStuff(LPVOID lpParameter)
{
    int counterThread = 0;

    uint16_t * pointerCH1 = &bufferADC[0][0];
    uint16_t * pointerCH2 = &bufferADC[0][1];
    char c;
    // The new thri<ead will start here
    for(;;){

    Sleep(1000);
     //fill with sin
     *pointerCH1 = MIDDLE_VALUE + (uint16_t)(float)amp1sin*sin((float)counterThread/2.0);
     pointerCH1+=2;
     counterArray++;
 //    pointerCH1++;
     if (pointerCH1 > &bufferADC[BUFFER_SIZE -1][0]) pointerCH1 = &bufferADC[0][0];

     *pointerCH2 = MIDDLE_VALUE + (uint16_t)(float)amp2sin*sin((float)counterThread/2.7 + 0.17);
     pointerCH2+=2;
     counterArray++;

     counterArray %= BUFFER_SIZE*CHANNELS_N;

     if(counterArray == BUFFER_SIZE){
            FP_push(ApmControl,&(struct fParams){0,0.0});
     }
     if(counterArray == 0){
            FP_push(ApmControl,&(struct fParams){1,0.0});
     }
 //    pointerCH1++;
     if (pointerCH2 > &bufferADC[BUFFER_SIZE -1][1]) pointerCH2 = &bufferADC[0][1];
    //printf("%d\n",counterThread++);

  //  printf("Addr: bufferADC[BUFFER_SIZE -1][0]: %d\n",(int)&bufferADC[BUFFER_SIZE -1][0]);
  //  printf("Addr: pointerCH1: %d\n\n",(int)pointerCH1);
  //  printf("Addr: bufferADC[BUFFER_SIZE -1][1]: %d\n",(int)&bufferADC[BUFFER_SIZE -1][1]);
  //  printf("Addr: pointerCH2: %d\n\n",(int)pointerCH2);

    for(int i = 0; i < CHANNELS_N; i++){
     printf("\nCH%d\n",i + 1);
       for (int j = 0; j < BUFFER_SIZE; j++){
        printf("%d  ",bufferADC[j][i]);
       }

    }
    printf("\n----------------------------------------\n");
    counterThread++;
  //  c = getc(stdin);
    }
    return 0;
}

int main()
{
    FP_QueueIni();
    // Create a new thread which will start at the DoStuff function
    HANDLE hThread = CreateThread(
        NULL,    // Thread attributes
        0,       // Stack size (0 = use default)
        DoStuff, // Thread start address
        NULL,    // Parameter to pass to the thread
        0,       // Creation flags
        NULL);   // Thread id
    if (hThread == NULL)
    {
        // Thread creation failed.
        // More details can be retrieved by calling GetLastError()
        return 1;
    }

    // Wait for thread to finish execution
  //
    printf("BUFFER bufferADC contain:\n");
    for(;;){
    Sleep(1000);
  //  printf("Main thread: #%d\n",counterMain++);

    FP_pull();
    }
    WaitForSingleObject(hThread, INFINITE);
    // Thread handle must be closed when no longer needed
    CloseHandle(hThread);

    return 0;
}
