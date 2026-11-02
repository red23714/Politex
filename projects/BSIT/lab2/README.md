# WinBackupService — лабораторная работа «Сервисы MS Windows»

## Что реализовано

- Нативная Windows-служба `WinBackupService` для Windows 7–10.
- Конфигурация читается из `backup.ini`, который должен лежать рядом с EXE.
- Рекурсивное наблюдение за исходной директорией через `ReadDirectoryChangesW`.
- При старте создаётся начальный ZIP; при появлении/изменении файлов выполняется пересборка архива. Файлы, совпадающие с масками, обновляются в архиве; удалённые из источника файлы также исчезают из актуальной резервной копии.
- ZIP создаётся непосредственно в программе, без запуска внешних архиваторов. Используется метод ZIP STORE (без сжатия), поэтому формат совместим с Проводником Windows и стандартными ZIP-библиотеками.
- Маски `*` и `?`, несколько шаблонов через `;`, поиск по имени файла и относительному пути.
- Команды SCP: `install`, `remove`, `start`, `stop`, `restart`, `status`.
- Обработчики SCM для STOP, SHUTDOWN и INTERROGATE.
- Логирование в `WinBackupService.log`.

## Важное замечание о ZIP

Архив при каждом событии пересобирается во временный файл `backup.zip.tmp`, после чего заменяет старый архив. Это надёжно поддерживает актуальное содержимое и не требует сложного редактирования центрального каталога ZIP. Для лабораторной работы это соответствует требованию автоматического дополнения/замены файлов, хотя на больших каталогах полная пересборка может занимать больше времени.

## Конфигурация

Скопируйте `backup.ini.example` в `backup.ini` рядом с `WinBackupService.exe` и отредактируйте параметры:

```ini
source_dir=\\server\share\source
archive_path=\\server\share\backup\backup.zip
patterns=*.txt;*.log;report?.csv
debounce_ms=1500
rescan_seconds=60
```

- `source_dir` — UNC-путь к исходной папке.
- `archive_path` — полный путь к ZIP-архиву.
- `patterns` — маски/имена через `;`.
- `debounce_ms` — задержка после файлового события, чтобы не архивировать файл посреди записи.
- `rescan_seconds` — периодическая полная пересборка на случай пропущенного события.

Служба работает под LocalSystem по умолчанию. Для сетевых UNC-папок это может быть неподходящей учётной записью: в реальной сети следует назначить службе учётную запись с правами чтения исходной папки и записи в каталог архива. Не используйте административные `C$`-шары для обычной эксплуатации. Пример с `\\localhost\C$` приведён только для краткой проверки, если административные шары доступны.

## Сборка на Linux (Arch Linux) через MinGW-w64

Установите компилятор:

```bash
sudo pacman -S mingw-w64-gcc
```

Из каталога проекта:

```bash
x86_64-w64-mingw32-g++ -std=c++17 -O2 -municode WinBackupService.cpp -o WinBackupService.exe -ladvapi32 -lshell32 -lshlwapi
```

Для 32-битного EXE (если нужно для старой 32-битной Windows/Wine):

```bash
i686-w64-mingw32-g++ -std=c++17 -O2 -municode WinBackupService.cpp -o WinBackupService.exe -ladvapi32 -lshell32 -lshlwapi
```

Примечание: конкретная комплектация пакета MinGW-w64 в дистрибутиве может отличаться; если 32-битный компилятор не установлен, используйте x86_64-сборку.

## Проверка в Wine без виртуальной машины

Wine позволяет проверить ZIP-логику в режиме `debug`, но запуск полноценной службы через SCM в Wine зависит от версии/настройки Wine и не является эквивалентом проверки в настоящей Windows.

1. Соберите EXE.
2. Создайте каталог теста и скопируйте туда EXE и конфиг:
   ```bash
   mkdir -p ~/wine-backup-test/source ~/wine-backup-test/archive
   cp WinBackupService.exe ~/wine-backup-test/
   cp backup.ini.example ~/wine-backup-test/backup.ini
   ```
3. Отредактируйте `backup.ini`, указав Wine-путь `Z:\\home\\ВАШ_ПОЛЬЗОВАТЕЛЬ\\wine-backup-test\\source` и архив `Z:\\home\\ВАШ_ПОЛЬЗОВАТЕЛЬ\\wine-backup-test\\archive\\backup.zip`. В INI записывайте обратные слеши как обычные Windows-пути, например:
   ```ini
   source_dir=Z:\home\alice\wine-backup-test\source
   archive_path=Z:\home\alice\wine-backup-test\archive\backup.zip
   patterns=*.txt;*.log
   ```
   Замените `alice` на имя своего Linux-пользователя. Для проверки в Wine разрешены пути `Z:\...`; на целевой Windows используйте UNC-пути.
4. Создайте файл и запустите:
   ```bash
   printf 'first version\n' > ~/wine-backup-test/source/demo.txt
   cd ~/wine-backup-test
   wine ./WinBackupService.exe debug
   ```
5. В другом терминале измените или добавьте файл:
   ```bash
   printf 'second version\n' >> ~/wine-backup-test/source/demo.txt
   printf 'log entry\n' > ~/wine-backup-test/source/app.log
   ```
6. Остановите debug-процесс через Ctrl+C. Проверьте `archive/backup.zip`:
   ```bash
   unzip -l ~/wine-backup-test/archive/backup.zip
   unzip -p ~/wine-backup-test/archive/backup.zip demo.txt
   ```
   Убедитесь, что `demo.txt` содержит новую версию и `app.log` есть в архиве. Также проверьте файл `WinBackupService.log`.

### Ограничение теста в Wine

`debug` запускает тот же механизм наблюдения и архивации, но не проходит через Service Control Manager.
 Для проверки `install/start/stop/restart`, команд SCM и оснастки «Службы» нужна Windows (физическая машина/виртуальная машина)
 либо специально настроенная среда Wine с работающим service manager; поведение может отличаться. Не делайте вывод о корректности
 SCM только по режиму `debug`.

## Проверка в Windows 7–10

1. Соберите EXE или перенесите собранный EXE в тестовый каталог.
2. Положите рядом `backup.ini` и задайте реальные UNC-пути, например `\\NAS\share\source` и `\\NAS\share\backup\backup.zip`.
3. Откройте cmd от имени администратора и перейдите в каталог программы.
4. Выполните:
   ```bat
   WinBackupService.exe install
   WinBackupService.exe start
   WinBackupService.exe status
   ```
5. Создайте/измените файл в исходной папке и проверьте ZIP.
6. Проверьте штатную остановку и перезапуск:
   ```bat
   WinBackupService.exe stop
   WinBackupService.exe start
   WinBackupService.exe restart
   ```
7. Через `services.msc` найдите `Windows ZIP Backup Service`; остановите и запустите службу. Проверьте, что она корректно реагирует на запросы SCM.
8. Для удаления:
   ```bat
   WinBackupService.exe stop
   WinBackupService.exe remove
   ```
   Удаление службы требует прав администратора. Если она ещё работает, Windows может отложить фактическое удаление до завершения процесса.

## Контрольные вопросы

1. **Какие операции выполняются при установке сервиса?**  
   SCP открывает SCM с нужными правами, регистрирует службу через `CreateService`, указывая имя, тип, путь к исполняемому файлу, тип запуска и политику ошибок. SCM сохраняет параметры службы в системной конфигурации. Установка регистрирует службу, но не обязательно запускает её.

2. **Что такое SCP и зачем его реализовывать?**  
   SCP (Service Control Program) — управляющая программа, использующая API SCM (`OpenSCManager`, `CreateService`, `OpenService`, `StartService`, `ControlService`, `DeleteService`). Она нужна для установки, удаления, запуска и остановки службы из командной строки без ручного управления через оснастку.

3. **Какие возможности по управлению сервисами не предоставляет оснастка «Службы»?**  
   Оснастка позволяет выполнять стандартные операции управления, но не предоставляет произвольные команды, специфичные для приложения, и не заменяет собственный SCP для автоматизации установки/удаления и сценариев командной строки. Дополнительные действия, реализованные только в SCP или отдельном приложении, например специализированные проверки конфигурации, пакетные сценарии и собственные команды, выполняются через командную строку.
