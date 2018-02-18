#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

void main(){
        MYSQL *conn;
        MYSQL_RES *res;
        MYSQL_ROW row;
        char *server = "localhost";                            //혹은 ip
        char *user = "root";
        char *password = "dbqlznjxjtm1!";
        char *database = "Ubi";
        if( !(conn = mysql_init((MYSQL*)NULL))){        //초기화 함수
                printf("init fail\n");
                exit(1);
        }
        printf("mysql_init sucsess.\n");
        if(!mysql_real_connect(conn, server, user, password, NULL, 3306, NULL, 0)){
                printf("connect error.\n");     //DB접속 (MYSQL*, host, id, pw, null, port, 0)
                exit(1);
        }
        printf("mysql_real_connect suc.\n");
        if(mysql_select_db(conn, database) != 0){
                mysql_close(conn);
                printf("select_db fail.\n");
                exit(1);
        }
        printf("select mydb suc.\n");
 
        //printf("%d", mysql_query(conn,"select * from testtab" ));   //성공시 0리턴 (false)
        if(mysql_query(conn,"insert into Account(Id, Name, Phone) VALUES('111', '111','111')" )){
                printf("query fail\n");
                exit(1);
        }
        printf("query sucsess\n");
        res = mysql_store_result(conn);                 //쿼리에 대한 결과를 row에 저장
        printf("res suceese\n");
       /*
        while( (row=mysql_fetch_row(res))!=NULL){
                printf("%s %s\n", row[0], row[1]);       //이전과 같이 디비테이블을 만들었다면 id와 패스워드값이 나온다.
        }
		*/
        mysql_close(conn);
}
