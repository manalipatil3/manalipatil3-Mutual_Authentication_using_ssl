### Manali Patil
### B-Number :- B01035189
### Programming Language : C

### Base Program - Complete
### Bonus Program -Complete

### Note:-- I have attched the screenshot for output 

Compile the code :- make all


### Base Program Scenarios
### Scenario 1
----Server 
    patil3@remote00:~/CS/p4_base_bkp$ ./server 8999 oneway "server.key" "server.crt"
    Listening on port 8999
    Connection from 127.0.0.1:54152
    Received: 23
    Received: 32
----Client
    mpatil3@remote00:~/CS/p4_base_bkp$ ./client 8999
    Connection established. Please input numbers.
    Input a number: 23
    Received from server: 24
    Input a number: 32
    Received from server: 33
### Scenario 2
----Server 
    mpatil3@remote00:~/CS/p4_base_bkp$ ./server 8999 oneway "server.key" "self_signed_server.crt"
    Listening on port 8999
----Client
    mpatil3@remote00:~/CS/p4_base_bkp$ ./client 8999
    Server certificate verification failed
### Scenario 3
----Server 
    mmpatil3@remote00:~/CS/p4_base_bkp$ ./server 8080 oneway "server.key" 
    Listening on port 8080
----Client
    mpatil3@remote00:~/CS/p4_base_bkp$ ./client 8080
    Server Certificate Missing
    
    
### BONUS Program Scenarios
### Scenario 1
----Server 
    mpatil3@remote00:~/CS/p4_base_bkp$ ./server 8999 mutual "server.key" "server.crt"
    Listening on port 8999
    Connection from 127.0.0.1:58178
    Received: 23
    Received: 32
----Client
    mpatil3@remote00:~/CS/p4_base_bkp$ ./client 8999 "client.key" "client.crt"
    Connection established. Please input numbers.
    Input a number: 23
    Received from server: 24
    Input a number: 32
    Received from server: 33
    Input a number: 
### Scenario 2
----Server 
    mpatil3@remote00:~/CS/p4_base_bkp$ ./server 8999 mutual "server.key" "server.crt"
    Listening on port 8999
    Client Certificate Verification Failed
----Client
    mpatil3@remote00:~/CS/p4_base_bkp$ ./client 8999 "client.key" "self_signed_client.crt"
    Connection established. Please input numbers.
    Input a number: 2
    Connection Closed by peer
### Scenario 3
----Server 
    mpatil3@remote00:~/CS/p4_base_bkp$ ./server 8999 mutual "server.key" "server.crt"
    Listening on port 8999
    Client Certificate Missing
----Client
    mpatil3@remote00:~/CS/p4_base_bkp$ ./client 8999 "client.key" 
    Connection established. Please input numbers.
    Input a number: 432
    mpatil3@remote00:~/CS/p4_base_bkp$ 


    test log:-

### server log:-

mpatil3@remote01:~/CS/p4_base_bkp$ ./server 9000 oneway "server.key" "server.crt"
Listening on port 9000
Connection from 127.0.0.1:37596
Received: 12
403722A56A7F0000:error:0A000126:SSL routines:ssl3_read_n:unexpected eof while reading:../ssl/record/rec_layer_s3.c:303:
Connection from 127.0.0.1:39916
Received: 232
Received: 32
Received: 32
403722A56A7F0000:error:0A000126:SSL routines:ssl3_read_n:unexpected eof while reading:../ssl/record/rec_layer_s3.c:303:
^C
mpatil3@remote01:~/CS/p4_base_bkp$ ./server 9000 oneway "server.key" "self_signed_server.crt"
Listening on port 9000
^C
mpatil3@remote01:~/CS/p4_base_bkp$ ./server 9000 oneway "server.key"
Listening on port 9000
^C
mpatil3@remote01:~/CS/p4_base_bkp$ ./server 8000 mutual "server.key" "se
rver.crt"
Listening on port 8000
Connection from 127.0.0.1:43088
Received: 21
Received: 21
40A725F1D37F0000:error:0A000126:SSL routines:ssl3_read_n:unexpected eof while reading:../ssl/record/rec_layer_s3.c:303:
Client certificate verification failed
: Success
mpatil3@remote01:~/CS/p4_base_bkp$ ./server 8000 mutual "server.key" "server.crt"
Listening on port 8000
Client Certificate Missing
: Success
^C
mpatil3@remote01:~/CS/p4_base_bkp$ 


### client log :--
mpatil3@remote01:~/CS/p4_base_bkp$ ./client 9000
Connection established. Please input numbers.
Input a number: 12
Received from server: 13
Input a number: ^C
mpatil3@remote01:~/CS/p4_base_bkp$ ./client 9000
Connection established. Please input numbers.
Input a number: 232
Received from server: 233
Input a number: 32
Received from server: 33
Input a number: 32
Received from server: 33
Input a number: ^C
mpatil3@remote01:~/CS/p4_base_bkp$ ./client 9000
Server certificate verification failed
: Success
mpatil3@remote01:~/CS/p4_base_bkp$ ./client 9000
Server Certificate Missing
: Success
mpatil3@remote01:~/CS/p4_base_bkp$ ./client 8000 "client.key" "client.
crt"
Connection established. Please input numbers.
Input a number: 21
Received from server: 22
Input a number: 21
Received from server: 22
Input a number: ^C
mpatil3@remote01:~/CS/p4_base_bkp$ ./client 8000 "client.key" "self_signed_client.crt"
Connection established. Please input numbers.
Input a number: ^C
mpatil3@remote01:~/CS/p4_base_bkp$ ./client 8000 "client.key" 
Connection established. Please input numbers.
Input a number: ^C


