; przeprogramowanie uk?adu 8259, tak by przerwanie z zegara (nr 8 w trybie
; rzeczywistym) by?o zwi?zane z nr 32, przerwanie z klawiatury (nr 9)
; - z nr 33, itd.
przestaw_8259  PROC
        mov     al,11H
        out     20H,al      ; ladowanie ICW1 (d4=1 - znacznik
                            ; wskazujacy, ze programujemy ICW1
                            ; bit d0=1 - znacznik ,ze wystapi ICW4
        mov     al,32
        out     21H,al
                            ; ustawienie bazowego wektora przerwan
                            ; (ustawienie ICW2)
        mov     al,4
        out     21H,al
                            ; ICW3 - d2=1 - tryb niebuforowany, 8259 master
        mov     al,1
        out     21H,al
                            ; ICW4 - d0=1 - potwierdzenie ICW2 jako wektora
                            ; przerwan
        ret
przestaw_8259      ENDP
;--------------------------------------------------------------------------
; wyswietlanie tekstu na ekranie - tekst zawarty jest w segmencie danych
; dla trybu chronionego (selektor 0010H), a offset podany jest w BX
; tekst wyswietlany jest w wierszu wskazanym przez zmienna ekran_y

;        ekran_x    db       0               ; wskaznik nr kolumny do zapisu na ekranie
;        ekran_y    db       0               ; wskaznik nr iersza do zapisu na ekranie
wyswietl   PROC
                            ; selektor 0010H (2 * 8) wskazuje deskryptor segmentu danych dla trybu
                            ; chronionego ('nowe_dane')
        push    ds
        push    es
        mov     ax, 2 * 8
        mov     ds, ax
        mov     ax, 3 * 8   ; selektor pamieci ekranu
        mov     es,ax
        mov     al, 160
        mul     byte PTR ds:ekran_y         ; w AX adres ekranu do zapisu
        mov     di, ax
        xor     ax,ax
        mov     al,ds:ekran_x
        add     di,ax
        mov     ah,ds:atr        ; atrybut wyswietlania
ptl_kom:
        mov     al,ds:[bx]  ; pobranie kolejnego znaku komunikatu
        or      al, al
        jz      koniec_wysw ; bajt zerowy wskazuje koniec tekstu
        mov     es:[di],ax  ; przeslanie znaku do pamieci ekranu
        inc     bx          ; inkrementacja BX
        inc     di          ; inkrementacja DI
        inc     di          ; inkrementacja DI
        jmp     ptl_kom
koniec_wysw:

        pop     es
        pop     ds
        ret
                            ; przesuwanie calego ekranu o jeden wiersz w gore

        wyswietl   ENDP
;----------------------------------------------------------------------------
; zerowanie bufora klawiatury i zerowanie wskaznika indeksowego klawiatury

zeruj_bufor_klaw   PROC
        push    ds
        push    bx
        push    cx
        mov     bx, 2 * 8   ; selektor segmentu 'nowe_dane'
        mov     ds, bx
        mov     word PTR ds:indeks_klaw, 0
        mov     cx, 128
        mov     bx, 0
        ptl_inic_klaw2:
        mov     byte PTR ds:bufor_klaw[bx], 0
        inc     bx
        loop    ptl_inic_klaw2
                            ; wpisywanie znaku zachety i wypelnianie spacjami przedostatniego
                            ; wiersza ekranu
        mov     bx, 3 * 8   ; selektor pamieci ekranu
        mov     ds, bx
        mov     byte PTR ds:[160 * 23], '#'
        mov     cx, 79
        mov     bx, 2
        ptl_wp:
        mov     byte PTR ds:[bx + 160 * 23], ' '
        add     bx, 2
        loop    ptl_wp
        pop     cx
        pop     bx
        pop     ds
        ret
zeruj_bufor_klaw   ENDP
;-------------------------------------------------------
MONITOR PROC
        push ax
        push bx

        mov al,ds:monitorX
        mov ds:ekran_x,al
        mov al,ds:monitorY
        mov ds:ekran_y,al
        mov bx,OFFSET tekst5
        call wyswietl

        add ds:ekran_x,50

        mov al,14h
        out 70h,al
        jmp $+2
        in al,71h
        and al,00110000b

        cmp al,00000000b
        jne monitor_dalej1
        mov bx,OFFSET monitor1
        call wyswietl
        jmp monitor_koniec
monitor_dalej1:
        cmp al,00010000b
        jne monitor_dalej2
        mov bx,OFFSET monitor2
        call wyswietl
        jmp monitor_koniec
monitor_dalej2:
        cmp al,00100000b
        jne monitor_dalej3
        mov bx,OFFSET monitor3
        call wyswietl
        jmp monitor_koniec
monitor_dalej3:
        mov bx,OFFSET monitor4
        call wyswietl

monitor_koniec:

        pop ax
        pop bx
        ret
MONITOR ENDP
;***********************************************************************************
RAMKA PROC
      pusha
      push es

      mov ax,3*8
      mov es,ax
      mov ax,2*8
      mov ds,ax
          ;-
      mov al,201
      mov di,0
      mov es:[di],al
          ;=====
      mov cx,15
      mov di,2
      mov al,205
rysuj1:
      mov es:[di],al
      add di,2
      loop rysuj1
           ;napis
      mov cx,47
      mov bx,0
rysuj2:
      mov al,ds:info1[bx]
      mov es:[di],al
      add di,2
      inc bx
      loop rysuj2
           ;=====
      mov cx,16
      mov al,205
rysuj3:
      mov es:[di],al
      add di,2
      loop rysuj3
           ;�
      mov al,187
      mov es:[di],al
      add di,2
          ;�
      mov al,186
      mov es:[di],al
      add di,158
      mov es:[di],al
      add di,2
          ;�
      mov al,204
      mov es:[di],al
      add di,2
          ;=
      mov al,205
      mov cx,78
rysuj4:
      mov es:[di],al
      add di,2
      loop rysuj4
           ;�
      mov al,185
      mov es:[di],al
      add di,2
           ;�
      mov al,186
      mov cx,21
rysuj5:
      mov es:[di],al
      add di,158
      mov es:[di],al
      add di,2
      loop rysuj5
           ;L
      mov al,200
      mov es:[di],al
      add di,2
           ;=
      mov cx,50
      mov al,205
rysuj6:
      mov es:[di],al
      add di,2
      loop rysuj6
           ;napis info2
      mov cx,11
      mov bx,0
rysuj7:
      mov al,ds:info2[bx]
      inc bx
      mov es:[di],al
      add di,2
      loop rysuj7
           ;=
      mov cx,17
      mov al,205
rysuj8:
      mov es:[di],al
      add di,2
      loop rysuj8
           ;-
      mov al,188
      mov es:[di],al


;--poprawka
        mov di,3360
        mov al,204
        mov es:[di],al
        add di,2

        mov cx,78
        mov al,205
poprawka1:
        mov es:[di],al
        add di,2
        loop poprawka1

        mov al,185
        mov es:[di],al

        mov di,3690
        mov bx,0
poprawka2:
        mov al,ds:tekst8[bx]
        cmp al,0
        je koniecp
        mov es:[di],al
        add di,2
        inc bx
        jmp poprawka2
koniecp:
         mov al,ds:dataX
         mov ds:ekran_x,al
         mov al,ds:dataY
         mov ds:ekran_y,al
         mov bx,OFFSET tekst1
         call wyswietl

         mov al,ds:czasX
         mov ds:ekran_x,al
         mov al,ds:czasY
         mov ds:ekran_y,al
         mov bx,OFFSET tekst2
         call wyswietl


      pop es
      popa
      ret
RAMKA ENDP
;***********************************************************************************
CLRSCR PROC
       pusha
       push es

      mov ax,3*8
      mov es,ax
      mov ax,2*8
      mov ds,ax

        mov ah,ds:Atr
        mov al,' '
        mov cx,2000
        mov di,0
petla_czysc:
        mov es:[di],ax
        inc di
        inc di
        loop petla_czysc

        pop es
        popa
        ret
CLRSCR ENDP
;***********************************************************************
BASE_MEMORY PROC
        pusha
        push es

        mov ax,2*8
        mov ds,ax
        mov ax,3*8
        mov es,ax


                ;odczyt mlodszej czesci pamieci podstawowej
        mov al,15h
        out 70h,al
        jmp $+2
        in al,71h
                ;zapisanie mlodszej czescni do zmiennej pomoc
        mov ds:pomoc,al
                ;odczytanie starszej czescni pamieci podstawowej
        mov al,16h
        out 70h,al
        jmp $+2
        in al,71h
                ;wpisanie rozmiaru calej pamieci do AX
        mov ah,al
        mov al,ds:pomoc
                ;zamiana na dziesietne

        mov dx,0
        mov bx,10
        mov cx,0

dziel_bm:
        div bx
        inc cx
        add dl,'0'
        push dx
        cmp ax,0
                ;gdy czesc calkowita  = 0
        je dalej_bm
        mov dx,0
        jmp dziel_bm

dalej_bm:
                ;wyswietlenie na ekranie
        mov al,ds:Base_memoryX
        mov ds:ekran_x,al
        mov al,ds:Base_memoryY
        mov ds:ekran_y,al
        mov bx,OFFSET tekst3
        call wyswietl

        mov al,160
        mul byte PTR ds:ekran_y
        mov di,ax
        xor ax,ax
        mov al,ds:ekran_x
        add al,50
        add di,ax

dalej_bm2:
                ;wyswietlanie wartosci base memory
wyswietl_bm:
        pop dx
        mov es:[di],dl
        add di,2
        loop wyswietl_bm

        pop es
        popa
        ret
BASE_MEMORY ENDP
;***************************************************************************************
EXTD_MEMORY PROC
        pusha
        push es

        mov ax,2*8
        mov ds,ax
        mov ax,3*8
        mov es,ax

                ;odczyt mlodszej czesci pamieci podstawowej
        mov al,17h
        out 70h,al
        jmp $+2
        in al,71h
                ;zapisanie mlodszej czescni do zmiennej pomoc
        mov ds:pomoc,al
                ;odczytanie starszej czescni pamieci podstawowej
        mov al,18h
        out 70h,al
        jmp $+2
        in al,71h
                ;wpisanie rozmiaru calej pamieci do AX
        mov ah,al
        mov al,ds:pomoc
                ;zamiana na dziesietne

        mov dx,0
        mov bx,10
        mov cx,0

dziel_em:
        div bx
        inc cx
        add dl,'0'
        push dx
        cmp ax,0
                ;gdy czesc calkowita  = 0
        je dalej_em
        mov dx,0
        jmp dziel_em

dalej_em:
                ;wyswietlenie na ekranie
        mov al,ds:Ext_memoryX
        mov ds:ekran_x,al
        mov al,ds:Ext_memoryY
        mov ds:ekran_y,al
        mov bx,OFFSET tekst4
        call wyswietl


        mov al,160
        mul byte PTR ds:ekran_y
        mov di,ax
        xor ax,ax
        mov al,ds:ekran_x
        add al,50
        add di,ax

dalej_em2:
wyswietl_em:
        pop dx
        mov es:[di],dl
        add di,2
        loop wyswietl_em

        pop es
        popa
        ret
EXTD_MEMORY ENDP
;******************************************************************************
STAT_REG_B PROC
        pusha
        push es

        mov ax,2*8
        mov ds,ax

        mov al,0Bh
        out 70h,al
        jmp $+2
        in al,71h
           ;wyswietlenie napisu 'data mode'
        mov ds:pomoc,al
        push ax
        mov al,ds:data_modeX
        mov ds:ekran_x,al
        mov al,ds:data_modeY
        mov ds:ekran_y,al
        pop ax
        mov bx,OFFSET tekst6
        call wyswietl
        
        add di,2

        and al,00000100b

        cmp al,00000100b

        je dalej_dm2
                ;BCD

        mov bx,OFFSET ZBCD
        add ds:ekran_x,50
        call wyswietl
        jmp dalej_dm3
        
dalej_dm2:
        mov bx,OFFSET Zbinary
        add ds:ekran_x,50
        call wyswietl
dalej_dm3:


;;;---------------hour mode---------

        push ax
        mov al,ds:hour_modeX
        mov ds:ekran_x,al
        mov al,ds:hour_modeY
        mov ds:ekran_y,al
        pop ax

        mov bx,OFFSET tekst7
                ;wyswietlenie napisu 'hour mode'
        call wyswietl

        mov al,ds:pomoc
        and al,00000010b
        cmp al,00000010b
        add ds:ekran_x,50
        je dalej_hm2
                ;12 godzinny
        mov bx,OFFSET godz1
        call wyswietl
        jmp dalej_hm3

dalej_hm2:
        mov bx,OFFSET godz2
                ;24 godzinny
        call wyswietl
        
dalej_hm3:


        pop es
        popa
        ret
STAT_REG_B ENDP
;***********************************************************************
FLOPY_DRIVE_TYPE PROC
        pusha
        push es

	mov ax,2*8
	mov ds,ax        

        mov al,10h
        out 70h,al
        jmp $+2
        in al,71h
            ;w zmiennej pomoc mamy informacje dotyczace floppy
        mov ds:pomoc,al
            ;pierwszy

	push ax
        mov al,ds:FirstFX
        mov ds:ekran_x,al
        mov al,ds:FirstFY
        mov ds:ekran_y,al
        pop ax

        mov bx,OFFSET first_floppy
		;wyswietlenie napisu first_floppy
	call wyswietl

        mov al,ds:pomoc
        shr al,4
        call FLOPPY_POMOC

	push ax
        mov al,ds:SecondFX
        mov ds:ekran_x,al
        mov al,ds:SecondFY
        mov ds:ekran_y,al
        pop ax

        mov bx,OFFSET second_floppy
		; wyswietlenie napisu second_floppy
	call wyswietl        

        mov al,ds:pomoc
        and al,0fh
        call FLOPPY_POMOC
        pop es
        popa
        ret
FLOPY_DRIVE_TYPE ENDP
;***************************************************************
FLOPPY_POMOC PROC

        add ds:ekran_x,50
	add di,2
	cmp al,01h
        je fp1
	cmp al,02h
	je fp2
	cmp al,03h
	je fp3
	cmp al,04h
	je fp4
	cmp al,05h
	je fp5
	jmp fp6



fp1:
        mov bx,OFFSET floppy2
	call wyswietl
        jmp koniec_fp
        
fp2:
        mov bx,OFFSET floppy3
	call wyswietl
        jmp koniec_fp

fp3:
        mov bx,OFFSET floppy4
	call wyswietl
        jmp koniec_fp

fp4:
        mov bx,OFFSET floppy5
	call wyswietl
        jmp koniec_fp

fp5:
        mov bx,OFFSET floppy6
	call wyswietl
        jmp koniec_fp

fp6:
        mov bx,OFFSET floppy1
	call wyswietl
        jmp koniec_fp

koniec_fp:

        ret
FLOPPY_POMOC ENDP
;*************************************************************************************
;ILE_FLOPPY PROC
;	pusha
;	push es
;	
;	mov ax,0B800h
;	mov es,ax
;	mov di,gdzie_ile_floppy
;
;	mov al,14h
;	out 70h,al
;	jmp $+2
;	in al,70h
;
;	and al,11000000b
;
;	cmp al,00000000b
;	jne dalej_if1
;	mov dl,'1'
;	mov es:di,dl
;	jmp koniec_if
;
;dalej_if1:
;	cmp al,01000000b	
;	jne dalej_if2
;	mov dl,'2'
;	mov es:di,dl
;	jmp koniec_if
;
;dalej_if2:
;	cmp al,10000000b	
;	jne dalej_if3
;	mov dl,'3'
;	mov es:di,dl
;	jmp koniec_if
;
;
;dalej_if3:
;	mov dl,'4'
;	mov es:di,dl
;
;koniec_if:
;	pop es
;	popa
;	ret
;ILE_FLOPPY ENDP











