#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "speech_recognizer.h"

#define FRAME_LEN	640 
#define	BUFFER_SIZE	4096

bool initialization();
int upload_userwords();
void show_result(char *string, char is_over);
void on_result(const char *result, char is_last);
void on_speech_begin();
void on_speech_end(int reason);
void demo_file(const char* audio_file, const char* session_begin_params);
void demo_mic(const char* session_begin_params);

static char *g_result = NULL;
static unsigned int g_buffersize = BUFFER_SIZE;
