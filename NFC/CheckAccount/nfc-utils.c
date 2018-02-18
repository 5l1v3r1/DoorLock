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
 * @file nfc-utils.c
 * @brief Provide some examples shared functions like print, parity calculation, options parsing.
 */
#include <nfc/nfc.h>
#include <string.h>
#include <err.h>
//#include <mysql/mysql.h>

#include "nfc-utils.h"

uint8_t
oddparity(const uint8_t bt)
{
  // cf http://graphics.stanford.edu/~seander/bithacks.html#ParityParallel
  return (0x9669 >> ((bt ^ (bt >> 4)) & 0xF)) & 1;
}

void
oddparity_bytes_ts(const uint8_t *pbtData, const size_t szLen, uint8_t *pbtPar)
{
  size_t  szByteNr;
  // Calculate the parity bits for the command
  for (szByteNr = 0; szByteNr < szLen; szByteNr++) {
    pbtPar[szByteNr] = oddparity(pbtData[szByteNr]);
  }
}

void
print_hex(const uint8_t *pbtData, const size_t szBytes)
{
  size_t  szPos;
  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}

void
print_hexs(char *str, const uint8_t *pbtData, const size_t szBytes)
{
	int off=0;
  size_t  szPos;
  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
	off+= snprintf(str + off, 1024 - off, "%02x ", pbtData[szPos]); 
  }
  printf("\n");

  //check_account_mysql(str);
}


void
print_hex_bits(const uint8_t *pbtData, const size_t szBits)
{
  uint8_t uRemainder;
  size_t  szPos;
  size_t  szBytes = szBits / 8;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }

  uRemainder = szBits % 8;
  // Print the rest bits
  if (uRemainder != 0) {
    if (uRemainder < 5)
      printf("%01x (%d bits)", pbtData[szBytes], uRemainder);
    else
      printf("%02x (%d bits)", pbtData[szBytes], uRemainder);
  }
  printf("\n");
}

/*
void check_account_mysql(const char* s){
  MYSQL *conn;
  MYSQL_RES *sql_res;
  MYSQL_ROW row;
  char *server = "localhost";                            
  char *user = "root";
  char *password = "dbqlznjxjtm1!";
  char *database = "Ubi";
  char *str;

  if (!(conn = mysql_init((MYSQL*)NULL))) {        //초기화 함수
	  printf("init fail\n");
	  exit(1);
  }
  printf("mysql_init sucsess.\n");
  if (!mysql_real_connect(conn, server, user, password, NULL, 3306, NULL, 0)) {
	  printf("connect error.\n");     //DB접속 (MYSQL*, host, id, pw, null, port, 0)
	  exit(1);
  }
  printf("mysql_real_connect suc.\n");
  if (mysql_select_db(conn, database) != 0) {
	  mysql_close(conn);
	  printf("select_db fail.\n");
	  exit(1);
  }
  printf("select mydb suc.\n");
	sprintf(str, "select * from Account WHERE Id = '%s'", s);

	if (mysql_query(conn, str)) {
		printf("query fail\n");
		exit(1);
	}

	printf("query sucsess\n");
	sql_res = mysql_store_result(conn);                 // 쿼리에 대한 결과를 row에 저장
	printf("res suceese\n");

	if((row=mysql_fetch_row(sql_res))!=NULL){			// 쿼리 값에 Id가 저장되어 있는 경우
		printf("In\n");
		//move_servo();
	}
	else{
		printf("Out\n");
	}
	mysql_close(conn);
}
*/

void
print_hex_par(const uint8_t *pbtData, const size_t szBits, const uint8_t *pbtDataPar)
{
  uint8_t uRemainder;
  size_t  szPos;
  size_t  szBytes = szBits / 8;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x", pbtData[szPos]);
    if (oddparity(pbtData[szPos]) != pbtDataPar[szPos]) {
      printf("! ");
    } else {
      printf("  ");
    }
  }

  uRemainder = szBits % 8;
  // Print the rest bits, these cannot have parity bit
  if (uRemainder != 0) {
    if (uRemainder < 5)
      printf("%01x (%d bits)", pbtData[szBytes], uRemainder);
    else
      printf("%02x (%d bits)", pbtData[szBytes], uRemainder);
  }
  printf("\n");
}

void
print_nfc_target(const nfc_target *pnt, bool verbose)
{  
  char *s;
  str_nfc_target(&s, pnt, verbose);
  printf("%s", s);
  nfc_free(s);
}

/*
void 
move_servo(){
	int num;
	if(wiringPiSetupGpio ()  ==  - 1 )  { 
    printf("Error"); 
    exit(1);
  }

  pinMode ( 18 ,  PWM_OUTPUT ); 
  pwmSetMode ( PWM_MODE_MS ); 
  pwmSetClock ( 400 ); 
  pwmSetRange ( 1024 );

  pwmWrite ( 18 ,  115 );
  delay(10000);
  pwmWrite ( 18 ,  25 );
  delay(1000);
}
*/