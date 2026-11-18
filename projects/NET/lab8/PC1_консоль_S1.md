# PC1 — консольная станция S1

Роль: только терминал для настройки S1. **Ethernet PC1 ни к чему не подключается.**
Вся конфигурация берётся из файла `S1_настройка.md`.

## 1. Подключение и терминал
- Rollover-кабель: RJ-45 → порт **CONSOLE** на S1; второй конец (DB-9) → COM-порт PC1. Нет COM-порта — USB→COM адаптер (в системе появится, например, COM3).
- Tera Term: New connection → **Serial** → COM-порт → OK. Setup → Serial port…: **9600** baud, 8-N-1, flow control **none**. (PuTTY: Connection type Serial, COMx, Speed 9600.)
- Enter → приглашение `Switch>`.

## 2. Таймлайн работы
1. **Сброс S1** — раздел «0» файла `S1_настройка.md`. Внимание к вопросам: `vlan.dat` → Enter; `Save?` → **no**; setup dialog → **no**.
2. **База** — вставь блок раздела 1 (начни с `enable`). Проверь `show interfaces status` и `ping 192.168.1.1`.
3. **Часть 2c:** когда PC3 настроит NAT на R1 и PC4/PC3 пропингуют — в priv EXEC: `ping 209.165.200.1` → должен пройти. PC3 подтвердит трансляцию 192.168.1.11 → 209.165.200.228.
4. **Часть 3 (массовый тест):** `ping 209.165.200.1 repeat 2000`, прервать — **Ctrl+Shift+6**.

## Чек-лист
- [ ] Сброс: vlan.dat удалён, startup-config стёрт, setup dialog — «no»
- [ ] hostname S1, пароли (class / cisco), banner, service password-encryption
- [ ] Vlan1 = 192.168.1.11/24 + default-gateway 192.168.1.1
- [ ] Неиспользуемые порты погашены (Fa0/2-4, Fa0/7-24, Gi0/1-2)
- [ ] Fa0/1, Fa0/5, Fa0/6 — connected
- [ ] ping 192.168.1.1 ✔, позже ping 209.165.200.1 ✔
