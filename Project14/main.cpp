#include<stdio.h>
#include<winsqlite\winsqlite3.h>

#pragma comment(lib,"winsqlite3.lib")

static int SQLITE_CALLBACK callback(void *NotUsed, int argc, char **argv, char **azColName) {
	int i;

	for (i = 0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}


int main(int argc, char* argv[])
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open("testDB.db", &db);
	if (rc) 
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	//sqlite> CREATE TABLE COMPANY(
	//	ID INT PRIMARY KEY     NOT NULL,
	//	NAME           TEXT    NOT NULL,
	//	AGE            INT     NOT NULL,
	//	ADDRESS        CHAR(50),
	//	SALARY         REAL
	//);
	char *sql=(char*)"INSERT INTO\"COMPANY\"VALUES(1,'Rise',24,NULL,NULL);";
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) 
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	rc = sqlite3_exec(db, "select * from COMPANY;", callback, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	
	int row = 0, column = 0;//用于记录下面结果集中的行数和列数
	char **result;//二维数组用于存放结果
	rc = sqlite3_get_table(db, "select * from COMPANY;", &result, &row, &column, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	int i = 0;
	printf("row:%d column:%d\n", row, column);
	printf("\nThe result of querying is:\n");

	//输出查询的数据
	for (i = 0; i<(row + 1)*column; i++)
		printf("result[%d]=%s\n", i, result[i]);

	sqlite3_free_table(result);//释放result的内存空间


	getchar();
	sqlite3_close(db);

	return 0;
}