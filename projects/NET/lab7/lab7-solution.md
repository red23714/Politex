# Lab 7 — Implement DHCPv4: решение для реального железа (R1 = 2801, S1/S2 = 2960)

Доступ ко всем устройствам — консольный кабель (rollover), терминал 9600 8N1.

## Оборудование

| Устройство | Модель | Порты |
|---|---|---|
| R1 | Router 2801 | F0/0 → S1, F0/1 → S2 |
| S1 | Switch 2960 | F0/3 → R1, F0/1 → PC1, F0/2 → PC2 |
| S2 | Switch 2960 | F0/1 → PC3, F0/2 → R1 — настройка не нужна |
| PC1, PC2, PC3 | PC | Copper Straight-Through |

Если коммутатор физически один — PC3 можно воткнуть прямо в R1 F0/1,
на работу DHCP не влияет.

## Схема подключения

```
                         10.10.10.0/24
                         2001:db8:f:f::/64
                        ┌─────────┐
                        │   PC3   │  .10 — IPv4 статически, IPv6 — по DHCPv6
                        └────┬────┘
                             │
                        ┌────┴────┐
                        │   S2    │  ← обычный L2, без настройки
                        └────┬────┘
                             │ F0/1
                        ┌────┴────┐
                        │   R1    │  ← DHCP-сервер
                        └────┬────┘
                             │ F0/0: 10.0.0.1/30, 2001:db8:0:a::1/64
                             │
                             │      VLAN 10: 10.0.0.0/30, 2001:db8:0:a::/64
                        ┌────┴────┐
                        │   S1    │  ← DHCP-ретранслятор + L3 (приложение А!)
                        └┬───────┬┘
                 F0/1 ┌──┘       └──┐ F0/2
                    ┌──┴──┐       ┌──┴──┐
                    │ PC1 │       │ PC2 │
                    └─────┘       └─────┘
                    VLAN 11       VLAN 12
```

### Кабели (все Copper Straight-Through)

| Откуда | Куда |
|---|---|
| R1 FastEthernet0/1 | S2 FastEthernet0/2 |
| S2 FastEthernet0/1 | PC3 FastEthernet0 |
| R1 FastEthernet0/0 | S1 FastEthernet0/3 |
| S1 FastEthernet0/1 | PC1 FastEthernet0 |
| S1 FastEthernet0/2 | PC2 FastEthernet0 |

### Адресация по сегментам

| Сегмент | IPv4 | IPv6 | Шлюз |
|---|---|---|---|
| VLAN 10 (R1↔S1) | 10.0.0.0/30 | 2001:db8:0:a::/64 | — |
| VLAN 11 (PC1) | 172.20.11.0/24 | 2001:db8:0:b::/64 | 172.20.11.1 (SVI S1) |
| VLAN 12 (PC2) | 172.20.12.0/24 | 2001:db8:0:c::/64 | 172.20.12.1 (SVI S1) |
| Сеть PC3 | 10.10.10.0/24 | 2001:db8:f:f::/64 | 10.10.10.1 (R1 F0/1) |

Ожидаемые адреса клиентов: PC1 = 172.20.11.2, PC2 = 172.20.12.2 (.1 исключён,
сервер выдаёт с наименьшего свободного), PC3 IPv6 = 2001:db8:f:f::2.
Точные значения: R1 `show ip dhcp binding` и `show ipv6 dhcp binding`.
S2 адресов не имеет (L2) — не пингуется, это норма.
Матрица пингов: PC1→172.20.12.2, PC1/PC2→10.10.10.10, PC3→172.20.11.2 и
172.20.12.2, PC3→2001:db8:f:f::1 (v6). Первый ping теряется на ARP — `-c 3`.

SVI на S1: Vlan10 = 10.0.0.2/30, Vlan11 = 172.20.11.1/24, Vlan12 = 172.20.12.1/24.

## Сброс устройств (начать с чистого)

Роутер R1:
```
enable
write erase
reload
```
`Proceed with reload?` — Enter; после перезагрузки на
`Would you like to enter the initial configuration dialog?` — **no**.

Коммутаторы (write erase НЕ удаляет VLAN-ы, поэтому сначала vlan.dat):
```
enable
delete flash:vlan.dat
write erase
reload
```
те же ответы на вопросы перезагрузки.

PC: в настройках адаптера выставить DHCP (PC1, PC2) и статические настройки (PC3),
либо сбросить сам адаптер.

## ШАГ 0 — только для S1: приложение А (на реальном железе обязательно!)

Маршрутизация на 2960 по умолчанию выключена и включается через шаблон SDM:

```
enable
conf t
sdm prefer lanbase-routing
end
copy running-config startup-config
reload
```

Только после перезагрузки `ip routing` начнёт приниматься. Если
`sdm prefer lanbase-routing` неизвестна или `ip routing` после reload снова
«invalid input» — на свиче образ LAN Lite, маршрутизацию он не потянет:
сохранить `show version` и `show sdm prefer`, нужен план Б (router-on-a-stick).

## Конфиг R1 (2801)

```
enable
conf t
hostname R1
interface f0/0
 ip address 10.0.0.1 255.255.255.252
 ipv6 address 2001:db8:0:a::1/64
 no shutdown
exit
interface f0/1
 ip address 10.10.10.1 255.255.255.0
 ipv6 address 2001:db8:f:f::1/64
 no shutdown
exit
ip dhcp excluded-address 172.20.11.1
ip dhcp excluded-address 172.20.12.1
ip dhcp pool VLAN11
 network 172.20.11.0 255.255.255.0
 default-router 172.20.11.1
exit
ip dhcp pool VLAN12
 network 172.20.12.0 255.255.255.0
 default-router 172.20.12.1
exit
ipv6 unicast-routing
ipv6 dhcp pool PC3
 address prefix 2001:db8:f:f::/64
 dns-server 2001:db8:f:f::eeee
exit
interface f0/1
 ipv6 dhcp server PC3
 ipv6 nd managed-config-flag
exit
ip route 172.20.11.0 255.255.255.0 10.0.0.2
ip route 172.20.12.0 255.255.255.0 10.0.0.2
ipv6 route 2001:db8:0:b::/64 f0/0 2001:db8:0:a::2
ipv6 route 2001:db8:0:c::/64 f0/0 2001:db8:0:a::2
end
copy running-config startup-config
```

Форма `ipv6 route <префикс> <интерфейс> <next-hop>` используется потому, что
часть версий IOS на 2800-х не принимает рекурсивную форму (только префикс +
next-hop) — это канонический синтаксис, на работу не влияет.

## Конфиг S1 (вводить ПОСЛЕ шага 0 — sdm + reload)

IPv6-команды на старых 2960 могут не приняться — их можно пропустить:
по заданию IPv6 нужен только между R1 и PC3, к S1 он не заходит.

```
enable
conf t
hostname S1
ip routing
vlan 10
vlan 11
vlan 12
exit
interface f0/3
 switchport mode access
 switchport access vlan 10
 no shutdown
exit
interface f0/1
 switchport mode access
 switchport access vlan 11
 no shutdown
exit
interface f0/2
 switchport mode access
 switchport access vlan 12
 no shutdown
exit
interface vlan 10
 ip address 10.0.0.2 255.255.255.252
 ipv6 address 2001:db8:0:a::2/64
 no shutdown
exit
interface vlan 11
 ip address 172.20.11.1 255.255.255.0
 ipv6 address 2001:db8:0:b::1/64
 ip helper-address 10.0.0.1
 no shutdown
exit
interface vlan 12
 ip address 172.20.12.1 255.255.255.0
 ipv6 address 2001:db8:0:c::1/64
 ip helper-address 10.0.0.1
 no shutdown
exit
ip route 0.0.0.0 0.0.0.0 10.0.0.1
ipv6 route ::/0 Vlan10 2001:db8:0:a::1
end
copy running-config startup-config
```

## Промежуточная проверка без PC

Нюанс: SVI поднимается только если в VLAN есть порт с активным линком.
Без PC `Vlan11/12` будут down/down — это норма. Пинги до 172.20.11.1/12.1 пойдут,
если воткнуть хоть что-то с живой сетевухой (даже ноут без настройки).

На R1:
- `show ip interface brief` — F0/0, F0/1 up/up
- `ping 10.0.0.2` — отвечает SVI VLAN 10 на S1
- `show ip dhcp pool` — оба пула, 0 leased
- `show ip dhcp binding` — No bindings (норма)
- `show ipv6 dhcp pool` — пул PC3, Active clients: 0
- `show ipv6 interface f0/1` — есть строка про ND managed config flag

На S1:
- `show ip interface brief` — Vlan10 up/up
- `show vlan brief` — VLAN 10/11/12, порты в нужных VLAN
- `show ip route` — S* 0.0.0.0/0 → 10.0.0.1
- `ping 10.0.0.1`, `ping 10.10.10.1` — отвечают

Наблюдать DHCP вживую на R1: `debug ip dhcp server packet`
(выключить — `undebug all`). Любой ноутбук с Ethernet в F0/1 = тест
ретрансляции без «настоящих» PC.

## Сброс DHCP на защите

Уровень 1 — очистить аренды и показать заново:
```
R1# clear ip dhcp binding *
R1# clear ipv6 dhcp binding
R1# show ip dhcp binding          → No bindings
```
на клиентах: `sudo dhclient -r -v ethX` затем `sudo dhclient -v ethX`;
PC3: `sudo dhclient -6 -r ethX` затем `sudo dhclient -6 -v ethX`.
Лучше держать на R1 включённым `debug ip dhcp server packet` — DORA видна вживую.

Уровень 2 — только клиент: `dhclient -r -v` → `dhclient -v`. Адрес вернётся тот же
(привязка по MAC) — нормальное поведение DHCP.

На PC3 стереть всё, что дал DHCPv6, без sudo: в настройках соединения IPv6-метод
«Игнорировать» → переподключить сеть (`ip -6 a` остаётся только fe80:: — это
link-local, он не от DHCP и не стирается) → вернуть «Автоматически» → снова
переподключить. С sudo быстрее: `sudo dhclient -6 -r ethX` +
`sudo ip -6 addr flush dev ethX scope global`, затем `sudo dhclient -6 -v ethX`.
Демо с серверной стороны: `clear ipv6 dhcp binding` → `show ipv6 dhcp pool`
(Active clients: 0 → 1).

Уровень 3 — полный сброс стенда: write erase (+ delete flash:vlan.dat на свичах)
+ reload, затем шаг 0 на S1 (sdm + reload, БЕЗ него ip routing не примется),
конфиги из этого файла, dhclient на клиентах.

Вопросы по теме: аренда на IOS по умолчанию сутки (`lease <дни> [часы]` в пуле);
при недоступном сервере адрес по истечении аренды теряется (на Windows — APIPA
169.254.x.x, на Ubuntu адрес пропадает).

## Настройка PC (Ubuntu 16.04)

Имя интерфейса узнать: `ip a` (eth0 или enpXsY, ниже — ethX).

**PC1, PC2 (клиенты DHCPv4):** апплет сети → «Изменить соединения» → проводное
соединение → IPv4: способ «Автоматически (DHCP)»; IPv6: «Игнорировать».
Перезапрос: `sudo dhclient -r -v ethX && sudo dhclient -v ethX`.
Проверка: `ip a show ethX` (172.20.11.x), `ip route show default` (via 172.20.11.1),
`ping -c 3 172.20.11.1`.

**PC3 (статика IPv4 + stateful DHCPv6):** IPv4 — способ «Вручную»:
10.10.10.10/255.255.255.0, шлюз 10.10.10.1. IPv6 — способ
**«Автоматически, только DHCP»** (это и есть stateful DHCPv6). CLI-запрос:
`sudo dhclient -6 -v ethX`. Проверка: `ip -6 a` (2001:db8:f:f::x),
`ip -6 r` (default via fe80::...), `cat /etc/resolv.conf`
(nameserver 2001:db8:f:f::eeee), `ping -c 3 2001:db8:f:f::1`.

## Проверка

1. R1: `show ip dhcp binding` — две аренды (PC1, PC2).
2. R1: `show ipv6 dhcp pool` — пул PC3.
3. R1: `show ip interface brief` — F0/0 и F0/1 up/up.
4. S1: `show ip route` — default через 10.0.0.1, connected VLAN10/11/12.
5. PC1 → PC2: `ping 172.20.12.x` (первый ping может потеряться на ARP — повторить).
6. PC1 → PC3: `ping 10.10.10.10`.
7. PC3: `ipconfig` (Windows) — IPv6-адрес вида 2001:db8:f:f::x получен по DHCPv6,
   DNS = 2001:db8:f:f::eeee.

## Контрольные вопросы (типовые)

1. **Как работает DHCPv4?** Четыре сообщения DORA: Discover (клиент ищет сервер,
   broadcast), Offer (сервер предлагает адрес), Request (клиент просит предложенный),
   Ack (сервер подтверждает аренду).
2. **Зачем нужен ip helper-address?** DHCP-Discover — broadcast и не проходит через
   маршрутизатор. Ретранслятор на интерфейсе (SVI VLAN 11/12) принимает его, пересылает
   unicast на сервер 10.0.0.1, подставляя в поле giaddr адрес своего интерфейса.
3. **Как сервер выбирает пул?** По адресу giaddr: если ретранслятор указал 172.20.11.1,
   сервер выдаёт адрес из пула сети 172.20.11.0/24.
4. **Зачем ip dhcp excluded-address?** Чтобы сервер не выдавал адреса, уже занятые
   вручную (здесь — адреса шлюзов .1 на SVI).
5. **Чем stateful DHCPv6 отличается от stateless?** Stateful сервер выдаёт сам адрес
   и параметры (аналог DHCPv4, флаг M в RA), stateless — только параметры (DNS и т.п.),
   а адрес клиент получает через SLAAC (флаг O).
6. **Что делает ipv6 nd managed-config-flag?** Ставит флаг M в Router Advertisement:
   клиент должен получить адрес по stateful DHCPv6, а не SLAAC.
7. **Что даёт ip routing на коммутаторе L3?** Включает маршрутизацию между SVI:
   коммутатор пересылает пакеты между VLAN 11 и 12 и в сторону R1.
8. **Зачем sdm prefer lanbase-routing?** Загружает шаблон возможностей, который
   добавляет коммутатору 2960 функцию маршрутизации (требует перезагрузки).

## Возможные грабли (реальное железо)

- `ip routing` → invalid input: не сделан шаг 0 (sdm prefer lanbase-routing +
  reload) или на свиче LAN Lite без поддержки маршрутизации.
- `incomplete command` на `ipv6 route префикс next-hop`: вводить форму
  с выходным интерфейсом (в конфигах выше уже так). Проверить, что команда
  вводится на правильном устройстве (подсказка R1(config)# / S1(config)#).
- IPv6-команды на 2960 могут не приняться — пропустить, лабе не мешает.
- `show ipv6 interface` без строки managed flag: проверять так —
  `show run interface f0/1` (есть ли `ipv6 nd managed-config-flag` и
  `ipv6 dhcp server PC3`), `show ipv6 interface f0/1 | include managed`,
  `show ipv6 dhcp interface` («in server mode, Using pool: PC3»).
  И обязательно `show run | include unicast` — без `ipv6 unicast-routing`
  R1 не рассылает RA с флагом M, и PC3 не придёт на DHCPv6.
- На Ubuntu /etc/resolv.conf с `nameserver 127.0.0.53` — норма (заглушка
  systemd-resolved). Реальный DNS: `systemd-resolve --status | grep -iA2 dns`.
  Не появился — `nmcli con mod <имя> ipv6.dns "2001:db8:f:f::eeee"`.
- Нет IPv6 default у клиента: шлюз приходит из RA, не из DHCPv6 — чинится на R1
  (`show run | include unicast`; если нет — `conf t` → `ipv6 unicast-routing`),
  на клиенте IPv6-метод «Автоматически» + переподключение сети. Руками на PC
  (`sudo ip -6 r add ...`) — только если есть права, по заданию не нужно.
- `ip dhcp pool` не выдаёт адреса: проверь `service dhcp` (на IOS включён по
  умолчанию), excluded-address, helper-address на SVI 11/12, и что на PC выбран DHCP.
- PC не видит DHCP-сервер: линк R1↔S1 должен быть зелёным на обоих концах
  (на роутере порт поднимается только после `no shutdown`).
