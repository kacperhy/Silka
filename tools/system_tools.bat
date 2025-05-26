@echo off
chcp 65001 >nul
REM =================================================================
REM    SYSTEM ZARZĄDZANIA SIŁOWNIĄ - NARZĘDZIA WINDOWS
REM =================================================================
REM    Plik: tools/system_tools.bat
REM    Opis: Narzędzia pomocnicze zoptymalizowane dla Windows
REM    Wymagania: Windows 10/11, Visual Studio (do kompilacji)
REM =================================================================

REM Ustawienia Windows
setlocal enabledelayedexpansion
color 0F
title System Zarządzania Siłownią - Narzędzia

:MENU
cls
echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║        SYSTEM ZARZĄDZANIA SIŁOWNIĄ - NARZĘDZIA           ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.
echo  💻 GŁÓWNE OPERACJE:
echo  1. 🚀 Uruchom system
echo  2. 💾 Backup bazy danych  
echo  3. 🔄 Przywróć backup
echo  4. 📊 Sprawdź status systemu
echo.
echo  🧹 MAINTENANCE:
echo  5. 🗑️  Wyczyść pliki tymczasowe
echo  6. 📝 Generuj przykładowe dane
echo  7. 📦 Eksport wszystkich danych
echo  8. 🔍 Sprawdź integralność bazy
echo.
echo  🛠️ NARZĘDZIA DEWELOPERSKIE:
echo  9. ⚙️  Kompiluj w Visual Studio
echo  10. 📁 Otwórz folder projektu
echo  11. 🌐 Otwórz dokumentację
echo  12. 💡 Pomoc techniczna
echo.
echo  0. ❌ Wyjście
echo.
echo ╚═══════════════════════════════════════════════════════════╝
echo.
set /p choice="Wybierz opcję (0-12): "

if "%choice%"=="1" goto RUN_SYSTEM
if "%choice%"=="2" goto BACKUP_DB
if "%choice%"=="3" goto RESTORE_DB
if "%choice%"=="4" goto CHECK_STATUS
if "%choice%"=="5" goto CLEANUP
if "%choice%"=="6" goto GENERATE_SAMPLE
if "%choice%"=="7" goto EXPORT_ALL
if "%choice%"=="8" goto CHECK_INTEGRITY
if "%choice%"=="9" goto COMPILE_VS
if "%choice%"=="10" goto OPEN_FOLDER
if "%choice%"=="11" goto OPEN_DOCS
if "%choice%"=="12" goto TECH_HELP
if "%choice%"=="0" goto EXIT

echo Nieprawidłowy wybór!
pause
goto MENU

:RUN_SYSTEM
cls
echo.
echo =========================================================
echo                URUCHAMIANIE SYSTEMU
echo =========================================================
echo.

REM Sprawdź czy plik wykonywalny istnieje
if not exist "SilowkaSystem.exe" (
    echo BŁĄD: Nie znaleziono pliku SilowkaSystem.exe
    echo.
    echo Możliwe rozwiązania:
    echo 1. Skompiluj projekt używając Visual Studio
    echo 2. Upewnij się, że jesteś w prawidłowym folderze
    echo 3. Sprawdź czy plik nie został przeniesiony
    echo.
    pause
    goto MENU
)

echo Uruchamianie Systemu Zarządzania Siłownią...
echo.
SilowkaSystem.exe

echo.
echo System został zamknięty.
pause
goto MENU

:BACKUP_DB
cls
echo.
echo =========================================================
echo                  BACKUP BAZY DANYCH
echo =========================================================
echo.

REM Sprawdź czy baza danych istnieje
if not exist "silownia.db" (
    echo BŁĄD: Nie znaleziono pliku bazy danych silownia.db
    echo Prawdopodobnie system nie był jeszcze uruchamiany.
    echo.
    pause
    goto MENU
)

REM Utwórz folder backups jeśli nie istnieje
if not exist "backups" mkdir backups

REM Wygeneruj nazwę pliku backup z datą i czasem
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
set "HH=%dt:~8,2%" & set "MIN=%dt:~10,2%" & set "SS=%dt:~12,2%"
set "datestamp=%YYYY%-%MM%-%DD%_%HH%-%MIN%-%SS%"

set "backup_file=backups\silownia_backup_%datestamp%.db"

echo Tworzenie kopii zapasowej...
echo Źródło: silownia.db
echo Cel: %backup_file%
echo.

copy "silownia.db" "%backup_file%" >nul

if exist "%backup_file%" (
    echo ✓ Backup został utworzony pomyślnie!
    echo.
    echo Plik: %backup_file%
    echo Rozmiar: 
    for %%I in ("%backup_file%") do echo %%~zI bajtów
    echo.
    
    REM Pokaż dostępne backupy
    echo Dostępne kopie zapasowe:
    echo ------------------------
    dir backups\silownia_backup_*.db /b
) else (
    echo ✗ BŁĄD: Nie udało się utworzyć backupu!
)

echo.
pause
goto MENU

:RESTORE_DB
cls
echo.
echo =========================================================
echo              PRZYWRACANIE Z BACKUPU
echo =========================================================
echo.

REM Sprawdź czy folder backups istnieje
if not exist "backups" (
    echo BŁĄD: Folder backups nie istnieje.
    echo Nie ma dostępnych kopii zapasowych.
    echo.
    pause
    goto MENU
)

REM Pokaż dostępne backupy
echo Dostępne kopie zapasowe:
echo ========================
dir backups\silownia_backup_*.db /b 2>nul

if errorlevel 1 (
    echo Brak dostępnych kopii zapasowych.
    echo.
    pause
    goto MENU
)

echo.
set /p backup_name="Podaj nazwę pliku backup (bez ścieżki): "

if not exist "backups\%backup_name%" (
    echo BŁĄD: Plik backups\%backup_name% nie istnieje!
    echo.
    pause
    goto MENU
)

echo.
echo UWAGA: Ta operacja zastąpi obecną bazę danych!
echo.
set /p confirm="Czy na pewno chcesz kontynuować? (T/N): "

if /i not "%confirm%"=="T" (
    echo Operacja anulowana.
    echo.
    pause
    goto MENU
)

REM Utwórz backup obecnej bazy przed przywróceniem
if exist "silownia.db" (
    echo Tworzenie kopii bezpieczeństwa obecnej bazy...
    copy "silownia.db" "silownia_before_restore.db" >nul
)

echo.
echo Przywracanie z backupu...
copy "backups\%backup_name%" "silownia.db" >nul

if exist "silownia.db" (
    echo ✓ Baza danych została przywrócona pomyślnie!
    echo.
    echo Przywrócono z: backups\%backup_name%
    echo Kopia bezpieczeństwa: silownia_before_restore.db
) else (
    echo ✗ BŁĄD: Nie udało się przywrócić bazy danych!
)

echo.
pause
goto MENU

:CHECK_STATUS
cls
echo.
echo =========================================================
echo                 STATUS SYSTEMU
echo =========================================================
echo.

echo Sprawdzanie plików systemu...
echo =============================

REM Sprawdź główne pliki
if exist "SilowkaSystem.exe" (
    echo ✓ Plik wykonywalny: SilowkaSystem.exe
    for %%I in ("SilowkaSystem.exe") do echo   Rozmiar: %%~zI bajtów, Data: %%~tI
) else (
    echo ✗ BRAK: SilowkaSystem.exe
)

if exist "silownia.db" (
    echo ✓ Baza danych: silownia.db
    for %%I in ("silownia.db") do echo   Rozmiar: %%~zI bajtów, Data: %%~tI
) else (
    echo ✗ BRAK: silownia.db (zostanie utworzona przy pierwszym uruchomieniu)
)

if exist "config.ini" (
    echo ✓ Konfiguracja: config.ini
) else (
    echo ! BRAK: config.ini (zostanie utworzona z domyślnymi ustawieniami)
)

echo.
echo Sprawdzanie folderów...
echo =======================

if exist "backups" (
    echo ✓ Folder backups istnieje
    dir backups\*.db /b 2>nul | find /c /v "" > temp_count.txt
    set /p backup_count=<temp_count.txt
    del temp_count.txt
    echo   Liczba backupów: %backup_count%
) else (
    echo ! Folder backups nie istnieje (zostanie utworzony automatycznie)
)

if exist "raporty" (
    echo ✓ Folder raporty istnieje
) else (
    echo ! Folder raporty nie istnieje (zostanie utworzony automatycznie)
)

echo.
echo Sprawdzanie miejsca na dysku...
echo ===============================
for /f "tokens=3" %%a in ('dir /-c ^| find "bytes free"') do echo Wolne miejsce: %%a bajtów

echo.
pause
goto MENU

:CLEANUP
cls
echo.
echo =========================================================
echo           CZYSZCZENIE PLIKÓW TYMCZASOWYCH
echo =========================================================
echo.

echo Usuwanie plików tymczasowych...
echo.

REM Usuń pliki tymczasowe
if exist "*.tmp" (
    echo Usuwanie plików *.tmp...
    del *.tmp /q
)

if exist "*.log" (
    echo Usuwanie plików *.log...
    del *.log /q
)

if exist "temp_*.*" (
    echo Usuwanie plików temp_*.*...
    del temp_*.* /q
)

REM Usuń stare raporty (starsze niż 30 dni)
echo Sprawdzanie starych raportów...
forfiles /p raporty /s /m *.* /d -30 /c "cmd /c echo Usuwanie: @path & del @path" 2>nul

echo.
echo ✓ Czyszczenie zakończone.
echo.
pause
goto MENU

:GENERATE_SAMPLE
cls
echo.
echo =========================================================
echo            GENEROWANIE PRZYKŁADOWYCH DANYCH
echo =========================================================
echo.

echo Tworzenie pliku z przykładowymi danymi...
echo.

REM Utwórz przykładowe dane CSV dla klientów
echo id,first_name,last_name,email,phone,birth_date,registration_date,notes > sample_clients.csv
echo 1,Jan,Kowalski,jan.kowalski@email.com,123-456-789,1990-05-15,2025-01-15,Regularny klient >> sample_clients.csv
echo 2,Anna,Nowak,anna.nowak@email.com,987-654-321,1985-08-22,2025-02-01,Student >> sample_clients.csv
echo 3,Piotr,Wiśniewski,piotr.w@email.com,555-123-456,1992-12-10,2025-02-15,Preferencje wieczorne >> sample_clients.csv
echo 4,Maria,Kowalczyk,maria.k@email.com,111-222-333,1988-03-05,2025-03-01,Karnet rodzinny >> sample_clients.csv
echo 5,Tomasz,Lewandowski,tomasz.l@email.com,999-888-777,1995-07-18,2025-03-15,Nowy członek >> sample_clients.csv

REM Utwórz przykładowe dane CSV dla karnetów
echo id,client_id,type,start_date,end_date,price,is_active > sample_memberships.csv
echo 1,1,normalny_miesieczny,2025-01-15,2025-02-15,120.00,1 >> sample_memberships.csv
echo 2,2,student_kwartalny,2025-02-01,2025-05-01,200.00,1 >> sample_memberships.csv
echo 3,3,normalny_roczny,2025-02-15,2026-02-15,1000.00,1 >> sample_memberships.csv
echo 4,4,student_miesieczny,2025-03-01,2025-04-01,80.00,1 >> sample_memberships.csv
echo 5,1,normalny_kwartalny,2025-02-15,2025-05-15,300.00,1 >> sample_memberships.csv

echo ✓ Utworzono pliki z przykładowymi danymi:
echo   - sample_clients.csv (5 klientów)
echo   - sample_memberships.csv (5 karnetów)
echo.
echo Te pliki możesz zaimportować do systemu używając funkcji Import/Export.
echo.
pause
goto MENU

:EXPORT_ALL
cls
echo.
echo =========================================================
echo             EKSPORT WSZYSTKICH DANYCH
echo =========================================================
echo.

if not exist "silownia.db" (
    echo BŁĄD: Baza danych nie istnieje!
    echo.
    pause
    goto MENU
)

REM Utwórz folder eksportu z datą
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
set "export_folder=export_%YYYY%-%MM%-%DD%"

if not exist "%export_folder%" mkdir "%export_folder%"

echo Eksportowanie danych do folderu: %export_folder%
echo.

REM Skopiuj bazę danych
echo Kopiowanie bazy danych...
copy "silownia.db" "%export_folder%\silownia_backup.db" >nul

REM Skopiuj konfigurację
if exist "config.ini" (
    echo Kopiowanie konfiguracji...
    copy "config.ini" "%export_folder%\config.ini" >nul
)

REM Skopiuj raporty
if exist "raporty" (
    echo Kopiowanie raportów...
    xcopy raporty "%export_folder%\raporty" /s /i /q >nul 2>&1
)

echo.
echo ✓ Eksport zakończony pomyślnie!
echo.
echo Zawartość eksportu:
echo ===================
dir "%export_folder%" /b

echo.
pause
goto MENU

:CHECK_INTEGRITY
cls
echo.
echo =========================================================
echo            SPRAWDZANIE INTEGRALNOŚCI BAZY
echo =========================================================
echo.

if not exist "silownia.db" (
    echo BŁĄD: Baza danych nie istnieje!
    echo.
    pause
    goto MENU
)

echo Sprawdzanie integralności bazy danych...
echo.

REM Sprawdź rozmiar pliku
for %%I in ("silownia.db") do (
    echo Rozmiar bazy: %%~zI bajtów
    echo Data modyfikacji: %%~tI
)

echo.
echo Próba otwarcia bazy danych...

REM Utwórz prosty test SQL (wymaga sqlite3.exe)
if exist "sqlite3.exe" (
    echo Wykonywanie testów SQL...
    echo .tables > test_query.sql
    sqlite3.exe silownia.db < test_query.sql > test_result.txt 2>&1
    
    if exist "test_result.txt" (
        echo.
        echo Tabele w bazie danych:
        type test_result.txt
        del test_query.sql
        del test_result.txt
    )
    
    echo.
    echo ✓ Baza danych wydaje się być w porządku.
) else (
    echo UWAGA: Brak narzędzia sqlite3.exe
    echo Nie można wykonać szczegółowego testu integralności.
    echo.
    echo Podstawowy test: Plik istnieje i ma rozmiar większy od 0.
    for %%I in ("silownia.db") do (
        if %%~zI GTR 0 (
            echo ✓ Test podstawowy: PASSED
        ) else (
            echo ✗ Test podstawowy: FAILED - plik jest pusty!
        )
    )
)

echo.
pause
goto MENU

:COMPILE_VS
cls
echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║              KOMPILACJA W VISUAL STUDIO                   ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.

REM Sprawdź czy Project1.sln istnieje
if not exist "Project1.sln" (
    echo ❌ BŁĄD: Nie znaleziono pliku Project1.sln
    echo.
    echo 📝 Możliwe rozwiązania:
    echo 1. Upewnij się że jesteś w głównym folderze projektu
    echo 2. Sprawdź czy plik Project1.sln nie został przeniesiony
    echo.
    pause
    goto MENU
)

echo 🔍 Sprawdzanie Visual Studio...
echo.

REM Próbuj znaleźć Visual Studio
set "VS_PATH="
for %%i in (2022 2019 2017) do (
    if exist "C:\Program Files\Microsoft Visual Studio\%%i\Community\Common7\IDE\devenv.exe" (
        set "VS_PATH=C:\Program Files\Microsoft Visual Studio\%%i\Community\Common7\IDE\devenv.exe"
        echo ✅ Znaleziono Visual Studio %%i Community
        goto :found_vs
    )
    if exist "C:\Program Files\Microsoft Visual Studio\%%i\Professional\Common7\IDE\devenv.exe" (
        set "VS_PATH=C:\Program Files\Microsoft Visual Studio\%%i\Professional\Common7\IDE\devenv.exe"
        echo ✅ Znaleziono Visual Studio %%i Professional
        goto :found_vs
    )
    if exist "C:\Program Files\Microsoft Visual Studio\%%i\Enterprise\Common7\IDE\devenv.exe" (
        set "VS_PATH=C:\Program Files\Microsoft Visual Studio\%%i\Enterprise\Common7\IDE\devenv.exe"
        echo ✅ Znaleziono Visual Studio %%i Enterprise
        goto :found_vs
    )
)

echo ❌ Nie znaleziono Visual Studio
echo.
echo 📥 Pobierz Visual Studio Community (darmowe):
echo https://visualstudio.microsoft.com/vs/community/
echo.
echo 📋 Podczas instalacji wybierz:
echo - Desktop development with C++
echo - Windows 10/11 SDK
echo.
pause
goto MENU

:found_vs
echo.
echo 🚀 Otwieranie projektu w Visual Studio...
echo 📁 Projekt: Project1.sln
echo 🛠️  IDE: %VS_PATH%
echo.

start "" "%VS_PATH%" "Project1.sln"

echo ✅ Visual Studio zostało uruchomione z projektem
echo.
echo 📋 Kroki kompilacji:
echo 1. Wybierz konfigurację: Release (zalecane) lub Debug
echo 2. Wybierz platformę: x64 (zalecane) lub x86
echo 3. Build → Build Solution (Ctrl+Shift+B)
echo 4. Sprawdź Output window
echo 5. Uruchom: Debug → Start Without Debugging (Ctrl+F5)
echo.
echo 📁 Skompilowany plik znajdziesz w:
echo    x64\Release\SilowkaSystem.exe
echo.
pause
goto MENU

:OPEN_FOLDER
cls
echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║                OTWIERANIE FOLDERÓW                        ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.

echo 📁 Co chcesz otworzyć?
echo.
echo 1. 🏠 Główny folder projektu
echo 2. 💾 Folder backups
echo 3. 📊 Folder raporty  
echo 4. 📦 Folder x64\Release (skompilowane pliki)
echo 5. 🛠️  Folder tools
echo.
echo 0. ⬅️  Powrót
echo.
set /p folder_choice="Wybierz opcję (0-5): "

if "%folder_choice%"=="1" (
    start explorer .
    echo ✅ Otwarto główny folder projektu
)
if "%folder_choice%"=="2" (
    if not exist "backups" mkdir backups
    start explorer backups
    echo ✅ Otwarto folder backups
)
if "%folder_choice%"=="3" (
    if not exist "raporty" mkdir raporty  
    start explorer raporty
    echo ✅ Otwarto folder raporty
)
if "%folder_choice%"=="4" (
    if exist "x64\Release" (
        start explorer x64\Release
        echo ✅ Otwarto folder x64\Release
    ) else (
        echo ❌ Folder x64\Release nie istnieje
        echo 💡 Skompiluj projekt najpierw (opcja 9)
    )
)
if "%folder_choice%"=="5" (
    start explorer tools
    echo ✅ Otwarto folder tools
)
if "%folder_choice%"=="0" (
    goto MENU
)

echo.
pause
goto MENU

:OPEN_DOCS
cls
echo.
echo ╔═══════════════════════════════════════════════════════════╗  
echo ║                    DOKUMENTACJA                           ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.

echo 📚 Dostępna dokumentacja:
echo.

if exist "README.md" (
    echo ✅ README.md - Główna dokumentacja projektu
) else (
    echo ❌ README.md - Nie znaleziono
)

if exist "instrukcja_uzytkownika.md" (
    echo ✅ instrukcja_uzytkownika.md - Przewodnik użytkownika  
) else (
    echo ❌ instrukcja_uzytkownika.md - Nie znaleziono
)

if exist "config.ini.example" (
    echo ✅ config.ini.example - Przykładowa konfiguracja
) else (
    echo ❌ config.ini.example - Nie znaleziono
)

echo.
echo 🌐 Co chcesz otworzyć?
echo.
echo 1. 📖 README.md (w domyślnym edytorze)
echo 2. 📋 Instrukcja użytkownika  
echo 3. ⚙️  Przykładowa konfiguracja
echo 4. 🌍 Cały folder w Eksploratorze
echo.
echo 0. ⬅️  Powrót
echo.
set /p doc_choice="Wybierz opcję (0-4): "

if "%doc_choice%"=="1" (
    if exist "README.md" (
        start README.md
        echo ✅ Otwarto README.md
    ) else (
        echo ❌ Plik README.md nie istnieje
    )
)
if "%doc_choice%"=="2" (
    if exist "instrukcja_uzytkownika.md" (
        start instrukcja_uzytkownika.md
        echo ✅ Otwarto instrukcję użytkownika
    ) else (
        echo ❌ Plik instrukcja_uzytkownika.md nie istnieje
    )
)
if "%doc_choice%"=="3" (
    if exist "config.ini.example" (
        start config.ini.example
        echo ✅ Otwarto przykładową konfigurację
    ) else (
        echo ❌ Plik config.ini.example nie istnieje
    )
)
if "%doc_choice%"=="4" (
    start explorer .
    echo ✅ Otwarto folder w Eksploratorze
)
if "%doc_choice%"=="0" (
    goto MENU
)

echo.
pause
goto MENU

:TECH_HELP
cls
echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║                  POMOC TECHNICZNA                         ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.

echo 🆘 NAJCZĘSTSZE PROBLEMY I ROZWIĄZANIA:
echo.

echo ❓ PROBLEM: "Aplikacja nie uruchamia się"
echo 💡 ROZWIĄZANIE:
echo    1. Sprawdź czy sqlite3.dll jest w folderze z .exe
echo    2. Uruchom jako Administrator
echo    3. Zainstaluj Visual C++ Redistributable
echo    4. Sprawdź czy masz uprawnienia do folderu
echo.

echo ❓ PROBLEM: "Nie można kompilować w Visual Studio"  
echo 💡 ROZWIĄZANIE:
echo    1. Sprawdź czy masz Desktop development with C++
echo    2. Zainstaluj Windows SDK 10/11
echo    3. Ustaw C++ Language Standard na C++17
echo    4. Dodaj sqlite3.lib do Linker Dependencies
echo.

echo ❓ PROBLEM: "Błąd bazy danych"
echo 💡 ROZWIĄZANIE:
echo    1. Sprawdź czy folder nie jest Read-Only
echo    2. Uruchom diagnostykę (opcja 4 w menu głównym)
echo    3. Przywróć z backupu (opcja 3)
echo    4. Usuń silownia.db - zostanie utworzona nowa
echo.

echo ❓ PROBLEM: "Polskie znaki nie wyświetlają się"
echo 💡 ROZWIĄZANIE:
echo    1. Użyj Command Prompt zamiast PowerShell
echo    2. Sprawdź kodowanie konsoli: chcp 65001
echo    3. Ustaw font konsoli na Consolas lub Courier New
echo.

echo.
echo 🔧 NARZĘDZIA DIAGNOSTYCZNE:
echo.
echo 1. 🏥 Szybka diagnostyka systemu
echo 2. 💾 Informacje o bazie danych
echo 3. 🖥️  Informacje o systemie Windows
echo 4. 📋 Generuj raport diagnostyczny
echo.
echo 0. ⬅️  Powrót
echo.
set /p help_choice="Wybierz opcję (0-4): "

if "%help_choice%"=="1" (
    echo.
    echo 🔍 Uruchamianie diagnostyki...
    echo.
    
    REM Sprawdź główne pliki
    if exist "SilowkaSystem.exe" (
        echo ✅ SilowkaSystem.exe - OK
    ) else (
        echo ❌ SilowkaSystem.exe - BRAK
    )
    
    if exist "silownia.db" (
        echo ✅ silownia.db - OK
    ) else (
        echo ⚠️  silownia.db - Zostanie utworzona przy pierwszym uruchomieniu
    )
    
    if exist "sqlite3.dll" (
        echo ✅ sqlite3.dll - OK
    ) else (
        echo ❌ sqlite3.dll - BRAK (może powodować błędy)
    )
    
    REM Sprawdź foldery
    if exist "backups" (
        echo ✅ Folder backups - OK
    ) else (
        echo ⚠️  Folder backups - Zostanie utworzony automatycznie
    )
    
    REM Sprawdź miejsce na dysku
    echo.
    echo 💾 Miejsce na dysku:
    for /f "tokens=3" %%a in ('dir /-c ^| find "bytes free"') do echo    Wolne: %%a bajtów
)

if "%help_choice%"=="2" (
    echo.
    echo 🗃️  INFORMACJE O BAZIE DANYCH:
    echo.
    
    if exist "silownia.db" (
        for %%I in ("silownia.db") do (
            echo 📁 Plik: silownia.db
            echo 📏 Rozmiar: %%~zI bajtów
            echo 📅 Data modyfikacji: %%~tI
        )
        
        echo.
        echo 📊 Kopie zapasowe:
        if exist "backups\*.db" (
            dir backups\silownia_backup_*.db /b 2>nul | find /c /v "" > temp_count.txt
            set /p backup_count=<temp_count.txt
            del temp_count.txt
            echo    Liczba backupów: !backup_count!
        ) else (
            echo    Brak kopii zapasowych
        )
    ) else (
        echo ❌ Baza danych nie istnieje
        echo 💡 Zostanie utworzona przy pierwszym uruchomieniu systemu
    )
)

if "%help_choice%"=="3" (
    echo.
    echo 🖥️  INFORMACJE O SYSTEMIE:
    echo.
    
    systeminfo | findstr /B /C:"OS Name" /C:"OS Version" /C:"System Type" /C:"Total Physical Memory"
    
    echo.
    echo 🛠️  Visual Studio:
    if exist "C:\Program Files\Microsoft Visual Studio\2022" (
        echo    ✅ Visual Studio 2022 zainstalowane
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2019" (
        echo    ✅ Visual Studio 2019 zainstalowane  
    ) else (
        echo    ❌ Visual Studio nie znalezione
    )
)

if "%help_choice%"=="4" (
    echo.
    echo 📋 Generowanie raportu diagnostycznego...
    
    set "report_file=diagnostic_report_%date:~-4,4%-%date:~-10,2%-%date:~-7,2%.txt"
    
    echo System Zarządzania Siłownią - Raport Diagnostyczny > "!report_file!"
    echo Wygenerowano: %date% %time% >> "!report_file!"
    echo. >> "!report_file!"
    
    echo === PLIKI SYSTEMU === >> "!report_file!"
    dir SilowkaSystem.exe silownia.db sqlite3.dll config.ini 2>nul >> "!report_file!"
    echo. >> "!report_file!"
    
    echo === INFORMACJE O SYSTEMIE === >> "!report_file!"
    systeminfo | findstr /B /C:"OS Name" /C:"OS Version" /C:"System Type" >> "!report_file!"
    echo. >> "!report_file!"
    
    echo === MIEJSCE NA DYSKU === >> "!report_file!"
    dir /-c | find "bytes free" >> "!report_file!"
    
    echo ✅ Raport zapisano jako: !report_file!
    echo 📁 Możesz go wysłać wraz z opisem problemu
)

if "%help_choice%"=="0" (
    goto MENU
)

echo.
pause
goto MENU

:EXIT
cls
echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║                     DZIĘKUJEMY!                           ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.
echo 🏋️‍♂️ Dziękujemy za korzystanie z narzędzi 
echo    System Zarządzania Siłownią!
echo.
echo 💪 Powodzenia w zarządzaniu Twoją siłownią!
echo.
pause
exit

REM =================================================================
REM                         KONIEC SKRYPTU
REM =================================================================