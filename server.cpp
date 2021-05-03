#include <iostream>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
 
using namespace std;

#define BUFF 3072

class Error{
    string err_type;
public:
    Error(string tmp){err_type = tmp;}
    string Get(){return(err_type);}
};

class SocketAddress{
protected:
    struct sockaddr_in saddr;
public:
    SocketAddress(){}
    SocketAddress(const char * IP_addr, short port)
    {
        memset(&saddr, 0, sizeof(saddr));
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr.s_addr = inet_addr(IP_addr); 
    }
    const int GetAddrlen() const {return(sizeof(saddr));}
    const struct sockaddr* GetAddr() const { return (sockaddr*)&saddr;}
};

class Socket{
protected:
    int sd;
    explicit Socket(int sd_){sd = sd_;}
public:
    Socket()
    {
        if((sd = socket(AF_INET, SOCK_STREAM, 0))==-1)
            throw Error("Socket");
    }
    void Shutdown()
    {
        int res = shutdown(sd, 2);
        if (res == -1)
            throw Error("Shutdown");
    }
    ~Socket(){close(sd);}
    int Get() const{ return sd;}
};

class ServerSocket: public Socket{
public:
    ServerSocket() : Socket(){};
    void Bind(const SocketAddress& ipAddr)
    {
        if(bind(sd, (struct sockaddr*)ipAddr.GetAddr(), ipAddr.GetAddrlen() )==-1)
            throw Error("Bind");
    }
    void Listen(int backlog)
    {
        if(listen(sd, backlog) == -1)
            throw Error("Listen");
    }
    int Accept(SocketAddress& clAddr)
    {
        socklen_t addrlen = clAddr.GetAddrlen();
        int sd_ = accept(sd, (struct sockaddr*)clAddr.GetAddr(), &addrlen);
        if(sd_ == -1)
            throw Error("Accept");
        return sd_;
    }
};

class ConnectedSocket: public Socket{
public:
    ConnectedSocket(): Socket(){}
    explicit ConnectedSocket(int sd) : Socket(sd){}
    string Read()
    {
        char chstr[BUFF];
        int size = recv(sd, chstr, BUFF, 0);
        if(size == -1)
            throw Error("Read");
        string str(chstr);
        return(str);

    }
    void Read (string &s)
    {
        char msg[BUFF];
        memset(msg,0,BUFF);
        int tmp = recv(sd, msg, BUFF, 0);
        if (tmp < 0)
            throw Error("Recv");
        else{
            s = msg; 
            memset(msg,0,BUFF);
        }
    }  
    void Write(const string& str)
    {
        vector<char> tmp(str.begin(), str.end());
        if(send(sd, tmp.data(), tmp.size(), 0) == -1)
            throw Error("Send");
    }
    void Write(char *c, int len)
    {
        if (send(sd, c, len, 0) == -1)
            throw Error("Send");
    }
};

class HttpHeader{
    string name;
    string value;
public:
    HttpHeader(const string& n, const string& v) : name(n), value(v) {}
    explicit HttpHeader(const string&str)
    {
        string tmp = str;
        int position = tmp.find(":");
        name = tmp.substr(0, position);
        tmp.erase(0, position + 1);
        value = tmp;    
    }
    const string GetHeader() const
    {
        string tmp=name;
        tmp += ": ";
        tmp += value;
        return tmp;
    }
};

class HttpRequest{
    string method;
    string uri_way;
    string arg;
    string version;
    int syntaxerr;
public:
    HttpRequest(const string& request) : method(), uri_way(), arg(), version(), syntaxerr(0)
    {
        string tmp = request;
        int pos1 = tmp.find(" ");
        if (pos1 < 0)
            syntaxerr = 1;
        else
        {
            method = tmp.substr(0, pos1);
            tmp.erase(0, pos1+1);
        }
        pos1 = tmp.find(" ");
        if (pos1 >= 0)
        {
            string uri = tmp.substr(1, pos1);
            if (pos1 == 1)
            {
                uri = "index.html";    
                uri_way = uri.substr(0, uri.size());
            }
            else
            {
                int pos2 = tmp.find("?");
                if (pos2 != -1)
                {
                    uri_way = uri.substr(0, pos2);
                    uri.erase(0, pos2+1);
                    arg = uri;
                }   
                else
                {
                    uri_way = uri.substr(0, uri.size()-1);
                    arg = " ";   
                }
                tmp.erase(0,pos1+1);
            }
        }
        int pos = tmp.find("/");
        if (pos1 < 0)
            syntaxerr = 1;
        else 
            tmp.erase(0,pos);
        pos=tmp.find("\n");
        if (pos >= 0)
        {
            version = tmp.substr(0, pos);
            tmp.erase(0, pos);
        }
        else 
        {
            version = tmp.substr(0);
            tmp.erase(0);
        }
    } 
    friend class HttpResponse;
};

class HttpResponse{ 
    string header;
    string code;
    string answer;
public:
    HttpResponse(HttpRequest& request, ConnectedSocket &csd) : header(), code(), answer(){
        int fd=open(request.uri_way.c_str(),O_RDONLY);
        if (fd == -1)
        {
            if (errno == EACCES)
                code = "403 Forbidden";
            else if (request.syntaxerr==1)
                code = "400 Bad Request";
            else 
                code = "404 Not Found";
        }
        else{
            code = "200 OK"; 
        }
        if (request.method != "GET"&&request.method!="HEAD")
        {
            code = "501 Not Implemented";
            HttpHeader allow("Allow","GET,HEAD");
            header += allow.GetHeader()+"\n"; 
        }
        answer = "HTTP/1.0 ";
        answer += code+"\n";
        time_t t = time(0);
        HttpHeader date("Date",asctime(localtime(&t)));
        header += date.GetHeader();
        
        if (fd>=0)
        {
            char c;
            int length=0;
            while(read(fd,&c,1))
                length++;
            lseek(fd,0,0);
            HttpHeader content_len("Content-length",to_string(length));
            header+=content_len.GetHeader()+"\n";
        }
        int ind = request.uri_way.find('.');
        string extension;
        if (ind >= 0)
        {
            extension = request.uri_way.substr(ind+1);
            if (extension == "html")
            {
                HttpHeader content_type("Content-type", "text/html");
                header += content_type.GetHeader()+"\n";
            }
            else if(extension == "jpg" || extension == "jpeg")
            {
                HttpHeader content_type("Content-type", "image/jpeg");
                header += content_type.GetHeader()+"\n";
            }
            else 
            {
                HttpHeader content_type("Content-type", "text/plain");
                header += content_type.GetHeader()+"\n";
            }
        }
        if (fd>=0)
        {
            struct stat result;
            if (stat(request.uri_way.c_str(),&result) == 0)
            {
                time_t t1 = result.st_mtime;
                HttpHeader last_mod("Last-modified", asctime(localtime(&t1)));
                header += last_mod.GetHeader();
            }  
        } 
        answer += header+"\r\n";
        if (fd >= 0 && request.method=="GET")
        {
            char mas[BUFF];
            int i;
            csd.Write(answer);
            while((i=read(fd,mas,BUFF))>0)
                csd.Write(mas,i);
        }
        else
        {
            answer += code;
            answer += "\r\n";
            csd.Write(answer);
        }
        extension.clear();
        close(fd);
    }
    const string GetAnswer() const {return answer;}
};

class HttpServer{
    ServerSocket server;
    SocketAddress servaddr;
    int queue;
public:
    HttpServer(int port, int n) : server(), servaddr("127.0.0.1",port), queue(n) {}
    void ProcessConnection(int cd, const SocketAddress& claddr)
    {
        ConnectedSocket cs(cd);
        string request;  
        cs.Read(request); 

        if (!request.empty())
        {
            HttpRequest text(request);
            HttpResponse response(text,cs);
            cout<<response.GetAnswer();
        }
        request.clear();
        cs.Shutdown();
        cout<<"Shutdown client \n"<<endl;
    }
    void ServerLoop()
    {
        try
        {
            server.Bind(servaddr);
            server.Listen(queue);
        }
        catch(Error err)
        {
            cerr<<"Server Error "<<err.Get()<<endl;
            exit(2);                
        } 
        for(;;)
        {
            int cd;
            SocketAddress claddr;
            cout << "Waiting for connection...\n";
            try{cd = server.Accept(claddr);}
            catch(Error err)
            {
                cerr << "Error: "<<err.Get()<<endl;
                exit(1);
            }
            if (cd == -1)
                break;
            cout << "Client Accepted! \n\n";
            ProcessConnection(cd, claddr);
        }
    }
}; 


int main()
{
    try{
        HttpServer server(8081,5);
        server.ServerLoop();
    }
    catch(Error err){
        cout<<"Error: "<<err.Get()<<endl;
    };
}