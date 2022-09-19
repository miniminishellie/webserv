# webserv
Webserv is Nginx-like web server using I/O multiplexing implemented with C++.

Supports HTTP/1.1 according to RFC7230.

GET, HEAD, POST, PUT, DELETE request supported.

PHP CGI supported, other binaries as CGI supported as well.

# Build & Usage

#### 1. Compile the repository using GNU make.
 ```bash
 # Compile the source
 make
 ```
 
 #### 2. Run the server with config file path 
Default file provided if no arguments are given in directory `./config/default.conf`.

Configure the resource directory in the default config file.

```bash
# run server
./webserv [config_file_path]
```
 
 #### 3. Config file example
 ```
SOFTWARE_NAME ft_nginx
SOFTWARE_VERSION 1.0
HTTP_VERSION 1.1
CGI_VERSION 1.1

server {
    server_name default
    host 127.0.0.1
    port 8080
    REQUEST_URI_LIMIT_SIZE 1024
    REQUEST_HEADER_LIMIT_SIZE 256
    DEFAULT_ERROR_PAGE config/html/default_error.html
    LIMIT_CLIENT_BODY_SIZE 200000000

    location / {
        root /Users/plee/42/webserv/etc/for_test
        allow_method GET
        index index.html
        autoindex off
        cgi .php
    }

    location /auth_test {
        root /Users/plee/42/webserv/etc/for_test
        allow_method GET PUT DELETE
        index index.html
        auth_basic_realm realm
        auth_basic_file /Users/plee/42/webserv/config/auth/jihoolee.htpasswd
        autoindex on
        cgi .php
    }
}
 ```
 - `server`: Server block to handle configuration of a server.
   - `host` & `port`: Host address and port for server.
   - `location`: Defines method to proccess requests for different `URI` and resources in the server. Only exact match supported.
    - `root`: Directory where resources are located.
    - `allow_method`: Allowed HTTP requests.
    - `index`: File brought to client when location block is selected by `URI`
    - `auth-basic_file`: Authentification file location. Password is saved `base64` encoded.
    - `autoindex`: When turned on, shows listed resources of the directory.
    - `CGI`: Extentsion format for `CGI`.
    
# Demo
 #### Using Browser
 1. Getting normal `HTML` document
![get-2022-09-19-at-11 37 47-AM](https://user-images.githubusercontent.com/48874494/190950138-5ef2a853-9b5d-407c-ae01-2b26e17a187f.gif)

 2. Getting `PHP CGI`
 ![cgiScreen-Recording-2022-09-19-at-11 38 56-AM](https://user-images.githubusercontent.com/48874494/190950342-fcaed511-2b32-404a-9400-0d552e02583d.gif)


 #### Using Postman
 
  ```
 [PUT] localhost:8080/hello
 [DELETE] localhost:8080/hello
 ```
 ![PutDeleteScreen-Recording-2022-09-19-at-12 33 05-PM](https://user-images.githubusercontent.com/48874494/190950296-704bbd3f-046c-49f5-b585-3c7c37846166.gif)
 

 
 #### Siege Test
 ```bash
 siege -b 127.0.0.1:8080
 ```
 ![Screen-Recording-2022-09-19-at-1 22 18-PM](https://user-images.githubusercontent.com/48874494/190950467-b25229ec-5021-4898-a954-c5f19af696a5.gif)

# Authors
- [Pureum Lee](https://github.com/pupureum)

- [Jihoon Lee](https://github.com/JihoonNoahLee)

- [ByungKyu Lee](https://github.com/Lee-Byung-Kyu)

- [:globe_with_meridians: 노션 페이지](https://tide-anglerfish-b94.notion.site/Webserv-Team-Page-1463f88b2e914d35a21d021857caca22)
