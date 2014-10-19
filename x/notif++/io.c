/* Event driven I/O */

#include <stdlib.h>
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

extern int errno;

Link<Port> ports;	// Active ports
static int firstopen=0;			/* Set if we've inited */

/* Lock for all above variables */
// static mutex_t io_mutex[1]={MUTEX_INITIALIZER};

/* Create a port for a file descriptor */

Port::Port(int new_fd)
  {
  /** Per thread? They have to call portopen before creating threads. */
  if(!firstopen)
    {
    signal(SIGPIPE,SIG_IGN);
    firstopen=1;
    }

  fd=new_fd;
  fcntl(fd,F_SETFL,O_NONBLOCK);

  count=1;
  read=0;
  nnew=0;
  old=0; 
  mode=1;
  taken=0;
  iflush=0;
  has_reader=0;
  has_writer=0;
  buf=(char *)malloc(siz=1024);
  onew=0;
  oold=0;
  obuf=(char *)malloc(osiz=1024);

  ports.push_back(this);
  }

/* Close port */

void Port::close()
 {
 if(fd!=-1)		/* Already closed? */
   {
   if(read) read->cancel();
   read=0;
   while(!out.empty())
     {
     Output *o=out.next;
     out.deque(o);
     if(o->write) o->write->cancel();
     delete o;
     }
   ::close(fd);
   fd= -1;
   }
 }

void Port::free()
  {
  if(!--count)
    delete this;
  }

Port::~Port()
  {
  ports.deque(this);
  }

Port *Port::dup()
  {
  ++count;
  return this;
  }

/* Handle read buffering mode */

void Port::bufread()
  {
  switch(mode)
    {
    case 0: /* Raw mode */
      {
      /* Record amount of data */
      taken=nnew-old;

      /* Pass data to callback fuction. */
      if(read) read->cont(read,buf+old,taken);
      break;
      }

    case 1: /* Line mode */
      { /* Line mode */
      char *f;
      if(f=(char *)memchr(buf+old,'\n',nnew-old))
        { /* We found a '\n'! */
        int len=f-(buf+old);

        /* Amount of data callback function will take */
        taken=len+1;

        /* If line ends in CR-LF, kill the CR too */
        if(f!=buf+old && f[-1]=='\r') f[-1]=0, --len;

        *f=0; /* Zero terminate the line */

        /* Call callback function */
        if(read) read->cont(read,buf+old,len);
        }
      /* If we didn't get a full line, return to doread for more data. */
      break;
      }

    case 2: /* Block mode */
      { /* Block mode */
      if(nnew-old>=bksize)
        {
        /* We will take one block */
        taken=bksize;
        /* Call callback function */
        if(read) read->cont(read,buf+old,taken);
        }
      break;
      }
    }
  }

/* Handle read event (called when port is enabled and there is read data
 * available).
 */

void Port::doread()
  {
  int len;

  if(!read) return;

  if(mode==-1)
    { /* Very raw mode */
    read->cont(read,NULL,0);
    return;
    }

  if(mode==3)
    { /* This is a listen port */
    Port *newport;
    struct sockaddr_in addr;
    socklen_t len=sizeof(addr);
    newport=new Port(accept(fd,(struct sockaddr *)&addr,&len));
    read->cont(read,(char *)newport,0); // Yuck...
    return;
    }

  /* Read as much data as is available and which can fit in our buffer */
  while((len=::read(fd,buf+nnew,siz-nnew))>0)
    {
    /* Indicate amount read */
    nnew+=len;

    /* Expand our buffer if data can't fit in it */
    if(nnew==siz) buf=(char *)realloc(buf,siz*=2);

    /* Process the data we have */
    bufread();
    if(!read) break;	/* No more reads if we became disabled */
    }
  if(len==0) close();	/* If port closed */
  }

/* Set function to receive data from port */

void Port::stportread(Fn_2<void,char *,int> *fn,int len)
  {
  // mutex_lock(io_mutex);

  read=fn;

  /* Delete data which is no longer needed */
  old+=len;
  taken=0;
  if(old)
    {
    if(nnew!=old)
     memmove(buf,buf+old,nnew-old);
    nnew-=old;
    old=0;
    /* If we have data and we have taken data on last read, restart the port
       (serv() will cause another read() call to be made to see if the remaining
       data can be taken). */
    if(nnew && len) bufread();
    }

  // mutex_unlock(io_mutex);
  }

/* Change mode */

void Port::stlinemode()
  {
  mode=1;
  }

void Port::strawmode()
  {
  mode=0;
  }

void Port::stblockmode(int size)
  {
  bksize=size;
  mode=2;
  }

void Port::stnomode()
  {
  mode=-1;
  }

/* Handle write event (called by serv() when there is data to be written and
 * when select() says it's ok to write).
 */

void Port::dowrite()
  {
  Output *o;
  int len;
  while(oold!=onew || !out.empty())
    { /* While we have data */
    /* Merge data into output buffer- do not deque here.  Only deque when we
       have actually written the data (in this case, when the output buffer
       becomes completely empty */
    for(o=out.next;o;o=o->link.next)
     if(o->len)
      if(osiz-onew>=o->len)
       {
       memcpy(obuf+onew,o->buf,o->len);
       onew+=o->len;
       o->len=0;
       }
      else break;
    if(oold!=onew)
      { /* The output buffer is not empty: write it */
      len=::write(fd,obuf+oold,onew-oold);
      if(len>0)
        { /* Update by amount written */
        oold+=len;
        if(oold==onew)
          { /* We wrote the entire output buffer - acknowledge queued items */
          oold=onew=0;
          while(!out.empty())
            {
            o=out.next;
            if(!o->len)
              {
              if (o->write) o->write->cont(o->write);
              delete out.deque(o);
              }
            }
          }
        /* Loop to try to write more data */
        }
      else if(errno==EPIPE)
        { /* Pipe error- the port closed on us */
        close();
        break;
        }
      else
       /* Wrote as much data as we could.  Have to wait till next time for more */
       break;
      }
    else
      { /* Write data directly from queue
           (items must be larger than output buffer) */
      o=out.next;
      len=::write(fd,o->buf,o->len);	/* Write it */
      if(len>0)
        { /* Update by amount written */
        o->buf+=len;
        o->len-=len;
        if(!o->len)
          { /* Move to next buffer if we're done */
          if(o->write) o->write->cont(o->write);
          delete out.deque(o);
          }
        /* Loop to try to write more data */
        }
      else if(errno==EPIPE)
        { /* Pipe error- the port closed on us */
        close();
        break;
        }
      else
        /* Wrote as much data as we could.  We'll be back when we can write more */
        break;
      }
    }
  }

/* Queue up write data.  Callback executed when data has been written. */

void Port::dofinalwrite(char *data,int len,Fn_0<void> *fn)
  {
  if(fd!=-1)
    {
    Output *o = new Output();
    o->buf=data;
    o->len=len;
    o->write=fn;
    out.push_back(o);
    }
  else fn->cancel();
  }

void Port::finalwrite(char *data,int len,Fn_0<void> *fn)
  {
  // mutex_lock(io_mutex);
  dofinalwrite(data,len,fn);
  // mutex_unlock(io_mutex);
  }

/* Try to add data to output buffer.  If it doesn't fit, queue up write */

int Port::write(char *data,int datalen,Fn_0<void> *fn)
  {
  // mutex_lock(io_mutex);
  if(fd==-1)
    {
    if(fn) fn->cancel();
    // mutex_unlock(io_mutex);
    return 0;
    }
  else if(osiz-onew<datalen || !out.empty())
    { /* Not enough space in buffer, or data is queued up in front of us.
         Enque our data and write it when we can */
    dofinalwrite(data,datalen,fn);
    // mutex_unlock(io_mutex);
    return 0;
    }
  else
    { /* Data fits in output buffer */
    memcpy(obuf+onew,data,datalen);
    onew+=datalen;
    // mutex_unlock(io_mutex);
    return 1;
    }
  }

/* Set up a socket for listening */

Port *portlisten(int listen_port)
  {
  Port *port;
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

  port=new Port(lstn);
  port->mode=3;

  return port;
  }

/* Connect to a remote port */

Port *portconnect(char *name,int port)
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
  return new Port(fd);
  }

/* Connect to a remote port from root */

extern void doX();

Port *privconnect(char *name,int port)
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
  return new Port(fd);
  }

/* Call select() and process any events */

int io()
  {
  Port *p, *n;

  fd_set rdfds, wrfds, exfds;	/* fds to check */
  struct timeval tm;		/* Timeout value */
  struct itimerval tmrtn;
  int last;			/* Highest fd */
  int init;

  /* Build arguments for select: clear file descriptors */
  FD_ZERO(&rdfds); FD_ZERO(&wrfds); FD_ZERO(&exfds);

  // mutex_lock(io_mutex);

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
  tmrtn.it_value.tv_usec=0;
  tmrtn.it_value.tv_sec=3600;

  /* Find highest fd */
  last=0;

  /* Find fds which need to be checked */
  for(p=ports.next;p;p=p->link.next)
    {
    if(p->fd>last) last=p->fd;
    /*  printf("%d:\n",p->fd); */
    if(p->read && !p->has_reader)
      {
      /*    printf("selecton %d\n",p->fd); */
      FD_SET(p->fd,&rdfds);
      if(p->fd>last) last=p->fd+1;
      }
    if((p->oold!=p->onew || !p->out.empty()) && !p->has_writer)
      {
      FD_SET(p->fd,&wrfds);
      if(p->fd>last) last=p->fd+1;
      }
    /* we should up the open count on the port here */
    }

  // mutex_unlock(io_mutex);

  /* Return if there was nothing to do. */
  if(!last && init==-1) return 0;

  doX();

  /* Check for events */
  setitimer(ITIMER_REAL,&tmrtn,NULL);
  select(last+1,&rdfds,&wrfds,&exfds,&tm);
  /* printf("select returns\n"); */

  // mutex_lock(io_mutex);

  /* Handle any timer events */
  if(init!=-1)
    {
    getitimer(ITIMER_REAL,&tmrtn);
    note((3600 - tmrtn.it_value.tv_sec) * 1000000 - tmrtn.it_value.tv_usec);
    // if(anyfns()) tasks=1;
    }

  /* Check which fds were returned by select() */
  /* printf("thread=%d\n",thread_self()); */
  for(p=ports.next;p;p=n)
    {
    n=p->link.next;
    if(p->fd!=-1 && FD_ISSET(p->fd,&rdfds))
      {
      p->doread();
      }
    if(p->fd!=-1 && FD_ISSET(p->fd,&wrfds))
      {
      p->dowrite();
      }
    }

  // mutex_unlock(io_mutex);

  return 1;
  }

// Timer stuff

struct Ttask
  {
  Link<Ttask> link;
  int usecs;
  Fn_0<void> *fn;
  };

Link<Ttask> timer_tasks;

void submit(int usecs,Fn_0<void> *fn)
  {
  Ttask *t=new Ttask();
  Ttask *u;
  t->usecs=usecs;
  t->fn=fn;
  for(u=timer_tasks.next;u && usecs>u->usecs;u=u->link.next);
  if (u)
    timer_tasks.insert_before(u,t);
  else
    timer_tasks.push_back(t);
  }

void note(int usecs)
 {
 Ttask *t;
 for(t=timer_tasks.next;t;t=t->link.next)
   if(t->usecs>0)
     {
     t->usecs-=usecs;
     }
 while (!timer_tasks.empty() && timer_tasks.next->usecs<=0)
   {
   t=timer_tasks.next;
   timer_tasks.deque(t);
   t->fn->cont(t->fn);
   delete t;
   }
 }

/* How long to wait for next event */

int waittim()
  {
  int tim= -1;
  if(timer_tasks.next) tim=timer_tasks.next->usecs;
  return tim;
  }

void cancel(Fn_0<void> *fn)
  {
  if(!fn) return;
  Ttask *t;
  for(t=timer_tasks.next;t;t=t->link.next)
    if(t->fn==fn)
      break;
  if(t)
    {
    t->fn->cancel();
    t->fn=0;
    timer_tasks.deque(t);
    delete t;
    }
  }
