/* Event driven I/O */

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

#include "io.h"

extern void *malloc();
extern void *realloc();
extern void *calloc();
extern void free();

extern int errno;

static PORT ports[1]={{ports,ports}};	/* Active ports */
static PORT *freeports[1];		/* Free ports */
static struct output *freeouts[1];	/* Free output queue items */
static int firstopen=0;			/* Set if we've inited */

/* Lock for all above variables */
static mutex_t io_mutex[1]={MUTEX_INITIALIZER};

/* Create a port for a file descriptor */

static PORT *doopen(fd)
 {
 PORT *port=(PORT *)alitem(freeports,sizeof(PORT));

 /** Per thread? They have to call portopen before creating threads. */
 if(!firstopen)
  {
  signal(SIGPIPE,SIG_IGN);
  firstopen=1;
  }

 port->fd=fd;
 fcntl(fd,F_SETFL,O_NONBLOCK);

 port->count=1;
 port->read=0;
 port->new=0;
 port->old=0; 
 port->mode=1;
 port->taken=0;
 port->iflush=0;
 port->has_reader=0;
 port->has_writer=0;
 if(!port->buf) port->buf=malloc(port->siz=1024);

 IZQUE(port->out);
 port->onew=0;
 port->oold=0;
 if(!port->obuf) port->obuf=malloc(port->osiz=1024);

 ENQUEF(ports,port);

 return port;
 }

PORT *portopen(fd)
 {
 PORT *port;
 mutex_lock(io_mutex);
 port=doopen(fd);
 mutex_unlock(io_mutex);
 return port;
 }

/* Close port */

void doclose(port)
PORT *port;
 {
 if(port->fd!=-1)		/* Already closed? */
  {
  cancel(port->read); port->read=0;
  while(!QEMPTY(port->out))
   {
   struct output *out=port->out->next;
   DEQUE(out);
   cancel(out->write); out->write=0;
   fritem(freeouts,out);
   }
  close(port->fd);
  port->fd= -1;
  }
 }

void portclose(port)
PORT *port;
 {
 mutex_lock(io_mutex);
 doclose(port);
 mutex_unlock(io_mutex);
 }

void dofree(port)
PORT *port;
 {
 if(!--port->count)
  fritem(freeports,DEQUE(port));
 }

void portfree(port)
PORT *port;
 {
 mutex_lock(io_mutex);
 dofree(port);
 mutex_unlock(io_mutex);
 }

void dodup(port)
PORT *port;
 {
 ++port->count;
 }

PORT *portdup(port)
PORT *port;
 {
 mutex_lock(io_mutex);
 dodup(port);
 mutex_unlock(io_mutex);
 return port;
 }

/* Handle read buffering mode */

static void bufread(port)
PORT *port;
 {
 switch(port->mode)
  {
  case 0: /* Raw mode */
   {
   /* Record amount of data */
   port->taken=port->new-port->old;

   /* Pass data to callback fuction. */
   cont2(port->read,port->buf+port->old,port->taken);
   port->read=0;
   break;
   }

  case 1: /* Line mode */
   { /* Line mode */
   char *f;
   if(f=memchr(port->buf+port->old,'\n',port->new-port->old))
    { /* We found a '\n'! */
    int len=f-(port->buf+port->old);

    /* Amount of data callback function will take */
    port->taken=len+1;

    /* If line ends in CR-LF, kill the CR too */
    if(f!=port->buf+port->old && f[-1]=='\r') f[-1]=0, --len;

    *f=0; /* Zero terminate the line */

    /* Call callback function */
    cont2(port->read,port->buf+port->old,len);
    port->read=0;
    }
   /* If we didn't get a full line, return to doread for more data. */
   break;
   }

  case 2: /* Block mode */
   { /* Block mode */
   if(port->new-port->old>=port->bksize)
    {
    /* We will take one block */
    port->taken=port->bksize;
    /* Call callback function */
    cont2(port->read,port->buf+port->old,port->taken);
    port->read=0;
    }
   break;
   }
  }
 }

/* Handle read event (called when port is enabled and there is read data
 * available).
 */

static void doread(port)
PORT *port;
 {
 int len;

 if(port->mode==-1)
  { /* Very raw mode */
  cont0(port->read);
  port->read=0;
  return;
  }

 if(!port->read) return;

 if(port->mode==3)
  { /* This is a listen port */
  PORT *newport;
  struct sockaddr_in addr;
  int len=sizeof(addr);
  newport=portopen(accept(port->fd,(struct sockaddr *)&addr,&len));
  cont1(port->read,newport);
  port->read=0;
  return;
  }

 /* Read as much data as is available and which can fit in our buffer */
 while((len=read(port->fd,port->buf+port->new,port->siz-port->new))>0)
  {
  /* Indicate amount read */
  port->new+=len;

  /* Expand our buffer if data can't fit in it */
  if(port->new==port->siz) port->buf=realloc(port->buf,port->siz*=2);

  /* Process the data we have */
  bufread(port);
  if(!port->read) break;	/* No more reads if we became disabled */
  }
 if(len==0) doclose(port);	/* If port closed */
 }

/* Set function to receive data from port */

void stportread(port,fn,len)
PORT *port;
TASK *fn;
int len;
 {
 mutex_lock(io_mutex);

 port->read=fn;

 /* Delete data which is no longer needed */
 port->old+=len;
 port->taken=0;
 if(port->old)
  {
  if(port->new!=port->old)
   memmove(port->buf,port->buf+port->old,port->new-port->old);
  port->new-=port->old;
  port->old=0;
  /* If we have data and we have taken data on last read, restart the port
     (serv() will cause another read() call to be made to see if the remaining
     data can be taken). */
  if(port->new && len) bufread(port);
  }

 mutex_unlock(io_mutex);
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
 port->bksize=size;
 port->mode=2;
 }

void stnomode(port)
PORT *port;
 {
 port->mode=-1;
 }

/* Handle write event (called by serv() when there is data to be written and
 * when select() says it's ok to write).
 */

static void dowrite(port)
PORT *port;
 {
 struct output *out;
 int len;
 while(port->oold!=port->onew || !QEMPTY(port->out))
  { /* While we have data */
  /* Merge data into output buffer- do not deque here.  Only deque when we
     have actually written the data (in this case, when the output buffer
     becomes completely empty */
  for(out=port->out->next;out!=(struct output *)port->out;out=out->next)
   if(out->len)
    if(port->osiz-port->onew>=out->len)
     {
     memcpy(port->obuf+port->onew,out->buf,out->len);
     port->onew+=out->len;
     out->len=0;
     }
    else break;
  if(port->oold!=port->onew)
   { /* The output buffer is not empty: write it */
   len=write(port->fd,port->obuf+port->oold,port->onew-port->oold);
   if(len>0)
    { /* Update by amount written */
    port->oold+=len;
    if(port->oold==port->onew)
     { /* We wrote the entire output buffer - acknowledge queued items */
     port->oold=port->onew=0;
     while(!QEMPTY(port->out))
      {
      out=port->out->next;
      if(!out->len)
       {
       cont0(out->write);
       out->write=0;
       fritem(freeouts,DEQUE(out));
       }
      }
     }
    /* Loop to try to write more data */
    }
   else if(errno==EPIPE)
    { /* Pipe error- the port closed on us */
    doclose(port);
    break;
    }
   else
    /* Wrote as much data as we could.  Have to wait till next time for more */
    break;
   }
  else
   { /* Write data directly from queue
        (items must be larger than output buffer) */
   out=port->out->next;
   len=write(port->fd,out->buf,out->len);	/* Write it */
   if(len>0)
    { /* Update by amount written */
    out->buf+=len;
    out->len-=len;
    if(!out->len)
     { /* Move to next buffer if we're done */
     cont0(out->write);
     out->write=0;
     fritem(freeouts,DEQUE(out));
     }
    /* Loop to try to write more data */
    }
   else if(errno==EPIPE)
    { /* Pipe error- the port closed on us */
    doclose(port);
    break;
    }
   else
    /* Wrote as much data as we could.  We'll be back when we can write more */
    break;
   }
  }
 }

/* Queue up write data.  Callback executed when data has been written. */

static void dofinalwrite(port,data,len,fn)
PORT *port;
char *data;
TASK *fn;
 {
 if(port->fd!=-1)
  {
  struct output *out=(struct output *)alitem(freeouts,sizeof(struct output));
  out->buf=data;
  out->len=len;
  out->write=fn;
  ENQUEB(port->out,out);
  }
 else cancel(fn);
 }

void finalwrite(port,data,len,fn)
PORT *port;
char *data;
TASK *fn;
 {
 mutex_lock(io_mutex);
 dofinalwrite(port,data,len,fn);
 mutex_unlock(io_mutex);
 }

/* Try to add data to output buffer.  If it doesn't fit, queue up write */

int portwrite(port,data,datalen,fn)
PORT *port;
char *data;
TASK *fn;
 {
 mutex_lock(io_mutex);
 if(port->fd==-1)
  {
  cancel(fn);
  mutex_unlock(io_mutex);
  return 0;
  }
 else if(port->osiz-port->onew<datalen || !QEMPTY(port->out))
  { /* Not enough space in buffer, or data is queued up in front of us.
       Enque our data and write it when we can */
  dofinalwrite(port,data,datalen,fn);
  mutex_unlock(io_mutex);
  return 0;
  }
 else
  { /* Data fits in output buffer */
  memcpy(port->obuf+port->onew,data,datalen);
  port->onew+=datalen;
  mutex_unlock(io_mutex);
  return 1;
  }
 }

/* Set up a socket for listening */

PORT *portlisten(listen_port)
 {
 PORT *port;
 int lstn;
 int rsvd=IPPORT_RESERVED-1;
 struct sockaddr_in addr;

 /* Create socket for listening */
 lstn=socket(AF_INET,SOCK_STREAM,0);
/* lstn=rresvport(&rsvd); (for special socket) */
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

/* Call select() and process any events */

int io()
 {
 PORT *p, *n;

 fd_set rdfds, wrfds, exfds;	/* fds to check */
 struct timeval tm;		/* Timeout value */
 struct itimerval tmrtn;
 int last;			/* Highest fd */
 int init;
 int tasks;;

 /* Build arguments for select: clear file descriptors */
 FD_ZERO(&rdfds); FD_ZERO(&wrfds); FD_ZERO(&exfds);

 mutex_lock(io_mutex);

 /* Set timeout */
 if((init=waittim())!=-1)
  {
  tm.tv_usec=init;
  tm.tv_sec=0;
  }
 else
  {
  tm.tv_sec=3600;
  tm.tv_usec=0;
  }
 
 tmrtn.it_interval.tv_usec=0;
 tmrtn.it_interval.tv_sec=0;
 tmrtn.it_value.tv_usec=init;
 tmrtn.it_value.tv_sec=3600;

 /* Find highest fd */
 last=0;

 /* Find fds which need to be checked */
 for(p=ports->next;p!=ports;p=p->next)
  {
  if(p->fd>last) last=p->fd;
/*  printf("%d:\n",p->fd); */
  if(p->read && !p->has_reader)
   {
/*    printf("selecton %d\n",p->fd); */
   FD_SET(p->fd,&rdfds);
   p->has_reader=1;
   p->reader=thread_self();
   dodup(p);
   if(p->fd>last) last=p->fd+1;
   }
  if((p->oold!=p->onew || !QEMPTY(p->out)) && !p->has_writer)
   {
   FD_SET(p->fd,&wrfds);
   p->has_writer=1;
   p->writer=thread_self();
   dodup(p);
   if(p->fd>last) last=p->fd+1;
   }
  /* we should up the open count on the port here */
  }

 mutex_unlock(io_mutex);

 /* Return if there was nothing to do. */
 if(!last && init==-1) return 0;

 doX();

 /* Check for events */
 setitimer(ITIMER_REAL,&tmrtn,NULL);
 select(last+1,&rdfds,&wrfds,&exfds,&tm);
/* printf("select returns\n"); */

 mutex_lock(io_mutex);

 tasks=0;

 /* Handle any timer events */
 if(init!=-1)
  {
  getitimer(ITIMER_REAL,&tmrtn);
  note(init-(tmrtn.it_value.tv_usec+(tmrtn.it_value.tv_sec-3600)*1000000));
  if(anyfns()) tasks=1;
  }

 /* Check which fds were returned by select() */
/* printf("thread=%d\n",thread_self()); */
 for(p=ports->next;p!=ports;p=n)
  {
  int flg=0;
  n=p->next;
  if(p->has_reader && p->reader==thread_self())/* thread_equal(p->reader,thread_self())) */
   {
   if(p->fd!=-1 && FD_ISSET(p->fd,&rdfds) /* && !tasks */)
    {
    doread(p);
    if(anyfns()) tasks=1;
    }
   p->has_reader=0;
   ++flg;
   }
  if(p->has_writer && p->writer==thread_self()) /* thread_equal(p->writer,thread_self())) */
   {
   if(p->fd!=-1 && FD_ISSET(p->fd,&wrfds) /* && !tasks */)
    {
    dowrite(p);
    if(anyfns()) tasks=1;
    }
   p->has_writer=0;
   ++flg;
   }
  while(flg--) dofree(p);
  }

 mutex_unlock(io_mutex);

 return 1;
 }
