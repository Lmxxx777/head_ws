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

// const char* session_begin_params =
//     "sub = iat, domain = iat, language = zh_cn, "
//     "accent = mandarin, sample_rate = 16000, "
//     "result_type = plain, result_encoding = utf8";

bool initialization(const char * userwords_path);
char * voiceRecorder(int mic1_file2 = 1, const char * file_path = "");
bool upload_userwords(const char * userwords_path);
void show_result(char *string, char is_over);
void on_result(const char *result, char is_last);
void on_speech_begin();
void on_speech_end(int reason);
bool demo_file(const char* audio_file, const char* session_begin_params);
bool demo_mic(const char* session_begin_params);

static char *g_result = NULL;
static unsigned int g_buffersize = BUFFER_SIZE;
