/*-
 * Free/Libre Near Field Communication (NFC) library
 *
 * Libnfc historical contributors:
 * Copyright (C) 2009      Roel Verdult
 * Copyright (C) 2009-2013 Romuald Conty
 * Copyright (C) 2010-2012 Romain Tartière
 * Copyright (C) 2010-2013 Philippe Teuwen
 * Copyright (C) 2012-2013 Ludovic Rousseau
 * See AUTHORS file for a more comprehensive list of contributors.
 * Additional contributors of this file:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1) Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *  2 )Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Note that this license only applies on the examples, NFC library itself is under LGPL
 *
 */

/**
 * @file nfc-poll.c
 * @brief Polling example
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif // HAVE_CONFIG_H

#include <err.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <mysql/mysql.h>
#include <time.h>

#include <nfc/nfc.h>
#include <nfc/nfc-types.h>

#include "nfc-utils.h"

#define MAX_DEVICE_COUNT 16

static nfc_device *pnd = NULL;
static nfc_context *context;

  


static void stop_polling(int sig)
{
  (void) sig;
  if (pnd != NULL)
    nfc_abort_command(pnd);
  else {
    nfc_exit(context);
    exit(EXIT_FAILURE);
  }
}

static void
print_usage(const char *progname)
{
  printf("usage: %s [-v]\n", progname);
  printf("  -v\t verbose display\n");
}

void 
move_servo(){
  pwmWrite ( 18 ,  115 );
  delay(10000);
  pwmWrite ( 18 ,  25 );
  delay(1000);
}


void check_account_mysql(const char* str){
  char *s;
  
  MYSQL *conn;
  MYSQL_RES *sql_res;
  MYSQL_ROW row;
  char *server = "localhost";                            
  char *user = "root";
  char *password = "dbqlznjxjtm1!";
  char *database = "Ubi";
  struct tm *today;
  time_t ltime;


  // 쿼리 세팅
 if (!(conn = mysql_init((MYSQL*)NULL))) {        //초기화 함수
	  printf("init fail\n");
  }
  printf("mysql_init sucsess.\n");
  if (!mysql_real_connect(conn, server, user, password, NULL, 3306, NULL, 0)) {
	  printf("connect error.\n");     //DB접속 (MYSQL*, host, id, pw, null, port, 0)
  }
  printf("mysql_real_connect suc.\n");
  if (mysql_select_db(conn, database) != 0) {
	  mysql_close(conn);
	  printf("select_db fail.\n");
  }

  
  printf("select mydb suc.\n");
	sprintf(s, "select * from Account WHERE Id = '%s'", str);

	if (mysql_query(conn, s)) {
		printf("query fail\n");
		return;
	}

	printf("query sucsess\n");
	sql_res = mysql_store_result(conn);                 // 쿼리에 대한 결과를 row에 저장
	printf("res suceese\n");
	
	time(&ltime);
	today = localtime(&ltime);
	if((row=mysql_fetch_row(sql_res))!=NULL)
	{			// 쿼리 값에 Id가 저장되어 있는 경우
		printf("In\n");
		sprintf(s, "insert into Log(Id, Date, Open_NFC, Error) VALUES('%s', '%s', '%d', '%d')",str, asctime(today), 1, 0);
		
		if (mysql_query(conn, s)) {
			printf("query fail\n");
		}
		move_servo();
	}
	else{
		printf("Out\n");
		sprintf(s, "insert into Log(Id ,Date, Open_NFC, Error) VALUES('%s', '%s', '%d', '%d')", str, asctime(today), 1, 1);
		if (mysql_query(conn, s)) {
			printf("query fail\n");
		}
	}
	mysql_close(conn);
}

int
main(int argc, const char *argv[])
{
  bool verbose = false;
  int num;
  char *str;

 // Pin 세팅
  if(wiringPiSetupGpio ()  ==  - 1 )  { 
    printf("Error"); 
    exit(1);
  }
 
  pinMode ( 18 ,  PWM_OUTPUT ); 
  pwmSetMode ( PWM_MODE_MS ); 
  pwmSetClock ( 400 ); 
  pwmSetRange ( 1024 );
 
 

 // NFC 세팅
  signal(SIGINT, stop_polling);

  // Display libnfc version
  const char *acLibnfcVersion = nfc_version();

  printf("%s uses libnfc %s\n", argv[0], acLibnfcVersion);
  if (argc != 1) {
    if ((argc == 2) && (0 == strcmp("-v", argv[1]))) {
      verbose = true;
    } else {
      print_usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  const uint8_t uiPollNr = 20;
  const uint8_t uiPeriod = 2;
  const nfc_modulation nmModulations[5] = {
    { .nmt = NMT_ISO14443A, .nbr = NBR_106 },
    { .nmt = NMT_ISO14443B, .nbr = NBR_106 },
    { .nmt = NMT_FELICA, .nbr = NBR_212 },
    { .nmt = NMT_FELICA, .nbr = NBR_424 },
    { .nmt = NMT_JEWEL, .nbr = NBR_106 },
  };
  const size_t szModulations = 5;

  nfc_target nt;
  int res = 0;

	nfc_init(&context);
	if (context == NULL) {
			ERR("Unable to init libnfc (malloc)");
			exit(EXIT_FAILURE);
		  }

		  pnd = nfc_open(context, NULL);

		  if (pnd == NULL) {
			ERR("%s", "Unable to open NFC device.");
			nfc_exit(context);
			exit(EXIT_FAILURE);
		  }

		  if (nfc_initiator_init(pnd) < 0) {
			nfc_perror(pnd, "nfc_initiator_init");
			nfc_close(pnd);
			nfc_exit(context);
			exit(EXIT_FAILURE);
		  }

		 
			pnd = nfc_open(context, NULL);

		  if (pnd == NULL) {
			ERR("%s", "Unable to open NFC device.");
			nfc_exit(context);
			exit(EXIT_FAILURE);
		  }

		  if (nfc_initiator_init(pnd) < 0) {
			nfc_perror(pnd, "nfc_initiator_init");
			nfc_close(pnd);
			nfc_exit(context);
			exit(EXIT_FAILURE);
		  }
	while(1)
	{
		   printf("NFC reader: %s opened\n", nfc_device_get_name(pnd));
		  printf("NFC device will poll during %ld ms (%u pollings of %lu ms for %" PRIdPTR " modulations)\n", (unsigned long) uiPollNr * szModulations * uiPeriod * 150, uiPollNr, (unsigned long) uiPeriod * 150, szModulations);
		  if ((res = nfc_initiator_poll_target(pnd, nmModulations, szModulations, uiPollNr, uiPeriod, &nt))  < 0) {
			nfc_perror(pnd, "nfc_initiator_poll_target");
			//nfc_close(pnd);
			//nfc_exit(context);
			continue;
		  }

		  if (res > 0) {
			  str = malloc(1024);
			switch (nt.nm.nmt) {
			  case NMT_ISO14443A:
				  print_hexs(str, nt.nti.nai.abtAts, nt.nti.nai.szAtsLen);
				break;
			  case NMT_FELICA:
				  print_hexs(str, nt.nti.nfi.abtId, 8);
				break;
			  case NMT_ISO14443B:
				  print_hexs(str, nt.nti.nbi.abtPupi, 4);
				break;
			  case NMT_ISO14443BI:
				  print_hexs(str, nt.nti.nii.abtAtr, nt.nti.nii.szAtrLen);
				break;
			  case NMT_ISO14443B2SR:
				  print_hexs(str, nt.nti.nsi.abtUID, 8);
				break;
			  case NMT_ISO14443B2CT:
				  print_hexs(str, nt.nti.nci.abtUID, sizeof(nt.nti.nci.abtUID));
				break;
			  case NMT_DEP:
				  print_hexs(str, nt.nti.ndi.abtNFCID3, 10);
				break;
			}
			
			check_account_mysql(str);

		  } else {
			printf("No target found.\n");
			continue;
		  }
		  //printf("Waiting for card removing...");
		while (0 == nfc_initiator_target_is_present(pnd, NULL)) {}
			nfc_perror(pnd, "nfc_initiator_target_is_present");
		printf("done.\n");
		free(str);
	  
		}
		nfc_close(pnd);
		nfc_exit(context);
}
