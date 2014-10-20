/* TCP/IP to Daemon interface (I/O multiplexer) */

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>

#include "queue.h"

#include "serv.h"

void *malloc();
void *realloc();
void *calloc();
void free();

extern int errno;

/* Output buffer */

struct output
 {
 QITEM(struct output);		/* Doubly-linked list of output buffers */
 char *buf;			/* Data */
 int len;			/* Length */
 int (*func)();			/* Function to call after data is sent */
 void *arg;			/* First arg to pass to func */
 };

/* A connected port */

struct connection
 {
 QBASE(struct output) out;	/* Output queue */
 int fd;			/* fd of socket attached to this port */
 int mode;			/* 0=raw mode, 1=line mode, 2=block mode, 3=
 				 * listen mode */
 int bksize;			/* Block mode block size */
 int iflush;			/* 1=flushing the input */
 int (*read)();			/* Function to call when line is received */
 void *readarg;
 int (*close)();		/* Function to call when port closes */
 void *closearg;
 char *buf;			/* Input buffer */
 int new;			/* New index */
 int old;			/* Old index */
 int siz;			/* Input buffer malloc size */
 };

static PORT **ports;		/* Array of ports */
static int nports;		/* Size of 'ports' */

static PORT *freeports=0;		/* Free ports */
static struct output *freeouts=0;	/* Free output buffer headers */

/* Initialize */

void izserv()
 {
 ports=calloc(sizeof(PORT *),(nports=32));
 signal(SIGPIPE,SIG_IGN);
 }

/* Create a port for a file descriptor */

PORT *portopen(fd)
 {
 PORT *port=(PORT *)alitem(&freeports,sizeof(PORT));
 if(!ports) izserv();
 if(fd>=nports) ports=realloc(ports,(nports=fd+32)*sizeof(PORT *));
 ports[fd]=port;
 port->fd=fd;
 port->close=0;
 port->read=0;
 port->new=0;
 port->old=0; 
 port->mode=1;
 port->iflush=0;
 if(!port->buf) port->buf=malloc(port->siz=1024);
 fcntl(fd,F_SETFL,O_NONBLOCK);
 IZQUE(&port->out);
 return port;
 }

/* Handle close event */

static void doclose(port)
PORT *port;
 {
 portclose(port);
 if(port->close) port->close(port->closearg);
 }

/* Handle read event - break input data into lines */

static void doread(port)
PORT *port;
 {
 int len;

 if(port->mode==3)
  { /* This is a listen port */
  PORT *newport;
  struct sockaddr_in addr;
  int len=sizeof(addr);
  newport=portopen(accept(port->fd,(struct sockaddr *)&addr,&len));
  if(port->read) port->read(port->readarg,newport);
  return;
  }

 /* Read as much data as is available and which can fit in our buffer */
 while((len=read(port->fd,port->buf+port->new,port->siz-port->new))>0)
  if(!port->iflush)
   {
   char *f;

   /* Indicate amount read */
   port->new+=len;

   /* Expand our buffer if data can't fit in it */
   if(port->new==port->siz) port->buf=realloc(port->buf,port->siz*=2);

   /* Process the data */
   loop:
   if(port->mode==0)
    { /* Raw mode */
    if(port->read)
     {
     /* Give data to callback function */
     int x=port->old;
     int y=port->new;
     int n=port->read(port->readarg,port->buf+port->old,port->new-port->old);
     /* Update by amount callback function took (but only if he didn't screw
      * with us) */
     if(x==port->old && y==port->new) port->old+=n;

     /* If it took some data or switched modes, loop */
     if(n || port->mode!=0) goto loop;
     }
    else
     port->new=port->old=0; /* Discard data if no callback function */
    }
   else if(port->mode==1)
    { /* Line mode */
    if(port->new!=port->old &&
       (f=memchr(port->buf+port->old,'\n',port->new-port->old)))
     { /* We found a '\n'! */
     int x=port->old;
     int len=f-(port->buf+port->old);

     /* If line ends in CR-LF, kill the CR too */
     if(f!=port->buf+port->old && f[-1]=='\r') f[-1]=0, --len;

     *f=0; /* Zero terminate the line */
     port->old=f-port->buf+1; /* Update pointer */
     if(port->read) port->read(port->readarg,port->buf+x,len);
     goto loop;
     }
    }
   else if(port->mode==2)
    { /* Block mode */
    if(port->new-port->old>=port->bksize)
     {
     int x=port->old;
     port->old+=port->bksize;
     if(port->read) port->read(port->readarg,port->buf+x,port->bksize);
     goto loop;
     }
    }

   /* Delete data which is no longer needed */
   if(port->old)
    {
    if(port->new!=port->old)
     memmove(port->buf,port->buf+port->old,port->new-port->old);
    port->new-=port->old;
    port->old=0;
    }
   }
 port->iflush=0;		/* No more flushing */
 if(len==0) doclose(port);	/* If port closed */
 }

/* Change mode */

void stlinemode(port)
PORT *port;
 {
 port->mode=1;
 }

void strawmode(port)
PORT *port;
 {
 port->mode=0;
 }

void stblockmode(port,size)
PORT *port;
 {
 port->mode=2;
 port->bksize=size;
 }

/* Handle write event */

static void dowrite(port)
PORT *port;
 {
 while(!QEMPTY(&port->out))
  { /* While we have data */
  struct output *out=port->out.next;
  int len=write(port->fd,out->buf,out->len);	/* Write it */
  if(len>0)
   { /* Update by amount written */
   out->buf+=len;
   out->len-=len;
   if(!out->len)
    { /* Move to next buffer if we're done */
    if(out->func) out->func(out->arg,0);
    fritem(&freeouts,DEQUE(out));
    }
   }
  else if(errno==EPIPE)
   { /* Pipe error if we tried to write to a closed port */
   doclose(port);
   break;
   }
  }
 }

/* Write data to a specific port */

void portwrite(port,data,len,func,arg)
PORT *port;
char *data;
int (*func)();
void *arg;
 {
 if(len)
  { /* Create outbuf buffer and enque it */
  struct output *out=(struct output *)alitem(&freeouts,sizeof(struct output));
  out->buf=data;
  out->len=len;
  out->func=func;
  out->arg=arg;
  ENQUEB(&port->out,out);
  }
 }

/* Close port */

void portclose(port)
PORT *port;
 {
 if(port->fd!=-1)
  {
  portoflush(port);
  ports[port->fd]=0;
  close(port->fd);
  port->fd= -1;
  fritem(&freeports,port);
  }
 }

/* Set function to receive data from port */

void stportread(port,func,arg)
PORT *port;
int (*func)();
void *arg;
 {
 port->read=func;
 port->readarg=arg;
 }

/* Set function which gets called when port is closed */

void stportclose(port,func,arg)
PORT *port;
int (*func)();
void *arg;
 {
 port->close=func;
 port->closearg=arg;
 }

/* Flush (discard) input data */

void portiflush(port)
PORT *port;
 {
 port->iflush=1;
 port->old=port->new=0;
 }

/* Flush (discard) output data */

void portoflush(port)
PORT *port;
 {
 while(!QEMPTY(&port->out)) fritem(&freeouts,DEQUE(port->out.next));
 }

/* Set up a socket for listening */

PORT *portlisten(listen_port,func,arg)
int (*func)();
void *arg;
 {
 PORT *port;
 int lstn;
 int rsvd=IPPORT_RESERVED-1;
 struct sockaddr_in addr;

 /* Create socket for listening */
/* lstn=socket(AF_INET,SOCK_STREAM,0);*/
 lstn=rresvport(&rsvd);
 if(lstn<0)
  {
  fprintf(stderr,"OOps\n");
  exit(1);
  }

 /* Build address */
 addr.sin_family=AF_INET;
 addr.sin_addr.s_addr=INADDR_ANY;
 addr.sin_port=htons(listen_port);

 /* Set address of socket */
 bind(lstn,(struct sockaddr *)&addr,sizeof(addr));

 /* Listen for new connections */
 listen(lstn,1);

 port=portopen(lstn);
 port->mode=3;
 stportread(port,func,arg);

 return port;
 }

/* Connect to a remote port */

PORT *portconnect(name,port)
char *name;
 {
 struct hostent *h=gethostbyname(name);
 struct sockaddr_in addr;
 int fd;
 if(!h) return 0;
 printf("Trying %s (%d.%d.%d.%d)...\n",h->h_name,
  ((unsigned char *)h->h_addr)[0],
  ((unsigned char *)h->h_addr)[1],
  ((unsigned char *)h->h_addr)[2],
  ((unsigned char *)h->h_addr)[3]);
 memset(&addr,0,sizeof(addr));
 addr.sin_family=h->h_addrtype;
 addr.sin_port=htons(port);
 memcpy(&addr.sin_addr.s_addr,h->h_addr,h->h_length);
 fd=socket(AF_INET,SOCK_STREAM,0);
 if(connect(fd,(struct sockaddr *)&addr,sizeof(addr)))
  {
  close(fd);
  return 0;
  }
 return portopen(fd);
 }

/* Connect to a remote port from root */

PORT *privconnect(name,port)
char *name;
 {
 struct hostent *h=gethostbyname(name);
 struct sockaddr_in addr;
 int rsvd=IPPORT_RESERVED-1;
 int fd;
 if(!h) return 0;
 printf("Trying %s (%d.%d.%d.%d)...\n",h->h_name,
  ((unsigned char *)h->h_addr)[0],
  ((unsigned char *)h->h_addr)[1],
  ((unsigned char *)h->h_addr)[2],
  ((unsigned char *)h->h_addr)[3]);
 memset(&addr,0,sizeof(addr));
 addr.sin_family=h->h_addrtype;
 addr.sin_port=htons(port);
 memcpy(&addr.sin_addr.s_addr,h->h_addr,h->h_length);
 /* Privilidged version of socket().  Can this be done with bind() instead? */
 for(rsvd=IPPORT_RESERVED-1;rsvd && (fd=rresvport(&rsvd))!=-1;--rsvd);
 if(connect(fd,(struct sockaddr *)&addr,sizeof(addr)))
  {
  close(fd);
  return 0;
  }
 return portopen(fd);
 }

void serv()
 {
 int x;
 int n;

 /* Event loop */
 for(;;)
  {
  fd_set rdfds, wrfds, exfds;	/* fds to check */
  struct timeval tm;		/* Timeout value */
  int last;			/* Highest fd */

  /* Build arguments for select: clear file descriptors */
  FD_ZERO(&rdfds); FD_ZERO(&wrfds); FD_ZERO(&exfds);

  /* Set timeout */
  tm.tv_sec=3600; tm.tv_usec=0;
  
  /* Find highest fd */
  last=0;

  /* Go through all ports.  Note highest fd and set fds to check */
  for(x=0;x!=nports;++x)
   {
   PORT *port;
   if(port=ports[x])
    {
    if(port->fd>last) last=port->fd;	/* Note highest fd value */
    FD_SET(port->fd,&rdfds);		/* Check for reading */
    if(!qempty(&port->out))		/* Check for writing if we have data */
     FD_SET(port->fd,&wrfds);
    }
   }

  /* Check for input */
  select(last+1,&rdfds,&wrfds,&exfds,&tm);

  /* Check which fds were returned by select() */
  for(x=0;x!=nports;++x)
   {
   PORT *port;
   /* Check if data available for reading */
   if((port=ports[x]) && FD_ISSET(port->fd,&rdfds)) doread(port);

   /* Check if we can write */
   /* doread() could have closed the port, so check only if port->fd is
      still not equal to -1 */
   if((port=ports[x]) && FD_ISSET(port->fd,&wrfds)) dowrite(port);
   }
  }
 }
