STM32F4_EthertnetProject
========================
Projekt pozwala kontrolować diody przez stronę WWW oraz wyświetla wartość ADC.
Zastosowano w projkcie komunikację poprzez sieć Ethernet wykorzystując zestaw protokołów LwIP oraz fizyczną kartę sieciową DP83848C.

LwIP
----
LwIP - uproszczony protokół IP oparty na stosie TCP/IP. Zawiera w sobie takie protokoły jak:
- IP
- ICMP
- UDP
- TCP
- DHCP 
- ARP

LwIP nie wymaga ścisłej bariery między warstwami, dzięki czemu można współdzielić pamięć warstw niższych razem z aplikacją która wykorzystuje te dane, dzięki czemu nie ma potrzeby kopiowania tych danych i zyskujemy przestrzeń pamięci co jest zaletą dla systemów wbudowanych.

LwIP zawiera podstawowe funkcje protokołu IP takie jak wysyłanie, odbieranie, przekierowanie – nie można jednak wysyłać ani odbierać pofragmentowanych pakietów.

http://savannah.nongnu.org/projects/lwip/

Kompilator
----------
Projekt w pełni zbudowany pod CooCox IDE w wersji 1.7.6: http://www.coocox.org/CooCox_CoIDE.htm


Sterowniki i narzędzia dla STM32F4xx
------------------------------------
- Strona WWW ze sterownikami pod STM32F4xx: http://www.st.com/web/en/catalog/tools/PF258167
- ARM-GCC compiler (niezbędna biblioteka dla CooCox IDE): https://launchpad.net/gcc-arm-embedded/+download

Uruchomienie projektu
---------------------
Projekt należy otworzyć za pomocą CooCox IDE.
Plik ub_lib/stm32_ub_http_server.h zawiera konfigurację adresu MAC oraz IP karty sieciowej. Domyślna konfiguracja IP to 192.168.0.10/24 oraz brama 192.168.0.1

Debugowanie
-----------
Oprogramowanie CooCox IDE pozwala na debugowanie projektu i tworzenie breakpoint'ów. Wsztsrkie opcje dostępne są w pasku narzędzi programu.

Możliwe ulepszenia
------------------
Możnaby rozbudować możliwości serwera, np. ciekawym pomysłem byłoby dodanie obsługi stron, które są odczytywane z zewnętrznej pamięci (np. karta SD).

Pomocne książki i strony
------------------------
- Adam Dunkels  - Design and Implementation of the lwIP TCP/IP Stack

- http://www.atmel.com/Images/Atmel-42134-Use-of-Ethernet-on-SAM4E-EK_AT02971_Application-Note.pdf
- http://lwip.wikia.com/wiki/LwIP_Application_Developers_Manual
- http://blog.tkjelectronics.dk/2012/08/ethernet-on-stm32f4discovery-using-external-phy/
- http://www.ti.com/lit/ds/symlink/dp83848c.pdf