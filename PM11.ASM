Comment |
Laboratorium Oprogramowania Systemowego
Cwiczenie 1 - r.ak. 2001/2002

Program przykladowy do testowania trybu chronionego procesora Pentium
wersja 2.0 - marzec 2000
|

.386p
CR         EQU     0DH
LF         EQU     0AH

;-------------------------------------------------------------------------

; globalna tablica deskryptor�w
segment_GDT    SEGMENT     use16
           dq      0       ; poz. 0 - deskryptor zerowy (nieuzywany)
           dq      0       ; poz. 1 - selektor 0008H
           dq      0       ; poz. 2 - selektor 0010H - segment 'nowe_dane'
           dq      0       ; poz. 3 - selektor 0018H
           dq      0       ; poz. 4 - selektor 0020H
           dq      0       ; poz. 5 - selektor 0028H
           dq      0       ; poz. 6 - selektor 0030H
           dq      0       ; poz. 7 - selektor 0038H
           dq      0       ; poz. 8 - selektor 0040H
           dq      0       ; poz. 9 - selektor 0048H
           dq      0       ; poz.10 - selektor 0050H

segment_GDT_end    LABEL   byte
segment_GDT    ENDS

;-------------------------------------------------------------------

; tablica deskryptorow przerwan
segment_IDT   SEGMENT use16

; Tablica zawiera furtki prowadz?ce do procedur obs?ugi przerwa�
; i wyj?tk�w o numerach 0 - 127. Pola 'offset' w kolejnych furtkach
; zawierac beda liczby 0, 7, 14, 21, itd.

xpom=0
           REPT    128
           dw      xpom        ; offset
           dw      6 SHL 3     ; selektor
           db      0           ; licznik
           db      10000110B   ; bajt dost?pu
           dw      0           ; rezerwa
xpom=xpom+7
           ENDM

segment_IDT_end       LABEL byte
segment_IDT   ENDS

;-------------------------------------------------------------------------

; segment danych dla trybu chronionego - segment ten wskazywany jest przez
; selektor 2 * 8 = 0010H
nowe_dane  SEGMENT use16
kom4       db      '  Procesor pracuje w trybie chronionym (pm)'
           db      ' - Esc - wyjscie',0
kom5       db      'Nierozpoznane polecenie', 0
bufor_klaw db      128 dup (?)      ; bufor klawiatury
indeks_klaw dw      ?
ekran_x    db       0               ; wskaznik nr kolumny do zapisu na ekranie
ekran_y    db       1               ; wskaznik nr wiersza do zapisu na ekranie
;dane       db       4000 dup (?)    ; a tu mamy bufor na ekran


; tablica do przekodowania kodow pozycji na kody ASCII
tabl_przekA db     0, 1BH, '1234567890-=',08H, 09H     ; kody  0 - 15
            db     'qwertyuiop[]', 0DH, 0, 'as'        ; kody 16 - 31
            db     'dfghjkl;''`',0,'\zxcv'             ; kody 32 - 47
            db     'bnm,./',0,'*',0,' ',0              ; kody 48 - ...

;-----dane ogolne------------
info1 db 'Konfiguracja komputera odczytana z pamieci CMOS',0
info2 db 'Gdansk 2005',0
tekst8 db 'ESC - wyjscie',0
;----monitor----------------
monitorX db 10
monitorY db 12
tekst5 db 'Monitor type:',0
monitor1 db 'EGA  & VGA',0
monitor2 db '40x25 CGA',0
monitor3 db '80x25 CGA',0
monitor4 db 'MDA (Monochrome)',0
;-----data-----------------
tekst1 db  'Dzis mamy:',0
dataX db 70
dataY db 1
;-----czas-----------------
tekst2 db 'godz.',0
czasX db 120
czasY db 1
;-----base memory----------
tekst3 db 'Base memory:',0
Base_memoryX db 10
Base_memoryY db 8
;-----extended memory------
tekst4 db 'Extended memory:',0
Ext_memoryX db 10
Ext_memoryY db 10
;-------floppy---------------
first_floppy db 'First floppy:',0
second_floppy db 'Second floppy:',0
floppy1 db 'No drive',0
floppy2 db '360KB 5.25"',0
floppy3 db '1.2MB 5.25"',0
floppy4 db '720KB 3.5"',0
floppy5 db '1.44MB 3.5"',0
floppy6 db '2.88MB 3.5"',0
FirstFX db 10
FirstFY db 4
SecondFX db 10
SecondFY db 6
;--------data mode----------
tekst6 db 'Data mode:',0
Zbinary db 'Binary',0
ZBCD    db 'BCD',0
Data_ModeX db 10
Data_ModeY db 14
;--------hour mode-----------
tekst7 db 'Hour mode:',0
godz1 db '12h',0
godz2 db '24h',0
Hour_ModeX db 10
Hour_ModeY db 16


pomoc db (?)
atr db 17h

nowe_dane_end      LABEL byte
nowe_dane  ENDS

;-------------------------------------------------------------------------
nowy_stos  SEGMENT use16            ; stos dla trybu chronionego
pstos      dw      2048 dup (?)
nowy_stos_end      LABEL byte
nowy_stos  ENDS

;---------------------------------------------------------------------------
stos_RM       SEGMENT use16 stack   ; stos dla trybu rzeczywistego
           dw      256 dup (?)
stos_RM       ENDS

;---------------------------------------------------------------------------
ekran      SEGMENT use16 at 0B800H   ; segment imituj?cy pami?? ekranu
           db      4000 dup (?)
ekran_end  LABEL   byte
ekran      ENDS

;---------------------------------------------------------------------------
rozkazy    SEGMENT use16            ; rozkazy programu

; obszary 48-bitowe potrzebne do ladowania rejestr�w GDTR oraz IDTR

zaw_GDTR   dq      0
zaw_IDTR   dq      0

rm_idt     LABEL   qword   ; wskaznik do tabl. wekt. przerwan (tryb rzecz.)
           dw      03FFH                       ; wielko?? tablicy IDT
           dd      0

komunikat1     db  CR, LF, 'Laboratorium Oprogramowania Systemowego', CR,LF
               db  'Program przykladowy do cwiczenia 8 - '
               db  'wersja 2.0 - marzec 2000', CR, LF, LF
               db  'Aktualnie procesor pracuje w trybie rzeczywistym', CR, LF
               db  'Nacisniecie dowolnego klawisza powoduje przejscie do '
               db  'trybu chronionego', CR, LF
               db  'Esc - wyjscie z programu',CR, LF
               db  1282 dup (32)
               db  '$'

komunikat2     db  CR, LF, LF, 'Zakonczenie programu - procesor pracuje '
               db  'ponownie w trybie rzeczywistym', CR, LF, '$'
komunikat6     db  CR, LF, 'Procesor pracuje aktualnie w trybie V86', CR, LF
               db  ' - przelaczenie do trybu chroniego jest mozliwe tylko '
               db  'z trybu rzeczywistego', CR, LF, '$'

;----------------------------------------------------------------------
; kod wykonywany ponownie w trybie rzeczywistym bezposrednio przed
; zakonczeniem wykonywania programu

           ASSUME  cs:rozkazy
ptt3:
           lidt    cs:rm_idt   ; podanie adresu tabl. wektorow przerwan
                               ; dla trybu rzeczywistego
           mov     ax, SEG stos_RM ; tworzenie stosu dla trybu rzecz.
           mov     ss, ax
           mov     sp, 80H
           sti
           call    czysc_ekran
           push    cs
           pop     ds
           mov     dx, OFFSET komunikat2
           mov     ah, 09H
           int     21H         ; wy?wietlenie komunikatu
pelny_koniec:
           mov     ax,4C00H      ; zako�czenie wykonywania programu
           int     21H


;----------------------------------------------------------------------

; kod wykonywany w trybie rzeczywistym bezposrednio po uruchomieniu programu


pocz:      nop

INCLUDE    GDT.INC
; fragment programu zawarty w pliku GDT.INC wykonywany jest w trybie
; rzeczywistym bezposrednio po uruchomieniu programu; wykonywane jest tam
; czyszczenie ekranu, po czym podawane sa komunikaty informacyjne, po
; ktorych oczekuje sie na potwierdzenie dalszego wykonywania programu
; nastepnie wpisuje odpowiednio zakodowane deskryptory
; do Globalnej Tablicy Deskryptorow GDT

;------------------------------------------------------------------------

; przygotowanie do wykonania rozkazu LGDT (ladowanie rejestru GDTR)
           mov     ax, OFFSET segment_GDT_end  ; rozmiar tablicy GDT
           mov     word PTR cs:zaw_GDTR+0, ax
           mov     ax, SEG segment_GDT ; adres (segmentowy) globalnej
                                       ; tablicy deskryptorow
           movzx   eax, ax             ; zerowanie starszej czesci EAX
           shl     eax, 4              ; w EAX adres bazowy segmentu
           mov     dword PTR cs:zaw_GDTR+2, eax


; przygotowanie do wykonania rozkazu LIDT (ladowanie rejestru IDTR)
           mov     ax, OFFSET segment_IDT_end  ; rozmiar tablicy IDT
           mov     word PTR cs:zaw_IDTR+0, ax
           mov     ax, SEG segment_IDT ; adres (segmentowy) tablicy
                                       ; deskryptorow przerwan
           movzx   eax, ax             ; zerowanie starszej czesci EAX
           shl     eax, 4              ; w EAX adres bazowy segmentu
           mov     dword PTR cs:zaw_IDTR+2, eax

           cli                         ; zablokowanie przyjmowania przerwan

           lgdt    cs:zaw_GDTR         ; ladowanie rejestru GDTR
           lidt    cs:zaw_IDTR         ; ladowanie rejestru IDTR

; przelaczenie do trybu chronionego
           mov     eax, CR0            ;pierwszy bit tego rejestru na jeden to wtedy przechodzi na tryb chroniony
           or      eax, 1
           mov     CR0, eax
           jmp     next        ; oczyszczenie kolejki rozkaz�w
                               ; oczekuj?cych na wykonanie
next:      nop


; Skok daleki do nastepnej instrukcji - instrukcja ta powoduje zapisanie
; takze rejestru CS w taki sposob, ze selektor 0040H (czyli 8 * 8)
; wpisany do CS wskazuje na deskryptor w tablicy GDT, opisujacy
; niniejszy segment. Zatem bedzie dalej wykonywany kod z tego samego
; segmentu, ale rejestr CS zawiera teraz selektor, a nie adres segmentu.
; Rozkaz skoku dalekiego jest kodowany bajtowo poniewaz
; w asemblerze brak potrzebnego trybu adresowania 'jmp far PTR ca1'
           DB      0EAH
           DW      tryb_PM
           dw      8 * 8       ; selektor wskazujacy na deskryptor
                               ; segmentu 'rozkazy'
tryb_PM:   nop

;-------------------------------------------------------------------------

; przeprogramowanie uk?adu 8259, tak by przerwanie z zegara (nr 8 w trybie
; rzeczywistym) by?o zwi?zane z nr 32, przerwanie z klawiatury (nr 9)
; - z nr 33, itd.
           call    przestaw_8259

; inicjalizacja stosu - segment stosu dla trybu chronionego
; opisany jest przez deskryptor 7
           mov     ax, 7 * 8
           mov     ss, ax
           mov     sp, 1000

; inicjalizacja rejestrow DS, ES, FS, GS - wpisanie selektora wskazujacego na
; segment 'nowe_dane'
           mov     ax, 2 * 8
           mov     ds, ax
           mov     es, ax
           mov     fs, ax
           mov     gs, ax

           call zeruj_bufor_klaw   ; zerowanie bufora klawiatury

           sti                     ; wlaczenie przyjmowania przerwan


;-----------------------------------------------------------------------
; Glowna petla przyjmowania zlecen od uzytkownika w trybie chronionym


; sprawdz, czy w buforze klawiatury jest kod Esc (1BH)

        call CLRSCR
        call ramka
        call monitor
        call Base_memory
        call Extd_memory
        call Stat_reg_B
	call FLOPY_DRIVE_TYPE
czytaj_od_nowa:
           mov     ax, 2 * 8
           mov     ds, ax
klaw14:
           mov     cx, ds:indeks_klaw
           jcxz    klaw14      ; skok, gdy pusty bufor
           mov     cx, ds:indeks_klaw
ptl_klaw3:
           mov     bx, 0
ptl_klaw2:
           mov     al, ds:bufor_klaw[bx]
           call zeruj_bufor_klaw
           cmp     al, 1BH
           je      byl_Esc
           jmp czytaj_od_nowa

byl_esc:
           call zeruj_bufor_klaw

; przygotowanie do powrotu do trybu rzeczywistego -  skok daleki
; do segmentu kodu (kodowany bajtowo, poniewaz
; w asemblerze brak potrzebnego trybu adresowania 'jmp far PTR przelacz_do_RM'
           DB      0EAH
           DW      przelacz_do_RM
           dw      10 * 8      ; nr pozycji deskryptora wskazuj?cego
                               ; segment rozkazowy

;-----------------------------------------------------------------------

INCLUDE    PODPROGR.INC
; plik PODPROGR.INC zawiera podprogram zmiany trybu ukladu 8259
; oraz podprogram wyswietlania tekstu na ekranie w trybie chronionym


rozkazy_end    LABEL near
rozkazy    ENDS

;----------------------------------------------------------------------

roz_zak    SEGMENT use16
           ASSUME  cs:roz_zak
; rozkazy zawarte w tym segmencie wykonywane sa bezposrednio przed
; przejsciem z trybu chronionego do trybu rzeczywistego
przelacz_do_RM:     nop

; przywr�cenie tradycyjnych numer�w przerwa�
           cli                 ; wy??czenie przyjmowania przerwa�
           mov     al,11H
           out     20H,al
           mov     al,8
           out     21H,al
           mov     al,4
           out     21H,al
           mov     al,1
           out     21H,al      ; ladowanie ICW4
           mov     al,0BCH
           out     21H,al

; przed przejsciem do trybu rzeczywistego rejestry segmentowe powinny
; zawierac selektory wskazujace fikcyjny segment danych (zob. opis
; deskryptora 10)
           mov     ax, 9 * 8   ; deskryptor na pozycji nr 9 wskazuje
                               ; fikcyjny segment danych
           mov     ds,ax
           mov     es,ax
           mov     ss,ax
           mov     fs, ax
           mov     gs, ax

; zerowanie bitu PG w CR0
           mov     eax, CR0
           and     eax, 7FFFFFFFH
           mov     CR0, eax

; zerowanie rejestru CR3
           mov     eax, 0
           mov     CR3, eax

; zerowanie bitu PE w rejestrze CR0 (wlaczenie trybu rzeczywistego)
           mov     eax, CR0
           and     eax,0FFFFFFFEH
           mov     CR0, eax

           jmp     far PTR ptt3

roz_zak    ENDS

;-----------------------------------------------------------------------
obsl_int   SEGMENT use16
           ASSUME  cs:obsl_int

; Ka�demu przerwaniu odpowiadaj? 3 rozkazy zajmuj?ce ??cznie 7 bajt�w.
; Pierwszy z tych rozkaz�w zapami?tuje rejestr CX na stosie, za? nast?pny
; wpisuje do CX nr przerwania lub wyj?tku, co pozwala, w dalszej cz??ci
; procedury obs?ugi, zidentyfikowa? przyczyn? przerwania i ewentualnie
; podj?? dzia?ania w?a?ciwe dla zaistnia?ego zdarzenia.

xpom=0
           REPT   128
           push    cx          ; nr przerwania lub wyj?tku
           mov     cx,xpom
           jmp     proc_obsl
xpom=xpom+1
           ENDM
           ORG     $+128       ; wymusza by nie wyst?powa?y JMP SHORT

proc_obsl: sti
           push    ds
           push    es
           push    ax
           push    bx
           push    dx
           push    si

           mov     ax, 3 * 8   ; selektor pamieci ekranu
           mov     ds,ax

; wyst?pienie przerwania lub wyj?tku b?dzie sygnalizowane za pomoc?
; znaku '*' wy?wietlanego w wierszu 15 na pozycji odpowiadaj?cej
; numerowi przerwania lub wyj?tku
;           mov     bx,160 * 15
;           add     bx,cx       ; w CX znajduje si? nr przerwania lub wyj?tku
;           add     bx,cx       ; dodawanie (BX) + 2 * (CX), bo znak
                               ; zajmuje w pami?ci ekranu dwa bajty
;           mov     byte PTR ds:[bx],'*'
;           inc     bx
;           mov     byte PTR ds:[bx],17h  ; atrybut znaku

           cmp     cx,33       ; czy przerwanie z klawiatury
           jne     przeskocz_klaw

; obsluga przerwania z klawiatury
           in      al,60H      ; odczyt kodu pozycji klawisza
           xor     ah,ah       ; zerowanie rejestru AH
           push    ax          ; przechowanie rejestru AX
           in      al,61H      ; moze to byc kod przeszukiwania
           or      al,82H
           out     61H,al
           and     al,7FH
           out     61H,al
           pop     ax          ; odtworzenie rejestru AX

           cmp     al, 55
           ja      przeskocz_klaw  ; nie sa obslugiwane klawisze o kodach
                                   ; pozycji > 55

           mov     bx, 2 * 8       ; selektor do segmentu 'nowe_dane'
           mov     ds, bx

           mov     si, OFFSET tabl_przekA  ; tablica konwersji znakow
           mov     bl, al
           xor     bh, bh

           mov     al, ds:[si+bx]
           cmp     al, 0
           jne     klaw2
           mov     al, ' '

; wyswietlenie znaku na ekranie w przedostatnim wierszu
klaw2:
           mov     bx, 3 * 8   ; selektor pamieci ekranu
           mov     es, bx
           mov     bx, ds:indeks_klaw
           shl     bx, 1
           add     bx, 160 * 23 + 4   ;160 liczba bajtow w lini 23 wiersz 4 bo znak zachety
           mov     es:[bx], al

; dopisanie znaku do bufora
           cli
           mov     bx, OFFSET bufor_klaw
           add     bx, ds:indeks_klaw
           mov     ds:[bx], al
           inc     word PTR ds:indeks_klaw
           cmp     ds:indeks_klaw, 128
           jb      klaw3
           dec     word PTR ds:indeks_klaw ; dekrementacja, gdy koniec bufora
klaw3:
           sti

;--------------------------------------------
;wyswietlenie czasu na ekranie


przeskocz_klaw:
           cmp     cx,32               ; obsluga zegara
           jne     przeskocz_klaw2
           cli
           mov     ebx, 0
           mov     al, 0       ; odczytywanie sekund
           out     70h, al
           jmp     $+2         ; opoznienie
           in      al, 71h
           or      bl, al
           ror     ebx, 8
           mov     al, 2       ; odczytywanie minut
           out     70h, al
           jmp     $+2         ; opoznienie
           in      al, 71h
           or      bl, al
           ror     ebx, 8
           mov     al, 4       ; odczytywanie godzin
           out     70h, al
           jmp     $+2         ; opoznienie
           in      al, 71h
           or      bl, al
           ror     ebx, 8

; wyswietlanie na ekranie
           mov     ax, 3 * 8   ; selektor pamieci ekranu
           mov     es, ax
           rol     ebx, 4
           add     bl, '0'
           mov     es:[292], bl
           xor     bl, bl
           rol     ebx, 4
           add     bl, '0'
           mov     es:[294], bl
           mov     bl, ':'
           mov     es:[296], bl
           xor     bl, bl
           rol     ebx, 4
           add     bl, '0'
           mov     es:[298], bl
           xor     bl, bl
           rol     ebx, 4
           add     bl, '0'
           mov     es:[300], bl
           mov     bl, ':'
           mov     es:[302], bl
           xor     bl, bl
           rol     ebx, 4
           add     bl, '0'
           mov     es:[304], bl
           xor     bl, bl
           rol     ebx, 4
           add     bl, '0'
           mov     es:[306], bl

;; wyswietlanie daty
           mov     ax, 3 * 8   ; selektor pamieci ekranu
           mov     es, ax

           xor ax,ax
           mov edx,0
           mov al,7    ; dzien miesiaca
           out 70h,al
           jmp $+2
           in al,71h
           or dl,al
           ror     edx, 4
           add     dl, '0'
           mov     es:[252], dl
           xor     dl, dl
     ; zerowanie
           rol   edx,4
           add dl,'0'
           mov es:[254],dl
           xor edx,edx
           mov     dl, '.'
           mov     es:[256], dl
           mov edx,0

           mov al,8   ; miesiac
           out 70h,al
           jmp $+2
           in al,71h
           or dl,al
           ror     edx, 4
           add     dl, '0'
           mov     es:[258], dl
           xor     dl, dl
     ; zerowanie
           rol   edx,4
           add dl,'0'
           mov es:[260],dl
           xor edx,edx
           mov     dl, '.'
           mov     es:[262], dl
           mov edx,0


           mov al,9   ;rok
           out 70h,al
           jmp $+2
           in al,71h
           or dl,al
           ror edx,4
           add dl,'0'
           mov es:[264],dl
           xor dl,dl
           rol edx,4
           add dl,'0'
           mov es:[266],dl
           mov edx,0
           sti


przeskocz_klaw2:

;--------------------------------------------------------------

           mov     al, 20H
           out     20H, al    ; end of interrupt (OCW2)
           pop     si
           pop     dx
           pop     bx
           pop     ax
           pop     es
           pop     ds
           pop     cx
           iret
obsl_int_end       LABEL near
obsl_int   ENDS
           END     pocz



