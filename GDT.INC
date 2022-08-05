

; Wyswietlanie komunikatow o przeznaczeniu programu i oczekiwanie

; na potwierdzenie dalszego wykonywania programu



           call    czysc_ekran

           jmp     cd_przyg



; czyszczenie ekranu - przewiniecie okna o zero wierszy

czysc_ekran        PROC    near

           mov     ah, 06      ; przewiniecie okna

           mov     al, 0       ; zero wierszy

           mov     bh, 07      ; atrybut

           mov     ch, 0       ; wspolrzedne okna

           mov     cl, 0       ; lewy gorny rog

           mov     dh, 24      ; prawy dolny rog

           mov     dl, 79

           int     10H



           mov     ah, 2      ; ustawienie kursora w gornym lewym rogu ekranu

           mov     dl, 0

           mov     dh, 0

           mov     bh, 0

           int     10H

           ret

czysc_ekran    ENDP



cd_przyg:

           push    cs

           pop     ds





; sprawdzenie czy procesor pracuje w trybie rzeczywistym - jesli pracuje

; w trybie V86, to nastepuje zakonczenie wykonywania programu

           smsw ax      ; czytaj slowo stanu procesora

           test    ax, 1

           jz      dalej_RM   ; skok, gdy tryb rzeczywisty



; procesor pracuje w trybie V86 - koniec programu

           mov     dx, OFFSET komunikat6

           mov     ah, 9

           int     21H     ; komunikat o zakonczeniu programu

           mov     ax, 4C03H

           int     21H

dalej_RM:

           mov     dx, OFFSET komunikat1

           mov     ah, 09H

           int     21H         ; wy?wietlenie komunikatu



           mov     ah, 07H     ; oczekiwanie na nacisniecie klawisza

           int     21H

           cmp     al, 27

           je      pelny_koniec; nacisnieto Esc



; ponowne czyszczenie ekranu - przewiniecie okna o zero wierszy

           mov     ah, 06      ; przewiniecie okna

           mov     al, 0       ; zero wierszy

           mov     bh, 07      ; atrybut

           mov     ch, 0       ; wspolrzedne okna

           mov     cl, 0       ; lewy gorny rog

           mov     dh, 24      ; prawy dolny rog

           mov     dl, 79

           int     10H







;------------------------------------------------------------------------



; tworzenie deskryptor�w w globalnej tablicy przerwa� (na razie wszystkie

; pola w deskryptorach zawieraj? zera)

; we wszystkich deskryptorach bajt 6 (bity G, D, 0, AVL, rozmiar 19 - 16)

; pozostawiany jest bez zmian (zawiera zero)



;           ASSUME  ds:segment_GDT

           mov     ax, SEG segment_GDT

           mov     ds, ax          ; ladowanie rejestru DS



; tworzenie deskryptora na pozycji 2 w GDT - deskryptor ten opisuje

; segment 'nowe_dane'

           mov     bx, 2 * 8       ; offset w segmencie segment_GDT

           mov     word PTR ds:[bx], OFFSET nowe_dane_end  - 1 ; rozmiar segmentu

           mov     ax, SEG nowe_dane

           movzx   eax, ax         ; zerowanie starszej czesci EAX

           shl     eax, 4          ; w EAX adres bazowy segmentu

           mov     ds:[bx]+2,ax       ; adres bazowy 15 - 0

           rol     eax, 16         ; zamiana polowek EAX

           mov     ds:[bx]+4, al      ; adres bazowy 23 - 16

           mov     ds:[bx]+7, ah      ; adres bazowy 31 - 24

           mov     byte PTR ds:[bx]+5, 10010010B ; bajt dostepu



; tworzenie deskryptora na pozycji 3 w GDT - deskryptor ten opisuje

; segment 'ekran'

           mov     bx, 3 * 8       ; offset w segmencie segment_GDT

           mov     word PTR ds:[bx], OFFSET ekran_end  - 1 ; rozmiar segmentu

           mov     ax, SEG ekran

           movzx   eax, ax         ; zerowanie starszej czesci EAX

           shl     eax, 4          ; w EAX adres bazowy segmentu

           mov     ds:[bx]+2,ax       ; adres bazowy 15 - 0

           rol     eax, 16         ; zamiana polowek EAX

           mov     ds:[bx]+4, al      ; adres bazowy 23 - 16

           mov     ds:[bx]+7, ah      ; adres bazowy 31 - 24

           mov     byte PTR ds:[bx]+5,10010010B ; bajt dostepu



; tworzenie deskryptora na pozycji 6 w GDT - deskryptor ten opisuje

; segment 'obsl_int'



           mov     bx, 6 * 8       ; offset w segmencie segment_GDT

           mov     word PTR ds:[bx], OFFSET obsl_int_end  - 1 ; rozmiar segmentu

           mov     ax, SEG obsl_int

           movzx   eax, ax         ; zerowanie starszej czesci EAX

           shl     eax, 4          ; w EAX adres bazowy segmentu

           mov     ds:[bx]+2,ax       ; adres bazowy 15 - 0

           rol     eax, 16         ; zamiana polowek EAX

           mov     ds:[bx]+4, al      ; adres bazowy 23 - 16

           mov     ds:[bx]+7, ah      ; adres bazowy 31 - 24

           mov     byte PTR ds:[bx]+5,10011010B ; bajt dostepu



; tworzenie deskryptora na pozycji 7 w GDT

           mov     bx, 7 * 8       ; offset w segmencie segment_GDT

           mov     word PTR ds:[bx], OFFSET nowy_stos_end  - 1 ; rozmiar segmentu

           mov     ax, SEG nowy_stos

           movzx   eax, ax         ; zerowanie starszej czesci EAX

           shl     eax, 4          ; w EAX adres bazowy segmentu

           mov     ds:[bx]+2,ax       ; adres bazowy 15 - 0

           rol     eax, 16         ; zamiana polowek EAX

           mov     ds:[bx]+4, al      ; adres bazowy 23 - 16

           mov     ds:[bx]+7, ah      ; adres bazowy 31 - 24

           mov     byte PTR ds:[bx]+5,10010010B ; bajt dostepu



; tworzenie deskryptora na pozycji 8 w GDT

           mov     bx, 8 * 8       ; offset w segmencie segment_GDT

           mov     word PTR ds:[bx], OFFSET rozkazy_end  - 1 ; rozmiar segmentu

           mov     ax, SEG rozkazy

           movzx   eax, ax         ; zerowanie starszej czesci EAX

           shl     eax, 4          ; w EAX adres bazowy segmentu

           mov     ds:[bx]+2,ax       ; adres bazowy 15 - 0

           rol     eax, 16         ; zamiana polowek EAX

           mov     ds:[bx]+4, al      ; adres bazowy 23 - 16

           mov     ds:[bx]+7, ah      ; adres bazowy 31 - 24

           mov     byte PTR ds:[bx]+5,10011010B ; bajt dostepu



; tworzenie deskryptora na pozycji 9 w GDT - deskryptor uzywany jako

; atrapa przy przejsciu z trybu chronionego do rzeczywistego (w tym

; przypadku adres bazowy w deskryptorze nie ma zadnego znaczenia)

           mov     bx, 9 * 8       ; offset w segmencie segment_GDT

           mov     word PTR ds:[bx],0FFFFH        ; wymagany rozmiar segmentu

; bajt dostepu musi opisywac segment danych (fikcyjny), w ktorym:

; P=1, ED=0, W=1

           mov     byte PTR ds:[bx]+5,10010010B ; bajt dost?pu



; tworzenie deskryptora na pozycji 10 w GDT - deskryptor ten opisuje

; segment kodu 'roz_zak', ktory jest wykonywany bezposrednio przed

; przejsciem do trybu rzeczywistego (pole "wielkosc" = FFFFH)

           mov     bx, 10 * 8       ; offset w segmencie segment_GDT

           mov     word PTR ds:[bx], 0FFFFH       ; rozmiar segmentu

           mov     ax, SEG roz_zak

           movzx   eax, ax         ; zerowanie starszej czesci EAX

           shl     eax, 4          ; w EAX adres bazowy segmentu

           mov     ds:[bx]+2,ax       ; adres bazowy 15 - 0

           rol     eax, 16         ; zamiana polowek EAX

           mov     ds:[bx]+4, al      ; adres bazowy 23 - 16

           mov     ds:[bx]+7, ah      ; adres bazowy 31 - 24

           mov     byte PTR ds:[bx]+5,10011010B ; bajt dostepu



