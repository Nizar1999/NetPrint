<p align="center">
  <img src="https://github.com/Nizar1999/NetPrint/blob/main/screenshots/Banner.png" width = 55%; height=55% />
</p>

![Languages](https://img.shields.io/badge/-C++-grey?style=for-the-badge&logo=cplusplus) 

Client-Server implementation of a networked printing service for handling vast amounts of print requests coming from users with different levels of privileges and priorities.

[Further Information](./NetPrint_Report.docx)

# Specifications

## Packet Composition

![packet](./screenshots/Packet.png)

A 1996 bytes payload size with a 4 bytes header, are enough to fit simple print requests.
Protocol codes, placed in the header, need to be defined in order to identify and process requests accordingly.

## Priority Data Structure
Print requests need to be handled based on priority.
![priorities](./screenshots/Priorities.png)

## Multithreading
To be able to handle multiple users simultaneously, multithreading was utilized.
![multiThreading](./screenshots/MultiThreading.png)

## Mutual Exclusion using Semaphores
Sempaphores were used to avoid two threads using the same Data simultaneously and to achieve uninterrupted printing.

# Usage Example
## Client
### Launch
![mainMenu](./screenshots/MainMenu.png)

### Register New Account
![RegisterUser](./screenshots/RegisterUser.png)

### Post Login
![PostLogin](./screenshots/PostLogin.png)

### Print
![Print](./screenshots/Print.png)

## Server
### Listening
![ServerListen](./screenshots/ServerListen.png)

### Receiving Register Request
![ReceiveRegister](./screenshots/ReceiveRegister.png)

### Receiving Print Request
![ServerPrint](./screenshots/ServerPrint.png)

### Receiving Multiple Print Requests From Different Users
![ServerMultiPrint](./screenshots/ServerMultiPrint.png)

### Logout
![ServerLogout](./screenshots/ServerLogout.png)
