# 🚗 Sistem de Gestiune Parcare (TCP Socket Server)

Un sistem client-server implementat în C pentru gestionarea automatizată a unei parcări. Proiectul utilizează socket-uri TCP și multiplexare I/O (`select`) pentru a gestiona conexiuni simultane fără a utiliza multithreading.

## 📋 Descriere

Acest proiect simulează interacțiunea dintre un sistem de parcare (server) și șoferi (clienți). Serverul menține evidența locurilor libere/ocupate, calculează costul parcării în funcție de timpul staționat și procesează plățile.

### Funcționalități Cheie:
* **Server Concurent:** Utilizează `select()` pentru a gestiona mai mulți clienți simultan într-un singur proces.
* **Protocol TCP:** Comunicare fiabilă între client și server.
* **Logică de Business:**
  * Alocare aleatorie a locurilor de parcare.
  * Identificare pe bază de număr de înmatriculare.
  * Calcul dinamic al prețului (bazat pe diferența de timp UNIX).
  * Sistem de plată cu comandă dedicată și calcul de rest.

## 🛠️ Cerințe

* Sistem de operare: Linux / Unix (pentru bibliotecile `sys/socket.h`, `netinet/in.h`, `unistd.h`).
* Compilator: GCC.

## 🚀 Instalare și Rulare

### 1. Compilare

Deschide terminalul în directorul proiectului și compilează sursele:

```bash
# Compilare Server
gcc server.c -o server

# Compilare Client
gcc client.c -o client
