// hiredis-w32-sample.c
//   hiredis client for windows x64
//
//   https://blog.csdn.net/xumaojun/article/details/51558128
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hiredis/hiredis.h>

#define NO_QFORKIMPL

#include <Win32_Interop/win32fixes.h>

#pragma comment(lib, "hiredis.lib")
#pragma comment(lib, "Win32_Interop.lib")

int main()
{
	unsigned int j;
	redisContext *c;
	redisReply *reply;

	struct timeval timeout = { 3, 500000 }; // 3.5 seconds
	c = redisConnect("192.168.39.111", 7001);
	if (c->err) {
		printf("Connection error: %s\n", c->errstr);
		exit(1);
	}

	/* PING server */
	reply = (redisReply *)redisCommand(c, "PING");
	printf("PING: %s\n", reply->str);
	freeReplyObject(reply);

	/* Set a key */
	reply = (redisReply *)redisCommand(c, "SET %s %s", "foo", "hello world");
	printf("SET: %s\n", reply->str);
	freeReplyObject(reply);

	/* Set a key using binary safe API */
	reply = (redisReply *)redisCommand(c, "SET %b %b", "bar", 3, "hello", 5);
	printf("SET (binary API): %s\n", reply->str);
	freeReplyObject(reply);

	/* Try a GET and two INCR */
	reply = (redisReply *)redisCommand(c, "GET foo");
	printf("GET foo: %s\n", reply->str);
	freeReplyObject(reply);

	reply = (redisReply *)redisCommand(c, "INCR counter");
	printf("INCR counter: %lld\n", reply->integer);
	freeReplyObject(reply);
	/* again ... */
	reply = (redisReply *)redisCommand(c, "INCR counter");
	printf("INCR counter: %lld\n", reply->integer);
	freeReplyObject(reply);

	/* Create a list of numbers, from 0 to 9 */
	reply = (redisReply *)redisCommand(c, "DEL mylist");
	freeReplyObject(reply);
	for (j = 0; j < 10; j++) {
		char buf[64];

		sprintf_s(buf, 64, "%d", j);
		reply = (redisReply *)redisCommand(c, "LPUSH mylist element-%s", buf);
		freeReplyObject(reply);
	}

	/* Let's check what we have inside the list */
	reply = (redisReply *)redisCommand(c, "LRANGE mylist 0 -1");
	if (reply->type == REDIS_REPLY_ARRAY) {
		for (j = 0; j < reply->elements; j++) {
			printf("%u) %s\n", j, reply->element[j]->str);
			getchar();
		}
	}
	freeReplyObject(reply);

	return 0;
}
