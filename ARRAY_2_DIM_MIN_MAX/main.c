#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdint.h>
#include <math.h>

#define CHANNELS_N      2   //channels of ADC collecting
#define BUFFER_SIZE     16// points for each ADC channel (collected in total)
#define STEP_AMP_MEAN   4 // how many points to average (steep control) (attack ) for each the channels
#define THRESHOLD_VALUE 700  //hihger apmlithude = increade Attenuation, lower = decrease ATT
#define MIDDLE_VALUE    2048  //cold be autocalibrated with muted input signal

volatile uint16_t bufferADC[BUFFER_SIZE][CHANNELS_N];    //buffer of ADC
volatile uint16_t position;                              //position inside the buffer

 int counterThread = 0;
 int counterMain = 0 ;

DWORD WINAPI DoStuff(LPVOID lpParameter)
{
    int counterThread = 0;

    uint16_t * pointerCH1 = &bufferADC[0][0];
    uint16_t * pointerCH2 = &bufferADC[0][1];
    char c;
    // The new thri<ead will start here
    for(;;){

    Sleep(2000);
     //fill with sin
     *pointerCH1 = 2048 + (uint16_t)2048.0*sin((float)counterThread/4.0);
     pointerCH1+=2;
 //    pointerCH1++;
     if (pointerCH1 > &bufferADC[BUFFER_SIZE -1][0]) pointerCH1 = &bufferADC[0][0];
     *pointerCH2 = 2048 + (uint16_t)2048.0*sin((float)counterThread/4.0);
     pointerCH2+=2;
 //    pointerCH1++;
     if (pointerCH2 > &bufferADC[BUFFER_SIZE -1][0]) pointerCH2 = &bufferADC[0][0];
    //printf("%d\n",counterThread++);

    printf("Addr: bufferADC[BUFFER_SIZE -1][0]: %d\n",(int)&bufferADC[BUFFER_SIZE -1][0]);
    printf("Addr: pointerCH1: %d\n",(int)pointerCH1);
    printf("Addr: bufferADC[BUFFER_SIZE -1][0]: %d\n",(int)&bufferADC[BUFFER_SIZE -1][0]);
    printf("Addr: pointerCH1: %d\n",(int)pointerCH1);

    for(int i = 0; i < CHANNELS_N; i++){
     printf("\nCH%d\n",i + 1);
       for (int j = 0; j < BUFFER_SIZE; j++){
        printf("%d  ",bufferADC[j][i]);
       }

    }
    printf("\n----------------------------------------\n");
    counterThread++;
    c = getc(stdin);
    }
    return 0;
}

int main()
{
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
    Sleep(2000);
  //  printf("Main thread: #%d\n",counterMain++);

    }
    WaitForSingleObject(hThread, INFINITE);
    // Thread handle must be closed when no longer needed
    CloseHandle(hThread);

    return 0;
}
