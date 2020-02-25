#ifndef __PUBLIC_H__
#define __PUBLIC_H__

enum TYPE {
	UPLOAD = 1,
	DOWNLOAD,
	COMMAND
};

#define TEXTLEN 1024
typedef struct msg {
	int type;
	char text[TEXTLEN];
} msg_t;

#endif
