#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <winsock2.h>
#include <windows.h>
#include "cpp-httplib-master/httplib.h"
#include "nlohmann/json.hpp"

using namespace std;
using namespace httplib;
using json = nlohmann::json; 


unordered_map<string ,string> shortToLong;

unsigned int generateHash(string longUrl)
{
    unsigned int hashValue =0;

    for(int i=0;i<longUrl.size();i++)
    {
        hashValue = (hashValue<<5)+(hashValue^longUrl[i]);
        
    }
    return hashValue;
}

string getShortUrlFromHash(unsigned int hashValue){
    char base62Arr[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const int base =62;
    string shortUrl = "";
    
    do {
        unsigned int index = (hashValue%base);
        char c = base62Arr[index];
        shortUrl.push_back(c);
        hashValue /= base;

    }while(hashValue >0);
  reverse(shortUrl.begin(),shortUrl.end());

  return shortUrl;
}
string shortenUrl(string longUrl)
{
    unsigned int hashValue = generateHash(longUrl);
    cout<<"the hashvalue is"<<hashValue<<endl;

    string shortUrl =getShortUrlFromHash(hashValue);
    cout<<"the short url is"<<shortUrl<<endl;

    shortToLong[shortUrl] = longUrl;
    return shortUrl;
}
int main(int argc,char const *argv[])
{
    Server server;
    server.Post("/shorten",[](const Request& req, Response& res)
    {
        string longUrl = req.get_param_value("longUrl");
        string shortUrl = shortenUrl(longUrl);
        


        shortToLong[shortUrl] = longUrl;
         res.set_content("{\"shortUrl\":\"" + shortUrl + "\"}", "application/json");

    });

     server.Get(R"(/(\w+))", [&](const Request& req, Response& res)
    {
         string shortUrl = req.matches[1];
         string response;
         

         json jsonResponse;
         if(shortToLong.find(shortUrl) != shortToLong.end() ){
           jsonResponse["url"] = shortToLong[shortUrl];
           jsonResponse["statusCode"] = 302;
         }
         else
         {
            jsonResponse["url"] = "";
            jsonResponse["statusCode"] = 404;
         }
         res.set_content(jsonResponse.dump(),"application/json");
    });
    cout<<"Server listening on port 8080"<<endl;

    server.listen("localhost",8080);

       return 0;
}