; Source de ma 4Ko (Lion)

INCLUDE                 MACROS.INC

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



;  Segment Principal 
CODE                    SEGMENT
                        ASSUME CS:CODE,ES:CODE,DS:CODE
                        .486P
                        ORG    100h

START:                  cld

;/-------------------------------------------------------------------------\
;|                         INITIALIZATIONS                                 |
;\-------------------------------------------------------------------------/
; Libère la mémoire non utilisée:
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

                        CALL    GEN_COS     ; Prépare la table de cosinus.
                        CALL    GEN_PLASMA  ; Génère le plasma.
                        CALL    GEN_ADRESSE ; Prépare les adresses du tunnel.
                        CALL    GEN_NORM    ; Précalcule les vecteurs normaux.
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
; Transfert de SegPlasma ──> SegAdresse1

                        mov     es,SegAdresse1
                        mov     ds,SegPlasma
                        xor     si,si
                        mov     di,si
                        mov     cx,65536/4
                        REPMOVSD
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
                        add     byte ptr cs:@popo+4,128
                        add     byte ptr cs:@popo1+4,128
                        add     byte ptr cs:@popo2+4,128
                        add     byte ptr cs:@popo3+4,128
                        CALL    FRESH
                        CALL    CHAUD                        
                        
                        CALL    MAIN_3D

;/-------------------------------------------------------------------------\
;|               RETOUR AU DOS ET REMISE DES SEGMENTS ALLOUéS.             |
;\-------------------------------------------------------------------------/

 Retour:                VGA     3
                        WRITE   message1
                        DISPOS  SegPlasma
                        DISPOS  Virtuel
                        DISPOS  SegAdresse1
                        DISPOS  SegAdresse2
                        sti
                        DOS_   0

; Création d'une palette.
MAKE_PAL                proc    near
                        mov     al,palTo
                        sub     al,palFrom
                        cbw
                        mov     bx,ax
                        mov     dx,3C8h
                        mov     al,palFrom
                        out     dx,al
                        xor     cx,cx
        
                @bclPal:mov     al,palDstR
                        sub     al,palSrcR
                        cbw
                        imul    cx
                        idiv    bx
                        add     al,palSrcR
                        mov     dx,03C9h
                        out     dx,al

                        mov     al,palDstG
                        sub     al,palSrcG
                        cbw
                        imul    cx
                        idiv    bx
                        add     al,palSrcG
                        mov     dx,03C9h
                        out     dx,al

                        mov     al,palDstB
                        sub     al,palSrcB
                        cbw
                        imul    cx
                        idiv    bx
                        add     al,palSrcB
                        mov     dx,03C9h
                        out     dx,al
                        
                        inc     cx
                        cmp     cx,bx
                        jle     @bclPal
                        ret
MAKE_PAL                endp

; Génère une table de cosinus.

GEN_COS         proc    near
                mov     Cos_Table[0],16777216     ; 2^24
                mov     Cos_Table[4],16776900     ; 2^24*cos(2π/1024)
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


; Passage de toutes les couleurs de 0──>256
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

FRESH                   proc    near
                        xor     al,al
           @popo:       mov     palFrom,0
                        mov     palSrcR,al
                        mov     palSrcG,al
                        mov     palSrcB,al
           @popo1:      mov     palTo,32
                        mov     palDstR,al
                        mov     palDstG,32
                        mov     palDstB,63
                        CALL    MAKE_PAL
           @popo2:      mov     palFrom,32
                        mov     palSrcR,0
                        mov     palSrcG,32
                        mov     palSrcB,63
           @popo3:      mov     palTo,63
                        mov     palDstR,0
                        mov     palDstG,63
                        mov     palDstB,63
                        CALL    MAKE_PAL
                        ret
FRESH                   endp                        
                    
CHAUD                   proc    near
                        xor     al,al
                        mov     palFrom,0
                        mov     palFrom,al
                        mov     palSrcR,al
                        mov     palSrcG,al
                        mov     palSrcB,al
                        mov     palTo,32
                        mov     palDstR,63
                        mov     palDstG,32
                        mov     palDstB,al
                        CALL    MAKE_PAL
                        mov     palFrom,32
                        mov     palSrcR,63
                        mov     palSrcG,32
                        mov     palSrcB,0
                        mov     palTo,63
                        mov     palDstR,63
                        mov     palDstG,63
                        mov     palDstB,0
                        CALL    MAKE_PAL
                        ret
CHAUD                   endp

INCLUDE                 plasma.inc
INCLUDE                 rotazoom.inc
INCLUDE                 tunnel.inc


Ecran                   dw 0A000h
message1                db ' *LION* THE 4Kb intro by Elliot/Σpsilon - 1995.',13,10,'$'
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






SegMappe                dw ?
Virtuel                 dw ?
Count                   dd ?
palFrom                 db ?
palSrcR                 db ?
palSrcG                 db ?
palSrcB                 db ?
palTo                   db ?
palDstR                 db ?
palDstG                 db ?
palDstB                 db ?
pile                    db 8000 dup (?)
FINCODE:                
                        CODE    ENDS
                        END     START

