; Source de ma 4Ko (Lion)

INCLUDE                 MACROS.INC

;   ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; Affiche un Lion.
LION                    MACRO
                        local   bclx1,bclx2,bclx3
                        mov     es,cs:Ecran
                        mov     cx,16
                        mov     si,OFFSET Img
                        mov     di,180*320+10
                        mov     dl,10
                        push    cs
                        pop     ds
                        
                 bclx1: lodsd
                        ror     eax,cl
                 bclx2: shl     eax,1
                        setb    bl
                        or      bl,bl
                        je      bclx3
                        mov     es:[di],bl
                 bclx3: inc     di
                        add     ch,8
                        jnz     bclx2
                        add     di,320-32
                        dec     dl
                        jnz     bclx1
                        push    cs
                        pop     es
                        ENDM
;   ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД

;  /ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД\
;  і°°°°°°°°°°°°°°°°°°° Segment Principal °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°і
;  \ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД/
CODE                    SEGMENT
                        ASSUME CS:CODE,ES:CODE,DS:CODE
                        .486P
                        ORG    100h
;   ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
START:                  cld

;/-------------------------------------------------------------------------\
;|                         INITIALIZATIONS                                 |
;\-------------------------------------------------------------------------/
; LibЉre la m‚moire non utilis‚e:
                        mov     ah,4Ah
                        mov     bx,Offset Fincode
                        shr     bx,4
                        inc     bx
                        int     21h
                        mov     sp,OFFSET Pile+8000

                        WRITE   message1
                        WRITE   message2

                        NEWP    SegPlasma,4096
                        NEWP    Virtuel,4096
                        NEWP    SegAdresse1,4096
                        NEWP    SegAdresse2,4096


                        CALL    GEN_COS     ; Pr‚pare la table de cosinus.
                        CALL    GEN_PLASMA  ; G‚nЉre le plasma.
                        CALL    GEN_ADRESSE ; Pr‚pare les adresses du tunnel.
                        CALL    GEN_NORM    ; Pr‚calcule les vecteurs normaux.
                        VGA     19          ; 320*200*256
                        CALL    FLASH
;/-------------------------------------------------------------------------\
;|                             ROTAZOOM                                    |
;\-------------------------------------------------------------------------/
                        mov     ax,SegPlasma
                        mov     SegRotazoom,ax
                        
                        CALL    CHAUD                        

                        mov     angle,0
                        mov     zoom,2000
             bclrot:    WAIT_VBL
                        CALL    ROTAZOOM
                        LION
                        sub     zoom,10
                        add     angle,10
                        in      al,60h
                        cmp     al,1
                        je      retour
                        cmp     zoom,64
                        jge     bclrot
                                                
;/-------------------------------------------------------------------------\
;|                             TRANSITION 1                                |
;\-------------------------------------------------------------------------/
                        
                        
@su:                    VGA     19
                        CALL    FLASH

                        mov     cl,20
               @vbl1:   WAIT_VBL
                        dec     cl
                        jnz     @Vbl1
                        
;/-------------------------------------------------------------------------\
;|                             TUNNEL                                      |
;\-------------------------------------------------------------------------/

                        CALL    FRESH                        

                        xor     eax,eax
                        mov     decal,ax
                        mov     count,eax
               @tun:    WAIT_VBL
                        CALL    TUNNEL
                        LION
                        in      al,60h
                        cmp     al,1
                        je      retour
                        add     decal,256+1
                        inc     count
                        cmp     count,72*30
                        jne     @tun
                        
;/-------------------------------------------------------------------------\
;|                             TRANSITION 2                                |
;\-------------------------------------------------------------------------/

         @su2:          VGA     19
                        CALL    FLASH

                        mov     cl,20
               @vbl2:   WAIT_VBL
                        dec     cl
                        jnz     @Vbl2
                        
;/-------------------------------------------------------------------------\
;|                    3D Environnement Mapping                             |
;\-------------------------------------------------------------------------/
; Transfert de SegPlasma ДД> SegAdresse1

                        mov     es,SegAdresse1
                        mov     ds,SegPlasma
                        xor     si,si
                        mov     di,si
                        mov     cx,65536/4
                        rep     movsd
                        mov     cx,65536/4
                        mov     es,cs:SegPlasma
               @tr:     mov     eax,es:[di]
                        or      eax,080808080h
                        stosd
                        dec     cx
                        jnz     @tr
                        mov     ax,cs
                        mov     ds,ax
                        mov     es,ax

                        CALL    INIT_3D
                        add     b cs:@popo+4,128
                        add     b cs:@popo1+4,128
                        add     b cs:@popo2+4,128
                        add     b cs:@popo3+4,128
                        CALL    FRESH
                        CALL    CHAUD                        
                        
                        CALL    MAIN_3D

;/-------------------------------------------------------------------------\
;|               RETOUR AU DOS ET REMISE DES SEGMENTS ALLOU‚S.             |
;\-------------------------------------------------------------------------/

 Retour:                VGA     3
                        WRITE   message1
                        DISPOS  SegPlasma
                        DISPOS  Virtuel
                        DISPOS  SegAdresse1
                        DISPOS  SegAdresse2
                        sti
                        DOS_   0

;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; Cr‚ation d'une palette.
; C(c1,c2,c3)ДД>D(d1,d2,d3)
MAKE_PAL                proc    near
                        mov     al,d
                        sub     al,c
                        cbw
                        mov     bx,ax
                        mov     dx,3C8h
                        mov     al,c
                        out     dx,al
                        xor     cx,cx
        
                @bclPal:mov     al,d1  ; r1 := i*(d1-c1) DIV (b-a) + c1
                        sub     al,c1
                        cbw
                        imul    cx
                        idiv    bx
                        add     al,c1
                        mov     dx,03C9h
                        out     dx,al

                        mov     al,d2  ; g1 := i*(d2-c2) DIV (b-a) + c2
                        sub     al,c2
                        cbw
                        imul    cx
                        idiv    bx
                        add     al,c2
                        mov     dx,03C9h
                        out     dx,al

                        mov     al,d3  ; b1 := i*(d3-c3) DIV (b-a) + c3
                        sub     al,c3
                        cbw
                        imul    cx
                        idiv    bx
                        add     al,c3
                        mov     dx,03C9h
                        out     dx,al
                        
                        inc     cx
                        cmp     cx,bx
                        jle     @bclPal
                        ret
MAKE_PAL                endp
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; G‚nЉre une table de cosinus.

GEN_COS         proc    near
                mov     Cos_Table[0],16777216     ; 2^24
                mov     Cos_Table[4],16776900     ; 2^24*cos(2г/1024)
                mov     si,OFFSET Cos_table
                lea     di,[si+8]
                mov     cx,1022          
                mov     ebx,cos_table[4] 
                mov     eax,ebx
        @@calc: imul    ebx
                shrd    eax,edx,23
                sub     eax,[di-8]
                stosd
                loop    @@calc
                
                mov     di,si
                mov     cx,1024
       @div:    lodsd
                sar     eax,14
                stosw
                dec     cx
                jnz     @div
; On a une table de Cosinus*2^10
                ret
GEN_COS         endp

;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; Passage de toutes les couleurs de 0ДД>256
FLASH                   proc    near
                        mov     dx,3C8h
                        xor     ax,ax
                        out     dx,al
                        
                        xor     cl,cl
                @Pal1:  movzx   ax,cl
                        shl     al,1
                        mov     dx,3C9h
                @pal2:  out     dx,al
                        out     dx,al
                        out     dx,al
                        dec     ah
                        jnz     @pal2
                        WAIT_VBL
                        inc     cl
                        cmp     cl,32
                        jnz     @pal1

                        dec     cl
                @pal3:  movzx   ax,cl
                        shl     al,1
                        mov     dx,3C9h
                @pal4:  out     dx,al
                        out     dx,al
                        out     dx,al
                        dec     ah
                        jnz     @pal4
                        WAIT_VBL
                        dec     cl
                        jns     @pal3
                        ret
FLASH                   endp
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
;   ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
FRESH                   proc    near
                        xor     al,al
           @popo:       mov     c,0
                        mov     c1,al
                        mov     c2,al
                        mov     c3,al
           @popo1:      mov     d,32
                        mov     d1,al
                        mov     d2,32
                        mov     d3,63
                        CALL    MAKE_PAL
           @popo2:      mov     c,32
                        mov     c1,0
                        mov     c2,32
                        mov     c3,63
           @popo3:      mov     d,63
                        mov     d1,0
                        mov     d2,63
                        mov     d3,63
                        CALL    MAKE_PAL
                        ret
FRESH                   endp                        
                    
;   ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
CHAUD                   proc    near
                        xor     al,al
                        mov     c,0                       
                        mov     c,al
                        mov     c1,al
                        mov     c2,al
                        mov     c3,al
                        mov     d,32
                        mov     d1,63
                        mov     d2,32
                        mov     d3,al
                        CALL    MAKE_PAL
                        mov     c,32
                        mov     c1,63
                        mov     c2,32
                        mov     c3,0
                        mov     d,63
                        mov     d1,63
                        mov     d2,63
                        mov     d3,0
                        CALL    MAKE_PAL
                        ret
CHAUD                   endp
;   ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД


;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
INCLUDE                 plasma.inc
INCLUDE                 rotazoom.inc
INCLUDE                 tunnel.inc
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
Ecran                   dw 0A000h
message1                db ' *LION* THE 4Kb intro by Elliot/дpsilon - 1995.',13,10,'$'
message2                db ' Initializing, please wait...',13,10,'$'
img:
dw 01000011100000000b,00000000000000000b
dw 00110110110010000b,00000000000000000b
dw 00001100010000000b,00000000000000000b
dw 00000111110000000b,00000000000000000b
dw 00000100000010001b,01110110110000000b
dw 00000100100010010b,01001101101000000b
dw 00110100010010010b,00111001001000000b
dw 01001100010010010b,00001001001000000b
dw 01001100010111010b,00001001001100110b
dw 00111011101101101b,01110001000111000b
INCLUDE                 3D2.inc
cos_table               dd 1024 dup (?)
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
;  /ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД\
;  і°°°°°°°°°°°°°°°°°°° Datas °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°і
;  \ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД/
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД


SegMappe                dw ?
Virtuel                 dw ?
Count                   dd ?
c                       db ?
c1                      db ?
c2                      db ?
c3                      db ?
d                       db ?
d1                      db ?
d2                      db ?
d3                      db ?
pile                    db 8000 dup (?)
FINCODE:                
                        CODE    ENDS
                        END     START

