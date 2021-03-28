# Меркурий 200.02(ESPHome) <-> Home Assistant

**Компонент еще не дописан и находиться в разработке**

Компонент отправляет команды электросчетчику Меркурий 200.02 и считывать ответы с него.

* Хорошее объяснение работы протокола описан в данном [репозитории](https://github.com/mrkrasser/MercuryStats)

:white_check_mark: Реализовано получение: напряжения, тока, мощности, тариф1, тариф2, сумма тарифа.

![](https://github.com/RocketFox2409/MercuryESPHome/blob/main/examples/Block-diagram.png)

Схема подключение к esp

![](https://github.com/RocketFox2409/MercuryESPHome/blob/main/examples/cxema.png)
Распиновка на счетчике:
1-GHD
2-A
3-B
4-+5
![](https://github.com/RocketFox2409/MercuryESPHome/blob/main/examples/RS485.png)
![](https://github.com/RocketFox2409/MercuryESPHome/blob/main/examples/RS485-2.png)