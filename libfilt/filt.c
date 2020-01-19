#include <u.h>
#include <libc.h>
#include <thread.h>
#include <mpipe.h>
#include <filt.h>

Mpipe *p;

void *
emalloc(ulong size)
{
        void *a;

        a = malloc(size);
        if(a == 0)
                sysfatal(0, "no memory");

        memset(a, 0, size);
        return a;
}


static void
mpout(void *v)
{
	Mpipe *p;
	Ioproc *io;
	int n, h, len;
	char *buf, *b;
	
	p = v;
	buf = emalloc(8192);
	n = 0;
	io = ioproc();
start:
	while((n = ioread(io, p->infd, buf+n, 8192)) > 0){
		b = buf;
		while(n > 0){
			h = p->sep(p, b, n, &len);
			if(len == -1){
				memmove(buf, b, n);
				goto start;
			}
			iowrite(io,p->fds[h], b, len);
			n -= len;
			b+=len;
		}
	}
	free(buf);
}
enum {
	Maxfd = 256,
};

Channel *c[Maxfd];

typedef struct Buffer Buffer;
struct Buffer {
	int n;
	char buf[1];
};

void
mpreader(void *v)
{
	int fd, n, len;
	Ioproc *io;
	char *buf, *bufbeg;
	Buffer *b;
	
	SET(n);

	fd = (int)v;
	io = ioproc();
	bufbeg = buf = malloc(8192);
	while((n = ioread(io, fd, buf+n, 8192)) > 0){
		while(p->sep(p, buf, n, &len) != -1){
			b = malloc(sizeof(Buffer)+len);
			b->n = len;
			memmove(b->buf, buf, len);
			sendp(c[fd], b);
			buf += len;
			n -= len;
		}
		buf = bufbeg;
	}
}

void
mpin(void *v)
{
	int i, n;
	Mpipe *p;
	Ioproc *io;
	Buffer *b;
	Alt *a;

	
	p = v;
	io = ioproc();
	a = emalloc(p->npipe * sizeof(Alt*));
	for(i = 0; i < p->npipe; i++){
		c[p->fds[i]] = chancreate(sizeof(Buffer*), 0);
		a[i].c = c[p->fds[i]];
		a[i].op = CHANRCV;
		threadcreate(mpreader, (void*)p->fds[i], 8192);
	}
	a[i+1].c = nil;
	a[i+1].op = CHANEND;
	for(;;){
		i = alt(a);
		b = recvp(a[i].c);
		n = iowrite(io, 1, b->buf, b->n);
		/* what about partial writes? */
		if(n < 0)
			break;
		free(b);
	}
	free(a);
}


void
threadmain(int argc, char **argv)
{
	int i;
	int *fds;
	
	ARGBEGIN{
	
	}ARGEND;
	
	fds = malloc(argc*sizeof(int));
	for(i = 0; i < argc; i++)
		fds[i] = atoi(argv[i]);
	p = emalloc(sizeof(Mpipe));
	p->fds = fds;
	p->npipe = argc;
	p->infd = 0;
	p->sep = filtmain;
	threadcreate(mpout, p, 8192);
	threadcreate(mpin, p, 8192);
}


