#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <zlib.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef __WIN32__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#else

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
typedef uint32_t in_addr_t;

#undef DrawText

#endif


#include "common.h"
#include "netloader.h"

// having trouble with non blocking sockets & udp on switch currently
#ifndef __SWITCH__
#define PING_ENABLED 1
#else
#define PING_ENABLED 0
#endif

#define ZLIB_CHUNK (16 * 1024)
#define FILE_BUFFER_SIZE (128*1024)

static int netloader_listenfd = -1;
static int netloader_datafd   = -1;
#if PING_ENABLED
static int netloader_udpfd = -1;
#endif
unsigned char in[ZLIB_CHUNK];
unsigned char out[ZLIB_CHUNK];

//---------------------------------------------------------------------------------
static void netloader_error(const char *func, int err) {
//---------------------------------------------------------------------------------
    char errortext[1024];

    sprintf(errortext, "%s: err=%d\n %s\n", func, err, strerror(errno));

    menuCreateMsgBox(640,480, errortext);

    netloader_deactivate();
}

//---------------------------------------------------------------------------------
static void netloader_socket_error(const char *func) {
//---------------------------------------------------------------------------------
    int errcode;
#ifdef __WIN32__
    errcode = WSAGetLastError();
#else
    errcode = errno;
#endif
    netloader_error(func,errcode);
}

//---------------------------------------------------------------------------------
void shutdownSocket(int socket) {
//---------------------------------------------------------------------------------
#ifdef __WIN32__
    shutdown (socket, SD_SEND);
    closesocket (socket);
#else
    close(socket);
#endif
}

static const char DIRECTORY_THIS[] = ".";
static const char DIRECTORY_PARENT[] = "..";

//---------------------------------------------------------------------------------
static bool isDirectorySeparator(int c) {
//---------------------------------------------------------------------------------
    return c == DIRECTORY_SEPARATOR_CHAR;
}

//---------------------------------------------------------------------------------
static void sanitisePath(char *path) {
//---------------------------------------------------------------------------------
    char *tmpPath = strdup(path);
    tmpPath[0] = 0;

    char *dirStart = path;
    char *curPath = tmpPath;

#ifdef _WIN32
    while(dirStart[0]) {
        if (dirStart[0] == '/') dirStart[0] =DIRECTORY_SEPARATOR_CHAR;
        dirStart++;
    }
#endif

    dirStart = path;

    while(isDirectorySeparator(dirStart[0])) dirStart++;


    do {
        char *dirEnd = strchr(dirStart, DIRECTORY_SEPARATOR_CHAR);
        if (dirEnd) {
            dirEnd++;
            if(!strncmp(DIRECTORY_PARENT,dirStart,strlen(DIRECTORY_PARENT))) {
                /* move back one directory */
                size_t pathlen = strlen(tmpPath);
                if(tmpPath[pathlen-1] == DIRECTORY_SEPARATOR_CHAR) tmpPath[pathlen-1] = 0;
                char *prev = strrchr(tmpPath,DIRECTORY_SEPARATOR_CHAR);
                if (prev) {
                    curPath = prev + 1;
                } else {
                    curPath = tmpPath;
                }


                dirStart = dirEnd;
            } else if (!strncmp(DIRECTORY_THIS,dirStart,strlen(DIRECTORY_THIS))) {
                /* strip this entry */
                dirStart = dirEnd;
            } else {
                size_t dirSize = dirEnd - dirStart;
                strncpy(curPath,dirStart,dirSize);
                curPath[dirSize] = 0;
                curPath += dirSize;
                dirStart += dirSize;
            }
        } else {
            strcpy(curPath,dirStart);
            dirStart += strlen(dirStart);
        }
    } while(dirStart[0]);

    strcpy(path, tmpPath);
    free(tmpPath);
}

//---------------------------------------------------------------------------------
static int set_socket_nonblocking(int sock) {
//---------------------------------------------------------------------------------

#ifndef __WIN32__
    int flags = fcntl(sock, F_GETFL);

    if(flags == -1) return -1;

    int rc = fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    if(rc != 0) return -1;

#else
    u_long opt = 1;
    ioctlsocket(sock, FIONBIO, &opt);
#endif


    return 0;
}


//---------------------------------------------------------------------------------
static int recvall(int sock, void *buffer, int size, int flags) {
//---------------------------------------------------------------------------------
    int len, sizeleft = size;

    while (sizeleft) {

        len = recv(sock,buffer,sizeleft,flags);

        if (len == 0) {
            size = 0;
            break;
        };

        if (len != -1) {
            sizeleft -=len;
            buffer +=len;
        } else {
#ifdef _WIN32
            int errcode = WSAGetLastError();
            if (errcode != WSAEWOULDBLOCK) {
                netloader_error("win socket error",errcode);
                break;
            }
#else
            if ( errno != EWOULDBLOCK && errno != EAGAIN) {
                perror(NULL);
                break;
            }
#endif
        }
    }
    return size;
}


//---------------------------------------------------------------------------------
static int decompress(int sock, FILE *fh, size_t filesize) {
//---------------------------------------------------------------------------------
    int ret;
    unsigned have;
    z_stream strm;
    size_t chunksize;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
        netloader_error("inflateInit failed.",ret);
        return ret;
    }

    size_t total = 0;
    /* decompress until deflate stream ends or end of file */
    do {

        int len = recvall(sock, &chunksize, 4, 0);

        if (len != 4) {
            (void)inflateEnd(&strm);
            netloader_error("Error getting chunk size",len);
            return Z_DATA_ERROR;
        }

    strm.avail_in = recvall(sock,in,chunksize,0);

    if (strm.avail_in == 0) {
        (void)inflateEnd(&strm);
        netloader_error("remote closed socket.",0);
        return Z_DATA_ERROR;
    }

    strm.next_in = in;

    /* run inflate() on input until output buffer not full */
    do {
        strm.avail_out = ZLIB_CHUNK;
        strm.next_out = out;
        ret = inflate(&strm, Z_NO_FLUSH);

        switch (ret) {

            case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     /* and fall through */

            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
            case Z_STREAM_ERROR:
                (void)inflateEnd(&strm);
                netloader_error("inflate error",ret);
                return ret;
        }

        have = ZLIB_CHUNK - strm.avail_out;

        if (fwrite(out, 1, have, fh) != have || ferror(fh)) {
            (void)inflateEnd(&strm);
            netloader_error("file write error",0);
            return Z_ERRNO;
        }

        total += have;
        //printf("%zu (%zd%%)",total, (100 * total) / filesize);
    } while (strm.avail_out == 0);

    /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}



//---------------------------------------------------------------------------------
int loadnro(menuEntry_s *me, int sock, struct in_addr remote) {
//---------------------------------------------------------------------------------
    int len, namelen, filelen;
    char filename[PATH_MAX+1];
    len = recvall(sock, &namelen, 4, 0);

    if (len != 4) {
        netloader_error("Error getting name length", errno);
        return -1;
    }

    if (namelen >= sizeof(filename)-1) {
        netloader_error("Filename length is too large",errno);
        return -1;
    }

    len = recvall(sock, filename, namelen, 0);

    if (len != namelen) {
        netloader_error("Error getting filename", errno);
        return -1;
    }

    filename[namelen] = 0;

    len = recvall(sock, &filelen, 4, 0);

    if (len != 4) {
        netloader_error("Error getting file length",errno);
        return -1;
    }

    int response = 0;

    sanitisePath(filename);

    snprintf(me->path, PATH_MAX, "%s%s%s", menuGetRootPath(), DIRECTORY_SEPARATOR,  filename);
    // make sure it's terminated
    me->path[PATH_MAX] = 0;

    argData_s* ad = &me->args;
    ad->dst = (char*)&ad->buf[1];
    ad->nxlink_host = remote;

    launchAddArg(ad, me->path);

#ifndef _WIN32
    int fd = open(me->path,O_CREAT|O_WRONLY, ACCESSPERMS);

    if (fd < 0) {
        response = -1;
        netloader_error("open", errno);
    } else {
        if (ftruncate(fd,filelen) == -1) {
            response = -2;
            netloader_error("ftruncate",errno);
        }
        close(fd);
    }
#endif

    FILE *file = NULL;

    if (response == 0) file = fopen(me->path,"wb");

    if(NULL == file) {
        perror("file");
        response = -1;
    }

    send(sock,(char *)&response,sizeof(response),0);

    if (response == 0 ) {

        //char *writebuffer=malloc(FILE_BUFFER_SIZE);
        //setvbuf(file,writebuffer,_IOFBF, FILE_BUFFER_SIZE);

        //printf("transferring %s\n%d bytes.\n", filename, filelen);

        if (decompress(sock,file,filelen)==Z_OK) {
            int netloaded_cmdlen = 0;
            send(sock,(char *)&response,sizeof(response),0);
            //printf("\ntransferring command line\n");
            len = recvall(sock,(char*)&netloaded_cmdlen,4,0);

            len = recvall(sock,me->args.dst, netloaded_cmdlen,0);

            while(netloaded_cmdlen) {
                size_t len = strlen(me->args.dst) + 1;
                ad->dst += len;
                ad->buf[0]++;
                netloaded_cmdlen -= len;
            }

        } else {
            response = -1;
        }

        //free(writebuffer);
        fflush(file);
        fclose(file);

    }

    return response;
}

//---------------------------------------------------------------------------------
int netloader_activate(void) {
//---------------------------------------------------------------------------------
    struct sockaddr_in serv_addr;

#ifdef __SWITCH__
    socketInitializeDefault();
#endif

#ifdef __WIN32__
    WSADATA wsa_data;
    if (WSAStartup (MAKEWORD(2,2), &wsa_data)) {
        netloader_error("WSAStartup failed\n",1);
        return 1;
    }
#endif


    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(NXLINK_SERVER_PORT);

#if PING_ENABLED
    // create udp socket for broadcast ping
    netloader_udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (netloader_udpfd < 0)
    {
        netloader_socket_error("udp socket");
        return -1;
    }

    if(bind(netloader_udpfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        netloader_socket_error("bind udp socket");
        return -1;
    }

    if (set_socket_nonblocking(netloader_udpfd) == -1)
    {
        netloader_socket_error("listen fcntl");
        return -1;
    }
#endif
    // create listening socket on all addresses on NXLINK_SERVER_PORT

    netloader_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(netloader_listenfd < 0)
    {
        netloader_socket_error("socket");
        return -1;
    }

    int rc = bind(netloader_listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(rc != 0)
    {
        netloader_socket_error("bind");
        return -1;
    }

    if (set_socket_nonblocking(netloader_listenfd) == -1)
    {
        netloader_socket_error("listen fcntl");
        return -1;
    }

    rc = listen(netloader_listenfd, 10);
        if(rc != 0)
    {
        netloader_socket_error("listen");
        return -1;
    }

    return 0;
}


//---------------------------------------------------------------------------------
int netloader_deactivate(void) {
//---------------------------------------------------------------------------------
    // close all remaining sockets and allow mainloop to return to main menu
    if(netloader_listenfd >= 0)
    {
        shutdownSocket(netloader_listenfd);
        netloader_listenfd = -1;
    }

    if(netloader_datafd >= 0)
    {
        shutdownSocket(netloader_datafd);
        netloader_datafd = -1;
    }

#if PING_ENABLED
    if(netloader_udpfd >= 0)
    {
        shutdownSocket(netloader_udpfd);
        netloader_udpfd = -1;
    }
#endif

#ifdef __SWITCH__
    socketExit();
#endif

#ifdef __WIN32__
    WSACleanup ();
#endif

    return 0;
}

//---------------------------------------------------------------------------------
int netloader_loop(menuEntry_s *me) {
//---------------------------------------------------------------------------------

    struct sockaddr_in sa_remote;

#if PING_ENABLED
    char recvbuf[256];
    socklen_t fromlen = sizeof(sa_remote);

    int len = recvfrom(netloader_udpfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*) &sa_remote, &fromlen);

    if (len!=-1) {
        if (strncmp(recvbuf,"nxboot",strlen("nxboot")) == 0) {
            sa_remote.sin_family=AF_INET;
            sa_remote.sin_port=htons(NXLINK_CLIENT_PORT);
            sendto(netloader_udpfd, "bootnx", strlen("bootnx"), 0, (struct sockaddr*) &sa_remote,sizeof(sa_remote));
        }
    }
#endif
    if(netloader_listenfd >= 0 && netloader_datafd < 0) {
        socklen_t addrlen = sizeof(sa_remote);
        netloader_datafd = accept(netloader_listenfd, (struct sockaddr*)&sa_remote, &addrlen);
        if(netloader_datafd < 0)
        {

#ifdef _WIN32
            int errcode = WSAGetLastError();
            if (errcode != WSAEWOULDBLOCK) {
                netloader_error("accept", errcode);
                return -1;
            }
#else
            if ( errno != EWOULDBLOCK && errno != EAGAIN) {
                netloader_error("accept", errno);
                return -1;
            }
#endif

        }
        else
        {
            close(netloader_listenfd);
            netloader_listenfd = -1;
        }
    }

    if(netloader_datafd >= 0)
    {
        int result = loadnro(me, netloader_datafd,sa_remote.sin_addr);
        netloader_deactivate();
        if (result== 0) {
            return 1;
        } else {
            return -1;
        }
    }

    return 0;
}
